/**
 * @file NeoPixelMQTT.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _NEOPIXELMQTT_H
#define _NEOPIXELMQTT_H

#include <stdint.h>

extern "C" {
    #include "lwip/apps/mqtt.h"
    #include "lwip/apps/mqtt_priv.h"
}

uint8_t const cMQTTBrokerIP[4]={192,168,68,64};


/** @brief Handles the MQTT Interface for the LED 
 * 
 * @param void * - Params is of type MQTTTaskInterface 
 */
void vMQTTInterface (void *params);

void vMQTTConnect(mqtt_client_t *);

#endif