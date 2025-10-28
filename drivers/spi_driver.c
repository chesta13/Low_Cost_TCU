#include "spi_driver.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_spi.h>
#include <ti/driverlib/dl_gpio.h>

void SPI_Init(void)
{
    // SPI initialized via SysConfig
    SYSCFG_DL_SPI_0_init();
}

void SPI_ChipSelect(bool select)
{
    if (select) {
        // Active low CS
        DL_GPIO_clearPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN);
    } else {
        DL_GPIO_setPins(GPIO_GRP_0_PORT, GPIO_GRP_0_PIN_0_PIN);
    }
}

uint8_t SPI_TransferByte(uint8_t data)
{
    // Wait for TX buffer empty
    while (DL_SPI_isBusy(SPI_0_INST));
    
    // Send data
    DL_SPI_transmitData8(SPI_0_INST, data);
    
    // Wait for RX data
    while (!DL_SPI_isRXFIFOEmpty(SPI_0_INST));
    
    // Read received data
    return DL_SPI_receiveData8(SPI_0_INST);
}

void SPI_Transfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        uint8_t tx_byte = tx_data ? tx_data[i] : 0xFF;
        uint8_t rx_byte = SPI_TransferByte(tx_byte);
        if (rx_data) {
            rx_data[i] = rx_byte;
        }
    }
}