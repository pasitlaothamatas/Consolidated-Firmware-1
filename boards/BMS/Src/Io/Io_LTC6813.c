#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stm32f3xx_hal.h>
#include "Io_LTC6813.h"
#include "Io_SharedSpi.h"
#include "config/Io_LTC6813Configs.h"

enum LTC6813_CellVoltageRegisterGroups
{
    LTC6813_REGISTER_GROUP_A = 0,
    LTC6813_REGISTER_GROUP_B,
    LTC6813_REGISTER_GROUP_C,
    LTC6813_REGISTER_GROUP_D,
    LTC6813_REGISTER_GROUP_E,
    LTC6813_REGISTER_GROUP_F,
    LTC6813_NUM_OF_CELL_REGISTER_GROUPS
};

struct LookupTables
{
    // Commands used to read cell voltage register groups.
    uint16_t cell_voltage_register_group_commands
        [LTC6813_NUM_OF_CELL_REGISTER_GROUPS];

    // The CRC table used to calculate 15 bit packet error codes (PEC15).
    uint16_t crc_table[256];
};

// clang-format off
static const struct LookupTables lookup_tables = {
    .cell_voltage_register_group_commands =
            {
                0x0400, // RDCVA
                0x0600, // RDCVB
                0x0800, // RDCVC
                0x0A00, // RDCVD
                0x0900, // RDCVE
                0x0B00  // RDCVF
            },

    .crc_table =
            {   0x0, 0xC599, 0xCEAB, 0xB32,  0xD8CF, 0x1D56, 0x1664, 0xD3FD, 0xF407,
                0x319E, 0x3AAC, 0xFF35, 0x2CC8, 0xE951, 0xE263, 0x27FA, 0xAD97, 0x680E,
                0x633C, 0xA6A5, 0x7558, 0xB0C1, 0xBBF3, 0x7E6A, 0x5990, 0x9C09, 0x973B,
                0x52A2, 0x815F, 0x44C6, 0x4FF4, 0x8A6D, 0x5B2E, 0x9EB7, 0x9585, 0x501C,
                0x83E1, 0x4678, 0x4D4A, 0x88D3, 0xAF29, 0x6AB0, 0x6182, 0xA41B, 0x77E6,
                0xB27F, 0xB94D, 0x7CD4, 0xF6B9, 0x3320, 0x3812, 0xFD8B, 0x2E76, 0xEBEF,
                0xE0DD, 0x2544, 0x2BE,  0xC727, 0xCC15, 0x98C,  0xDA71, 0x1FE8, 0x14DA,
                0xD143, 0xF3C5, 0x365C, 0x3D6E, 0xF8F7, 0x2B0A, 0xEE93, 0xE5A1, 0x2038,
                0x7C2,  0xC25B, 0xC969, 0xCF0,  0xDF0D, 0x1A94, 0x11A6, 0xD43F, 0x5E52,
                0x9BCB, 0x90F9, 0x5560, 0x869D, 0x4304, 0x4836, 0x8DAF, 0xAA55, 0x6FCC,
                0x64FE, 0xA167, 0x729A, 0xB703, 0xBC31, 0x79A8, 0xA8EB, 0x6D72, 0x6640,
                0xA3D9, 0x7024, 0xB5BD, 0xBE8F, 0x7B16, 0x5CEC, 0x9975, 0x9247, 0x57DE,
                0x8423, 0x41BA, 0x4A88, 0x8F11, 0x57C,  0xC0E5, 0xCBD7, 0xE4E,  0xDDB3,
                0x182A, 0x1318, 0xD681, 0xF17B, 0x34E2, 0x3FD0, 0xFA49, 0x29B4, 0xEC2D,
                0xE71F, 0x2286, 0xA213, 0x678A, 0x6CB8, 0xA921, 0x7ADC, 0xBF45, 0xB477,
                0x71EE, 0x5614, 0x938D, 0x98BF, 0x5D26, 0x8EDB, 0x4B42, 0x4070, 0x85E9,
                0xF84,  0xCA1D, 0xC12F, 0x4B6,  0xD74B, 0x12D2, 0x19E0, 0xDC79, 0xFB83,
                0x3E1A, 0x3528, 0xF0B1, 0x234C, 0xE6D5, 0xEDE7, 0x287E, 0xF93D, 0x3CA4,
                0x3796, 0xF20F, 0x21F2, 0xE46B, 0xEF59, 0x2AC0, 0xD3A,  0xC8A3, 0xC391,
                0x608,  0xD5F5, 0x106C, 0x1B5E, 0xDEC7, 0x54AA, 0x9133, 0x9A01, 0x5F98,
                0x8C65, 0x49FC, 0x42CE, 0x8757, 0xA0AD, 0x6534, 0x6E06, 0xAB9F, 0x7862,
                0xBDFB, 0xB6C9, 0x7350, 0x51D6, 0x944F, 0x9F7D, 0x5AE4, 0x8919, 0x4C80,
                0x47B2, 0x822B, 0xA5D1, 0x6048, 0x6B7A, 0xAEE3, 0x7D1E, 0xB887, 0xB3B5,
                0x762C, 0xFC41, 0x39D8, 0x32EA, 0xF773, 0x248E, 0xE117, 0xEA25, 0x2FBC,
                0x846,  0xCDDF, 0xC6ED, 0x374,  0xD089, 0x1510, 0x1E22, 0xDBBB, 0xAF8,
                0xCF61, 0xC453, 0x1CA,  0xD237, 0x17AE, 0x1C9C, 0xD905, 0xFEFF, 0x3B66,
                0x3054, 0xF5CD, 0x2630, 0xE3A9, 0xE89B, 0x2D02, 0xA76F, 0x62F6, 0x69C4,
                0xAC5D, 0x7fA0, 0xBA39, 0xB10B, 0x7492, 0x5368, 0x96F1, 0x9DC3, 0x585A,
                0x8BA7, 0x4E3E, 0x450C, 0x8095}
};
// clang-format on

struct LTC6813
{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *     nss_port;
    uint16_t           nss_pin;

    // A counter used to count the number of times the PEC fails.
    uint8_t pec15_error_counter;

    // An array containing flags for PEC15 errors for each register group. True
    // if a PEC15 error does not occur for the given register group, false if a
    // PEC15 error occurs.
    bool pec15_errors_for_register_groups[NUM_OF_LTC6813][6];

    // Array containing cell voltages for each LTC6813 IC.
    uint16_t cell_voltages[NUM_OF_LTC6813][NUM_CELLS_PER_LTC6813];
};

static struct LTC6813 ltc_6813;

static uint16_t Io_CalculatePec15(uint8_t *data, uint32_t size);
static ExitCode Io_LTC6813_EnterReadyState(void);
static ExitCode Io_LTC6813_StartADCConversion(void);
static ExitCode Io_LTC6813_PollAdcConversion(void);
static void     Io_LTC6813_ParseCellsAndPerformPec15Check(
        size_t   current_ic,
        size_t   current_register_group,
        uint8_t *rx_cell_voltages);

static uint16_t Io_CalculatePec15(uint8_t *data, uint32_t size)
{
    size_t   pec15_index;
    uint16_t pec15 = 16U;

    for (size_t i = 0U; i < size; i++)
    {
        pec15_index = ((pec15 >> 7) ^ data[i]) & 0xFF;
        pec15 = (uint16_t)((pec15 << 8) ^ lookup_tables.crc_table[pec15_index]);
    }

    // Set the LSB of the computed PEC15 to 0
    return (uint16_t)(pec15 << 1);
}

static ExitCode Io_LTC6813_EnterReadyState(void)
{
    uint8_t rx_data;
    if(Io_SharedSpi_Receive(
        ltc_6813.hspi, ltc_6813.nss_port, ltc_6813.nss_pin, &rx_data, 1U) != HAL_OK)
    {
        return EXIT_CODE_UNIMPLEMENTED;
    }
    UNUSED(rx_data);

    return EXIT_CODE_OK;
}

static ExitCode Io_LTC6813_StartADCConversion(void)
{
    const uint16_t ADCV =
        0x260 + (ADCOPT << 7) + (DCP_DISABLED << 4) + CELL_CH_ALL;

    uint8_t tx_cmd[4];
    tx_cmd[0] = (uint8_t)(ADCV >> 8);
    tx_cmd[1] = (uint8_t)ADCV;

    uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
    tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
    tx_cmd[3]             = (uint8_t)tx_cmd_pec15;

    return Io_SharedSpi_Transmit(
               ltc_6813.hspi, ltc_6813.nss_port, ltc_6813.nss_pin, tx_cmd, 4U)
               ? EXIT_CODE_OK
               : EXIT_CODE_UNIMPLEMENTED;
}

static ExitCode Io_LTC6813_PollAdcConversion(void)
{
    const uint16_t PLADC = 0x1407;

    uint8_t tx_cmd[4];
    uint8_t rx_data = 0xFF;

    tx_cmd[0] = (uint8_t)PLADC;
    tx_cmd[1] = (uint8_t)(PLADC >> 8);

    uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
    tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
    tx_cmd[3]             = (uint8_t)tx_cmd_pec15;

    // TODO: Add a timeout counter?
    while(rx_data == 0xFF)
    {
        if(Io_SharedSpi_TransmitAndReceive(
                ltc_6813.hspi, ltc_6813.nss_port, ltc_6813.nss_pin, tx_cmd, 4U,
                &rx_data, 1U) != HAL_OK)
        {
            return EXIT_CODE_UNIMPLEMENTED;
        }
    }

    return EXIT_CODE_OK;
}

static void Io_LTC6813_ParseCellsAndPerformPec15Check(
    size_t   current_ic,
    size_t   current_register_group,
    uint8_t *rx_cell_voltages)
{
    size_t cell_voltage_index =
        current_ic * NUM_CELLS_PER_LTC6813_REGISTER_GROUP;

    for (size_t current_cell = 0U;
         current_cell < NUM_CELLS_PER_LTC6813_REGISTER_GROUP; current_cell++)
    {
        uint32_t cell_voltage =
            (uint32_t)(rx_cell_voltages[cell_voltage_index]) |
            (uint32_t)((rx_cell_voltages[cell_voltage_index + 1] << 8));

        ltc_6813.cell_voltages[current_ic]
                              [current_cell +
                               current_register_group *
                                   NUM_CELLS_PER_LTC6813_REGISTER_GROUP] =
            (uint16_t)cell_voltage;

        // Each cell voltage is represented as a pair of bytes. Therefore
        // increment the cell voltage index by 2 to retrieve the next cell
        // voltage.
        cell_voltage_index += 2U;
    }

    uint32_t received_pec15 =
        (uint32_t)(rx_cell_voltages[cell_voltage_index] << 8) |
        (uint32_t)(rx_cell_voltages[cell_voltage_index + 1]);
    uint32_t calculated_pec15 =
        Io_CalculatePec15(&rx_cell_voltages[current_ic * NUM_OF_RX_BYTES], 6U);

    if (received_pec15 == calculated_pec15)
    {
        // Set the element of the given array as true if the PEC15 received is
        // the same as the PEC15 calculated.
        ltc_6813.pec15_errors_for_register_groups[current_ic]
                                                 [current_register_group] =
            true;
    }
    else
    {
        // Set the element of the given array as false if the PEC15 received is
        // different from the PEC15 calculated
        ltc_6813.pec15_errors_for_register_groups[current_ic]
                                                 [current_register_group] =
            false;

        // Increment the number of PEC15 errors.
        ltc_6813.pec15_error_counter++;
    }
}

void Io_LTC6813_Init(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin)
{
    assert(hspi != NULL);

    ltc_6813.hspi                = hspi;
    ltc_6813.nss_port            = nss_port;
    ltc_6813.nss_pin             = nss_pin;
    ltc_6813.pec15_error_counter = 0U;
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
    // LTC6813 chips
    HAL_GPIO_WritePin(ltc_6813.nss_port, ltc_6813.nss_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(ltc_6813.hspi, tx_cmd, 4U, 100U);

    // Send the tx_write command to all of the LTC6813 chips connected on the
    // daisy chain.
    for (size_t i = 0U; i < NUM_OF_LTC6813; i++)
    {
        HAL_SPI_Transmit(ltc_6813.hspi, tx_payload, 8U, 100U);
    }
    HAL_GPIO_WritePin(ltc_6813.nss_port, ltc_6813.nss_pin, GPIO_PIN_SET);
}

ExitCode Io_LTC6813_ReadAllCellRegisterGroups(void)
{
    uint16_t cell_register_group_cmd;
    uint8_t  tx_cmd[4];
    uint8_t  rx_cell_voltages[NUM_OF_RX_BYTES * NUM_OF_LTC6813] = { 0 };

    // TODO: Do something with the exti codes?
    Io_LTC6813_EnterReadyState();
    Io_LTC6813_StartADCConversion();
    Io_LTC6813_PollAdcConversion();

    for (size_t current_register_group = LTC6813_REGISTER_GROUP_A;
         current_register_group < LTC6813_NUM_OF_CELL_REGISTER_GROUPS;
         current_register_group++)
    {
        cell_register_group_cmd =
            lookup_tables
                .cell_voltage_register_group_commands[current_register_group];

        tx_cmd[0] = (uint8_t)cell_register_group_cmd;
        tx_cmd[1] = (uint8_t)(cell_register_group_cmd >> 8);

        uint16_t tx_cmd_pec15 = Io_CalculatePec15(tx_cmd, 2U);
        tx_cmd[2]             = (uint8_t)(tx_cmd_pec15 >> 8);
        tx_cmd[3]             = (uint8_t)(tx_cmd_pec15);

        Io_SharedSpi_TransmitAndReceive(
            ltc_6813.hspi, ltc_6813.nss_port, ltc_6813.nss_pin, tx_cmd, 4U,
            rx_cell_voltages, NUM_OF_RX_BYTES * NUM_OF_LTC6813);

        for (size_t current_ic = 0U; current_ic < NUM_OF_LTC6813; current_ic++)
        {
            Io_LTC6813_ParseCellsAndPerformPec15Check(
                current_ic, current_register_group, rx_cell_voltages);
        }
    }

    // Reset the value of the PEC15 error counter for the next cycle.
    ltc_6813.pec15_error_counter = 0U;

    // Return EXIT_CODE_UNIMPLEMENTED if PEC15 errors occur, else return
    // EXIT_CODE_OK.
    return (ltc_6813.pec15_error_counter > 0U) ? EXIT_CODE_UNIMPLEMENTED
                                               : EXIT_CODE_OK;
}

uint16_t *Io_LTC6813_GetCellVoltages(void)
{
    return &ltc_6813.cell_voltages[0][0];
}
