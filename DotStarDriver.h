/**
 * @file DotStarDriver.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief Defines interface for DotStar SPI-like driver.
 * @version 0.1
 * @date 2022-12-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include "PixelDriver.h"
/** @brief Defines interface for DotStar (APA102C) SPI-like driver.  On the Pico the default pins used for SPI are 24 (SCK) and 25 (MOSI). 
 * 
 */
class DotStarDriver : public PixelDriver
{
    public:
        /** @brief Perform required initialisations for the selected LED driver
         * @param[in] uint8_t - Number of pixels in string
        */
        void vLEDInit(uint8_t);

        /** @brief Set a number of LEDs to a given colour
         * @param[in] uint8_t const * - pointer to constant RGB array
         * @param[in] uin8_t const - Brightness
         * @param[in] uint8_t const - number of LEDs to set
         */
        void vSetLEDs(uint8_t const *, uint8_t, uint8_t const);
        /** @brief Set range of LEDs based on a vector of settings structures
         * @param[in] std::vector<PatternPair const & - Vector of settings
         */
        void vSetLEDFromVector(std::vector<PatternPair> const &);
        DotStarDriver();
        ~DotStarDriver(){delete puiDataBuffer;};
    private:
        /** @brief Send the buffered data using SPI & DMA.  No interrupts or callbacks are implemented.*/
        void vSendDMA();
        /** @brief Add four  bytes of 0xff at the end of the frame.*/
        void vAddEndFrame();
        uint8_t * puiDataBuffer=0;
        uint16_t uiPixelCount=0;
        uint8_t uiDMATxChannel=0;
        uint8_t uiDMARxChannel=0;
        uint32_t uiRxData;
        uint32_t uiBufferSize;
};
