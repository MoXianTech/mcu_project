#include "lin.h"
#include "board.h"


static void lin_gpio_init(void)
{
		rcu_periph_clock_enable(LIN0_CLK_TX);
		gpio_af_set(LIN0_PORT_TX, LIN0_AF_TX, LIN0_PIN_TX);
		gpio_mode_set(LIN0_PORT_TX, GPIO_MODE_AF, GPIO_PUPD_PULLUP, LIN0_PIN_TX);
		gpio_output_options_set(LIN0_PORT_TX, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LIN0_PIN_TX);
}

static void lin_periph_config()
{
		/* enable USART clock */
		rcu_periph_clock_enable(LIN0_CLK_PERIPH);
		
		/* configure USART */
		usart_deinit(LIN0_PERIPH);
    usart_baudrate_set(LIN0_PERIPH, LIN0_BAUDRATE);
    usart_word_length_set(LIN0_PERIPH, USART_WL_8BIT);
    usart_stop_bit_set(LIN0_PERIPH, USART_STB_1BIT);
    usart_parity_config(LIN0_PERIPH, USART_PM_NONE);
    //usart_receive_config(LIN0_PERIPH, USART_RECEIVE_ENABLE);
    usart_transmit_config(LIN0_PERIPH, USART_TRANSMIT_ENABLE);
    usart_hardware_flow_rts_config(LIN0_PERIPH, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(LIN0_PERIPH, USART_CTS_DISABLE);
    
    usart_lin_mode_enable(LIN0_PERIPH);
    usart_lin_break_detection_length_config(LIN0_PERIPH, USART_LBLEN_11B);
    usart_enable(LIN0_PERIPH);
    
    //usart_interrupt_enable(LIN0_PERIPH, USART_INT_LBD);
    //usart_interrupt_enable(LIN0_PERIPH, USART_INT_RBNE);
	
		
}

void lin_init(void)
{
	lin_gpio_init();
	lin_periph_config();
}

//发送同步间隔段
static void lin_break_send()
{
    usart_send_break(LIN0_PERIPH);
		while(RESET == usart_flag_get(LIN0_PERIPH, USART_FLAG_TBE));
}

//发送单个字节
static void lin_sendbyte(uint8_t ch)
{
		usart_data_transmit(LIN0_PERIPH, ch);
    while(RESET == usart_flag_get(LIN0_PERIPH, USART_FLAG_TBE));
}

//PID 计算
uint8_t lin_pid_calculate(uint8_t id)
{
    uint8_t pid, p0, p1;
    
    pid = id;
    p0 = (((pid >> 0) ^ (pid >> 1) ^ (pid >> 2) ^ (pid >> 4)) & 0x01) << 6;
    p1 = (~((pid >> 1) ^ (pid >> 3) ^ (pid >> 4) ^ (pid >> 5) & 0x01)) << 7;
    pid |= (p0 | p1);
    
    return pid;
}

//帧头发送
void lin_header_transmit(uint8_t pid)
{
		lin_break_send();
		lin_sendbyte(0x55);
		
		lin_sendbyte(pid);
		
}

//数据域发送
void lin_data_transmit(uint8_t *data, uint8_t data_len)
{
		uint8_t i = 0;
		for(i = 0; i < data_len; i++)
		{
				lin_sendbyte(data[i]);
				while(RESET == usart_flag_get(LIN0_PERIPH, USART_FLAG_TBE));
				
		}
}

//增强型校验
uint8_t lin_checksum_calculate(uint8_t id, uint8_t* data, uint8_t data_len)
{
    uint8_t i = 0;    
    uint32_t checksum = id;
    
    for(i = 0; i < data_len; i++){
        checksum += data[i];
        if(0!= (checksum & 0xFF00)){
            checksum = (checksum & 0xFF) + 1;
        }
    }
    checksum ^= 0xFF;
    
    return checksum;
}

//lin发送整个包
void lin_send_pack(lin_pack_stu* lin_pack)
{
		lin_header_transmit(lin_pack->pid);
		lin_data_transmit(lin_pack->data,LIN_DATA_SIZE);
		lin_sendbyte(lin_pack->checksum);
}