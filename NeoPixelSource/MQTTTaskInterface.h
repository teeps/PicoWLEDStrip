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

class MQTTState; //Forward declartaion to avoid circular references
/** @brief A text attribute, 20 characters long*/
struct TextAttributeType {
    char cData[20];
    uint8_t uiLength;
};

/** @brief Contains required MQTT interface data for MQTT Task*/
class MQTTTaskInterface
{
    public:
        /** @brief Set true when WiFi connection is made*/
        bool bWiFiConnectionStatus;
        /** @brief Set true when MQTT connection to broker is made*/
        bool bBrokerConnectionStatus;
        /** @brief Set true when a colour update is required*/
        bool bUpdateColourStatus;
        /** @brief Ident number from 1 to 4 to allow for a fleet of PICOLEDs to connect to the MTT Server*/
        uint8_t uiIdent;
        /** @brief Attribute for the On command from the MQTT server*/
        MQTTAttribute<bool> LightOnCommand;
        /** @brief Attribute for the On status to the MQTT server*/
        MQTTAttribute<bool> LightOnStatus;
        /** @brief Attribute for the RGB command from the MQTT server*/
        MQTTAttribute<RGBColour> RGBCommand;
        /** @brief Attribute for the RGB status to the MQTT server*/
        MQTTAttribute<RGBColour> RGBStatus;
        /** @brief Attribute for the Brightness command from the MQTT server*/
        MQTTAttribute<uint8_t> Brightness;
        /** @brief Attribute for the Effect command from the MQTT server*/
        MQTTAttribute<TextAttributeType> Effect;
        /** @brief Attribute for the Effects status to the MQTT server*/
        MQTTAttribute<TextAttributeType> EffectStatus;
        /** @brief Current state of the MQTT client state machine*/
        MQTTState * pxState;
        /** @brief Set the MQTT state machine state*/
        void SetState (MQTTState &);
        /** @brief pointer to an SDK MQTT Client object*/
        mqtt_client_t * pMQTTClient;

        MQTTTaskInterface();
        /** @brief advance the state machine to the next state*/
        void advance();
};
#endif
