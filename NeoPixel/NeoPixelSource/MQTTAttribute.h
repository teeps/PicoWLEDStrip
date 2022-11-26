/**
 * @file MQTTAttribute.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>

template <class T>
class MQTTAttribute
{
    private:
        T xAtt;
    public:
        /**
         * @brief Set the attribute by reference
         * @return uint8_t 
         */
        uint8_t uiSetAtt(T &);
        /** @brief Set the attribute by copy
         * 
         * @return uint8_t 
         */
        uint8_t uiSetAtt(T);
        /**
         * @brief Get a copy of the attribute
         * 
         * @return T 
         */
        T xGetAtt();
};