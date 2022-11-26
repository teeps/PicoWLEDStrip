/**
 * @file NeoPixelMQTT.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lwipopts.h"
#include "NeoPixel.h"
#include "NeoPixelMQTT.h"
#include "MQTTState.h"
#include "MQTTTaskInterface.h"
extern "C" {
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
//#include "lwip/arch.h"
//#include "lwip/sys.h"
//#include "lwip/sockets.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "FreeRTOS.h"
#include "task.h"
}

void vMQTTInterface (void *params)
{
    MQTTTaskInterface*  pMQTT;
    //Task Interface
    TaskInterface_t * pTaskIF = static_cast<TaskInterface_t *>(params);
    pMQTT = pTaskIF->pMQTT;
    int8_t * piReturn = &(pTaskIF->iGoLED);
    while (1)
    {
        pMQTT->advance();       
        vTaskDelay(100);
    }
}

static void  mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);

void vMQTTConnect(mqtt_client_t * pMQTTClient)
{
    struct mqtt_connect_client_info_t ci;
    err_t err;

    /* Setup an empty client info structure */
    memset(&ci, 0, sizeof(ci));

    /* Minimal amount of information required is client identifier, so set it here */ 
    ci.client_id = "PICOLEDx";

    /* Initiate client and connect to server, if this fails immediately an error code is returned
        otherwise mqtt_connection_cb will be called with connection result after attempting 
        to establish a connection with the server. 
        For now MQTT version 3.1.1 is always used */
    ip_addr_t BrokerIP;
    IP4_ADDR(&BrokerIP, cMQTTBrokerIP[0], cMQTTBrokerIP[1], cMQTTBrokerIP[2], cMQTTBrokerIP[3]);
    err = mqtt_client_connect(pMQTTClient, &BrokerIP, MQTT_PORT, mqtt_connection_cb, 0, &ci);

    /* For now just print the result code if something goes wrong*/
    if(err != ERR_OK) {
    printf("mqtt_connect return %d\n", err);
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  err_t err;
  if(status == MQTT_CONNECT_ACCEPTED) {
    printf("mqtt_connection_cb: Successfully connected\n");
    
    /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);
    
    /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */ 
    err = mqtt_subscribe(client, "lighting/picoled2/control", 1, mqtt_sub_request_cb, arg);

    if(err != ERR_OK) {
      printf("mqtt_subscribe return: %d\n", err);
    }
  } else {
    printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
    
    /* Its more nice to be connected, so try to reconnect */
    example_do_connect(client);
  }  
}

// @todo finish this