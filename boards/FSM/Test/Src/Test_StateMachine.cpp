#include "Test_Fsm.h"

extern "C"
{
#include "App_SharedStateMachine.h"
#include "states/App_AirOpenState.h"
#include "states/App_AirClosedState.h"
#include "App_SharedHeartbeatMonitor.h"
#include "App_FlowMeter.h"
}

FAKE_VOID_FUNC(
    send_non_periodic_msg_FSM_STARTUP,
    struct CanMsgs_fsm_startup_t *);
FAKE_VOID_FUNC(
    send_non_periodic_msg_FSM_AIR_SHUTDOWN,
    struct CanMsgs_fsm_air_shutdown_t *);
FAKE_VOID_FUNC(
    send_non_periodic_msg_FSM_WATCHDOG_TIMEOUT,
    struct CanMsgs_fsm_watchdog_timeout_t *);
FAKE_VALUE_FUNC(uint32_t, get_current_ms);
FAKE_VOID_FUNC(
    heartbeat_timeout_callback,
    enum HeartbeatOneHot,
    enum HeartbeatOneHot);
FAKE_VALUE_FUNC(float, get_primary_flow_rate);
FAKE_VALUE_FUNC(float, get_secondary_flow_rate);
FAKE_VALUE_FUNC(float, get_left_wheel_speed);
FAKE_VALUE_FUNC(float, get_right_wheel_speed);

class FsmStateMachineTest : public testing::Test
{
  protected:
    void SetUp() override
    {
        constexpr uint32_t DEFAULT_HEARTBEAT_TIMEOUT_PERIOD_MS = 500U;
        constexpr enum HeartbeatOneHot DEFAULT_HEARTBEAT_BOARDS_TO_CHECK =
            BMS_HEARTBEAT_ONE_HOT;

        can_tx_interface = App_CanTx_Create(
            send_non_periodic_msg_FSM_STARTUP,
            send_non_periodic_msg_FSM_WATCHDOG_TIMEOUT,
            send_non_periodic_msg_FSM_AIR_SHUTDOWN);
        can_rx_interface  = App_CanRx_Create();
        heartbeat_monitor = App_SharedHeartbeatMonitor_Create(
            get_current_ms, DEFAULT_HEARTBEAT_TIMEOUT_PERIOD_MS,
            DEFAULT_HEARTBEAT_BOARDS_TO_CHECK, heartbeat_timeout_callback);
        primary_flow_meter   = App_FlowMeter_Create(get_primary_flow_rate);
        secondary_flow_meter = App_FlowMeter_Create(get_secondary_flow_rate);
        left_wheel_speed_sensor =
            App_WheelSpeedSensor_Create(get_left_wheel_speed);
        right_wheel_speed_sensor =
            App_WheelSpeedSensor_Create(get_right_wheel_speed);

        world = App_FsmWorld_Create(
            can_tx_interface, can_rx_interface, heartbeat_monitor,
            primary_flow_meter, secondary_flow_meter, left_wheel_speed_sensor,
            right_wheel_speed_sensor);

        // Default to starting the state machine in the `AIR_OPEN` state
        state_machine =
            App_SharedStateMachine_Create(world, App_GetAirOpenState());

        // Reset fake functions
        RESET_FAKE(send_non_periodic_msg_FSM_STARTUP);
        RESET_FAKE(send_non_periodic_msg_FSM_AIR_SHUTDOWN);
        RESET_FAKE(send_non_periodic_msg_FSM_WATCHDOG_TIMEOUT);
        RESET_FAKE(get_current_ms);
        RESET_FAKE(heartbeat_timeout_callback);
        RESET_FAKE(get_primary_flow_rate);
        RESET_FAKE(get_secondary_flow_rate);
        RESET_FAKE(get_left_wheel_speed);
        RESET_FAKE(get_right_wheel_speed);
    }

    void TearDown() override
    {
        ASSERT_TRUE(world != NULL);
        ASSERT_TRUE(can_tx_interface != NULL);
        ASSERT_TRUE(can_rx_interface != NULL);
        ASSERT_TRUE(state_machine != NULL);
        ASSERT_TRUE(heartbeat_monitor != NULL);
        ASSERT_TRUE(primary_flow_meter != NULL);
        ASSERT_TRUE(secondary_flow_meter != NULL);
        ASSERT_TRUE(left_wheel_speed_sensor != NULL);
        ASSERT_TRUE(right_wheel_speed_sensor != NULL);

        App_FsmWorld_Destroy(world);
        App_CanTx_Destroy(can_tx_interface);
        App_CanRx_Destroy(can_rx_interface);
        App_SharedStateMachine_Destroy(state_machine);
        App_SharedHeartbeatMonitor_Destroy(heartbeat_monitor);
        App_FlowMeter_Destroy(primary_flow_meter);
        App_FlowMeter_Destroy(secondary_flow_meter);
        App_WheelSpeedSensor_Destroy(left_wheel_speed_sensor);
        App_WheelSpeedSensor_Destroy(right_wheel_speed_sensor);

        world                    = NULL;
        can_tx_interface         = NULL;
        can_rx_interface         = NULL;
        state_machine            = NULL;
        heartbeat_monitor        = NULL;
        primary_flow_meter       = NULL;
        secondary_flow_meter     = NULL;
        left_wheel_speed_sensor  = NULL;
        right_wheel_speed_sensor = NULL;
    }

    void SetInitialState(const struct State *const initial_state)
    {
        App_SharedStateMachine_Destroy(state_machine);
        state_machine = App_SharedStateMachine_Create(world, initial_state);
        ASSERT_TRUE(state_machine != NULL);
        ASSERT_EQ(
            initial_state,
            App_SharedStateMachine_GetCurrentState(state_machine));
    }

    std::vector<const struct State *> GetAllStates(void)
    {
        return std::vector<const struct State *>{ App_GetAirOpenState(),
                                                  App_GetAirClosedState() };
    }

    struct World *            world;
    struct FsmCanTxInterface *can_tx_interface;
    struct FsmCanRxInterface *can_rx_interface;
    struct StateMachine *     state_machine;
    struct HeartbeatMonitor * heartbeat_monitor;
    struct FlowMeter *        primary_flow_meter;
    struct FlowMeter *        secondary_flow_meter;
    struct WheelSpeedSensor * left_wheel_speed_sensor;
    struct WheelSpeedSensor * right_wheel_speed_sensor;
};

TEST_F(FsmStateMachineTest, check_air_open_state_is_broadcasted_over_can)
{
    SetInitialState(App_GetAirOpenState());

    EXPECT_EQ(
        CANMSGS_FSM_STATE_MACHINE_STATE_AIR_OPEN_CHOICE,
        App_CanTx_GetPeriodicSignal_STATE(can_tx_interface));
}

TEST_F(FsmStateMachineTest, check_air_closed_state_is_broadcasted_over_can)
{
    SetInitialState(App_GetAirClosedState());

    EXPECT_EQ(
        CANMSGS_FSM_STATE_MACHINE_STATE_AIR_CLOSED_CHOICE,
        App_CanTx_GetPeriodicSignal_STATE(can_tx_interface));
}

TEST_F(
    FsmStateMachineTest,
    check_if_primary_and_secondary_flow_rates_are_broadcasted_over_can_in_all_states)
{
    float fake_flow_rate = 1.0f;

    for (const auto &state : GetAllStates())
    {
        SetInitialState(state);
        get_primary_flow_rate_fake.return_val   = fake_flow_rate;
        get_secondary_flow_rate_fake.return_val = fake_flow_rate;
        App_SharedStateMachine_Tick(state_machine);

        EXPECT_EQ(
            fake_flow_rate,
            App_CanTx_GetPeriodicSignal_PRIMARY_FLOW_RATE(can_tx_interface));
        EXPECT_EQ(
            fake_flow_rate,
            App_CanTx_GetPeriodicSignal_SECONDARY_FLOW_RATE(can_tx_interface));

        // To avoid false positives, we use a different wheel speed each time
        fake_flow_rate++;
    }
}

TEST_F(
    FsmStateMachineTest,
    check_if_left_wheel_speed_and_right_wheel_speeds_are_broadcasted_over_can_in_all_states)
{
    float fake_wheel_speed = 1.0f;

    for (const auto &state : GetAllStates())
    {
        SetInitialState(state);
        get_left_wheel_speed_fake.return_val  = fake_wheel_speed;
        get_right_wheel_speed_fake.return_val = fake_wheel_speed;
        App_SharedStateMachine_Tick(state_machine);

        EXPECT_EQ(
            fake_wheel_speed,
            App_CanTx_GetPeriodicSignal_LEFT_WHEEL_SPEED(can_tx_interface));
        EXPECT_EQ(
            fake_wheel_speed,
            App_CanTx_GetPeriodicSignal_RIGHT_WHEEL_SPEED(can_tx_interface));

        // To avoid false positives, we use a different wheel speed each time
        fake_wheel_speed++;
    }
}
