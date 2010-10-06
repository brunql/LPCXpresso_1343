/*
 * projectconfig.h
 *
 *  Created on: 29.09.2010
 *      Author: brunql
 *     Project: LPCXpresso 1343. Led blink.
 */
 
 
#ifndef _PROJECTCONFIG_H_
#define _PROJECTCONFIG_H_

#include "sysdefs.h"
#include "lpc134x.h"

#define CFG_LED_PORT 0
#define CFG_LED_PIN  7
#define CFG_LED_TOGGLE()  {gpioSetValue(CFG_LED_PORT, CFG_LED_PIN, !gpioGetValue(CFG_LED_PORT, CFG_LED_PIN));}


#define CFG_CPU_CCLK    (72000000)

// UART Rx FIFO buffer size
#define CFG_UART_BUFSIZE (64)

// Video buffer size
#define BUFFER_SIZE 1774

// Update video buffer flag
extern volatile BOOL updateVideoBuffer;

// Saves one cadre from camera
extern uint8_t videoBuffer[BUFFER_SIZE];


#endif /* _PROJECTCONFIG_H_ */
