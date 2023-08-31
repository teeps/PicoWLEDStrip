/**
 * @file NeoPixelDriver.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "PixelDriver.h"
extern "C" {
    #include "NeoPixel.pio.h"
}

class NeoPixelDriver : public PixelDriver {
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
};
