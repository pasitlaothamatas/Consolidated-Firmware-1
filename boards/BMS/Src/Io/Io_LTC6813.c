#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Io_LTC6813.h"
#include "Io_SharedSpiDMA.h"
#include "Io_SharedSpi.h"
#include "config/Io_LTC6813Configs.h"

enum LTC6813_CellVoltageRegisterGroup
{
    LTC6813_REGISTER_GROUP_A,
    LTC6813_REGISTER_GROUP_B,
    LTC6813_REGISTER_GROUP_C,
    LTC6813_REGISTER_GROUP_D,
    LTC6813_REGISTER_GROUP_E,
    LTC6813_REGISTER_GROUP_F,
    LTC6813_NUM_OF_CELL_REGISTER_GROUPS
};

struct Ltc6813
{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *     nss_port;
    uint16_t           nss_pin;
    bool               is_adc_polling;
    bool               is_awake;

    uint16_t cell_voltages[NUM_OF_CELLS_PER_LTC6813][TOTAL_NUM_OF_LTC6813_IC];
};

struct CommandLookupTable
{
    uint16_t cell_voltage_register_group[LTC6813_NUM_OF_CELL_REGISTER_GROUPS];
};

static struct Ltc6813 ltc_6813;

// clang-format off
static const struct CommandLookupTable command_lookup_table = {
    .cell_voltage_register_group =
            {
                0x0400, // RDCVA
                0x0600, // RDCVB
                0x0800, // RDCVC
                0x0A00, // RDCVD
                0x0900, // RDCVE
                0x0B00  // RDCVF
            }
};
// clang-format on

static uint16_t Io_CalculatePec15(uint8_t *data, size_t num_of_bytes);
static uint16_t Io_CalculatePec15(uint8_t *data, size_t size)
{
    size_t   pec15_lut_index;
    uint16_t pec15 = 16U;

    for (size_t i = 0U; i < size; i++)
    {
        pec15_lut_index = ((pec15 >> 7) ^ data[i]) & 0xFF;
        pec15 = (uint16_t)((pec15 << 8) ^ PEC_15_LUT[pec15_lut_index]);
    }

    // Set the LSB of the computed PEC15 to 0
    return (uint16_t)(pec15 << 1);
}

void Io_LTC6813_Init(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin)
{
    assert(hspi != NULL);

    ltc_6813.hspi           = hspi;
    ltc_6813.nss_port       = nss_port;
    ltc_6813.nss_pin        = nss_pin;
    ltc_6813.is_adc_polling = false;
    ltc_6813.is_awake       = false;
}

void Io_LTC6813_StartWakeUp(void)
{
    HAL_GPIO_WritePin(ltc_6813.nss_port, ltc_6813.nss_pin, GPIO_PIN_RESET);
}

void Io_LTC6813_EndWakeUp(void)
{
    HAL_GPIO_WritePin(ltc_6813.nss_port, ltc_6813.nss_pin, GPIO_PIN_SET);
}

void Io_LTC6813_Configure(void)
{
    // Commands used to write to the Configuration Register groups
    const uint16_t WRCFGA                = 0x01;
    const uint8_t  DEFAULT_CONFIG_REG[4] = {
        (uint8_t)(REFON << 2) + (uint8_t)(DTEN << 1) + ADCOPT,
        (uint8_t)(CELL_UNDERVOLTAGE_THRESHOLD),
        (uint8_t)((CELL_OVERVOLTAGE_THRESHOLD & 0xF) << 4) +
            (uint8_t)(CELL_UNDERVOLTAGE_THRESHOLD >> 8),
        (uint8_t)(CELL_OVERVOLTAGE_THRESHOLD >> 4)
    };

    // Write to configuration register group A
    uint8_t tx_cmd[NUM_CMD_BYTES];
    tx_cmd[0]             = (uint8_t)(WRCFGA >> 8);
    tx_cmd[1]             = (uint8_t)WRCFGA;
    uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2);
    tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
    tx_cmd[3]             = (uint8_t)tx_cmd_pec15;

    uint8_t tx_payload[8] = { 0 };
    memcpy(tx_payload, DEFAULT_CONFIG_REG, 4U);
    uint16_t tx_write_cmd_pec15 = Io_CalculatePec15(tx_payload, 6U);
    tx_payload[6]               = (uint8_t)(tx_write_cmd_pec15 >> 8);
    tx_payload[7]               = (uint8_t)tx_write_cmd_pec15;

    // Transfer the command followed by the payload data to write configure the
    // Ltc6813 chips
    HAL_GPIO_WritePin(ltc_6813.nss_port, ltc_6813.nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(ltc_6813.hspi, tx_cmd, 4U, 100U);

    // Send the tx_write command to all of the Ltc6813 chips connected on the
    // daisy chain.
    for (size_t i = 0U; i < TOTAL_NUM_OF_LTC6813_IC; i++)
    {
        HAL_SPI_Transmit(ltc_6813.hspi, tx_payload, 8U, 100U);
    }
    HAL_GPIO_WritePin(ltc_6813.nss_port, ltc_6813.nss_pin, GPIO_PIN_SET);

    ltc_6813.is_adc_polling = true;
}

ExitCode Io_LTC6813_StartADCConversion(void)
{
    const uint16_t ADCV =
        0x260 + (ADCOPT << 7) + (DCP_DISABLED << 4) + CELL_CH_ALL;

    uint8_t tx_cmd[4];
    tx_cmd[0] = (uint8_t)(ADCV >> 8);
    tx_cmd[1] = (uint8_t)ADCV;

    uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
    tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
    tx_cmd[3]             = (uint8_t)tx_cmd_pec15;

    // Have the size be 4 bytes. This is the standard # of bytes used to
    // transfer a command
    if (Io_SharedSpi_Transmit(
            ltc_6813.hspi, ltc_6813.nss_port, ltc_6813.nss_pin, tx_cmd, 4U) !=
        HAL_OK)
    {
        return EXIT_CODE_UNIMPLEMENTED;
    }

    return EXIT_CODE_OK;
}

static void Io_LTC6813_IsAdcConversionComplete(void);
static void Io_LTC6813_IsAdcConversionComplete(void)
{
    const uint16_t PLADC = 0x1407;

    uint8_t tx_cmd[4];

    tx_cmd[0] = (uint8_t)PLADC;
    tx_cmd[1] = (uint8_t)(PLADC >> 1);

    uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
    tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
    tx_cmd[3]             = (uint8_t)tx_cmd_pec15;

    HAL_GPIO_WritePin(ltc_6813.nss_port, ltc_6813.nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(ltc_6813.hspi, tx_cmd, 4U, 100U);

    uint8_t rx_data = 0U;
    while (rx_data == 0U)
    {
        HAL_SPI_Receive(ltc_6813.hspi, &rx_data, 1U, 100U);
    }
    HAL_GPIO_WritePin(ltc_6813.nss_port, ltc_6813.nss_pin, GPIO_PIN_SET);
}

bool Io_LTC6813_ParseCellsAndPerformPec15Check(
    size_t   current_ic,
    size_t   current_register_group,
    uint8_t *rx_cell_voltages);

bool Io_LTC6813_ParseCellsAndPerformPec15Check(
    size_t   current_ic,
    size_t   current_register_group,
    uint8_t *rx_cell_voltages)
{
    size_t cell_voltage_index =
        current_ic * NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP;

    for (size_t current_cell = 0U;
         current_cell < NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP; current_cell++)
    {
        uint32_t cell_voltage =
            (uint32_t)(rx_cell_voltages[cell_voltage_index]) |
            (uint32_t)((rx_cell_voltages[cell_voltage_index + 1] << 8));

        ltc_6813.cell_voltages[current_ic]
                              [current_cell +
                               (current_register_group - 1) *
                                   NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP] =
            (uint16_t)cell_voltage;

        // Each cell voltage is represented as a pair of bytes. Therefore
        // increment the cell voltage index by 2 to retrieve the next cell
        // voltage.
        cell_voltage_index += 2U;
    }

    uint32_t received_pec15 =
        (uint32_t)(rx_cell_voltages[cell_voltage_index]) |
        (uint32_t)(rx_cell_voltages[cell_voltage_index + 1]);
    uint32_t calculated_pec15 =
        Io_CalculatePec15(&rx_cell_voltages[current_ic * NUM_OF_RX_BYTES], 6U);

    return (received_pec15 == calculated_pec15);
}

PEC15Codes Io_LTC6813_ReadAllCellRegisterGroups(void)
{
    uint16_t cell_register_group_cmd;
    uint8_t  tx_cmd[4];
    uint8_t  rx_cell_voltages[NUM_OF_RX_BYTES * TOTAL_NUM_OF_LTC6813_IC];

    for (size_t current_register_group = LTC6813_REGISTER_GROUP_A;
         current_register_group < LTC6813_NUM_OF_CELL_REGISTER_GROUPS;
         current_register_group++)
    {
        cell_register_group_cmd =
            command_lookup_table
                .cell_voltage_register_group[current_register_group];

        tx_cmd[0] = (uint8_t)cell_register_group_cmd;
        tx_cmd[1] = (uint8_t)(cell_register_group_cmd >> 8);

        uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
        tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
        tx_cmd[3]             = (uint8_t)(tx_cmd_pec15);

        Io_LTC6813_IsAdcConversionComplete();
        Io_SharedSpi_TransmitAndReceive(
            ltc_6813.hspi, ltc_6813.nss_port, ltc_6813.nss_pin, tx_cmd, 4U,
            rx_cell_voltages, NUM_OF_RX_BYTES * TOTAL_NUM_OF_LTC6813_IC);

        for (size_t current_ic = 0U; current_ic < TOTAL_NUM_OF_LTC6813_IC;
             current_ic++)
        {
            if (!Io_LTC6813_ParseCellsAndPerformPec15Check(
                    current_ic, current_register_group, rx_cell_voltages))
            {
                return PEC15_MISMATCH_ERROR;
            }
        }
    }

    return PEC15_OK;
}

uint16_t *Io_LTC6813_GetCellVoltages(void)
{
    return &ltc_6813.cell_voltages[0][0];
}
