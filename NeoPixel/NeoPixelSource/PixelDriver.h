/**
 * @file PixelDriver.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <vector>
#include "PatternPair.h"

/**
 * @brief Provides a base class for specific immplementations to inherit from
 * 
 */
class PixelDriver
{
    public:
        /** @brief Perform required initialisations for the selected LED driver
        * @param[in] uint8_t - Number of pixels in string
        */
        virtual void vLEDInit(uint8_t) = 0;

        /** @brief Set a number of LEDs to a given colour
         * @param[in] uint8_t const * - pointer to constant RGB array
         * @param[in] uin8_t const - Brightness
         * @param[in] uint8_t const - number of LEDs to set
         */
        virtual void vSetLEDs(uint8_t const *, uint8_t, uint8_t const) = 0;
        /** @brief Set range of LEDs based on a vector of settings structures
         * @param[in] std::vector<PatternPair const & - Vector of settings
         */
        virtual void vSetLEDFromVector(std::vector<PatternPair> const &) = 0;
        ~PixelDriver(){};
};
