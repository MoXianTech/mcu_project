/*!
    \file    app.c
    \brief   USB main routine for CDC device

    \version 2020-08-05, V2.0.0, firmware for GD32E10x
    \version 2020-12-31, V2.1.0, firmware for GD32E10x
    \version 2022-06-30, V2.2.0, firmware for GD32E10x
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc. 

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

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"

usb_core_driver *cdc_acm = NULL;

/*!
    \brief      main routine will construct a USB mass storage device
    \param[in]  none
    \param[out] none
    \retval     none
*/
//int main(void)
//{
//    usb_rcu_config();

//    usb_timer_init();

//    usbd_init(&cdc_acm, USB_CORE_ENUM_FS, &cdc_desc, &cdc_class);

//    usb_intr_config();
//    
//    /* main loop */
//    while (1) {
//        if (USBD_CONFIGURED == cdc_acm.dev.cur_status) {
//            if (0U == cdc_acm_check_ready(&cdc_acm)) {
//                cdc_acm_data_receive(&cdc_acm);
//            } else {
//                cdc_acm_data_send(&cdc_acm);
//            }
//        }
//    }
//}


void usb_init(void **usb_handle)
{
	usb_core_driver *usb_handle_obj = NULL;
	usb_handle_obj = calloc(1, sizeof(usb_core_driver));
	
	usb_rcu_config();

	usb_timer_init();

	usbd_init(usb_handle_obj, USB_CORE_ENUM_FS, &cdc_desc, &cdc_class);

	usb_intr_config();
	
	*usb_handle = usb_handle_obj;
	cdc_acm = usb_handle_obj;
}

void cdc_usb_loop(void *usb_handle)
{
	usb_core_driver *cdc_acm_handle = (usb_core_driver *)usb_handle;
	if (USBD_CONFIGURED == cdc_acm_handle->dev.cur_status) {
			if (0U == cdc_acm_check_ready(cdc_acm_handle)) {
					cdc_acm_data_receive(cdc_acm_handle);
			} else {
					cdc_acm_data_send(cdc_acm_handle);
			}
	}
}

void usb_send_buffer(void *usb_handle, uint8_t *respond, int len)
{
	usbd_ep_send((usb_core_driver *)usb_handle, CDC_DATA_IN_EP, respond, len);
}

void usb_receive_buffer(void *usb_handle, uint8_t *cache_buffer, int buffer_len)
{
	usbd_ep_recev((usb_core_driver *)usb_handle, CDC_DATA_OUT_EP, cache_buffer, buffer_len);
}

int usb_handle_check_receive_lens(void *usb_handle)
{
		usb_dev *udev = (usb_dev *)usb_handle;
    if (udev->dev.class_data[CDC_COM_INTERFACE] != NULL) {
        usb_cdc_handler *cdc = (usb_cdc_handler *)udev->dev.class_data[CDC_COM_INTERFACE];

        if ((1U == cdc->packet_receive) && (1U == cdc->packet_sent)) {
            return 0U;
        }
    }

    return 1U;
}

int usb_hanedle_check_status(void *usb_handle)
{
		usb_core_driver *cdc_acm = (usb_dev *)usb_handle;
		return USBD_CONFIGURED == cdc_acm->dev.cur_status;
}
