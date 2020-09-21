#include "Io_SharedSpi.h"

HAL_StatusTypeDef Io_SharedSpi_TransmitAndReceive(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin,
    uint8_t *          tx_data,
    uint16_t           tx_size,
    uint8_t *          rx_data,
    uint16_t           rx_size)

{
    HAL_GPIO_WritePin(nss_port, nss_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi, tx_data, tx_size, 100U);

    if (status != HAL_OK)
    {
        HAL_GPIO_WritePin(nss_port, nss_pin, GPIO_PIN_SET);
        return status;
    }

    status = HAL_SPI_Receive(hspi, rx_data, rx_size, 100U);
    HAL_GPIO_WritePin(nss_port, nss_pin, GPIO_PIN_SET);
    return status;
}

HAL_StatusTypeDef Io_SharedSpi_Transmit(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin,
    uint8_t *          tx_data,
    uint16_t           tx_size)
{
    HAL_GPIO_WritePin(nss_port, nss_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hspi, tx_data, tx_size, 100U);
    HAL_GPIO_WritePin(nss_port, nss_pin, GPIO_PIN_SET);
    return status;
}

HAL_StatusTypeDef Io_SharedSpi_Receive(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin,
    uint8_t *          rx_data,
    uint16_t           rx_size)
{
    HAL_GPIO_WritePin(nss_port, nss_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Receive(hspi, rx_data, rx_size, 100U);
    HAL_GPIO_WritePin(nss_port, nss_pin, GPIO_PIN_SET);

    return status;
}

HAL_StatusTypeDef Io_SharedSpi_TransmitMultiplePackets(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     chip_select_port,
    uint16_t           chip_select_pin,
    uint8_t *          tx_data,
    uint16_t           tx_size,
    size_t             tx_packet_size)
{
    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(chip_select_port, chip_select_pin, GPIO_PIN_RESET);

    for (size_t i = 0U; i < tx_packet_size; i++)
    {
        status = HAL_SPI_Transmit(hspi, tx_data, tx_size, 100U);
        if (status != HAL_OK)
        {
            HAL_GPIO_WritePin(chip_select_port, chip_select_pin, GPIO_PIN_SET);
            return status;
        }
    }

    HAL_GPIO_WritePin(chip_select_port, chip_select_pin, GPIO_PIN_SET);

    return status;
}
