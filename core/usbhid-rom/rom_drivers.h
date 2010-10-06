/**************************************************************************/
/*! 
    @file     rom_drivers.h
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#ifndef ROM_DRIVERS_H_
#define ROM_DRIVERS_H_

#include "sysdefs.h"
 
typedef struct _USB_DEVICE_INFO 
{
  uint16_t  DevType;
  uint32_t  DevDetailPtr;
} USB_DEV_INFO; 

typedef	struct _USBD 
{
  void      (*init_clk_pins)(void);
  void      (*isr)(void);
  void      (*init)( USB_DEV_INFO * DevInfoPtr ); 
  void      (*connect)(uint32_t con);
}  USBD;

typedef	struct _ROM 
{
   const    USBD * pUSBD;
}  ROM;

typedef	struct _HID_DEVICE_INFO 
{
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint32_t StrDescPtr;
  uint8_t  InReportCount;
  uint8_t  OutReportCount;
  uint8_t  SampleInterval;
  void      (*InReport)(uint8_t src[], uint32_t length); 
  void      (*OutReport)(uint8_t dst[], uint32_t length);
}  HID_DEVICE_INFO;

#endif
