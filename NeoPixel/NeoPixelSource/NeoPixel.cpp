/**
 * @file NeoPixel.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief Controls one or more strings of NeoPixel LEDs with integration with an MQTT broker, utlimately designed to work with Home Assistant or similar.
 * 
 * This project uses the Pico W board, along with a 74HCT125N chip to buffer the control signals for the NeoPixels.  NeoPixel control is achieved using a 
 * slightly tweaked version of the example PIO program from the SDK.  The project uses LWIP for WiFi access and MQTT operation.
 * 
 * Peripherals / Software Components Used:
 * - WiFi
 * - MQTT
 * - FreeRTOS
 * - GPIO
 * - PIO
 * - RTC 
 * 
 * Functional Requirements:
 * - FR1 - RGB Mode - Set all LEDs to defined RGB value
 * - FR2 - Diurnal Mode - Vary the LED output to correspond to diurnal cycle, e.g. evening light should be warmer, midday light should be cooler
 * - FR3 - Subscribe to MQTT topics for mode control, on/off control, RGB colour control, sunrise time, sunset time, current time
 * - FR4 - Publish to MQTT topics: On/Off State, Mode, Colour
 * - FR5 - HOSTNAME to be PICOLEDx, where x is an integer between 0 and 3 based on the input states of GPIO 14 and 15 (MSB) pins
 * - FR6 - MQTT topic names to include an element x, where x is an integer between 0 and 3 based on the input states of GPIO 14 and 15 (MSB) pins
 * 
 * @todo FR1
 * @todo FR2
 * @todo FR2
 * @todo FR3
 * @todo FR4
 * @todo FR6
 * 
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
#include "MQTTTaskInterface.h"

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

#define IS_RGBW false
uint8_t const NUM_PIXELS = 3;

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

static uint8_t uiGetIdent();
static uint8_t uiGetIdent()
{
    //Check the GPIO pins for ident setting
    gpio_set_dir (14, false );
    gpio_set_dir (15, false );
    return gpio_get(14) + (gpio_get(15)<<1);
}

void vInitTask(void *params);
void vInitTask(void *params)
{
    //Task Interface
    TaskInterface_t * pTaskIF = static_cast<TaskInterface_t *>(params);
    int8_t * piReturn = &(pTaskIF->iGoLED);
    
    //set_sys_clock_48() and init Pico W;
    stdio_init_all();

    //Setup PIO Program for NeoPixels
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    pio_sm_put_blocking(pio0, 0, 0x00000400); //blue
    for (uint8_t i=1; i<10; i++)
        pio_sm_put_blocking(pio0, 0, 0x00000000); 
    
    pTaskIF->pMQTT->uiIdent = uiGetIdent();
    char cHostname[9]="PICOLEDx";
    cHostname[8] = 0;
    cHostname[7] = pTaskIF->pMQTT->uiIdent + '0';
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
    struct netif *n = &cyw43_state.netif[CYW43_ITF_STA];
    netif_set_hostname(n,cHostname);
    netif_set_up(n);

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
    pTaskIF->pMQTT->bWiFiConnectionStatus=true;
    while (1)
    { 
        vTaskDelay(2000);
    }
    cyw43_arch_deinit();
}

static uint8_t powr10 (uint8_t power)
{
  uint8_t uiRetVal = 1;
  for (uint8_t i=power; i>0; i--)
    uiRetVal *=10;
  return uiRetVal;
}

/** @brief returns a 32 bit unsigned integer from a text field of format 0x00000000
 * 
 * @return uint32_t 
 */
uint32_t uiGRBFromGRBText(char* cData, uint8_t uiSize)
{
    if (uiSize ==11)
    {
        uint32_t uiRetVal=0;
        for (uint8_t i=2; i<11; i++)
        {
            uint8_t uiInteger;
            if  (cData[i] >='0' && cData[i] <='9')
                uiInteger  = cData[i] -'0';
            else if (cData[i] >='a' && cData[i] <='f')
                uiInteger = cData[i] - 'a' + 10;
            else if (cData[i] >='A' && cData[i] <='F')
                uiInteger = cData[i] - 'A' + 10;
            else uiInteger=0;
            uiRetVal += uiInteger << (4*(10-i)); //@todo The probelm is here
        }
        return uiRetVal;
    } else
        return 0;
}

void vLedTask(void *params);
void vLedTask(void *params)
{
    //Task Interface
    TaskInterface_t * pTaskIF = static_cast<TaskInterface_t *>(params);
    int8_t * piReturn = &(pTaskIF->iGoLED);
    
    while (*piReturn != 2) {
        vTaskDelay(1000);
    }
    static uint32_t uiPreviousColour=0;
    while (1)
    {
        uint8_t uiLength;
        char cAttributeText[20];
        pTaskIF->pMQTT->LightOnCommand.vGetAttText(cAttributeText, uiLength);
        TextAttributeType * puiMode =  pTaskIF->pMQTT->Effect.pGetPtr();
        if (!strcmp(cAttributeText,"ON"))
        {
            uint32_t uiLEDGRB=0;
            if (!strncmp(puiMode->cData,"Static",6)) 
            {
                //Set colour mode
                char cRGBCode[11];
                RGBColour * pNewColour = pTaskIF->pMQTT->RGBCommand.pGetPtr();
                uint8_t uiRed, uiGreen, uiBlue;
                pNewColour->vGetColour(uiRed, uiGreen, uiBlue);
                uiLEDGRB = (uiGreen << 24) | (uiRed << 16) | (uiBlue <<8);
            } else
            {
                //Diurnal mode
                uiLEDGRB = 0x10203000;
            }
            //Now load the value to the LEDs
            
            if (uiLEDGRB != uiPreviousColour)
            {
                for (uint8_t i=0; i<NUM_PIXELS; i++)
                {
                    pio_sm_put_blocking(pio0, 0, uiLEDGRB);
                }
                RGBColour * pUpdateColour = pTaskIF->pMQTT->RGBStatus.pGetPtr();
                pUpdateColour->vSetColourFromGRB(uiLEDGRB);
                pTaskIF->pMQTT->bUpdateColourStatus = true;
            }
            uiPreviousColour = uiLEDGRB;
        } else
        {
            for (uint8_t i=0; i<NUM_PIXELS; i++)
            {
                pio_sm_put_blocking(pio0, 0, 0);
            }
        }
        vLedFlash();
        vTaskDelay(100);
    }
}
static MQTTTaskInterface MQTTIF;

int main() {
    //assert(iInit()==0);
    
    MQTTIF.bWiFiConnectionStatus = false;
    TaskInterface_t TaskInterface;
    TaskInterface.pMQTT = &MQTTIF;

    TaskHandle_t task1, task2, task3;
    xTaskCreate(vInitTask, "Init Task", configMINIMAL_STACK_SIZE, static_cast<void *>(&TaskInterface), 3, &task1);
    xTaskCreate(vLedTask, "LED Task", configMINIMAL_STACK_SIZE, static_cast<void *>(&TaskInterface), 2, &task2);
    xTaskCreate(vMQTTInterface, "MQTT Task", 512, static_cast<void *>(&TaskInterface), 1, &task3);
    
    /* Start the tasks and timer running. */
    vTaskStartScheduler();
    while (1) {} 
    return 0;
}   
