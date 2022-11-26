/**
 * @file MQTTTaskInterface.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "MQTTTaskInterface.h"

mqtt_client_t static_client;

MQTTTaskInterface::MQTTTaskInterface ()
{
    pMQTTClient = &static_client;
    bConnectionStatus = false;
}

void MQTTTaskInterface::advance()
{
    pxState->advance(this);  // advance if able
}

void MQTTTaskInterface::SetState(MQTTState& newState)
{
	pxState->exit(this);  // do stuff before we change state
	pxState = &newState;  // actually change states now
	pxState->enter(this); // do stuff after we change state
}
