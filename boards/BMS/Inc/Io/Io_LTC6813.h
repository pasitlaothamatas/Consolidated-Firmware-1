#pragma once

#include <stm32f3xx_hal.h>
#include "App_SharedExitCode.h"

void Io_LTC6813_Init(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin);

void Io_LTC6813_Configure(void);

ExitCode Io_LTC6813_ReadAllCellRegisterGroups(void);

uint16_t *Io_LTC6813_GetCellVoltages(void);
