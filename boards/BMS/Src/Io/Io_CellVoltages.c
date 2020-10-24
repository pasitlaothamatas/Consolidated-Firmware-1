#include <stdint.h>
#include <assert.h>
#include "Io_SharedSpi.h"
#include "Io_LTC6813.h"
#include "Io_CellVoltages.h"

#define NUM_OF_CELL_VOLTAGE_RX_BYTES 8U
#define NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP 3U

static uint16_t cell_voltages[NUM_OF_LTC6813]
                             [NUM_OF_CELL_VOLTAGE_REGISTER_GROUPS] = { { 0 } };

// clang-format off
static const uint16_t cell_voltage_register_group_commands
    [NUM_OF_CELL_VOLTAGE_REGISTER_GROUPS] =
    {
        0x0400, // RDCVA
        0x0600, // RDCVB
        0x0800, // RDCVC
        0x0A00, // RDCVD
        0x0900, // RDCVE
        0x0B00  // RDCVF
    };
// clang-format on

/**
 *
 * @param current_ic
 * @param current_register_group
 * @param pec15_error_counter
 * @param rx_cell_voltages
 */
static void Io_LTC6813_ParseCellsAndPerformPec15Check(
    size_t    current_ic,
    size_t    current_register_group,
    uint32_t *pec15_error_counter,
    uint8_t
        rx_cell_voltages[static NUM_OF_CELL_VOLTAGE_RX_BYTES * NUM_OF_LTC6813]);

static void Io_LTC6813_ParseCellsAndPerformPec15Check(
    size_t    current_ic,
    size_t    current_register_group,
    uint32_t *pec15_error_counter,
    uint8_t   rx_cell_voltages[])
{
    size_t cell_voltage_index = current_ic * NUM_OF_CELL_VOLTAGE_RX_BYTES;

    for (size_t current_cell = 0U;
         current_cell < NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP; current_cell++)
    {
        assert(
            cell_voltage_index < NUM_OF_CELL_VOLTAGE_RX_BYTES * NUM_OF_LTC6813);
        uint32_t cell_voltage =
            (uint32_t)(rx_cell_voltages[cell_voltage_index]) |
            (uint32_t)((rx_cell_voltages[cell_voltage_index + 1] << 8));

        cell_voltages[current_ic]
                     [current_cell +
                      current_register_group *
                          NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP] =
                         (uint16_t)cell_voltage;

        // Each cell voltage is represented as a pair of bytes. Therefore,
        // the cell voltage index is incremented by 2 to retrieve the next cell
        // voltage.
        cell_voltage_index += 2U;
    }

    assert(cell_voltage_index < NUM_OF_CELL_VOLTAGE_RX_BYTES * NUM_OF_LTC6813);
    uint32_t received_pec15 =
        (uint32_t)(rx_cell_voltages[cell_voltage_index] << 8) |
        (uint32_t)(rx_cell_voltages[cell_voltage_index + 1]);

    // Calculate the PEC15 using the first 6 bytes of data received from the
    // chip.
    uint32_t calculated_pec15 = Io_CalculatePec15(
        &rx_cell_voltages[current_ic * NUM_OF_CELL_VOLTAGE_RX_BYTES], 6U);

    if (received_pec15 != calculated_pec15)
    {
        *pec15_error_counter = *pec15_error_counter + 1;
    }
}

ExitCode Io_LTC6813_ReadCellVoltages(void)
{
    uint16_t cell_register_group_cmd;
    uint8_t  tx_cmd[NUM_OF_CMD_BYTES];
    uint8_t  rx_cell_voltages[NUM_OF_CELL_VOLTAGE_RX_BYTES * NUM_OF_LTC6813] = {
        0
    };

    // Reset the value of the PEC15 error counter at the start of each cycle.
    uint32_t pec15_error_counter = 0U;

    RETURN_IF_EXIT_NOT_OK(Io_LTC6813_EnterReadyState())
    RETURN_IF_EXIT_NOT_OK(Io_LTC6813_StartADCConversion())
    RETURN_IF_EXIT_NOT_OK(Io_LTC6813_PollAdcConversion())

    for (size_t current_register_group = CELL_VOLTAGE_REGISTER_GROUP_A;
         current_register_group < NUM_OF_CELL_VOLTAGE_REGISTER_GROUPS;
         current_register_group++)
    {
        cell_register_group_cmd =
            cell_voltage_register_group_commands[current_register_group];

        tx_cmd[0] = (uint8_t)(cell_register_group_cmd >> 8);
        tx_cmd[1] = (uint8_t)(cell_register_group_cmd);
        uint16_t tx_cmd_pec15 =
            Io_CalculatePec15(tx_cmd, NUM_OF_PEC15_BYTES_PER_CMD);
        tx_cmd[2] = (uint8_t)(tx_cmd_pec15 >> 8);
        tx_cmd[3] = (uint8_t)tx_cmd_pec15;

        if (Io_SharedSpi_TransmitAndReceive(
                Io_LTC6813_GetSpiInterface(), tx_cmd, NUM_OF_CMD_BYTES,
                rx_cell_voltages,
                NUM_OF_CELL_VOLTAGE_RX_BYTES * NUM_OF_LTC6813) != HAL_OK)
        {
            return EXIT_CODE_TIMEOUT;
        }

        for (size_t current_ic = 0U; current_ic < NUM_OF_LTC6813; current_ic++)
        {
            Io_LTC6813_ParseCellsAndPerformPec15Check(
                current_ic, current_register_group, &pec15_error_counter,
                rx_cell_voltages);
        }
    }

    // Return EXIT_CODE_ERROR if PEC15 mismatches occur, else return
    // EXIT_CODE_OK.
    return (pec15_error_counter > 0U) ? EXIT_CODE_ERROR : EXIT_CODE_OK;
}

uint16_t *Io_LTC6813_GetCellVoltages(void)
{
    return &cell_voltages[0][0];
}
