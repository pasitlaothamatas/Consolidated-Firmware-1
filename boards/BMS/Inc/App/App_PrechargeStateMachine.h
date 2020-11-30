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
struct PrechargeState *
    App_PrechargeStateMachine_GetCurrentState(struct BmsWorld *const world);

/**
 *
 * @param state_machine
 * @param next_state
 */
void App_PrechargeStateMachine_SetNextState(
    struct BmsWorld *      world,
    struct PrechargeState *next_state);

/**
 *
 * @param world
 */
void App_PrechargeStateMachine_Tick(struct BmsWorld *world);
