#pragma once

#include "App_BmsWorld.h"

struct BmsWorld;
struct PrechargeState;
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
 * @return
 */
struct PrechargeState *App_PrechargeStateMachine_GetWaitBootState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeStateMachine_GetWaitAIRState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeStateMachine_GetPrechargeState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeStateMachine_GetPrechargeOkState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeStateMachine_GetPrechargeFailState(void);

/**
 *
 * @param world
 */
void App_PrechargeStateMachine_Tick(struct BmsWorld *world);
