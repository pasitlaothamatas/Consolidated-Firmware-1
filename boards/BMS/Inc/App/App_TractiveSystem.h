#pragma once

#include <stdint.h>
#include "App_SharedExitCode.h"

struct TractiveSystem;

enum TSExitCode
{
    TS_BUS_VOLTAGE_IN_RANGE,
    TS_BUS_OVERVOLTAGE_ERROR,
    TS_BUS_UNDERVOLTAGE_ERROR,
    TS_PRECHARGE_SUCCESS,
};

/**
 *
 * @param get_ts_adc_voltage
 * @param get_ts_voltage
 * @param enable_precharge
 * @param disable_precharge
 * @param accumulator_voltage
 * @param precharge_rc_ms
 * @return
 */
struct TractiveSystem *App_TractiveSystem_Create(
    float (*get_ts_adc_voltage)(void),
    ExitCode (*get_ts_voltage)(float, float *),
    void (*enable_precharge)(void),
    void (*disable_precharge)(void),
    float    accumulator_voltage,
    uint32_t precharge_rc_ms);

/**
 * Deallocate the memory used by the given tractive system
 * @param ts The given tractive system to deallocate
 */
void App_TractiveSystem_Destroy(struct TractiveSystem *ts);

/**
 *
 * @param ts
 * @param ts_exit_code
 * @param current_time_ms
 * @return
 */
void App_TractiveSystem_CheckPrechargeBusVoltage(
    struct TractiveSystem *const ts,
    enum TSExitCode *            ts_exit_code,
    uint32_t                     current_time_ms);

/**
 *
 * @param ts
 */
void App_TractiveSystem_EnablePrecharge(const struct TractiveSystem *ts);

/**
 *
 * @param ts
 */
void App_TractiveSystem_DisablePrecharge(const struct TractiveSystem *ts);
