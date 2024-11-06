#include "can.h"
#include <stdio.h>
#include "systick.h"

#define DEV_CAN0_MASK        (0xFFFF)
#define DEV_CAN1_ID          0x0000
#define DEV_CAN1_MASK        0x0000
/* config CAN baud rate to 500K Hz (range from 1Hz to 1MHz)*/
#define DEV_CAN_BAUD_RATE    1000000

can_trasnmit_message_struct g_transmit_message;
can_receive_message_struct can0_receive_message_fifo0;
volatile Enum_can_mode can0_receive_fifo0_flag = CAN_DISABLE;

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    //rcu_periph_clock_enable(RCU_CAN1);
    rcu_periph_clock_enable(RCU_GPIOA);
    //rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure CAN0 GPIO */
    gpio_init(GPIOA,GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_init(GPIOA,GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    
    /* configure CAN1 GPIO */
//    gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
//    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    
    //全部复用，增加会导致CAN失效
    //gpio_pin_remap_config(GPIO_CAN0_FULL_REMAP, ENABLE);
    //gpio_pin_remap_config(GPIO_CAN1_REMAP,ENABLE);
}

/*!
    \brief      initialize CAN function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_config(void)
{
    can_parameter_struct can_parameter;
     
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    /* initialize CAN register */
    can_deinit(CAN0);
    //can_deinit(CAN1);
    
    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = ENABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;  
    /* initialize CAN */
    can_init(CAN0, &can_parameter);
    //can_init(CAN1, &can_parameter);
    
    /* config CAN0 baud rate */
    can_frequency_set(CAN0, DEV_CAN_BAUD_RATE);
    /* config CAN1 baud rate */
    can_frequency_set(CAN1, DEV_CAN_BAUD_RATE);
    
    /* initialize filter */ 
    //can1_filter_start_bank(14);
    can_filter_mask_mode_init(DEV_CAN0_ID, DEV_CAN0_MASK, CAN_STANDARD_FIFO0, 0);
    //can_filter_mask_mode_init(DEV_CAN1_ID, DEV_CAN1_MASK, CAN_STANDARD_FIFO0, 15);
    
    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX0_IRQn, 0, 0);
    /* configure CAN1 NVIC */
    //nvic_irq_enable(CAN1_RX0_IRQn, 1, 0);
    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INTEN_RFNEIE0);
    //can_interrupt_enable(CAN1, CAN_INTEN_RFNEIE0);
}

void can0_init(void)
{
    can_gpio_config();
    can_config();
    
    /* initialize transmit message */
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &g_transmit_message);
    g_transmit_message.tx_sfid = 0x00;
    g_transmit_message.tx_efid = 0x00;
    g_transmit_message.tx_ft = CAN_FT_DATA;
    g_transmit_message.tx_ff = CAN_FF_STANDARD;
    g_transmit_message.tx_dlen = 8;
    /* initialize receive message */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &can0_receive_message_fifo0);
}

void send_can0_data(uint32_t id, uint8_t *msg, uint8_t msg_len)
{
		uint16_t over_time = 255;

		if(msg_len > 8)
		{
			msg_len = 8;
		}
		
		g_transmit_message.tx_sfid = id;
		g_transmit_message.tx_dlen = msg_len;
		
		for(uint8_t i = 0; i < msg_len; i++)
			g_transmit_message.tx_data[i] = msg[i];
    
		
    /* transmit message */
    while((can_message_transmit(CAN0, &g_transmit_message) == CAN_NOMAILBOX) /*&& (over_time)*/)
		{
				over_time--;
		}
}

void send_can0_buffer(uint32_t id, uint8_t *buffer, uint16_t buffer_len)
{
		uint8_t temp1 = buffer_len/8;
		uint8_t temp2 = buffer_len%8;

		
		if(temp1 == 0)
		{
			send_can0_data(id, buffer, buffer_len);
		}
		else
		{
			for(uint8_t i = 0; i < temp1; i++)
			{
				send_can0_data(id, buffer + 8*i, 8);
			}
			if(temp2 != 0)
			{
				send_can0_data(id, buffer+(buffer_len - temp2), temp2);
			}
		}
}

void can0_rx0_interrupt_handler(void)
{
	    /* check the receive message */
    can_message_receive(CAN0, CAN_FIFO0, &can0_receive_message_fifo0);
    
    if(DEV_CAN0_ID == can0_receive_message_fifo0.rx_sfid && can0_receive_message_fifo0.rx_dlen>=1)
			{
				if(can0_receive_message_fifo0.rx_data[0] == 0x01)
					can0_receive_fifo0_flag = CAN_ONCE_TIGGER;
				else if(can0_receive_message_fifo0.rx_data[0] == 0x02)
					can0_receive_fifo0_flag = CAN_CONTINUE;
				else 
					can0_receive_fifo0_flag = CAN_DISABLE;
    }else{
        can0_receive_fifo0_flag = CAN_DISABLE; 
    }
}

