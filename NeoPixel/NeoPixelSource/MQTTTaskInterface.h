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
#include "RGBColour.h"

extern "C" {
    #include "lwip/apps/mqtt.h"
    #include "lwip/apps/mqtt_priv.h"
}

class MQTTState;
struct TextAttributeType {
    char cData[20];
    uint8_t uiLength;
};

class MQTTTaskInterface
{
    public:
        bool bWiFiConnectionStatus;
        bool bBrokerConnectionStatus;
        bool bUpdateColourStatus;
        uint8_t uiIdent;
        MQTTAttribute<bool> LightOnCommand;
        MQTTAttribute<bool> LightOnStatus;
        MQTTAttribute<RGBColour> RGBCommand;
        MQTTAttribute<RGBColour> RGBStatus;
        MQTTAttribute<uint8_t> Brightness;
        MQTTAttribute<TextAttributeType> Effect;
        MQTTAttribute<TextAttributeType> EffectStatus;
        MQTTState * pxState;
        void SetState (MQTTState &);
        mqtt_client_t * pMQTTClient;
        MQTTTaskInterface();
        void advance();
};
#endif
