/**
 * @file MQTTState.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "MQTTState.h"
#include "MQTTTaskInterface.h"
#include "NeoPixelMQTT.h"
extern "C"
{
    #include "lwip/apps/mqtt.h"
    #include "lwip/apps/mqtt_priv.h"
}

MQTTState& MQTTNotConnected::GetInstance()
{
	static MQTTNotConnected singleton;
	return singleton;
}

MQTTState& MQTTConnected::GetInstance()
{
	static MQTTConnected singleton;
	return singleton;
}

void MQTTNotConnected::exit(MQTTTaskInterface * pMQTT)
{
    vMQTTConnect(pMQTT->pMQTTClient);    
}

void MQTTNotConnected::advance(MQTTTaskInterface * pMQTT)
{
    if (pMQTT->bConnectionStatus)
    {
        pMQTT->SetState(MQTTConnected::GetInstance());
    }
}
