#pragma once

#include <stdint.h>
#include "App_SharedExitCode.h"

struct TractiveSystem;

enum TSExitCode
{
    TS_BUS_VOLTAGE_IN_RANGE,
    TS_BUS_VOLTAGE_OVERFLOW_ERROR,
    TS_BUS_VOLTAGE_UNDERFLOW_ERROR,
    TS_PRECHARGE_SUCCESS,
};

struct TractiveSystem *App_TractiveSystem_Create(
    float (*get_ts_adc_voltage)(void),
    ExitCode (*get_ts_voltage)(float, float *),
    float    expected_accumulator_voltage,
    uint32_t precharge_rc_ms);

/**
 * Deallocate the memory used by the given tractive system
 * @param ts The given tractive system to deallocate
 */
void App_TractiveSystem_Destroy(struct TractiveSystem *ts);

/**
 * Get the voltage for the given tractive system
 * @param ts The given tractive system
 * @param ts_voltage
 * @return EXIT_CODE_OK
 */
ExitCode App_TractiveSystem_GetTsVoltage(
    const struct TractiveSystem *ts,
    float *                      ts_voltage);

enum TSExitCode App_TractiveSystem_CheckBusVoltage(
    struct TractiveSystem *ts,
    uint32_t               current_time_ms);
