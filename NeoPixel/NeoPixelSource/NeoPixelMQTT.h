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

enum ePayloads {
    LIGHT_STATUS=1,
    LIGHT_CONTROL,
    BRIGHTNESS,
    COLOUR,
    EFFECT,
    SUNRISE,
    SUNSET,
    TIME,
    UNKNOWN,
};

/**
 * @brief Publish current colour status
 * 
 * @param client 
 */
void vPublishRGBStatus (mqtt_client_t *client);

/** @brief Called when publish is complete either with sucess or failure
 * 
 * @param[in] void * arg - argument passed from requesting function, we will assume it to be an enum indicating which thing was published
 * @param[in] err_r - result*/
static void mqtt_pub_request_cb(void *arg, err_t result);

/** @brief Callback for an incoming publish message. Called when an incoming publish arrives to a subscribed topic
 * 
 * @param void * arg - Additional argument to pass to the callback function, assume payload enum
 * @param const char * topic - Zero-terminated topic string
 * @param u32_t tot_len - Total length of incoming published data
 */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
/**
 * @brief Callback for data associated with an incoming publish.  Called when data arrives to a subscribed topic.
 * 
 * @param arg 
 * @param const u8_t *data - data buffer 
 * @param u16_t len - data length
 * @param u8_t flags - MQTT_DATA_FLAG_LAST set when this call contains the last part of data from publish message 
 */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);

/**
 * @brief Callback for control subscription request
 * 
 * @param arg 
 * @param result 
 */
static void mqtt_control_sub_request_cb(void *arg, err_t result);

/**
 * @brief Callback for colour subscription request
 * 
 * @param arg 
 * @param result 
 */
static void mqtt_colour_sub_request_cb(void *arg, err_t result);

/**
 * @brief Callback for brigthness subscription request
 * 
 * @param arg 
 * @param result 
 */
static void mqtt_brigthness_sub_request_cb(void *arg, err_t result);

/**
 * @brief Callback for mode subscription request
 * 
 * @param arg 
 * @param result 
 */
static void mqtt_mode_sub_request_cb(void *arg, err_t result);

/**
 * @brief Callback for sunrise subscription request
 * 
 * @param arg 
 * @param result 
 */
static void mqtt_sunrise_sub_request_cb(void *arg, err_t result);

/**
 * @brief Callback for sunset subscription request
 * 
 * @param arg 
 * @param result 
 */
static void mqtt_sunset_sub_request_cb(void *arg, err_t result);

/**
 * @brief Callback for time subscription request
 * 
 * @param arg 
 * @param result 
 */
static void mqtt_time_sub_request_cb(void *arg, err_t result);

/**
 * @brief Subscribe to required topics
 * 
 */
void vSubscribeTopics(mqtt_client_t *client, void* arg);

/**
 * @brief Publish Discovery message
 * 
 */
void vPublishDiscovery(mqtt_client_t *client);
#endif