#include <assert.h>
#include <stdlib.h>
#include "App_PrechargeStateMachine.h"
#include "App_BmsWorld.h"

#define MAX_NAME_LENGTH 16U

struct PrechargeState
{
    char name[MAX_NAME_LENGTH];
    void (*run_on_tick)(struct BmsWorld *);
};

struct PrechargeStateMachine
{
    struct PrechargeState *current_state;
    struct PrechargeState *next_state;
};

struct PrechargeStateMachine *App_PrechargeStateMachine_Create(void)
{
    struct PrechargeStateMachine *state_machine =
        malloc(sizeof(struct PrechargeStateMachine));
    assert(state_machine != NULL);

    state_machine->current_state = App_PrechargeStateMachine_GetWaitBootState();
    state_machine->next_state    = state_machine->current_state;

    return state_machine;
}

void App_PrechargeStateMachine_SetNextState(
    struct BmsWorld *      world,
    struct PrechargeState *next_state)
{
    struct PrechargeStateMachine *state_machine =
        App_BmsWorld_GetPrechargeStateMachine(world);
    state_machine->next_state = next_state;
}

struct PrechargeState *
    App_PrechargeStateMachine_GetCurrentState(struct BmsWorld *const world)
{
    struct PrechargeStateMachine *pre_charge_state_machine =
        App_BmsWorld_GetPrechargeStateMachine(world);
    return pre_charge_state_machine->current_state;
}

void App_PrechargeStateMachine_Tick(struct BmsWorld *world)
{
    struct PrechargeStateMachine *state_machine =
        App_BmsWorld_GetPrechargeStateMachine(world);

    if (state_machine->current_state != state_machine->next_state)
    {
        state_machine->current_state = state_machine->next_state;
        state_machine->current_state->run_on_tick(world);
    }
}

static void App_PrechargeStateMachine_RunOnTickWaitBootState(
    struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx, CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_BOOT_CHOICE);
}

static void App_PrechargeStateMachine_RunOnTickWaitAIRState(
    struct BmsWorld *const world)
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

static void App_PrechargeStateMachine_RunOnTickPrechargingState(
    struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    struct TractiveSystem *   tractive_system =
        App_BmsWorld_GetTractiveSystem(world);
    struct Clock *clock = App_BmsWorld_GetClock(world);

    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx, CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_PRECHARGE_CHOICE);

    enum TSExitCode exit_code = App_TractiveSystem_CheckBusVoltage(
        tractive_system, App_SharedClock_GetCurrentTimeInMilliseconds(clock));

    if (exit_code == TS_PRECHARGE_SUCCESS)
    {
        App_PrechargeStateMachine_SetNextState(
            world, App_PrechargeStateMachine_GetPrechargeOkState());
    }
    else if (
        exit_code == TS_BUS_VOLTAGE_OVERFLOW_ERROR ||
        exit_code == TS_BUS_VOLTAGE_UNDERFLOW_ERROR)
    {
        App_PrechargeStateMachine_SetNextState(
            world, App_PrechargeStateMachine_GetPrechargeFailState());
    }
}

static void App_PrechargeStateMachine_RunOnTickPrechargeOkState(
    struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx,
        CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_PRECHARGE_OK_CHOICE);
}

static void App_PrechargeStateMachine_RunOnTickPrechargeFailState(
    struct BmsWorld *const world)
{
    struct BmsCanTxInterface *can_tx = App_BmsWorld_GetCanTx(world);
    App_CanTx_SetPeriodicSignal_PRECHARGE_STATE(
        can_tx,
        CANMSGS_BMS_PRECHARGE_STATES_PRECHARGE_STATE_PRECHARGE_FAIL_CHOICE);
    // TODO: Set AIR shutdown CAN tx signal
}

struct PrechargeState *App_PrechargeStateMachine_GetWaitBootState(void)
{
    static struct PrechargeState state = {
        .name        = "WAIT_BOOT",
        .run_on_tick = App_PrechargeStateMachine_RunOnTickWaitBootState
    };

    return &state;
}

struct PrechargeState *App_PrechargeStateMachine_GetWaitAIRState(void)
{
    static struct PrechargeState state = {
        .name        = "WAIT_AIR",
        .run_on_tick = App_PrechargeStateMachine_RunOnTickWaitAIRState
    };

    return &state;
}

struct PrechargeState *App_PrechargeStateMachine_GetPrechargingState(void)
{
    static struct PrechargeState state = {
        .name        = "PRECHARGING",
        .run_on_tick = App_PrechargeStateMachine_RunOnTickPrechargingState
    };

    return &state;
}

struct PrechargeState *App_PrechargeStateMachine_GetPrechargeOkState(void)
{
    static struct PrechargeState state = {
        .name        = "PRECHARGE_OK",
        .run_on_tick = App_PrechargeStateMachine_RunOnTickPrechargeOkState
    };

    return &state;
}

struct PrechargeState *App_PrechargeStateMachine_GetPrechargeFailState(void)
{
    static struct PrechargeState state = {
        .name        = "PRECHARGE_FAIL",
        .run_on_tick = App_PrechargeStateMachine_RunOnTickPrechargeFailState
    };

    return &state;
}
