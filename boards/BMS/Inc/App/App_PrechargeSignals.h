#pragma once

#include <stdbool.h>
#include "App_BmsWorld.h"

bool App_PrechargeSignals_IsWaitingAfterBoot(struct BmsWorld *world);

void App_PrechargeSignals_WaitingAfterBootCompleteCallback(struct BmsWorld *world);
