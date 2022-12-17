/**
 * @file NeoPixelDriver.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "NeoPixelDriver.h"
extern "C" {
#include "pico/stdlib.h"
#include "hardware/pio.h"
}
#include <stdint.h>

/** @brief Output Pin for Neopixel stream*/
uint8_t const WS2812_PIN = 1;

void NeoPixelDriver::vLEDInit(uint8_t uiPixelCount)
{
    //Setup PIO Program for NeoPixels
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, 0);
    pio_sm_put_blocking(pio0, 0, 0x00000400); //blue
    for (uint8_t i=1; i<uiPixelCount; i++)
        pio_sm_put_blocking(pio0, 0, 0x00000000); 
}

uint32_t uiApplyBrightness (uint32_t uiColour, uint8_t uiBrightness)
{
    uint8_t uiGreen = static_cast<uint32_t>((uiColour & 0xff000000) >> 24) * static_cast<uint32_t>(uiBrightness) / 255U;
    uint8_t uiRed = static_cast<uint32_t>((uiColour & 0x00ff0000) >> 16) * static_cast<uint32_t>(uiBrightness) / 255U;
    uint8_t uiBlue = static_cast<uint32_t>((uiColour & 0x0000ff00) >> 8) * static_cast<uint32_t>(uiBrightness) / 255U;
    return (uiGreen << 24) + (uiRed << 16) + (uiBlue << 8);
}

void NeoPixelDriver::vSetLEDs(uint8_t const * const puiRGB, uint8_t cuiBrightness, uint8_t const cuiNumPixels)
{

    uint32_t uiOutputColour = (puiRGB[1] <<24) + (puiRGB[0] << 16) + (puiRGB[2] << 8);
    uiOutputColour = uiApplyBrightness (uiOutputColour, cuiBrightness);
    for (uint8_t i=0; i< cuiNumPixels; i++)
    {
        pio_sm_put_blocking(pio0, 0, uiOutputColour); //red
    }
}

void NeoPixelDriver::vSetLEDFromVector(std::vector<PatternPair> const & cPatternPair)
{
    for (auto xPair:cPatternPair)
    {
        uint32_t uiOutputColour = (xPair.uiRGB[1] <<24) + (xPair.uiRGB[0] << 16) + (xPair.uiRGB[2] << 8);
        uiOutputColour = uiApplyBrightness (uiOutputColour, xPair.uiBrightness);
        for (uint8_t i=0; i<xPair.uiCount; i++)
            pio_sm_put_blocking(pio0, 0, uiOutputColour);
    }
}

