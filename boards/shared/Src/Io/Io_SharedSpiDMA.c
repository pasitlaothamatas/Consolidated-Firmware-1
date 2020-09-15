#include "Io_SharedSpiDMA.h"

HAL_StatusTypeDef Io_SharedSpiDMA_Transmit(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin,
    uint8_t *          tx_data,
    uint16_t           size)
{
    HAL_GPIO_WritePin(chip_select_port, chip_select_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(hspi, tx_data, size);
    return status;
}
