#include <assert.h>
#include <stdlib.h>
#include "Io_SharedSpi.h"

#define RETURN_IF_STATUS_NOT_OK(status, nss_port, nss_pin)        \
    if ((status) != HAL_OK)                                       \
    {                                                             \
        HAL_GPIO_WritePin((nss_port), (nss_pin), GPIO_PIN_RESET); \
        return (status);                                          \
    }

struct SharedSpi
{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *     nss_port;
    uint16_t           nss_pin;
};

struct SharedSpi *Io_SharedSpi_Create(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin)
{
    assert(hspi != NULL);

    struct SharedSpi *shared_spi = malloc(sizeof(struct SharedSpi));
    assert(shared_spi != NULL);

    shared_spi->hspi     = hspi;
    shared_spi->nss_pin  = nss_pin;
    shared_spi->nss_port = nss_port;

    return shared_spi;
}

void Io_SharedSpi_SetNssLow(const struct SharedSpi *const spi)
{
    HAL_GPIO_WritePin(spi->nss_port, spi->nss_pin, GPIO_PIN_RESET);
}

void Io_SharedSpi_SetNssHigh(const struct SharedSpi *const spi)
{
    HAL_GPIO_WritePin(spi->nss_port, spi->nss_pin, GPIO_PIN_SET);
}

HAL_StatusTypeDef Io_SharedSpi_TransmitAndReceive(
    const struct SharedSpi *const spi,
    uint8_t *                     tx_data,
    uint16_t                      tx_size,
    uint8_t *                     rx_data,
    uint16_t                      rx_size)

{
    Io_SharedSpi_SetNssLow(spi);
    RETURN_IF_STATUS_NOT_OK(
        HAL_SPI_Transmit(spi->hspi, tx_data, tx_size, 100U), spi->nss_port,
        spi->nss_pin)
    RETURN_IF_STATUS_NOT_OK(
        HAL_SPI_Receive(spi->hspi, rx_data, rx_size, 100U), spi->nss_port,
        spi->nss_pin)
    Io_SharedSpi_SetNssHigh(spi);

    return HAL_OK;
}

HAL_StatusTypeDef Io_SharedSpi_Transmit(
    const struct SharedSpi *const spi,
    uint8_t *                     tx_data,
    uint16_t                      tx_size)
{
    Io_SharedSpi_SetNssLow(spi);
    HAL_StatusTypeDef status =
        HAL_SPI_Transmit(spi->hspi, tx_data, tx_size, 100U);
    Io_SharedSpi_SetNssHigh(spi);

    return status;
}

HAL_StatusTypeDef Io_SharedSpi_Receive(
    const struct SharedSpi *const spi,
    uint8_t *                     rx_data,
    uint16_t                      rx_size)
{
    Io_SharedSpi_SetNssLow(spi);
    HAL_StatusTypeDef status =
        HAL_SPI_Receive(spi->hspi, rx_data, rx_size, 100U);
    Io_SharedSpi_SetNssHigh(spi);

    return status;
}

HAL_StatusTypeDef Io_SharedSpi_MultipleTransmitWithoutNssToggle(
    const struct SharedSpi *const spi,
    uint8_t *                     tx_data,
    uint16_t                      tx_size,
    size_t                        num_tx_data_copies)
{
    for (size_t i = 0U; i < num_tx_data_copies; i++)
    {
        RETURN_IF_STATUS_NOT_OK(
            HAL_SPI_Transmit(spi->hspi, tx_data, tx_size, 100U), spi->nss_port,
            spi->nss_pin)
    }

    return HAL_OK;
}

HAL_StatusTypeDef Io_SharedSpi_TransmitWithoutNssToggle(
    const struct SharedSpi *const spi,
    uint8_t *                     tx_data,
    uint16_t                      tx_size)
{
    return HAL_SPI_Transmit(spi->hspi, tx_data, tx_size, 100U);
}
