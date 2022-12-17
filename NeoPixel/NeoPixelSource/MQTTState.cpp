/**
 * @file MQTTState.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 * Defines state machine for MQTT operation. Possible states are:
 * State        | Description                                           | Transition
 * Initial        Initial State, waits for WiFi connection                When WiFi connects call the MQTT connect function and transition to Connect
 * Connect        Wait for connection to MQTT Broker                      Connect Call back sets a flag to trigger transition to Subscribe
 * 
 */
#define DEBUG_STATE_MACHINE
#include "MQTTState.h"
#include "MQTTTaskInterface.h"
#include "NeoPixelMQTT.h"
extern "C"
{
    #include "lwip/apps/mqtt.h"
    #include "lwip/apps/mqtt_priv.h"
    #include "hardware/pio.h"
    #include "FreeRTOS.h"
    #include "task.h"
}

MQTTState& MQTTInitial::GetInstance()
{
	static MQTTInitial singleton;
	return singleton;
}

MQTTState& MQTTConnect::GetInstance()
{
	static MQTTConnect singleton;
	return singleton;
}

MQTTState& MQTTDiscovery::GetInstance()
{
	static MQTTDiscovery singleton;
	return singleton;
}

MQTTState& MQTTTransfer::GetInstance()
{
	static MQTTTransfer singleton;
	return singleton;
}

void MQTTInitial::exit(MQTTTaskInterface * pMQTT)
{
}

void MQTTInitial::advance(MQTTTaskInterface * pMQTT)
{
    if (pMQTT->bWiFiConnectionStatus)
    {
        vTaskDelay(5000);// @todo can we replace this long delay with something that checks the connection is ready?
        pMQTT->SetState(MQTTConnect::GetInstance());
    }
}

void MQTTDiscovery::enter(MQTTTaskInterface * pMQTT)
{
    #ifdef DEBUG_STATE_MACHINE
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x00000400); //blue
    #endif
    vPublishDiscovery(pMQTT->pMQTTClient);
}

void MQTTDiscovery::advance(MQTTTaskInterface * pMQTT)
{    
}

void MQTTConnect::enter(MQTTTaskInterface *pMQTT)
{
    #ifdef DEBUG_STATE_MACHINE
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x00000400); //blue
    #endif
    vMQTTConnect(pMQTT->pMQTTClient);    
}
void MQTTConnect::advance(MQTTTaskInterface * pMQTT)
{
    if (pMQTT->bBrokerConnectionStatus)
    {
        pMQTT->SetState(MQTTDiscovery::GetInstance());
    }
}

void MQTTTransfer::enter(MQTTTaskInterface * pMQTT)
{
    #ifdef DEBUG_STATE_MACHINE
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x00000400); //blue
    #endif
    vSubscribeTopics(pMQTT->pMQTTClient,0);
}
void MQTTTransfer::advance(MQTTTaskInterface * pMQTT)
{
    if (!pMQTT->bBrokerConnectionStatus)
    {
        pMQTT->SetState(MQTTConnect::GetInstance());
    } else
    {
        if (pMQTT->bUpdateColourStatus)
        {
            vPublishRGBStatus (pMQTT->pMQTTClient);
            pMQTT->bUpdateColourStatus = false;
        }
    }
    if (!mqtt_client_is_connected (pMQTT->pMQTTClient))
    {
        //Connection lost, disconnect and revert to initial state
        mqtt_disconnect(pMQTT->pMQTTClient);
        pMQTT->SetState(MQTTInitial::GetInstance());
    }
}
