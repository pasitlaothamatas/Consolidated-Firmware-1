#pragma once

#include <stdint.h>
#include "App_SharedExitCode.h"

struct CellVoltages;

struct CellVoltages *App_CellVoltages_Create(
    void (*configure)(void),
    ExitCode (*calculate_cell_voltages)(void),
    uint16_t *(*get_cell_voltages)(void));

void App_CellVoltages_Configure(struct CellVoltages *cell_voltages);

ExitCode
    App_CellVoltages_CalculateCellVoltages(struct CellVoltages *cell_voltages);

uint16_t *App_CellVoltages_GetCellVoltages(
    struct CellVoltages *cell_voltages,
    size_t               ic);
