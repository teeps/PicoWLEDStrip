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
#define DEBUG_DISCOVERY
#define DEBUG_CONNECT
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include "lwipopts.h"
#include "NeoPixel.h"
#include "NeoPixelMQTT.h"
#include "MQTTState.h"
#include "MQTTTaskInterface.h"
#include "RGBColour.h"

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
#include "hardware/pio.h"
#include "task.h"
}
/** @brief MQTT Task Interface*/
static MQTTTaskInterface*  pMQTT;

static char cControlTopic[] = "lighting/picoledx/control";
static char cStatusTopic[] = "lighting/picoledx/status";
static char cBrightnessTopic[] = "lighting/picoledx/brightness";
static char cBrightnessStatusTopic[] = "lighting/picoledx/brightnessstatus";
static char cEffectTopic[] = "lighting/picoledx/mode";
static char cEffectStatusTopic[] = "lighting/picoledx/modestatus";
static char cColourTopic[] = "lighting/picoledx/rgb";
static char cColourStatusTopic[] = "lighting/picoledx/rgbstatus";

void vMQTTInterface (void *params)
{
    
    //Task Interface
    TaskInterface_t * pTaskIF = static_cast<TaskInterface_t *>(params);
    pMQTT = pTaskIF->pMQTT;
    //Publish stuff
    
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
    ci.client_user = "MQTTLighting";
    ci.client_pass = "MQTTLighting";
    ci.keep_alive = 12;

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
    #ifdef DEBUG_CONNECT
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x00040000); //red
    #endif
    pMQTT->SetState(MQTTInitial::GetInstance());
    
    }
}

static ePayloads eIncomingPayload;

void vSubscribeTopics(mqtt_client_t *client, void* arg)
{
  /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, &eIncomingPayload);
    
    /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */ 
    cControlTopic[16] = pMQTT->uiIdent + '0';
    cStatusTopic[16] = pMQTT->uiIdent + '0';
    cEffectTopic[16] = pMQTT->uiIdent + '0';
    cEffectStatusTopic[16] = pMQTT->uiIdent + '0';
    cColourTopic[16] = pMQTT->uiIdent + '0';
    cColourStatusTopic[16] = pMQTT->uiIdent + '0';
    cBrightnessTopic[16] = pMQTT->uiIdent + '0';
    cBrightnessStatusTopic[16] = pMQTT->uiIdent + '0';
    err_t err;   
    err = mqtt_subscribe(client, cControlTopic, 1, mqtt_control_sub_request_cb, arg);
    if(err != ERR_OK) {
      printf("mqtt_subscribe (control) return: %d\n", err);
    }
    err = mqtt_subscribe(client, cColourTopic, 1, mqtt_colour_sub_request_cb, arg);
    if(err != ERR_OK) {
      printf("mqtt_subscribe (colour) return: %d\n", err);
    }
    err = mqtt_subscribe(client, cEffectTopic, 1, mqtt_mode_sub_request_cb, arg);
    if(err != ERR_OK) {
      printf("mqtt_subscribe (mode) return: %d\n", err);
    }
    err = mqtt_subscribe(client, cBrightnessTopic, 1, mqtt_brightness_sub_request_cb, arg);
    if(err != ERR_OK) {
      printf("mqtt_subscribe (brightness) return: %d\n", err);
    }
    err = mqtt_subscribe(client, "home-assistant/data/sunrise", 1, mqtt_sunrise_sub_request_cb, arg);
    if(err != ERR_OK) {
      printf("mqtt_subscribe (sunrise) return: %d\n", err);
    }
    err = mqtt_subscribe(client, "home-assistant/data/sunset", 1, mqtt_sunrise_sub_request_cb, arg);
    if(err != ERR_OK) {
      printf("mqtt_subscribe (sunset) return: %d\n", err);
    }
    err = mqtt_subscribe(client, "home-assistant/data/time", 1, mqtt_time_sub_request_cb, arg);
    if(err != ERR_OK) {
      printf("mqtt_subscribe (time) return: %d\n", err);
    }
}


static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  if(status == MQTT_CONNECT_ACCEPTED) {
    printf("mqtt_connection_cb: Successfully connected\n");
    pMQTT->bBrokerConnectionStatus = true;
    
    
  } else {
    printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
    pMQTT->bBrokerConnectionStatus = false;
    /* Its more nice to be connected, so try to reconnect, but wait five seconds so we don't spam */
    vTaskDelay(5000);
    pMQTT->SetState(MQTTInitial::GetInstance());
  }  
}

static void mqtt_control_sub_request_cb(void *arg, err_t result)
{
    // @todo Do we need to do anything here?
    /* Just print the result code here for simplicity, 
        normal behaviour would be to take some action if subscribe fails like 
        notifying user, retry subscribe or disconnect from server */
    printf("Control Subscribe result: %d\n", result);
}

static void mqtt_brightness_sub_request_cb(void *arg, err_t result)
{
    // @todo Do we need to do anything here?
    /* Just print the result code here for simplicity, 
        normal behaviour would be to take some action if subscribe fails like 
        notifying user, retry subscribe or disconnect from server */
    printf("Control Subscribe result: %d\n", result);
}

static void mqtt_colour_sub_request_cb(void *arg, err_t result)
{
    // @todo Do we need to do anything here?
    /* Just print the result code here for simplicity, 
        normal behaviour would be to take some action if subscribe fails like 
        notifying user, retry subscribe or disconnect from server */
    printf("Colour Subscribe result: %d\n", result);
}

static void mqtt_mode_sub_request_cb(void *arg, err_t result)
{
    // @todo Do we need to do anything here?
    /* Just print the result code here for simplicity, 
        normal behaviour would be to take some action if subscribe fails like 
        notifying user, retry subscribe or disconnect from server */
    printf("Mode Subscribe result: %d\n", result);
}

static void mqtt_sunrise_sub_request_cb(void *arg, err_t result)
{
    // @todo Do we need to do anything here?
    /* Just print the result code here for simplicity, 
        normal behaviour would be to take some action if subscribe fails like 
        notifying user, retry subscribe or disconnect from server */
    printf("Sunrise Subscribe result: %d\n", result);
}

static void mqtt_sunset_sub_request_cb(void *arg, err_t result)
{
    // @todo Do we need to do anything here?
    /* Just print the result code here for simplicity, 
        normal behaviour would be to take some action if subscribe fails like 
        notifying user, retry subscribe or disconnect from server */
    printf("Sunset Subscribe result: %d\n", result);
}

static void mqtt_time_sub_request_cb(void *arg, err_t result)
{
    // @todo Do we need to do anything here?
    /* Just print the result code here for simplicity, 
        normal behaviour would be to take some action if subscribe fails like 
        notifying user, retry subscribe or disconnect from server */
    printf("Time Subscribe result: %d\n", result);
}

void vPublishRGBStatus(mqtt_client_t *client)
{
  char cPayload[11];
  uint8_t uiLength;
  pMQTT->RGBStatus.vGetAttText(cPayload, uiLength);
  err_t err;
  u8_t qos = 1; /* 0 1 or 2, see MQTT specification */
  u8_t retain = 1;
  err = mqtt_publish(client, cColourStatusTopic, cPayload, uiLength, qos, retain, NULL, NULL);
  if(err != ERR_OK) {
    printf("Publish err: %d\n", err);
  }
}

static void mqtt_pub_request_cb(void *arg, err_t result)
{
    ePayloads * pePayload = static_cast<ePayloads *>(arg);
    if(result != ERR_OK) {
    printf("Publish result for payload %d: %d\n", *pePayload, result);
    }
}

/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
*/

static int inpub_id;
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    printf("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);

    /* Decode topic string into a user defined reference */
    if (!strcmp(topic, cControlTopic))
    {
      eIncomingPayload = LIGHT_CONTROL;
    } else if (!strcmp(topic, cColourTopic)) {
      eIncomingPayload = COLOUR;
    } else if (!strcmp(topic, cEffectTopic)) {
      eIncomingPayload = EFFECT;
    } else if (!strcmp(topic, cBrightnessTopic)) {
      eIncomingPayload = BRIGHTNESS;
    } else if (!strcmp(topic, "home-assistant/data/sunrise")) {
      eIncomingPayload = SUNRISE;
    } else if (!strcmp(topic, "home-assistant/data/sunset")) {
      eIncomingPayload = SUNSET;
    } else if (!strcmp(topic, "home-assistant/data/time")) {
      eIncomingPayload = TIME;
    } else {
        eIncomingPayload = UNKNOWN;
    }
}



static uint8_t powr10 (uint8_t power)
{
  uint8_t uiRetVal = 1;
  for (uint8_t i=power; i>0; i--)
    uiRetVal *=10;
  return uiRetVal;
}
uint8_t uiTextToU8(char const * cText)
{
  uint8_t uiSize = sizeof(cText);
  uint8_t uiReturnValue=0;
  uint8_t i=0;
  uint8_t uiDigits[3];
  while (cText[i]!=0)
  {
    uiDigits[i]=(cText[i]-'0');
    i++;
  }
  for (uint8_t j=i; j>0; j--)
  {
    uiReturnValue += uiDigits[i-j]*(powr10(j-1));
  }
  return uiReturnValue;
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);
    /* Don't trust the publisher, check zero termination */
    if((flags & MQTT_DATA_FLAG_LAST) /* && data[len-1]==0 */) {
        /* Last fragment of payload received (or whole part if payload fits receive buffer
            See MQTT_VAR_HEADER_BUFFER_LEN)  */
        switch (eIncomingPayload)
        {
            case LIGHT_CONTROL:
            {
              if (data[0] =='O' && data[1]=='N' && len==2)
              {
                pMQTT->LightOnCommand.uiSetAtt(true);
                mqtt_publish(pMQTT->pMQTTClient,cStatusTopic,"ON",2,1,1,NULL,NULL);
              }
              else
              {
                pMQTT->LightOnCommand.uiSetAtt(false);
                mqtt_publish(pMQTT->pMQTTClient,cStatusTopic,"OFF",3,1,1,NULL,NULL);
              }
              break;
            }
            case COLOUR:
            {
              RGBColour NewColour;
              NewColour.ColourFromText(reinterpret_cast<char const *>(data), len);
              pMQTT->RGBCommand.uiSetAtt(NewColour);
              break;
            }
            case BRIGHTNESS:
            {
              uint8_t uiNewBrightness = std::stoi(reinterpret_cast<char const *>(data));
              pMQTT->Brightness.uiSetAtt(uiNewBrightness);
              char cText[4];
              uint8_t uiLen = sprintf(cText, "%d",uiNewBrightness);
              mqtt_publish(pMQTT->pMQTTClient,cBrightnessStatusTopic,cText,uiLen,1,1,NULL,NULL);
              break;
            }
            case EFFECT:
            {
              TextAttributeType newEffect;
              newEffect.uiLength = len;
              strncpy(newEffect.cData,reinterpret_cast<char const *>(data),len);
              pMQTT->Effect.uiSetAtt(newEffect);
              pMQTT->EffectStatus.uiSetAtt(newEffect);
              mqtt_publish(pMQTT->pMQTTClient,cEffectStatusTopic,data,len,1,1,NULL,NULL);
              break;
            }
            default:
                break;
        }
    } else {
    /* Handle fragmented payload, store in buffer, write to file or whatever */
    }
}

void vPublishDiscoverCB(void *arg, err_t err)
{
  if (err==ERR_OK)
  {
    pMQTT->SetState(MQTTTransfer::GetInstance());
  } else
  {
    pMQTT->SetState(MQTTConnect::GetInstance());
  }
}

static char const cTopic[] = "homeassistant/light/picoled2/config";
static char const cJSON[] = 
    "{\"name\": \"picoled2\", \"stat_t\": \"lighting/picoled2/status\", \"cmd_t\": \"lighting/picoled2/control\", " 
    "\"fx_cmd_t\": \"lighting/picoled2/mode\", " 
    "\"fx_stat_t\": \"lighting/picoled2/modestatus\", \"fx_list\": \"0,1\", \"rgb_cmd_t\": \"lighting/picoled2/rgb\", "
    "\"rgb_stat_t\": \"lighting/picoled2/rgbstatus\"}\n";
  

void vPublishDiscovery(mqtt_client_s * pClient)
{
  err_t err = mqtt_publish	(pClient, cTopic, cJSON, sizeof(cJSON), 1, 0, vPublishDiscoverCB, 0);
  if (err!=ERR_OK)
  {
    printf("Issue here \n");
    
    #ifdef DEBUG_DISCOVERY
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x0); //off
    pio_sm_put_blocking(pio0, 0, 0x01040000); //red
    #endif
  }
}

// @todo finish this

