#include "App_PrechargeState.h"

#define MAX_NAME_LENGTH 16U

struct PrechargeState
{
    char name[MAX_NAME_LENGTH];
    void (*run_on_tick)(struct BmsWorld *);
};

static void App_PrechargeState_RunOnTickInitState(struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx, CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_INIT_CHOICE);
}

static void App_PrechargeState_RunOnTickAIRState(struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx       = App_BmsWorld_GetCanTx(world);
    struct BinaryStatus *     air_negative = App_BmsWorld_GetAirNegative(world);

    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx, CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_AIR_CHOICE);

    if (App_SharedBinaryStatus_IsActive(air_negative))
    {
        App_PrechargeStateMachine_SetNextState(
            world, App_PrechargeStateMachine_GetPrechargeState());
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
        can_tx,
        CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_PRECHARGING_CHOICE);

    if (App_CanTx_GetPeriodicSignal_PRECHARGING_CONDITION(can_tx) ==
        CANMSGS_BMS_AIR_SHUTDOWN_ERRORS_PRECHARGING_CONDITION_PRECHARGING_CHOICE)
    {
        enum TSExitCode exit_code = App_TractiveSystem_CheckBusVoltage(
            tractive_system,
            App_SharedClock_GetCurrentTimeInMilliseconds(clock));

        if (exit_code != TS_BUS_VOLTAGE_IN_RANGE)
        {
            App_TractiveSystem_DisablePrecharge(tractive_system);
        }
        else
        {
            App_TractiveSystem_EnablePrecharge(tractive_system);
        }

        if (exit_code == TS_PRECHARGE_SUCCESS)
        {
            App_CanTx_SetPeriodicSignal_PRECHARGING_CONDITION(
                can_tx,
                CANMSGS_BMS_AIR_SHUTDOWN_ERRORS_PRECHARGING_CONDITION_SUCCESS_CHOICE);
        }
        else if (
            exit_code == TS_BUS_OVERVOLTAGE_ERROR ||
            exit_code == TS_BUS_UNDERVOLTAGE_ERROR)
        {
            App_CanTx_SetPeriodicSignal_PRECHARGING_CONDITION(
                can_tx,
                CANMSGS_BMS_AIR_SHUTDOWN_ERRORS_PRECHARGING_CONDITION_FAIL_CHOICE);
        }
    }
}

struct PrechargeState *App_PrechargeState_GetInitState(void)
{
    static struct PrechargeState state = {
        .name = "INIT", .run_on_tick = App_PrechargeState_RunOnTickInitState
    };

    return &state;
}

struct PrechargeState *App_PrechargeState_GetAIRState(void)
{
    static struct PrechargeState state = {
        .name = "AIR", .run_on_tick = App_PrechargeState_RunOnTickAIRState
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

void App_PrechargeState_RunOnTick(
    struct PrechargeState *state,
    struct BmsWorld *const world)
{
    state->run_on_tick(world);
}
