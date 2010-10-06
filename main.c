/*
 * main.c
 *
 *  Created on: 29.09.2010
 *      Author: brunql
 *     Project: LPCXpresso 1343. Camera digitizer
 */

#include <stdio.h>
#include <stdlib.h>

#include "projectconfig.h"

#include "core/cpu/cpu.h"
#include "core/gpio/gpio.h"
#include "core/uart/uart.h"
#include "core/adc/adc.h"
#include "core/usbhid-rom/usbhid.h"

// Cadres sync signal: CADRES_SYNC_NUMBER_OF_ONES ADC measurements '1'
#define CADRES_SYNC_NUMBER_OF_ONES    5


// Save one cadre to video buffer
uint8_t videoBuffer[BUFFER_SIZE];


volatile BOOL updateVideoBuffer; // pc by usb sets it to TRUE

void skipVideoLine() {
    // Skip line: find rise from 1 to smth > 1
    uint8_t result = adcRead_ADC0();
    uint8_t prev_result = 0;
    while (!(prev_result == 1 && result > 1)) {
        prev_result = result;
        result = adcRead_ADC0();
    }
}

int main(void) {
    cpuInit();
    gpioInit();
    adcInit();
    //uartInit(115200);
    //uartRxBufferInit();

    gpioSetDir(CFG_LED_PORT, CFG_LED_PIN, 1);
    gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, 0);

    usbHIDInit();


    updateVideoBuffer = TRUE;

    // Debug pin
    //gpioSetDir(1, 10, 1);

    while (1) {
        if (updateVideoBuffer) {
            
            gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, 1);

            // Find cadres sync signal: CADRES_SYNC_NUMBER_OF_ONES measurements '1'
            int one = 0;
            while (one < CADRES_SYNC_NUMBER_OF_ONES) {
                if (adcRead_ADC0() == 1) {
                    one++;
                    videoBuffer[one] = 1;
                } else {
                    one = 0;
                }
            }
            one = 0;

            // Measure signal to videoBuffer
            uint8_t prev_result = 0;
            for (int i = CADRES_SYNC_NUMBER_OF_ONES; i < BUFFER_SIZE; i++) {
                uint8_t result = adcRead_ADC0();
                videoBuffer[i] = result;

                if (prev_result == 1 && result != 1) {
                    // Skip some video lines
                    skipVideoLine();
                    skipVideoLine();
                    skipVideoLine();
                    skipVideoLine();
                }
                prev_result = result;
            }

            gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, 0);

            
            updateVideoBuffer = FALSE;
        }
    }

    return -1;
}
