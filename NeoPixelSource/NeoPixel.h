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

/** @brief Enumeration of possible values for iGoLED*/ 
enum eGOLEDValues
{
        GOLED_WIFI_SETUP_ERROR = -1, /**< @brief WiFi could not be setup, don't start LEDs*/
        GOLED_WIFI_CONNECT_TIMEOUT = 1,/**< @brief WiFi connect timeout error, don't start LEDs*/
        GOLED_WIFI_OK = 2,/**< @brief WiFi Good, start LEDs*/
};

/** @brief Task interface structure */
struct TaskInterface_t {
        /** @brief Flag variable used to indicate when setup is complete and LED logic should activate*/
        eGOLEDValues eGoLED;
        MQTTTaskInterface * pMQTT;
};

#endif