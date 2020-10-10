#pragma once

#include <stdint.h>
#include "App_SharedExitCode.h"

struct CellVoltages;

struct CellVoltages *App_ReadCells_Create(
    void (*configure_daisy_chain)(void),
    ExitCode (*calculate_cell_voltages)(void));

void App_ReadCells_Configure(const struct CellVoltages *cell_voltages);

ExitCode App_ReadCells_ReadCellVoltages(
    const struct CellVoltages *const cell_voltages);

/**
 * Get the pointer to the the given segment of the accumulator containing cell
 * voltages read from the cell monitoring daisy chain.
 * @param cell_voltages The given cell monitoring daisy chain.
 * @param segment_index The segment.
 * @return
 */
uint16_t *App_ReadCells_GetAverageSegmentCellVoltages(
    struct CellVoltages *cell_voltages,
    size_t               segment_index);
