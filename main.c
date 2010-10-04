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

#define BUFFER_SIZE 1860

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
 
    gpioSetDir(1, 10, 1);

    gpioSetDir(LED_PORT, LED_PIN, 1);
    gpioSetValue(LED_PORT, LED_PIN, 0);

    while (1) {

        // Wait start measurement byte
        while (uartRxBufferDataPending()) {
            uint8_t uartRx = uartRxBufferRead();
            if (uartRx == 0xf0) {

                gpioSetValue(1, 10, 1);
                gpioSetValue(LED_PORT, LED_PIN, 1);

                // Find cadres sync signal: 12 measurements == 1
                int i = 0;
                while (1) {
                    if (adcRead_ADC0() == 1) {
                        if (++i == 12) {
                            break;
                        }
                    } else {
                        i = 0;
                    }
                }

                // Skip black 21 lines
                for (int i = 0; i < 21; i++) {
                    skipVideoLine();
                }

                // Measure signal to videoBuffer
                uint8_t prev_result = 0;
                for (int i = 0; i < BUFFER_SIZE; i++) {                    
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
                for(volatile int i=0; i<2000; i++){
                    gpioSetValue(1, 10, 0);
                }
                gpioSetValue(1, 10, 1);

                // Send videoBuffer to PC
                for (int i = 0; i < BUFFER_SIZE; i++) {
                    uartSendByte(videoBuffer[i]);
                }

                gpioSetValue(1, 10, 0);
                gpioSetValue(LED_PORT, LED_PIN, 0);

            }
        }

    }

    return -1;
}
