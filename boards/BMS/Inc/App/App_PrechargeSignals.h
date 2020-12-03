#pragma once

#include <stdbool.h>
#include "App_BmsWorld.h"

struct BmsWorld;

/**
 *
 * @param world
 * @return
 */
bool App_PrechargeSignals_IsWaitingAfterBoot(struct BmsWorld *world);

/**
 *
 * @param world
 */
void App_PrechargeSignals_WaitingAfterBootCompleteCallback(
    struct BmsWorld *world);
