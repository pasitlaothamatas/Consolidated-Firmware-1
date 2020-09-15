#pragma once

#include <stm32f3xx.h>
#include <stdbool.h>
#include "App_SharedExitCode.h"

struct LTC6813;

struct LTC6813 *Io_LTC6813_Create(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin);

void Io_LTC6813_Configure(const struct LTC6813 *ltc_6813);
