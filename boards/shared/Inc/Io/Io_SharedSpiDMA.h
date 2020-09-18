#pragma once

#include <stm32f3xx_hal.h>

HAL_StatusTypeDef Io_SharedSpiDma_Transmit(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin,
    uint8_t *          tx_data,
    uint16_t           size);
