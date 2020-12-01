#include <assert.h>
#include <stdlib.h>
#include "App_PrechargeStateMachine.h"

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

    state_machine->current_state = App_PrechargeState_GetWaitBootState();
    state_machine->next_state    = state_machine->current_state;

    return state_machine;
}

void App_PrechargeStateMachine_SetNextState(
    struct BmsWorld *const world,
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

void App_PrechargeStateMachine_Tick(struct BmsWorld *const world)
{
    struct PrechargeStateMachine *state_machine =
        App_BmsWorld_GetPrechargeStateMachine(world);

    if (state_machine->current_state != state_machine->next_state)
    {
        state_machine->current_state = state_machine->next_state;
        App_PrechargeState_RunOnTick(state_machine->current_state, world);
    }
}
