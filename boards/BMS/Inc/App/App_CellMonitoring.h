#pragma once

#include <stdint.h>
#include "App_SharedExitCode.h"

struct CellVoltages;

struct CellVoltages *App_CellVoltages_Create(
    void (*configure_daisy_chain)(void),
    ExitCode (*calculate_cell_voltages)(void),
    uint16_t *(*get_cell_voltages)(void),
    uint32_t num_of_daisy_chained_devices);

void App_CellVoltages_Configure(struct CellVoltages *cell_voltages);

ExitCode
    App_CellVoltages_CalculateCellVoltages(struct CellVoltages *cell_voltages);

/**
 * Get the pointer to the the given segment of the accumulator containing cell
 * voltages read from the cell monitoring daisy chain.
 * @param cell_voltages The given cell monitoring daisy chain.
 * @param segment_index The segment.
 * @return
 */
uint16_t *App_CellVoltages_GetAverageSegmentCellVoltages(
    struct CellVoltages *cell_voltages,
    size_t               segment_index);
