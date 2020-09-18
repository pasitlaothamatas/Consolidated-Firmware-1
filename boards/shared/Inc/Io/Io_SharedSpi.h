#pragma once

#include <stm32f3xx_hal.h>

HAL_StatusTypeDef Io_SharedSpi_TransmitAndReceive(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin,
    uint8_t *          tx_data,
    uint16_t           tx_size,
    uint8_t *          rx_data,
    uint16_t           rx_size);

HAL_StatusTypeDef Io_SharedSpi_Transmit(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin,
    uint8_t *          tx_data,
    uint16_t           tx_size);

HAL_StatusTypeDef Io_SharedSpi_Receive(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin,
    uint8_t *          rx_data,
    uint16_t           rx_size);

HAL_StatusTypeDef Io_SharedSpi_Receive(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin,
    uint8_t *          rx_data,
    uint16_t           rx_size);
