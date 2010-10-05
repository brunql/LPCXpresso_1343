/*
 * main.c
 *
 *  Created on: 29.09.2010
 *      Author: brunql
 *     Project: LPCXpresso 1343. UART
 */

#include <stdio.h>
#include <stdlib.h>

#include "projectconfig.h"

#include "core/cpu/cpu.h"
#include "core/gpio/gpio.h"
#include "core/uart/uart.h"
#include "core/adc/adc.h"

// Cadres sync signal: CADRES_SYNC_NUMBER_OF_ONES ADC measurements '1'
#define CADRES_SYNC_NUMBER_OF_ONES    5

// Video buffer size
#define BUFFER_SIZE 1774
uint8_t videoBuffer[BUFFER_SIZE];

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
    uartInit(115200);
    uartRxBufferInit();

    // Debug pin
    gpioSetDir(1, 10, 1);

    gpioSetDir(LED_PORT, LED_PIN, 1);
    gpioSetValue(LED_PORT, LED_PIN, 0);

    while (1) {

        // Wait start measurement byte
        while (uartRxBufferDataPending()) {
            uint8_t uartRx = uartRxBufferRead();
            if (uartRx == 0xf0) {
                
                gpioSetValue(LED_PORT, LED_PIN, 1);

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
                gpioSetValue(1, 10, 0);
                

                // Send videoBuffer to PC
                for (int i = 0; i < BUFFER_SIZE; i++) {
                    uartSendByte(videoBuffer[i]);
                }

                gpioSetValue(LED_PORT, LED_PIN, 0);

            }
        }

    }

    return -1;
}
