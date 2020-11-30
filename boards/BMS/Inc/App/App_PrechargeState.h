#pragma once

#include "App_BmsWorld.h"

struct BmsWorld;
struct PrechargeState;

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeState_GetWaitBootState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeState_GetWaitAIRState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeStateMachine_GetPrechargeState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeState_GetPrechargeOkState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeState_GetPrechargeFailState(void);

/**
 *
 * @param state
 * @param world
 */
void App_PrechargeState_RunOnTick(
    struct PrechargeState *state,
    struct BmsWorld *      world);
