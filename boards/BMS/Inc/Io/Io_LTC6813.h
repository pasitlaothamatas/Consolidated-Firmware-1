#pragma once

#include <stm32f3xx_hal.h>
#include <stdbool.h>
#include "App_SharedExitCode.h"

struct Ltc6813;

typedef enum
{
    PEC15_MISMATCH_ERROR,
    PEC15_OK
} PEC15Codes;

void Io_LTC6813_Init(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin);

void Io_LTC6813_Configure(void);

ExitCode Io_LTC6813_StartADCConversion(void);

PEC15Codes Io_LTC6813_ReadAllCellRegisterGroups(void);

uint16_t *Io_LTC6813_GetCellVoltages(void);
