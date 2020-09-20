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
    GPIO_TypeDef *     chip_select_port;
    uint16_t           chip_select_pin;
    bool               is_adc_polling;
    bool               is_awake;

    uint16_t cell_voltages[NUM_OF_CELLS_PER_LTC6813][TOTAL_NUM_OF_LTC6813_IC];
};

struct CommandLookupTable
{
    uint16_t cell_voltage_register_group[LTC6813_NUM_OF_CELL_REGISTER_GROUPS];
    uint16_t pec15_lut[UINT8_MAX + 1];
};

static struct Ltc6813 ltc_6813;

// clang-format off
static const struct CommandLookupTable command_lookup_table = {
    .cell_voltage_register_group =
            {
                RDCVA,
                RDCVB,
                RDCVC,
                RDCVD,
                RDCVE,
                RDCVF
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
        pec15_lut_index = ((pec15 >> 7U) ^ data[i]) & 0xFF;
        pec15 = (uint16_t)((pec15 << 8U) ^ PEC_15_LUT[pec15_lut_index]);
    }

    // Set the LSB of the computed PEC15 to 0
    return (uint16_t)(pec15 << 1U);
}

static HAL_StatusTypeDef
    Io_LTC6813_BroadcastCommand(uint16_t tx_cmd, uint16_t size);
static HAL_StatusTypeDef
    Io_LTC6813_BroadcastCommand(uint16_t tx_cmd, uint16_t size)
{
    uint8_t _tx_cmd[4];

    // Split tx_cmd into two bytes
    _tx_cmd[0] = (uint8_t)(tx_cmd >> 8U);
    _tx_cmd[1] = (uint8_t)tx_cmd;

    // Calculate the PEC15 and split the command into two bytes
    uint16_t _tx_cmd_pec15 = Io_CalculatePec15(_tx_cmd, 2U);
    _tx_cmd[2]             = (uint8_t)(_tx_cmd_pec15 >> 8U);
    _tx_cmd[3]             = (uint8_t)_tx_cmd_pec15;

    return Io_SharedSpi_Transmit(
        ltc_6813.hspi, ltc_6813.chip_select_port, ltc_6813.chip_select_pin,
        _tx_cmd, size);
}

void Io_LTC6813_Init(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin)
{
    assert(hspi != NULL);

    ltc_6813.hspi             = hspi;
    ltc_6813.chip_select_port = chip_select_port;
    ltc_6813.chip_select_pin  = chip_select_pin;
    ltc_6813.is_adc_polling   = false;
    ltc_6813.is_awake         = false;
}

void Io_LTC6813_StartWakeUp(void)
{
    HAL_GPIO_WritePin(
        ltc_6813.chip_select_port, ltc_6813.chip_select_pin, GPIO_PIN_RESET);
}

void Io_LTC6813_EndWakeUp(void)
{
    HAL_GPIO_WritePin(
        ltc_6813.chip_select_port, ltc_6813.chip_select_pin, GPIO_PIN_SET);
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
    tx_cmd[0]             = (uint8_t)(WRCFGA >> 8U);
    tx_cmd[1]             = (uint8_t)WRCFGA;
    uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
    tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8U);
    tx_cmd[3]             = (uint8_t)tx_cmd_pec15;

    uint8_t tx_payload[8] = { 0 };
    memcpy(tx_payload, DEFAULT_CONFIG_REG, 4U);
    uint16_t tx_write_cmd_pec15 = Io_CalculatePec15(tx_payload, 6U);
    tx_payload[6]               = (uint8_t)(tx_write_cmd_pec15 >> 8U);
    tx_payload[7]               = (uint8_t)tx_write_cmd_pec15;

    // Transfer the command followed by the payload data to write configure the
    // Ltc6813 chips
    HAL_GPIO_WritePin(
        ltc_6813.chip_select_port, ltc_6813.chip_select_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(ltc_6813.hspi, tx_cmd, 4U, 100U);

    // Send the tx_write command to all of the Ltc6813 chips connected on the
    // daisy chain.
    for (size_t i = 0U; i < TOTAL_NUM_OF_LTC6813_IC; i++)
    {
        HAL_SPI_Transmit(ltc_6813.hspi, tx_payload, 8U, 100U);
    }
    HAL_GPIO_WritePin(
        ltc_6813.chip_select_port, ltc_6813.chip_select_pin, GPIO_PIN_SET);

    ltc_6813.is_adc_polling = true;
}

void Io_LTC6813_StartADCConversion(void)
{
    const uint16_t ADCV =
        0x260 + (ADCOPT << 7) + (DCP_DISABLED << 4) + CELL_CH_ALL;

    // Have the size be 4 bytes. This is the standard # of bytes used to
    // transfer a command
    Io_LTC6813_BroadcastCommand(ADCV, 4U);
}

bool Io_LTC6813_IsAdcConversionComplete(void)
{
    const uint16_t PLADC = 0x1407;

    uint8_t rx_data;
    uint8_t tx_cmd[4];

    tx_cmd[0] = (uint8_t)PLADC;
    tx_cmd[1] = (uint8_t)(PLADC >> 1);

    uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
    tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
    tx_cmd[3]             = (uint8_t)tx_cmd_pec15;

    HAL_GPIO_WritePin(
        ltc_6813.chip_select_port, ltc_6813.chip_select_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(ltc_6813.hspi, tx_cmd, 4U, 100U);
    HAL_SPI_Receive(ltc_6813.hspi, &rx_data, 1U, 100U);
    HAL_GPIO_WritePin(
        ltc_6813.chip_select_port, ltc_6813.chip_select_pin, GPIO_PIN_SET);

    if (rx_data > 0U)
    {
        ltc_6813.is_adc_polling = false;
        return true;
    }

    return false;
}

void Io_LTC6813_ParseCells(
    size_t   current_ic,
    size_t   current_register_group,
    uint8_t *rx_cell_voltages)
{
    for (size_t current_cell = 0U;
         current_cell < NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP; current_cell++)
    {
        size_t cell_voltage_index =
            current_ic * NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP;
        uint32_t cell_voltage =
            (uint32_t)(rx_cell_voltages[cell_voltage_index]) |
            (uint32_t)((rx_cell_voltages[cell_voltage_index + 1] << 8));

        ltc_6813.cell_voltages[current_ic]
                              [current_cell +
                               (current_register_group - 1) *
                                   NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP] =
            (uint16_t)cell_voltage;
    }
}

void Io_LTC6813_ReadAllCellRegisterGroups(uint8_t *rx_cell_voltages)
{
    uint16_t cell_register_group_cmd;
    uint8_t  tx_cmd[4];

    for (size_t current_register_group = LTC6813_REGISTER_GROUP_A;
         current_register_group < LTC6813_NUM_OF_CELL_REGISTER_GROUPS;
         current_register_group++)
    {
        cell_register_group_cmd =
            command_lookup_table
                .cell_voltage_register_group[current_register_group];

        // TODO: Create a macro to pack tx_command?
        tx_cmd[0] = (uint8_t)cell_register_group_cmd;
        tx_cmd[1] = (uint8_t)(cell_register_group_cmd >> 8U);

        uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
        tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
        tx_cmd[3]             = (uint8_t)(tx_cmd_pec15);

        // 8U represents the reg length value
        Io_SharedSpi_TransmitAndReceive(
            ltc_6813.hspi, ltc_6813.chip_select_port, ltc_6813.chip_select_pin,
            tx_cmd, 4U, rx_cell_voltages, 8U * TOTAL_NUM_OF_LTC6813_IC);

        for (size_t current_ic = 0U; current_ic < TOTAL_NUM_OF_LTC6813_IC;
             current_ic++)
        {
            // Parse cells
        }
    }
}
