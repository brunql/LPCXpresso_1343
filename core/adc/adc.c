/**************************************************************************/
/*! 
    @file     adc.c
    @author   K. Townsend (microBuilder.eu)
    @date     22 March 2010
    @version  0.10
	
    @section Description
	
    SW-based single-channel A/D conversion.  If you wish to convert
    multiple ADC channels simultaneously, this code will need to be
    modified to work in BURST mode.

    @section Example

    @code
    #include "core/cpu/cpu.h"
    #include "core/adc/adc.h"

    void main (void)
    {
      cpuInit();
      adcInit();

      uint32_t results = 0;  
      while(1)
      {
        // Get A/D conversion results from A/D channel 0
        results = adcRead(0);
      }
    }
    @endcode

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

#include "adc.h"

// brunql
#define ADC_DIVIDER 0b011



/**************************************************************************/
/*! 
    @brief Returns the conversion results on the specified ADC channel.

    This function will manually start an A/D conversion on a single
    channel and return the results.  

    @param[in]  channelNum
                The A/D channel [0..7] that will be used during the A/D 
                conversion.  (Note that only A/D channel's 0..3 are 
                configured by default in adcInit.)

    @return     0 if an overrun error occured, otherwise a 10-bit value
                containing the A/D conversion results.
    @warning    Only AD channels 0..3 are configured for A/D in adcInit.
                If you wish to use A/D pins 4..7 they will also need to
                be added to the adcInit function.
*/

/**************************************************************************/
uint8_t adcRead_ADC0() {
    uint32_t regVal;
    uint8_t adcData;

    /* deselect all channels */
    ADC_AD0CR &= ~ADC_AD0CR_SEL_MASK;

    /* switch channel and start converting */
    ADC_AD0CR |= (ADC_AD0CR_START_STARTNOW | ADC_AD0CR_SEL_AD0);

    /* wait until end of A/D convert */
    while (1) {
        regVal = (*(pREG32(ADC_AD0DR0)));

        // Set channel in AD0CR
        ADC_AD0CR = (ADC_AD0CR_SEL_AD0 |
                (ADC_DIVIDER << 8) |
                ADC_AD0CR_BURST_SWMODE | /* BURST = 0, no BURST, software controlled */
                ADC_AD0CR_CLKS_8BITS | /* CLKS = 0, 9 clocks / 8 bits */
                ADC_AD0CR_START_NOSTART | /* START = 0 A/D conversion stops */
                ADC_AD0CR_EDGE_RISING); /* EDGE = 0 (CAP/MAT signal falling, trigger A/D conversion) */

        /* read result of A/D conversion */
        if (regVal & ADC_DR_DONE) {
            break;
        }
    }

    /* stop ADC */
    ADC_AD0CR &= ~ADC_AD0CR_START_MASK;

    /* return 0 if an overrun occurred */
    if (regVal & ADC_DR_OVERRUN) {
        return (0);
    }

    /* return conversion results */
    adcData = (regVal >> 8) & 0xFF; // 8-bit result
    return (adcData);
}

/**************************************************************************/
/*! 
    @brief      Initialises the A/D converter and configures channels 0..3
                for 10-bit, SW-controlled A/D conversion.

    @return     Nothing
*/
/**************************************************************************/
void adcInit (void)
{
  /* Disable Power down bit to the ADC block. */
  SCB_PDRUNCFG &= ~(SCB_PDRUNCFG_ADC);

  /* Enable AHB clock to the ADC. */
  SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_ADC);

  /* Digital pins need to have the 'analog' bit set in addition
     to changing their pin function */

  /* Set AD0 to analog input */
  IOCON_JTAG_TDI_PIO0_11 &= ~(IOCON_JTAG_TDI_PIO0_11_ADMODE_MASK |
                              IOCON_JTAG_TDI_PIO0_11_FUNC_MASK |
                              IOCON_JTAG_TDI_PIO0_11_MODE_MASK);
  IOCON_JTAG_TDI_PIO0_11 |=  (IOCON_JTAG_TDI_PIO0_11_FUNC_AD0 &
                              IOCON_JTAG_TDI_PIO0_11_ADMODE_ANALOG);

  /* Note that in SW mode only one channel can be selected at a time (AD0 in this case)
     To select multiple channels, ADC_AD0CR_BURST_HWSCANMODE must be used */
  ADC_AD0CR = (ADC_AD0CR_SEL_AD0 |
                (ADC_DIVIDER << 8) |
                ADC_AD0CR_BURST_SWMODE | /* BURST = 0, no BURST, software controlled */
                ADC_AD0CR_CLKS_8BITS | /* CLKS = 0, 9 clocks / 8 bits */
                ADC_AD0CR_START_NOSTART | /* START = 0 A/D conversion stops */
                ADC_AD0CR_EDGE_RISING); /* EDGE = 0 (CAP/MAT signal falling, trigger A/D conversion) */

  return;
}
