#pragma once

#include <stdint.h>
#include "App_SharedExitCode.h"

// TODO: Include this here for now
#include "Io_LTC6813.h"

struct CellMonitoring;

struct CellMonitoring *App_CellMonitoring_Create(
    void (*configure_ic)(void),
    ExitCode (*start_adc_conversion)(void),
    PEC15Codes (*read_register_groups)(void),
    uint16_t *(*get_cell_voltages)(void));
