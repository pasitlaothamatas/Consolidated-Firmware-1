#include "App_PrechargeSignals.h"
#include "App_PrechargeState.h"

bool App_PrechargeSignals_IsWaitingAfterBoot(struct BmsWorld *world)
{
    struct PreChargeStateMachine *state_machine =
        App_BmsWorld_GetPrechargeStateMachine(world);

    return App_PreChargeStateMachine_GetCurrentState(state_machine) ==
           App_PreChargeState_GetInitState();
}

void App_PrechargeSignals_WaitingAfterBootCompleteCallback(
    struct BmsWorld *world)
{
    struct PreChargeStateMachine *state_machine =
        App_BmsWorld_GetPrechargeStateMachine(world);

    App_PreChargeStateMachine_SetNextState(
        state_machine, App_PreChargeState_GetAIRState());
}
