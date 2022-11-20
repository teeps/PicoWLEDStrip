/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lwipopts.h"

extern "C" {
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "NeoPixel.pio.h"
#include "pico/cyw43_arch.h"
//#include "lwip/arch.h"
//#include "lwip/sys.h"
//#include "lwip/sockets.h"
//#include "lwip/apps/mqtt.h"
#include "FreeRTOS.h"
#include "task.h"
}

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

/** @brief WiFi SSID*/
char ssid[] = "WelcomeToTheNewWorld";
/** @brief WiFi Password*/
char pass[] = "Fallsch1rm";

/** @brief MQTT Client*/
//mqtt_client_t xMQTTClient;

#define IS_RGBW false
uint8_t const NUM_PIXELS = 10;

#define WS2812_PIN 1

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

/**
 * @brief Sets the colour of consecutive LEDS to Red, Green and Blue, cycling the intensity over time  
 * 
 * @param uint8_t const uiPixelCount - Number of Pixels in the string
 * @param uint8_t & uiIntensity - Intensity Value 0-255
 */
void pattern_rgbs(uint8_t const cuiPixelCount, uint8_t & uiIntensity) {
    uint8_t const cuiMax = 0x0f; // let's not draw too much current!
    if (uiIntensity > cuiMax) {uiIntensity = 0;}

    for (int i = 0; i < cuiPixelCount; ++i) {
        uint8_t uiColour = i % 3;
        put_pixel(uiIntensity << (uiColour*8));
    }

    uiIntensity++;
}

inline void vLEDStep(uint8_t const cuiRed, uint8_t const cuiBlue, uint8_t const cuiGreen, uint8_t uiPosition)
{
    uint32_t uiDataWord = cuiGreen <<24 + cuiRed << 16 + cuiBlue<<8;
    for (uint8_t i=0; i<NUM_PIXELS; i++)
        pio_sm_put_blocking(pio0, 0, i==uiPosition ? uiDataWord : 0);
}

inline void set_value(uint8_t const cuiValue)
{
    uint32_t const cuiGREEN = cuiValue << 24;
    uint32_t const cuiRED = cuiValue << 16;
    uint32_t const cuiBLUE = cuiValue << 8;
    pio_sm_put_blocking(pio0, 0, cuiGREEN);
    pio_sm_put_blocking(pio0, 0, cuiRED);
    pio_sm_put_blocking(pio0, 0, cuiBLUE);
    pio_sm_put_blocking(pio0, 0, cuiGREEN);
}

inline void vLedFlash()
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(250);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}
void vInitTask(void *params);
void vInitTask(void *params)
{
    //set_sys_clock_48() and init Pico W;
    stdio_init_all();
    int8_t * piReturn = static_cast<int8_t *>(params);
    //Setup PIO Program for uint8_t * pUIReturn = static_cast<uint8_t *>params; NeoPixels
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    pio_sm_put_blocking(pio0, 0, 0x00000400); //blue
    for (uint8_t i=1; i<10; i++)
        pio_sm_put_blocking(pio0, 0, 0x00000000); 

    //Setup WiFi
/*    PICO_OK = 0,
    PICO_ERROR_NONE = 0,
    PICO_ERROR_TIMEOUT = -1,
    PICO_ERROR_GENERIC = -2,
    PICO_ERROR_NO_DATA = -3,
    PICO_ERROR_NOT_PERMITTED = -4,
    PICO_ERROR_INVALID_ARG = -5,
    PICO_ERROR_IO = -6,*/
    //int iRetVal = cyw43_arch_init_with_country(CYW43_COUNTRY_UK);
    int iRetVal = cyw43_arch_init();

    if (iRetVal < 0)
    {
        iRetVal *=-1;
        for (int i=0; i< iRetVal; i++)
        {
            pio_sm_put_blocking(pio0, 0, 0x00040000); //red
        }
        *piReturn = -1;
    }
    pio_sm_put_blocking(pio0, 0, 0x00040400); //purple
    for (uint8_t i=1; i<10; i++)
        pio_sm_put_blocking(pio0, 0, 0x00000000); 

    cyw43_arch_enable_sta_mode();
    pio_sm_put_blocking(pio0, 0, 0x04000400); //cyan
    for (uint8_t i=1; i<10; i++)
        pio_sm_put_blocking(pio0, 0, 0x00000000); 

    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("failed to connect\n");
        pio_sm_put_blocking(pio0, 0, 0x00040000); //2 red
        pio_sm_put_blocking(pio0, 0, 0x00040000);
        *piReturn = 1;
    }
    printf("connected\n");
    printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);
    pio_sm_put_blocking(pio0, 0, 0x04000000); //green
    for (uint8_t i=1; i<10; i++)
        pio_sm_put_blocking(pio0, 0, 0x00000000); 

    //Setup MQTT
    vTaskDelay(100);
    *piReturn =2;
    while (1)
    { 
        vTaskDelay(2000);
    }
    cyw43_arch_deinit();
}

void vLedTask(void *params);
void vLedTask(void *params)
{
    int8_t * piReturn = static_cast<int8_t *>(params);
    uint32_t const cuiLEDCode1 = 0x10120000;
    uint32_t const cuiLEDCode2 = 0x20240000;
    while (*piReturn != 2) {
        vTaskDelay(1000);
    }
    while (1)
    {
        for (uint8_t i=0; i<NUM_PIXELS; i++)
        {
            //vLEDStep(0x08,0x00,0x08,i);
            pio_sm_put_blocking(pio0, 0, i%2==0 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==1 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==0 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==1 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==0 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==1 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==0 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==1 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==0 ? cuiLEDCode1 : cuiLEDCode2);
            pio_sm_put_blocking(pio0, 0, i%2==1 ? cuiLEDCode1 : cuiLEDCode2);
            vLedFlash();
            vTaskDelay(5);
        }
        vTaskDelay(100);
    }
}

int main() {
    //assert(iInit()==0);
    TaskHandle_t task1, task2;
    int8_t iGoLED;
    xTaskCreate(vInitTask, "Init Task", configMINIMAL_STACK_SIZE, &iGoLED, 2, &task1);
    xTaskCreate(vLedTask, "LED Task", configMINIMAL_STACK_SIZE, &iGoLED, 1, &task2);
    
    /* Start the tasks and timer running. */
    vTaskStartScheduler();
    while (1) {} 
    return 0;
}   
