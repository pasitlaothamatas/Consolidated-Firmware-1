#pragma once

#include <stdint.h>
#include "App_SharedExitCode.h"

// TODO: Include this here for now
#include "Io_LTC6813.h"

struct CellMonitoring;

struct CellMonitoring *App_CellMonitoring_Create(
    bool (*is_awake)(void),
    void (*start_wakeup)(void),
    void (*end_wakeup)(void),
    void (*configure_ic)(void),
    ExitCode (*start_adc_conversion)(void),
    PEC15Codes (*read_register_groups)(void),
    uint16_t *(*get_cell_voltages)(void));

bool App_CellMonitoring_IsAwake(const struct CellMonitoring *cell_monitoring);

void App_CellMonitoring_StartWakeUp(
    const struct CellMonitoring *cell_monitoring);

void App_CellMonitoring_EndWakeUp(const struct CellMonitoring *cell_monitoring);
