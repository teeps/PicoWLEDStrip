/**
 * @file MQTTTaskInterface.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _MQTTTASKINTERFACE__H
#define _MQTTTASKINTERFACE__H

#include "MQTTAttribute.h"
#include "MQTTState.h"

extern "C" {
    #include "lwip/apps/mqtt.h"
    #include "lwip/apps/mqtt_priv.h"
}

class MQTTState;

class MQTTTaskInterface
{
    public:
        bool bConnectionStatus;
        MQTTAttribute<bool> bLightOnCommand;
        MQTTAttribute<bool> bLightOnStatus;
        MQTTAttribute<uint32_t> uiRGBCommand;
        MQTTAttribute<uint32_t> uiRGBStatus;
        MQTTState * pxState;
        void SetState (MQTTState &);
        mqtt_client_t * pMQTTClient;
        MQTTTaskInterface();
        void advance();
};
#endif
