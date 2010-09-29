/*
 * main.c
 *
 *  Created on: 29.09.2010
 *      Author: brunql
 *     Project: LPCXpresso 1343. Led blink.
 */

#include <stdio.h>
#include <stdlib.h>

#include "core/cpu/cpu.h"
#include "core/gpio/gpio.h"
#include "projectconfig.h"


int main (void)
{
  cpuInit();
  gpioInit();
  
  gpioSetDir(LED_PORT, LED_PIN, 1);
  
  while (1)
  {
    gpioSetValue (LED_PORT, LED_PIN, 1);
    for(REG32 i=0; i<(CFG_CPU_CCLK / 16); i++);
    gpioSetValue (LED_PORT, LED_PIN, 0);
    for(REG32 i=0; i<(CFG_CPU_CCLK / 16); i++);
  }

  return -1;
}
