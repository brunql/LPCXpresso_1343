/*
 * usbhid.c
 *
 *  Created on: 6.10.2010
 *      Author: brunql
 *     Project: LPCXpresso 1343. Camera digitizer
 */

#include "core/usbhid-rom/usb.h"
#include "core/usbhid-rom/usbconfig.h"
#include "core/usbhid-rom/rom_drivers.h"
#include "core/gpio/gpio.h"

#include "usbhid.h"

USB_DEV_INFO DeviceInfo;
HID_DEVICE_INFO HidDevInfo;
ROM ** rom = (ROM **)0x1fff1ff8;


// Index of last sended byte of videoBuffer
uint32_t videoBuffIndex = 0;


// Send to PC
void usbHIDGetInReport (uint8_t src[], uint32_t length)
{
    for(int i=0; i<length; i++){
        src[i] = i;
    }


/*
    if (updateVideoBuffer == FALSE) {
        for (int i = 0; i < length; i++) {
            src[i] = videoBuffer[videoBuffIndex];
            if (++videoBuffIndex > BUFFER_SIZE) {
                videoBuffIndex = 0;
                updateVideoBuffer = TRUE;
                break;
            }
        }
    }
*/
}

// Get from PC
void usbHIDSetOutReport (uint8_t dst[], uint32_t length)
{
  // Get cmd-s and/or settings from PC
}




/**************************************************************************/
/*! 
    @brief Initialises the USB port
 *
 * microBuilder LPC Code base
 *
    The ROM-based USB HID code is capable of configuring the PLL and pins
    for USB, but there seems to be a bug in the code that sets the system
    clock to 48MHz (normally the USB and System clocks can be configured
    seperately).  As such, this code does not use the "init_clk_pins()"
    function in the rom, and the USB clock and pins are manually
    configured.
*/
/**************************************************************************/
void usbHIDInit (void)
{
  // Setup USB clock
  SCB_PDRUNCFG &= ~(SCB_PDSLEEPCFG_USBPAD_PD);        // Power-up USB PHY
  SCB_PDRUNCFG &= ~(SCB_PDSLEEPCFG_USBPLL_PD);        // Power-up USB PLL

  SCB_USBPLLCLKSEL = SCB_USBPLLCLKSEL_SOURCE_MAINOSC; // Select PLL Input
  SCB_USBPLLCLKUEN = SCB_USBPLLCLKUEN_UPDATE;         // Update Clock Source
  SCB_USBPLLCLKUEN = SCB_USBPLLCLKUEN_DISABLE;        // Toggle Update Register
  SCB_USBPLLCLKUEN = SCB_USBPLLCLKUEN_UPDATE;
  
  // Wait until the USB clock is updated
  while (!(SCB_USBPLLCLKUEN & SCB_USBPLLCLKUEN_UPDATE));

  // Set USB clock to 48MHz (12MHz x 4)
  SCB_USBPLLCTRL = (SCB_USBPLLCTRL_MULT_4);  
  //while (!(SCB_USBPLLSTAT & SCB_USBPLLSTAT_LOCK));    // Wait Until PLL Locked
  SCB_USBCLKSEL = SCB_USBCLKSEL_SOURCE_USBPLLOUT;

  // Set USB pin functions
  IOCON_PIO0_1 &= ~IOCON_PIO0_1_FUNC_MASK;
  IOCON_PIO0_1 |= IOCON_PIO0_1_FUNC_CLKOUT;           // CLK OUT
  IOCON_PIO0_3 &= ~IOCON_PIO0_3_FUNC_MASK;
  IOCON_PIO0_3 |= IOCON_PIO0_3_FUNC_USB_VBUS;         // VBus
  IOCON_PIO0_6 &= ~IOCON_PIO0_6_FUNC_MASK;
  IOCON_PIO0_6 |= IOCON_PIO0_6_FUNC_USB_CONNECT;      // Soft Connect

  // Disable internal resistor on VBUS (0.3)
  gpioSetPullup(&IOCON_PIO0_3, gpioPullupMode_Inactive);

  // HID Device Info
  volatile int n;
  HidDevInfo.idVendor = USB_VENDOR_ID;
  HidDevInfo.idProduct = USB_PROD_ID;
  HidDevInfo.bcdDevice = USB_DEVICE; 
  HidDevInfo.StrDescPtr = (uint32_t)&USB_HIDStringDescriptor[0];
  #ifdef CFG_USBHID_EXAMPLE  
  HidDevInfo.InReportCount = 2;
  HidDevInfo.OutReportCount = 2;
  #else
  HidDevInfo.InReportCount = 1;
  HidDevInfo.OutReportCount = 1;
  #endif
  HidDevInfo.SampleInterval = 0x20;
  HidDevInfo.InReport = usbHIDGetInReport;
  HidDevInfo.OutReport = usbHIDSetOutReport;

  DeviceInfo.DevType = USB_DEVICE_CLASS_HUMAN_INTERFACE;
  DeviceInfo.DevDetailPtr = (uint32_t)&HidDevInfo;
  
  /* Enable Timer32_1, IOCON, and USB blocks (for USB ROM driver) */
  SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_CT32B1 | SCB_SYSAHBCLKCTRL_IOCON | SCB_SYSAHBCLKCTRL_USB_REG);

  /* Use pll and pin init function in rom */
  /* Warning: This will also set the system clock to 48MHz! */
  //(*rom)->pUSBD->init_clk_pins();

  /* insert a delay between clk init and usb init */
  for (n = 0; n < 75; n++) {__asm("nop");}

  (*rom)->pUSBD->init(&DeviceInfo); /* USB Initialization */
  (*rom)->pUSBD->connect(TRUE);     /* USB Connect */
}

/**************************************************************************/
/*! 
    @brief Passes the USB interrupt to the internal ROM-based handler
*/
/**************************************************************************/
void USB_IRQHandler()
{
  (*rom)->pUSBD->isr();
}