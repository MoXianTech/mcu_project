/*!
  \file  main.c
  \brief ADC0 regular channel with DMA
  \version 2017-12-26, V1.0.0, firmware for GD32E10x
  */
/*
   Copyright (c) 2017, GigaDevice Semiconductor Inc.
   All rights reserved.
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
   3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software without
   specific prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
   OF SUCH DAMAGE.
   */
#include <stdlib.h>
#include <stdio.h>
#include "gd32e10x.h"
#include "systick.h"
#include "uart.h"
#include "timer.h"
#include "flash.h"
#include "main.h"
#include "led.h"
#include "eBus_port.h"
#include "eBus_mcuboot.h"

/*!
  \brief      main function
  \param[in]  none
  \param[out] none
  \retval     none
  */

uint16_t boot_wait=0;

void usart0_receive_back(uint8_t value)
{

}

typedef void (*MCURESET)(void);
void load_appx(uint32_t appaddx)
{
    MCURESET reset;

    if(((*(uint32_t*)appaddx)&0x2FFE0000)==0x20000000)
    {
        reset = (MCURESET)*(uint32_t*)(appaddx+4);
        __set_MSP(*(uint32_t*)appaddx);
        reset();
    }
}

int main(void)
{
    bool flag = false;
    uint64_t time_1ms = 0;

    systick_config();
        load_appx(0x8002000);

    eBus_init();
    led_init();

    while(1)
    {
        time_1ms ++;
        if (time_1ms % 50 == 0)
        {
            flag = !flag;
            GREEN_LED(flag);
        }
        delay_1ms(1);
    }
    while(1)
    {
        delay_1ms(1);
        eBus_run(1);
        delay_1ms(1);
        if(eBus_mcuboot_state_get()==EBUS_MCUBOOT_STATE_ENUM_ENABLE)
        {
            boot_wait=1;
            break;
        }
        boot_wait++;
        if(boot_wait>200)
        {
            boot_wait=0;
            break;
        }
    }
    //
    if(eBus_mcuboot_state_get()!=EBUS_MCUBOOT_STATE_ENUM_ENABLE)
    {
        __set_PRIMASK(1);
        load_appx(0x8002000);
        __enable_irq();
    }

    while(1)
    {
        boot_wait++;
        if(boot_wait>=100)
        {
            boot_wait=0; 

        }
        eBus_run(1);
        delay_1ms(1);
    }
}

