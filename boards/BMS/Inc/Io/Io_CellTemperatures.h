#pragma once

#include "App_SharedExitCode.h"

/**
 * Read internal die temperatures from cell monitoring chips.
 * @return EXIT_CODE_OK if internal die temperatures can be read successfully.
 * Else, EXIT_CODE_ERROR if a PEC15 error was detected.
 */
ExitCode Io_CellTemperatures_ReadInternalDieTemperatures(void);

/**
 * Get the array containing internal die temperatures for all cell monitoring
 * chips.
 * @note The array is a 1D array containing NUM_OF_LTC6813 elements.
 * @return A pointer to a 1D array containing internal die temperatures for all
 * cell monitoring chips.
 */
uint16_t *Io_CellTemperatures_GetInternalDieTemperatures(void);
