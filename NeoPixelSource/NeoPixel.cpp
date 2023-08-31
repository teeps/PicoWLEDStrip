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
//#define USE_NEOPIXEL
#define USE_DOTSTAR
#define DEBUG_vLedTask
extern "C" {
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
}

#include <stdio.h>
#include <stdint.h>
#include "lwipopts.h"
#include "NeoPixel.h"
#include "NeoPixelMQTT.h"
#include "MQTTTaskInterface.h"

/** @brief Defines the number of pixels in the string*/
uint8_t const NUM_PIXELS = 27;

#if defined(USE_NEOPIXEL)
    #include "NeoPixelDriver.h"
    NeoPixelDriver xPixelDriver;
#elif defined(USE_DOTSTAR)
    #include "DotStarDriver.h"
    DotStarDriver xPixelDriver;
#else
    static_assert(0,"No LED Driver Defined");
#endif

#define REDLEDS {0xff,0x00,0x00}
#define CYANLEDS {0x00,0xff,0xff}
#define PURPLELEDS {0xff,0x00,0xff}
#define BLUELEDS {0x00,0x00,0xff}
#define GREENLEDS {0x00,0xff,0x00}

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

/** @brief WiFi SSID*/
char ssid[] = "WelcomeToTheNewWorld";
/** @brief WiFi Password*/
char pass[] = "Fallsch1rm";


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
    xPixelDriver.vLEDInit(NUM_PIXELS);
    
    
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
    uint8_t const cuiRED[3] = REDLEDS;
    uint8_t const cuiBLUE[3] = BLUELEDS;
    uint8_t const cuiPURPLE[3] = PURPLELEDS;
    uint8_t const cuiCYAN[3] = CYANLEDS;
    uint8_t const cuiGREEN[3] = GREENLEDS;
    uint8_t const cuiBrightness = 8;
    if (iRetVal < 0)
    {
        iRetVal *=-1;
        xPixelDriver.vSetLEDs(cuiRED, cuiBrightness, iRetVal);
        *piReturn = -1;
    }
    xPixelDriver.vSetLEDs(cuiPURPLE, cuiBrightness, 1);
    cyw43_arch_enable_sta_mode();
    struct netif *n = &cyw43_state.netif[CYW43_ITF_STA];
    netif_set_hostname(n,cHostname);
    netif_set_up(n);
    xPixelDriver.vSetLEDs(cuiCYAN, cuiBrightness, 1);
    
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("failed to connect\n");
        xPixelDriver.vSetLEDs(cuiRED, cuiBrightness, 2);
        *piReturn = 1;
    }
    printf("connected\n");
    std::vector<PatternPair> xPattern;
    xPattern.resize(static_cast<std::size_t>(2));
    auto xPatternIterator = xPattern.begin();
    xPatternIterator->uiRGB[0] = 0x00;
    xPatternIterator->uiRGB[1] = 0xff;
    xPatternIterator->uiRGB[2] = 0x00;
    xPatternIterator->uiCount = 1;
    xPatternIterator->uiBrightness = 8;
    xPatternIterator++;
    for (xPatternIterator; xPatternIterator!= xPattern.end(); ++xPatternIterator)
    {
        xPatternIterator->uiRGB[0] = 0x00;
        xPatternIterator->uiRGB[1] = 0x00;
        xPatternIterator->uiRGB[2] = 0x00;    
        xPatternIterator->uiCount = NUM_PIXELS-1;
        xPatternIterator->uiBrightness = 0;
    }
    xPixelDriver.vSetLEDFromVector(xPattern);
    //Setup MQTT
    vTaskDelay(100);
    *piReturn =2;
    pTaskIF->pMQTT->bWiFiConnectionStatus=true;
    vTaskDelete(NULL);
}

static uint8_t powr10 (uint8_t power)
{
  uint8_t uiRetVal = 1;
  for (uint8_t i=power; i>0; i--)
    uiRetVal *=10;
  return uiRetVal;
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
    static uint32_t uiPreviousBrightness=0;
    while (1)
    {
        uint8_t uiLength;
        char cAttributeText[20];
        pTaskIF->pMQTT->LightOnCommand.vGetAttText(cAttributeText, uiLength);
        TextAttributeType * puiMode =  pTaskIF->pMQTT->Effect.pGetPtr();
        if (!strcmp(cAttributeText,"ON"))
        {
            uint32_t uiLEDRGB=0;
            if (!strncmp(puiMode->cData,"Static",6)) 
            {
                //Set colour mode
                char cRGBCode[11];
                RGBColour * pNewColour = pTaskIF->pMQTT->RGBCommand.pGetPtr();
                uint8_t uiRed, uiGreen, uiBlue;
                pNewColour->vGetColour(uiRed, uiGreen, uiBlue);
                uiLEDRGB = (uiRed << 24) | (uiGreen << 16) | (uiBlue <<8);
            } else
            {
                //Diurnal mode
                uiLEDRGB = 0x10203000;
            }
            //Now load the value to the LEDs
            //brightness modification
            uint8_t *puiBrightness = pTaskIF->pMQTT->Brightness.pGetPtr();
            if (uiLEDRGB!= uiPreviousColour || *puiBrightness != uiPreviousBrightness)
            {
                uint8_t uiRGB[3];
                uiRGB[0] = (uiLEDRGB & 0xff000000) >> 24;
                uiRGB[1] = (uiLEDRGB & 0x00ff0000) >> 16;
                uiRGB[2] = (uiLEDRGB & 0x0000ff00) >> 8;
                xPixelDriver.vSetLEDs(uiRGB, *puiBrightness, NUM_PIXELS);
                RGBColour * pUpdateColour = pTaskIF->pMQTT->RGBStatus.pGetPtr();
                pUpdateColour->vSetColourFromRGB(uiLEDRGB);
                pTaskIF->pMQTT->bUpdateColourStatus = true;
            }
            uiPreviousColour = uiLEDRGB;
            uiPreviousBrightness = *puiBrightness;
        } else
        {
            uint8_t uiRGB[3];
            uiRGB[0] = (uiPreviousColour & 0xff000000) >> 24;
            uiRGB[1] = (uiPreviousColour & 0x00ff0000) >> 16;
            uiRGB[2] = (uiPreviousColour & 0x0000ff00) >> 8;        
            xPixelDriver.vSetLEDs(uiRGB,0, NUM_PIXELS);
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
    xTaskCreate(vInitTask, "Init Task", configMINIMAL_STACK_SIZE, static_cast<void *>(&TaskInterface), 1, &task1);
    xTaskCreate(vLedTask, "LED Task", configMINIMAL_STACK_SIZE, static_cast<void *>(&TaskInterface), 1, &task2);
    xTaskCreate(vMQTTInterface, "MQTT Task", 512, static_cast<void *>(&TaskInterface), 1, &task3);
    
    /* Start the tasks and timer running. */
    vTaskStartScheduler();
    while (1) {} 
    return 0;
}   
