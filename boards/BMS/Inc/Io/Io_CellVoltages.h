#pragma once

#include "App_SharedExitCode.h"

/**
 * Read all cell voltages for each chip on the LTC6813 daisy chain.
 * @return If data cannot be transmitted to or received from the chips on the
 * LTC6813 daisy chain, return EXIT_CODE_TIMEOUT. Else if PEC15 mismatches
 * are detected, return EXIT_CODE_ERROR. Else, return EXIT_CODE_OK.
 */
ExitCode Io_CellVoltages_ReadCellVoltages(void);

/**
 * Get the pointer to the 2D array containing converted cell voltages for all
 * cell monitoring chips.
 *
 * @note The 2D array has its first subscript value as NUM_OF_LTC6813 (the total
 * number of cell monitoring chips connected on the daisy chain), and its second
 * subscript value as NUM_OF_CELLS_PER_LTC6813 (the number of cell voltages that
 * can be monitored per LTC6813 chip).
 * Each cell voltage read back from the cell monitoring chips is a 16-bit
 * unsigned integer where the LSB represents 100µV.
 *
 * @return A pointer to the 2D array containing converted chip cell
 * voltages.
 */
uint16_t *Io_CellVoltages_GetCellVoltages(void);
