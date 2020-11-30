#include <assert.h>
#include <stdlib.h>
#include "App_TractiveSystem.h"

#define NUM_OF_RC_TIME_CONSTANTS 5U

struct TractiveSystem
{
    float (*get_ts_adc_voltage)(void);
    ExitCode (*get_ts_voltage)(float, float *);

    // The expected accumulator voltage
    float expected_accumulator_voltage;

    // The RC time constant for the motor controller and charger capacitors
    uint32_t precharge_rc_ms;

    // The index used to track the number of time increments in RC
    size_t   precharge_rc_index;
    uint32_t precharge_curr_time_ms;
    uint32_t precharge_prev_time_ms;
};

struct TractiveSystem *App_TractiveSystem_Create(
    float (*get_ts_adc_voltage)(void),
    ExitCode (*get_ts_voltage)(float, float *),
    float    expected_accumulator_voltage,
    uint32_t precharge_rc_ms)
{
    struct TractiveSystem *ts = malloc(sizeof(struct TractiveSystem));
    assert(ts != NULL);

    ts->get_ts_adc_voltage           = get_ts_adc_voltage;
    ts->get_ts_voltage               = get_ts_voltage;
    ts->expected_accumulator_voltage = expected_accumulator_voltage;
    ts->precharge_rc_ms              = precharge_rc_ms;
    ts->precharge_curr_time_ms       = 0U;
    ts->precharge_prev_time_ms       = 0U;
    ts->precharge_rc_index           = 0U;

    return ts;
}

void App_TractiveSystem_Destroy(struct TractiveSystem *ts)
{
    free(ts);
}

ExitCode App_TractiveSystem_GetTsVoltage(
    const struct TractiveSystem *const ts,
    float *                            ts_voltage)
{
    return ts->get_ts_voltage(ts->get_ts_adc_voltage(), ts_voltage);
}

enum TSExitCode App_TractiveSystem_CheckBusVoltage(
    struct TractiveSystem *ts,
    uint32_t               current_time_ms)
{
    // The lower bound for the voltage expected at RC, 2RC, ... , 5RC expressed
    // as a percentage of the expected voltage
    const float min_cap_voltage_pct[NUM_OF_RC_TIME_CONSTANTS] = {
        0.5689f, 0.8414f, 0.9417f, 0.9785f, 0.9800f
    };

    // The upper bound for the voltage expected at RC, 2RC, ... , 5RC expressed
    // as a percentage of the expected voltage
    const float max_cap_voltage_pct[NUM_OF_RC_TIME_CONSTANTS] = {
        0.6953f, 0.8879f, 0.9587f, 0.9800f, 1.0000f
    };

    enum TSExitCode ts_exit_code = TS_BUS_VOLTAGE_IN_RANGE;

    if (ts->precharge_rc_index == 0U)
    {
        ts->precharge_curr_time_ms = current_time_ms;
        ts->precharge_prev_time_ms = current_time_ms;
    }
    else
    {
        ts->precharge_curr_time_ms = current_time_ms;
    }

    // Check if the voltage is in range for each RC (ms) passed
    if ((ts->precharge_curr_time_ms - ts->precharge_prev_time_ms) >=
        ts->precharge_rc_ms)
    {
        ts->precharge_prev_time_ms = ts->precharge_curr_time_ms;

        const float max_accumulator_voltage = ts->expected_accumulator_voltage;
        const float min_ts_threshold =
            max_accumulator_voltage *
            min_cap_voltage_pct[ts->precharge_rc_index];
        const float max_ts_threshold =
            max_accumulator_voltage *
            max_cap_voltage_pct[++ts->precharge_rc_index];

        float ts_voltage;
        App_TractiveSystem_GetTsVoltage(ts, &ts_voltage);

        if (ts_voltage > min_ts_threshold && ts_voltage < max_ts_threshold)
        {
            if (ts->precharge_rc_index == NUM_OF_RC_TIME_CONSTANTS)
            {
                // If 5 RCs have passed, and the measured tractive system
                // voltage is greater or equal to 98% of the expected
                // accumulator voltage, the precharge is successful
                ts_exit_code = TS_PRECHARGE_SUCCESS;
            }
        }
        else if (ts_voltage > max_ts_threshold)
        {
            // The bus voltage rose faster than expected
            ts_exit_code = TS_BUS_VOLTAGE_OVERFLOW_ERROR;
        }
        else if (ts_voltage < min_ts_threshold)
        {
            // The bus voltage rose slower than expected
            ts_exit_code = TS_BUS_VOLTAGE_UNDERFLOW_ERROR;
        }
    }

    return ts_exit_code;
}
