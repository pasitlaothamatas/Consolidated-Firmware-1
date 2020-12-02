#include "App_PrechargeSignals.h"
#include "App_PrechargeState.h"

bool App_PrechargeSignals_IsWaitingAfterBoot(struct BmsWorld *world)
{
    struct PrechargeStateMachine *state_machine =
        App_BmsWorld_GetPrechargeStateMachine(world);
    return App_PrechargeStateMachine_GetCurrentState(state_machine) ==
           App_PrechargeState_GetInitState();
}

void App_PrechargeSignals_WaitingAfterBootCompleteCallback(struct BmsWorld *world)
{
    struct PrechargeStateMachine *state_machine =
        App_BmsWorld_GetPrechargeStateMachine(world);

    App_PrechargeStateMachine_SetNextState(
        state_machine, App_PrechargeState_GetAIRState());
}
