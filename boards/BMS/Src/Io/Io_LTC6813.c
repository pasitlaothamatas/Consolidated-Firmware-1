#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Io_LTC6813.h"
#include "Io_SharedSpiDMA.h"
#include "Io_SharedSpi.h"
#include "config/Io_LTC6813Configs.h"

struct LTC6813
{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *     chip_select_port;
    uint16_t           chip_select_pin;
    uint16_t           cell_voltages[NUM_OF_CELLS_PER_LTC6813];
    bool               is_adc_polling;
    bool               is_awake;

    uint16_t rx_cell_voltages[100];
};

static uint16_t Io_CalculatePec15(uint8_t *data, size_t num_of_bytes);

static void Io_TransmitCommandAndReceiveData(
    uint16_t        tx_command,
    uint8_t *       rx_data,
    uint16_t        size_tx_command,
    uint16_t        size_rx_data,
    struct LTC6813 *ltc_6813);

static uint16_t Io_CalculatePec15(uint8_t *data, size_t size)
{
    size_t   pec15_lut_index;
    uint16_t pec15 = 16U;

    for (size_t i = 0U; i < size; i++)
    {
        pec15_lut_index = ((pec15 >> 7U) ^ data[i]) & 0xFF;
        pec15 = (uint16_t)((pec15 << 8U) ^ PEC_15_LUT[pec15_lut_index]);
    }

    // Set the LSB of the computed PEC15 to 0
    return (uint16_t)(pec15 << 1U);
}

HAL_StatusTypeDef Io_LTC6813_BroadcastCommand(
    uint16_t              tx_command,
    uint16_t              size,
    struct LTC6813 *const ltc_6813)
{
    uint8_t _tx_command[4];

    // Split tx_command into two bytes
    _tx_command[0] = (uint8_t)(tx_command >> 8U);
    _tx_command[1] = (uint8_t)tx_command;

    // Calculate the PEC15 and split the command into two bytes
    uint16_t _tx_command_pec15 = Io_CalculatePec15(_tx_command, 2U);
    _tx_command[2]             = (uint8_t)(_tx_command_pec15 >> 8U);
    _tx_command[3]             = (uint8_t)_tx_command_pec15;

    return Io_SharedSpi_Transmit(
        ltc_6813->hspi, ltc_6813->chip_select_port, ltc_6813->chip_select_pin,
        _tx_command, size);
}

static void Io_TransmitCommandAndReceiveData(
    uint16_t        tx_command,
    uint8_t *       rx_data,
    uint16_t        size_tx_command,
    uint16_t        size_rx_data,
    struct LTC6813 *ltc_6813)
{
    uint8_t _tx_command[4];
    _tx_command[0]           = (uint8_t)tx_command;
    _tx_command[1]           = (uint8_t)(tx_command >> 8);
    uint16_t _tx_command_pec = Io_CalculatePec15(_tx_command, 2U);
    _tx_command[2]           = (uint8_t)(_tx_command_pec >> 8);
    _tx_command[3]           = (uint8_t)(_tx_command_pec);

    // TODO: Include the Io_SharedSpi_MultipleTransmitReceive
    UNUSED(size_rx_data);
    UNUSED(size_tx_command);
    UNUSED(rx_data);
    UNUSED(ltc_6813);
}

enum LTC6813_CellVoltageRegisterGroup
{
    LTC6813_REGISTER_GROUP_A,
    LTC6813_REGISTER_GROUP_B,
    LTC6813_REGISTER_GROUP_C,
    LTC6813_REGISTER_GROUP_D,
    LTC6813_REGISTER_GROUP_E,
    LTC6813_REGISTER_GROUP_F,
    LTC6813_NUM_OF_REGISTER_GROUPS
};

struct LTC6813 *Io_LTC6813_Create(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin)
{
    assert(hspi != NULL);

    struct LTC6813 *ltc_6813 = malloc(sizeof(struct LTC6813));
    assert(ltc_6813 != NULL);

    ltc_6813->hspi             = hspi;
    ltc_6813->chip_select_port = chip_select_port;
    ltc_6813->chip_select_pin  = chip_select_pin;

    ltc_6813->is_adc_polling = false;
    ltc_6813->is_awake       = false;

    return ltc_6813;
}

void Io_LTC6813_StartCellADCConversion(struct LTC6813 *ltc_6813)
{
    Io_LTC6813_BroadcastCommand(
        ADCV, NUM_OF_TX_BYTES_PER_LTC6813_CMD, ltc_6813);
}

bool Io_LTC6813_IsAdcReady(struct LTC6813 *ltc_6813)
{
    uint8_t rx_data;
    Io_TransmitCommandAndReceiveData(
        PLADC, &rx_data, NUM_OF_TX_BYTES_PER_LTC6813_CMD, 1U, ltc_6813);

    return rx_data > 0U;
}

void Io_LTC6813_StartWakeUp(const struct LTC6813 *const ltc6813)
{
    HAL_GPIO_WritePin(
        ltc6813->chip_select_port, ltc6813->chip_select_pin, GPIO_PIN_RESET);
}

void Io_LTC6813_EndWakeUp(const struct LTC6813 *ltc6813)
{
    HAL_GPIO_WritePin(
        ltc6813->chip_select_port, ltc6813->chip_select_pin, GPIO_PIN_SET);
}

void Io_LTC6813_Configure(const struct LTC6813 *const ltc_6813)
{
    // Commands used to write to the Configuration Register groups
    static const uint16_t WRCFGA                = 0x01;

    // TODO: PEC15 should already be computed and included in this array
    static uint8_t        DEFAULT_CONFIG_REG[8] = {
        (uint8_t)(REFON << 2) + (uint8_t)(DTEN << 1) + ADCOPT,
        (uint8_t)(CELL_UNDERVOLTAGE_THRESHOLD),
        (uint8_t)((CELL_OVERVOLTAGE_THRESHOLD & 0xF) << 4) +
            (uint8_t)(CELL_UNDERVOLTAGE_THRESHOLD >> 8),
        (uint8_t)(CELL_OVERVOLTAGE_THRESHOLD >> 4),
        0,
        0,
        0,
        0
    };

    // Write to configuration register group A
    uint8_t tx_cmd[NUM_CMD_BYTES];
    tx_cmd[0] = (uint8_t)(WRCFGA >> 8);
    tx_cmd[1] = (uint8_t)WRCFGA;

    // Calculate the PEC15 and split the result into 2 bytes
    uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
    tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8U);
    tx_cmd[3]             = (uint8_t)tx_cmd_pec15;

    uint16_t tx_write_cmd_pec15 = Io_CalculatePec15(DEFAULT_CONFIG_REG, 6U);
    DEFAULT_CONFIG_REG[6]       = (uint8_t)(tx_write_cmd_pec15 >> 8U);
    DEFAULT_CONFIG_REG[7]       = (uint8_t)tx_write_cmd_pec15;

    uint8_t tx_write_cmd[NUM_CMD_BYTES + TOTAL_NUM_OF_PAYLOAD_BYTES_LTC6813];
    memcpy(tx_write_cmd, tx_cmd, NUM_CMD_BYTES * sizeof(uint8_t));

    for (size_t i = 0U; i < TOTAL_NUM_OF_LTC6813; i++)
    {
        memcpy(
            &tx_write_cmd[NUM_CMD_BYTES + i * 8], DEFAULT_CONFIG_REG, 8 * sizeof(uint8_t));
    }

    HAL_GPIO_WritePin(
        ltc_6813->chip_select_port, ltc_6813->chip_select_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit_DMA(
        ltc_6813->hspi, tx_write_cmd, 4 * TOTAL_NUM_OF_PAYLOAD_BYTES_LTC6813);
}

void Io_LTC6813_ReadAllCellVoltages(
    uint8_t *             rx_cell_voltages,
    struct LTC6813 *const ltc_6813)
{
    uint16_t tx_command;

    for (size_t cell_register_group = LTC6813_REGISTER_GROUP_A;
         cell_register_group < LTC6813_NUM_OF_REGISTER_GROUPS;
         cell_register_group++)
    {
        switch (cell_register_group)
        {
            case LTC6813_REGISTER_GROUP_A:
                tx_command = RDCVA;
                break;
            case LTC6813_REGISTER_GROUP_B:
                tx_command = RDCVB;
                break;
            case LTC6813_REGISTER_GROUP_C:
                tx_command = RDCVC;
                break;
            case LTC6813_REGISTER_GROUP_D:
                tx_command = RDCVD;
                break;
            case LTC6813_REGISTER_GROUP_E:
                tx_command = RDCVE;
                break;
            case LTC6813_REGISTER_GROUP_F:
                tx_command = RDCVF;
                break;
            default:
                break;
        }

        Io_TransmitCommandAndReceiveData(
            tx_command, rx_cell_voltages, NUM_OF_TX_BYTES_PER_LTC6813_CMD,
            NUM_OF_BYTES_PER_LTC6813_REGISTER + NUM_OF_PEC_BYTES, ltc_6813);

        for (size_t current_ltc_6813 = 0U;
             current_ltc_6813 < TOTAL_NUM_OF_LTC6813; current_ltc_6813++)
        {
            size_t rx_cell_voltages_index = current_ltc_6813 * NUM_OF_RX_BYTES;
            for (size_t current_cell = 0U;
                 current_cell < NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP;
                 current_cell++)
            {
                ltc_6813->cell_voltages
                    [current_cell + ((cell_register_group - 1U) *
                                     NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP)] =
                    (uint16_t)(
                        (rx_cell_voltages[rx_cell_voltages_index] +
                         (rx_cell_voltages[rx_cell_voltages_index + 1] << 8)));

                // An individual cell voltage received is read back as two
                // bytes. Thus, we increment the index by two to get the next
                // cell voltage.
                rx_cell_voltages_index += 2;
            }

            uint16_t pec_received = (uint16_t)(
                (rx_cell_voltages[rx_cell_voltages_index] << 8) |
                rx_cell_voltages[rx_cell_voltages_index + 1]);
            uint16_t pec_calculated = Io_CalculatePec15(
                &rx_cell_voltages[current_ltc_6813 * NUM_OF_RX_BYTES],
                NUM_OF_BYTES_PER_LTC6813_REGISTER);
            if (pec_received != pec_calculated)
            {
                // Do something
            }
        }
    }
}
