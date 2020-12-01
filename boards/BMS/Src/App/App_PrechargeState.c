#include "App_PrechargeState.h"

#define MAX_NAME_LENGTH 16U

struct PrechargeState
{
    char name[MAX_NAME_LENGTH];
    void (*run_on_tick)(struct BmsWorld *);
};

static void
    App_PrechargeState_RunOnTickWaitBootState(struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx, CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_BOOT_CHOICE);

    App_CanTx_SetPeriodicSignal_PRECHARGE_HAS_FAILED(
        can_tx,
        CANMSGS_BMS_AIR_SHUTDOWN_ERRORS_PRECHARGE_HAS_FAILED_FALSE_CHOICE);
}

static void
    App_PrechargeState_RunOnTickWaitAIRState(struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx       = App_BmsWorld_GetCanTx(world);
    struct BinaryStatus *     air_negative = App_BmsWorld_GetAirNegative(world);
    struct TractiveSystem *   ts = App_BmsWorld_GetTractiveSystem(world);

    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx, CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_AIR_CHOICE);

    if (App_SharedBinaryStatus_IsActive(air_negative))
    {
        App_PrechargeStateMachine_SetNextState(
            world, App_PrechargeStateMachine_GetPrechargeState());

        App_TractiveSystem_EnablePrecharge(ts);
    }
}

static void
    App_PrechargeState_RunOnTickPrechargingState(struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    struct TractiveSystem *   tractive_system =
        App_BmsWorld_GetTractiveSystem(world);
    struct Clock *clock = App_BmsWorld_GetClock(world);

    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx, CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_PRECHARGE_CHOICE);

    enum TSExitCode exit_code = App_TractiveSystem_CheckBusVoltage(
        tractive_system, App_SharedClock_GetCurrentTimeInMilliseconds(clock));

    if (exit_code != TS_BUS_VOLTAGE_IN_RANGE)
    {
        // Disable precharge if precharge errors have been detected or if the
        // precharge was successful
        App_TractiveSystem_DisablePrecharge(tractive_system);
    }

    if (exit_code == TS_PRECHARGE_SUCCESS)
    {
        App_PrechargeStateMachine_SetNextState(
            world, App_PrechargeState_GetPrechargeOkState());
    }
    else if (
        exit_code == TS_BUS_VOLTAGE_OVERFLOW_ERROR ||
        exit_code == TS_BUS_VOLTAGE_UNDERFLOW_ERROR)
    {
        App_PrechargeStateMachine_SetNextState(
            world, App_PrechargeState_GetPrechargeFailState());
    }
}

static void
    App_PrechargeState_RunOnTickPrechargeOkState(struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx,
        CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_PRECHARGE_OK_CHOICE);
}

static void
    App_PrechargeState_RunOnTickPrechargeFailState(struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx,
        CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_PRECHARGE_FAIL_CHOICE);
    App_CanTx_SetPeriodicSignal_PRECHARGE_HAS_FAILED(
        can_tx,
        CANMSGS_BMS_AIR_SHUTDOWN_ERRORS_PRECHARGE_HAS_FAILED_TRUE_CHOICE);
}

struct PrechargeState *App_PrechargeState_GetWaitBootState(void)
{
    static struct PrechargeState state = {
        .name        = "WAIT_BOOT",
        .run_on_tick = App_PrechargeState_RunOnTickWaitBootState
    };

    return &state;
}

struct PrechargeState *App_PrechargeState_GetWaitAIRState(void)
{
    static struct PrechargeState state = {
        .name        = "WAIT_AIR",
        .run_on_tick = App_PrechargeState_RunOnTickWaitAIRState
    };

    return &state;
}

struct PrechargeState *App_PrechargeState_GetPrechargingState(void)
{
    static struct PrechargeState state = {
        .name        = "PRECHARGING",
        .run_on_tick = App_PrechargeState_RunOnTickPrechargingState
    };

    return &state;
}

struct PrechargeState *App_PrechargeState_GetPrechargeOkState(void)
{
    static struct PrechargeState state = {
        .name        = "PRECHARGE_OK",
        .run_on_tick = App_PrechargeState_RunOnTickPrechargeOkState
    };

    return &state;
}

struct PrechargeState *App_PrechargeState_GetPrechargeFailState(void)
{
    static struct PrechargeState state = {
        .name        = "PRECHARGE_FAIL",
        .run_on_tick = App_PrechargeState_RunOnTickPrechargeFailState
    };

    return &state;
}

void App_PrechargeState_RunOnTick(
    struct PrechargeState *state,
    struct BmsWorld *const world)
{
    state->run_on_tick(world);
}
