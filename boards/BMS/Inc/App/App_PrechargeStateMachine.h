#pragma once

#include "App_PrechargeState.h"

struct BmsWorld;
struct PrechargeStateMachine;

/**
 *
 * @return
 */
struct PrechargeStateMachine *App_PrechargeStateMachine_Create(void);

/**
 *
 * @param world
 * @return
 */
struct PrechargeState *App_PrechargeStateMachine_GetCurrentState(
    struct PrechargeStateMachine *const state_machine);

/**
 *
 * @param state_machine
 * @param next_state
 */
void App_PrechargeStateMachine_SetNextState(
    struct PrechargeStateMachine *state_machine,
    struct PrechargeState *       next_state);

/**
 *
 * @param world
 */
void App_PrechargeStateMachine_Tick(struct BmsWorld *world);
