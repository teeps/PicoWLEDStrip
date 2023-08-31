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
#ifdef DEBUG_STATE_MACHINE
#include "PixelDriver.h"
extern PixelDriver xPixelDriver;
#endif

extern "C"
{
    #include "lwip/apps/mqtt.h"
    #include "lwip/apps/mqtt_priv.h"
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
        #ifdef DEBUG_STATE_MACHINE
        printf ("MQTTState, leaving Initial for Connect\n"); 
        #endif
        vTaskDelay(5000);// @todo can we replace this long delay with something that checks the connection is ready?
        pMQTT->SetState(MQTTConnect::GetInstance());
    }
}

void MQTTDiscovery::enter(MQTTTaskInterface * pMQTT)
{
    #ifdef DEBUG_STATE_MACHINE
    {
      std::vector<PatternPair> xPattern;
      xPattern.resize(static_cast<std::size_t>(2));
      auto xPatternIterator = xPattern.begin();
      xPatternIterator->uiRGB[0] = 0x00;
      xPatternIterator->uiRGB[1] = 0x00;
      xPatternIterator->uiRGB[2] = 0x00;
      xPatternIterator->uiCount = 1;
      xPatternIterator->uiBrightness = 0x00;
      for (xPatternIterator; xPatternIterator!= xPattern.end(); ++xPatternIterator)
      {
          xPatternIterator->uiRGB[0] = 0x00;
          xPatternIterator->uiRGB[1] = 0x00;
          xPatternIterator->uiRGB[2] = 0xff;    
          xPatternIterator->uiCount = 1;
          xPatternIterator->uiBrightness = 8;
      }
      xPixelDriver.vSetLEDFromVector(xPattern);
    }
    #endif
    vPublishDiscovery(pMQTT->pMQTTClient);
}

void MQTTDiscovery::advance(MQTTTaskInterface * pMQTT)
{    
}

void MQTTConnect::enter(MQTTTaskInterface *pMQTT)
{
    #ifdef DEBUG_STATE_MACHINE
    {
      std::vector<PatternPair> xPattern;
      xPattern.resize(static_cast<std::size_t>(2));
      auto xPatternIterator = xPattern.begin();
      xPatternIterator->uiRGB[0] = 0x00;
      xPatternIterator->uiRGB[1] = 0x00;
      xPatternIterator->uiRGB[2] = 0x00;
      xPatternIterator->uiCount = 2;
      xPatternIterator->uiBrightness = 0x00;
      xPatternIterator++;
      for (xPatternIterator; xPatternIterator!= xPattern.end(); ++xPatternIterator)
      {
          xPatternIterator->uiRGB[0] = 0x00;
          xPatternIterator->uiRGB[1] = 0x00;
          xPatternIterator->uiRGB[2] = 0xff;    
          xPatternIterator->uiCount = 1;
          xPatternIterator->uiBrightness = 8;
      }
      xPixelDriver.vSetLEDFromVector(xPattern);
    }
    #endif
    vMQTTConnect(pMQTT->pMQTTClient);    
}
void MQTTConnect::advance(MQTTTaskInterface * pMQTT)
{
    if (pMQTT->bBrokerConnectionStatus)
    {
        pMQTT->SetState(MQTTDiscovery::GetInstance());
        #ifdef DEBUG_STATE_MACHINE
        printf ("MQTTState, leaving Connect for Discovery\n"); 
        #endif
    }
}

void MQTTTransfer::enter(MQTTTaskInterface * pMQTT)
{
    #ifdef DEBUG_STATE_MACHINE
        printf ("MQTTState, Entering Transfer from Discovery\n"); 
    #endif
    #ifdef DEBUG_STATE_MACHINE
    {
      std::vector<PatternPair> xPattern;
      xPattern.resize(static_cast<std::size_t>(2));
      auto xPatternIterator = xPattern.begin();
      xPatternIterator->uiRGB[0] = 0x00;
      xPatternIterator->uiRGB[1] = 0x00;
      xPatternIterator->uiRGB[2] = 0x00;
      xPatternIterator->uiCount = 3;
      xPatternIterator->uiBrightness = 0x00;
      xPatternIterator++;
      for (xPatternIterator; xPatternIterator!= xPattern.end(); ++xPatternIterator)
      {
          xPatternIterator->uiRGB[0] = 0x00;
          xPatternIterator->uiRGB[1] = 0x00;
          xPatternIterator->uiRGB[2] = 0xff;    
          xPatternIterator->uiCount = 1;
          xPatternIterator->uiBrightness = 8;
      }
      xPixelDriver.vSetLEDFromVector(xPattern);
    }
    #endif
    vSubscribeTopics(pMQTT->pMQTTClient,0);
}

void MQTTTransfer::advance(MQTTTaskInterface * pMQTT)
{
    if (!pMQTT->bBrokerConnectionStatus)
    {
        pMQTT->SetState(MQTTConnect::GetInstance());
        #ifdef DEBUG_STATE_MACHINE
        printf ("MQTTState, leaving Transfer for Connect on loss of connection\n"); 
        #endif
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
        #ifdef DEBUG_STATE_MACHINE
        printf ("MQTTState, leaving Transfer for Initial on loss of connection\n"); 
        #endif
    }
}
