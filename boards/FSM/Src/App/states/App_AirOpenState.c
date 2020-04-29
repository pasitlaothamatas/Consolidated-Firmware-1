#include "states/App_AirOpenState.h"
#include "App_SetPeriodicCanSignals.h"
#include "App_SharedMacros.h"

static void AirOpenStateRunOnEntry(struct StateMachine *const state_machine)
{
    struct FsmWorld *world = App_SharedStateMachine_GetWorld(state_machine);
    struct FsmCanTxInterface *can_tx_interface = App_FsmWorld_GetCanTx(world);
    App_CanTx_SetPeriodicSignal_STATE(
        can_tx_interface, CANMSGS_FSM_STATE_MACHINE_STATE_AIR_OPEN_CHOICE);
}

static void AirOpenStateRunOnTick(struct StateMachine *const state_machine)
{
    struct FsmWorld *world = App_SharedStateMachine_GetWorld(state_machine);
    struct FsmCanTxInterface *can_tx_interface = App_FsmWorld_GetCanTx(world);
    struct FlowMeter *        primary_flow_meter =
        App_FsmWorld_GetPrimaryFlowMeter(world);
    struct FlowMeter *secondary_flow_meter =
        App_FsmWorld_GetSecondaryFlowMeter(world);
    struct WheelSpeedSensor *left_wheel_speed_sensor =
        App_FsmWorld_GetLeftWheelSpeedSensor(world);
    struct WheelSpeedSensor *right_wheel_speed_sensor =
        App_FsmWorld_GetRightWheelSpeedSensor(world);

    App_SetPeriodicCanSignals_PrimaryFlowMeter(
        can_tx_interface, primary_flow_meter);
    App_SetPeriodicCanSignals_SecondaryFlowMeter(
        can_tx_interface, secondary_flow_meter);

    const float left_wheel_speed =
        App_Wheel_GetWheelSpeed(left_wheel_speed_sensor);
    const float right_wheel_speed =
        App_Wheel_GetWheelSpeed(right_wheel_speed_sensor);
    App_CanTx_SetPeriodicSignal_LEFT_WHEEL_SPEED(
        can_tx_interface, left_wheel_speed);
    App_CanTx_SetPeriodicSignal_RIGHT_WHEEL_SPEED(
        can_tx_interface, right_wheel_speed);

    if ((left_wheel_speed > App_Wheel_GetThreshold(left_wheel_speed_sensor)) ||
        (right_wheel_speed > App_Wheel_GetThreshold(right_wheel_speed_sensor)))
    {
        App_CanTx_SetPeriodicSignal_WHEEL_SPEED_NON_CRITICAL_FAULT(
            can_tx_interface, true);
    }
    else
    {
        App_CanTx_SetPeriodicSignal_WHEEL_SPEED_NON_CRITICAL_FAULT(
            can_tx_interface, false);
    }
}

static void AirOpenStateRunOnExit(struct StateMachine *const state_machine)
{
    UNUSED(state_machine);
}

const struct State *App_GetAirOpenState(void)
{
    static struct State air_open_state = {
        .name             = "AIR OPEN",
        .run_on_entry     = AirOpenStateRunOnEntry,
        .run_on_tick_1kHz = AirOpenStateRunOnTick,
        .run_on_tick_1Hz  = NULL,
        .run_on_exit      = AirOpenStateRunOnExit,
    };

    return &air_open_state;
}
