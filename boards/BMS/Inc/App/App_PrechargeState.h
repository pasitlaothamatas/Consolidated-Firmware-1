#pragma once

#include "App_BmsWorld.h"

struct BmsWorld;
struct PrechargeState;

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeState_GetInitState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeState_GetAIRState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeStateMachine_GetPrechargeState(void);

/**
 *
 * @return
 */
struct PrechargeState *App_PrechargeState_GetPrechargeFaultState(void);

/**
 *
 * @param state
 * @param world
 */
void App_PrechargeState_RunOnTick(
    struct PrechargeState *state,
    struct BmsWorld *      world);
