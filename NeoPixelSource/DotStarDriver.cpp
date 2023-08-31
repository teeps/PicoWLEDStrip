/**
 * @file DotStarDriver.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "DotStarDriver.h"

extern "C" {
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/malloc.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
}
#include <stdint.h>
#include <stdio.h>
static constexpr uint8_t cuiBytesPerPixel=4; //4 bytes per pixel
static constexpr uint8_t cuiStartFrameBytes=4; //4 bytes per pixel
static constexpr uint8_t cuiEndFrameBytes=4; //4 bytes per pixel

DotStarDriver::DotStarDriver()
{
    uiRxData = 0;
    uiBufferSize = 0;
}

void DotStarDriver::vLEDInit(uint8_t uiCount)
{
    //Setup buffer
    uiPixelCount = uiCount;
    uiBufferSize = uiCount * cuiBytesPerPixel + cuiStartFrameBytes + cuiEndFrameBytes;
    puiDataBuffer = new uint8_t [uiBufferSize];
    assert(puiDataBuffer!=0);
    for (uint16_t i=0; i<(uiBufferSize); i++)
        *puiDataBuffer = 0;

    //Setup SPI
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    spi_init(spi0, 2000000); //spi0, 2MHz
    spi_set_format(spi0,8,SPI_CPOL_1,SPI_CPHA_1,SPI_MSB_FIRST);
    spi_set_slave(spi0,false);
    //Setup DMA
    uiDMATxChannel = dma_claim_unused_channel(true);
    uiDMARxChannel = dma_claim_unused_channel(true);
    //Configure Tx Channel
    dma_channel_config xDMAChannelConfig;
    xDMAChannelConfig = dma_channel_get_default_config(uiDMATxChannel);
    channel_config_set_transfer_data_size (&xDMAChannelConfig, DMA_SIZE_8);
    channel_config_set_read_increment(&xDMAChannelConfig, true);
    channel_config_set_write_increment(&xDMAChannelConfig, false);
    channel_config_set_dreq(&xDMAChannelConfig,spi_get_dreq(spi0, true));
    dma_channel_configure(
        uiDMATxChannel, //Channel
        &xDMAChannelConfig, //Config
        &spi_get_hw(spi0)->dr, //Initial Write Address
        puiDataBuffer, //Initial Read Address
        uiBufferSize, //Number of transfers
        false //Do not Start Immediately
        );

    //Configure Rx Channel
    xDMAChannelConfig = dma_channel_get_default_config(uiDMARxChannel);
    channel_config_set_transfer_data_size (&xDMAChannelConfig, DMA_SIZE_8);
    channel_config_set_read_increment(&xDMAChannelConfig, false);
    channel_config_set_write_increment(&xDMAChannelConfig, false);
    channel_config_set_dreq(&xDMAChannelConfig,spi_get_dreq(spi0, false));
    dma_channel_configure(
        uiDMARxChannel, //Channel
        &xDMAChannelConfig, //Config
        &uiRxData, //Initial Write Address
        &spi_get_hw(spi0)->dr, //Initial Read Address
        uiBufferSize, //Number of transfers
        false //Do not Start Immediately
        );

}

void DotStarDriver::vAddEndFrame()
{
    //Add end frame
    uint16_t uiIdx = uiBufferSize-4;
    puiDataBuffer[uiIdx++] = 0x00;
    puiDataBuffer[uiIdx++] = 0x00;
    puiDataBuffer[uiIdx++] = 0x00;
    puiDataBuffer[uiIdx++] = 0x00; 
    
}

void DotStarDriver::vSetLEDs(uint8_t const * puiRGB, uint8_t uiBrightness, uint8_t const cuiCount)
{
    //Fill the buffer, bytes 0-3 must be all zero

    for (uint8_t i=0; i<uiBufferSize; i++)
        puiDataBuffer[i]=0;
    for (uint8_t i=4; i<uiBufferSize; i+=4)
        puiDataBuffer[i]=0xe0;
    
    for (uint16_t i=0; i<uiBufferSize; i+=4)
    {
        if (i>cuiCount*4)
            break;
        puiDataBuffer[i+4]=(uiBrightness >> 3) | 0xe0;
        puiDataBuffer[i+5] = puiRGB[2]; //Blue
        puiDataBuffer[i+6] = puiRGB[1]; //Green
        puiDataBuffer[i+7] = puiRGB[0]; //Red
    }
    vAddEndFrame();
    vSendDMA();
}

void DotStarDriver::vSendDMA()
{
    //Send data via DMA
    bool bSPIReady = spi_is_writable(spi0);
    bool bDMAReady = !(dma_channel_is_busy(uiDMATxChannel));  
    if ( bSPIReady && bDMAReady)
    {
        //Reset DMA Tx Start Address, then start send
        dma_channel_set_read_addr(uiDMATxChannel,puiDataBuffer,true);
        //dma_start_channel_mask((1U << uiDMATxChannel) | (1U << uiDMARxChannel));
    } else
    {
        //Some error indication?
        printf("SPI not writeable or DMA not ready.");
    }
}

void DotStarDriver::vSetLEDFromVector(std::vector<PatternPair> const & xPatternPair)
{
    //Fill the buffer, bytes 0-3 must be all zero
    for (uint8_t i=0; i<uiBufferSize; i++)
        puiDataBuffer[i]=0;
    auto xPatternIterator = xPatternPair.begin();
    
    uint16_t uiBufferIdx = 4;
    for (xPatternIterator; xPatternIterator!= xPatternPair.end(); ++xPatternIterator)
    {
        for (uint16_t i=0; i< xPatternIterator->uiCount; i++)
        {
            puiDataBuffer[uiBufferIdx++]=(xPatternIterator->uiBrightness >> 3) | 0xe0;
            puiDataBuffer[uiBufferIdx++]=xPatternIterator->uiRGB[2]; //Blue
            puiDataBuffer[uiBufferIdx++]=xPatternIterator->uiRGB[1]; //Green
            puiDataBuffer[uiBufferIdx++]=xPatternIterator->uiRGB[0]; //Red
        }
    }
    vAddEndFrame();
    vSendDMA();
}