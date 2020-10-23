#pragma once

#include "Io_SharedSpi.h"
#include "App_SharedExitCode.h"

#define STATIC_DEFINE_IO_LTC6813_PACK_TX_CMD(tx_cmd, packed_tx_cmd)                      \
    (packed_tx_cmd)[0] = (uint8_t)((tx_cmd) >> 8);                      \
    (packed_tx_cmd)[1] = (uint8_t)((tx_cmd));                           \
    uint16_t tx_cmd_pec15 =                                             \
        Io_CalculatePec15((packed_tx_cmd), NUM_OF_PEC15_BYTES_PER_CMD); \
    (packed_tx_cmd)[2] = (uint8_t)(tx_cmd_pec15 >> 8);                  \
    (packed_tx_cmd)[3] = (uint8_t)tx_cmd_pec15;

enum CellVoltageRegisterGroups
{
    CELL_VOLTAGE_REGISTER_GROUP_A,
    CELL_VOLTAGE_REGISTER_GROUP_B,
    CELL_VOLTAGE_REGISTER_GROUP_C,
    CELL_VOLTAGE_REGISTER_GROUP_D,
    CELL_VOLTAGE_REGISTER_GROUP_E,
    CELL_VOLTAGE_REGISTER_GROUP_F,
    NUM_OF_CELL_VOLTAGE_REGISTER_GROUPS
};

/**
 * Calculate the 15-bit packet error code (PEC15) for the given data buffer.
 * @param data_buffer A pointer to the buffer containing data used to calculate
 * the PEC15 code.
 * @param size The number of data elements used to calculate the PEC15 code
 * (Note: The size can be a positive integer less than or equal to the size
 * of the data buffer).
 * @return The calculated PEC15 code
 */
uint16_t Io_CalculatePec15(uint8_t *data_buffer, uint32_t size);

/**
 * Transition all LTC6813 chips on the daisy chain from the IDLE state to the
 * READY state.
 * @param spi_interface The given SPI interface.
 * @return EXIT_CODE_OK if the SCK and NSS pin can be toggled without timing out
 * or errors. Else, EXIT_CODE_UNIMPLEMENTED.
 */
ExitCode Io_LTC6813_EnterReadyState(const struct SharedSpi *spi_interface);

/**
 * Send a command to the LTC6813 chips on the daisy chain.
 * @param tx_cmd The command to be sent to the LTC6813 chips on the daisy chain.
 * @return EXIT_CODE_OK if tx_cmd was transmitted successfully. Else,
 * EXIT_CODE_TIMEOUT.
 */
ExitCode Io_LTC6813_SendCommand(
    uint32_t                tx_cmd,
    const struct SharedSpi *spi_interface);

/**
 * Check if all LTC6813 chips in the daisy chain have all completed ADC
 * conversions.
 * @param spi_interface The given spi interface.
 * @return EXIT_CODE_OK if all LTC6813 chips on the daisy chain have completed
 * ADC conversions. EXIT_CODE_TIMEOUT if ADC conversions were unfinished
 * before timing out. EXIT_CODE_UNIMPLEMENTED if the commands sent to the
 * LTC6813 daisy chain were not successfully transmitted or received.
 */
ExitCode Io_LTC6813_PollAdcConversion(void);
