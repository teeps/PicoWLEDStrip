/**
 * @file NeoPixel.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _NEOPIXEL_H
#define _NEOPIXEL_H

#include "MQTTTaskInterface.h"
/** @brief Task interface structure 
 * 
 */
struct TaskInterface_t {
        /** @brief to be removed?*/
        int8_t iGoLED;
        MQTTTaskInterface * pMQTT;
};

#endif