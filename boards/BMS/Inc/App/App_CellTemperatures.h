#pragma once

/**
 * Initialize a set of functions used to calculate temperatures pertaining to
 * the accumulator and the cell monitoring chips.
 * @param get_internal_die_temperatures A function used to get an array
 * containing internal die temperatures for the cell monitoring ICs.
 */
void App_CellTemperatures_Init(
    uint16_t *(*get_internal_die_temperatures)(void));

/**
 * Get the internal die temperature (°C) of the 0th cell monitoring chip.
 * @return The internal die temperature (°C) of the 0th cell monitoring chip.
 */
float App_CellTemperatures_GetInternalDieTemperature0(void);

/**
 * Get the internal die temperature (°C) of the 1st cell monitoring chip.
 * @return The internal die temperature (°C) of the 1st cell monitoring chip.
 */
float App_CellTemperatures_GetInternalDieTemperature1(void);
