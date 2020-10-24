#pragma once

#include <stm32f3xx_hal.h>
#include "App_SharedExitCode.h"
#include "configs/Io_LTC6813Configs.h"

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
 * Initialize all chips on the LTC6813 daisy chain.
 * @param spi_handle The given SPI handle for the LTC6813 daisy chain.
 * @param nss_port The given GPIO port of the NSS pin.
 * @param nss_pin The given GPIO NSS pin.
 */
void Io_LTC6813_Init(
    SPI_HandleTypeDef *spi_handle,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin);

/**
 * Get the SPI interface for the LTC6813 daisy chain.
 * @return The SPI interface for the LTC6813 daisy chian.
 */
struct SharedSpi *Io_LTC6813_GetSpiInterface(void);

/**
 * Calculate the 15-bit packet error code (PEC15) for the given data buffer.
 * @param data_buffer A pointer to the buffer containing data used to calculate
 * the PEC15 code.
 * @param size The number of data elements used to calculate the PEC15 code
 * (Note: The size can be a positive integer less than or equal to the size
 * of the data buffer).
 * @return The calculated PEC15 code.
 */
uint16_t Io_CalculatePec15(uint8_t *data_buffer, uint32_t size);

/**
 * Transition all LTC6813 chips on the daisy chain from the IDLE state to the
 * READY state.
 * @return EXIT_CODE_OK if the SCK and NSS pin can be toggled successfully.
 * Else, EXIT_CODE_ERROR.
 */
ExitCode Io_LTC6813_EnterReadyState(void);

/**
 * Start ADC conversions for cell voltages on all LTC6813 chips on the daisy
 * chain.
 * @return EXIT_CODE_OK if the command is sent to the LTC6813 daisy chain
 * successfully. Else, EXIT_CODE_ERROR.
 */
ExitCode Io_LTC6813_StartADCConversion(void);

/**
 * Check if all LTC6813 chips in the daisy chain have all completed ADC
 * conversions.
 * @return EXIT_CODE_OK if all LTC6813 chips on the daisy chain have completed
 * ADC conversions. EXIT_CODE_TIMEOUT if ADC conversions timed out before
 * completion. Else, EXIT_CODE_ERROR.
 */
ExitCode Io_LTC6813_PollAdcConversion(void);

/**
 * Configure all chips on the LTC6813 daisy chain.
 * @return EXIT_CODE_OK if all chips on the daisy chain are configured
 * successfully. Else, return EXIT_CODE_ERROR if the data cannot be
 * transmitted to the chips on the LTC6813 daisy chain.
 */
ExitCode Io_LTC6813_ConfigureRegisterA(void);

/**
 * Start ADC conversions for internal die temperatures on all LTC6813 chips on
 * the daisy chain.
 * @return EXIT_CODE_OK if the command is sent to the LTC6813 daisy chain
 * successfully. Else, EXIT_CODE_ERROR.
 */
ExitCode Io_LTC6813_StartInternalDieTemperatureConversion(void);
