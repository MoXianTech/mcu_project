#include "mx_serial_core.h"
#include "priv_malloc.h"
static uint8_t mx_serial_buff[MX_SERIAL_BUFF_SIZE];
int all_pressure;


uint16_t CalChecksum(uint8_t * data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}
uint16_t CalChecksum2(uint8_t * data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 6; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}
int get_pressure_all(){
	return all_pressure;
}
/*
功能：帧接收处理校验函数
参数：rev_buff--接收好一帧数据的buff地址,rev_buff_len--接收好一帧数据的长度，
返回：帧数据正确或错误代号


*/
int define_date = 0;
uint8_t mx_frame_rev(uint8_t* rev_buff, uint16_t rev_buff_len, mx_frame_rx_struct *mx_frame)
{
		uint8_t scan_max_number = 30;
		uint8_t state = 0;
		uint16_t index = 0;
		uint16_t start_index = 0;
		uint16_t data_index = 0;
		uint16_t surplus_len = 0;
		mx_frame_rx_struct frame_temp = {0};

	
		if(rev_buff_len < 8)
		{
				return FRAME_LEN_LITTLE;
		}
	
		while(1)
		{
				switch(state)
				{
					case SOF_GET:
						{
								frame_temp.sof = (rev_buff[index]<<8) + rev_buff[index + 1];
								index++;
								surplus_len = rev_buff_len - index;
								if(frame_temp.sof == FRAME_HEAD)
								{
										start_index = index - 1;
										state = TRANTYPE_GET;
										index++;
								}
								else if(index == scan_max_number || surplus_len < 8)
								{
										return SOF_ERR;
								}
						}
						break;
						
					case TRANTYPE_GET:
						{
								frame_temp.trantype = rev_buff[index];
								index++;
								state = DATALEN_GET;
						}
						break;
						
					case DATALEN_GET:
						{
								frame_temp.datalen = rev_buff[index] + (rev_buff[index + 1]<<8);
								index = index + 2;
								surplus_len = rev_buff_len - index;
								if(frame_temp.datalen - 3 <= surplus_len)
								{
										state = TYPE_GET;
								}
								else
								{
										return DATA_LEN_OVER;
								}
						}
						break;
						
					case TYPE_GET:
						{
								frame_temp.type = rev_buff[index];
								index++;
								state = DATA_GET;
						}
						break;
					
					case DATA_GET:
						{
								frame_temp.data = &rev_buff[index];
								data_index = index;
								index = index + frame_temp.datalen - 6;
								state = CHECKSUM_GET;
						}
						break;
						
					case CHECKSUM_GET:
						{
								frame_temp.checksum = (rev_buff[index + 1]<<8) + rev_buff[index];
								define_date = index;
								if(frame_temp.checksum == CalChecksum((uint8_t*)&rev_buff[start_index], frame_temp.datalen))
								{
										mx_frame->type = frame_temp.type;
										mx_frame->trantype = frame_temp.trantype;
										mx_frame->sof = frame_temp.sof;
										mx_frame->datalen = frame_temp.datalen;
										
									if (mx_frame->data != NULL)
									{
											board_free(mx_frame->data);
											mx_frame->data = NULL;  // 避免悬空指针
									}
									// 然后重新申请新的内存空间
									mx_frame->data = board_malloc(frame_temp.datalen - 6);
									// 检查是否申请成功
									if (mx_frame->data != NULL)
									{
											// 拷贝数据
											memcpy(mx_frame->data, frame_temp.data, frame_temp.datalen - 6);
									}
									
//										memcpy(mx_frame->data,frame_temp.data,frame_temp.datalen-6);
										mx_frame->checksum = frame_temp.checksum;
										all_pressure = CalChecksum2((uint8_t*)&rev_buff[start_index], frame_temp.datalen);
//										if(pressure%100 == 0){
//											UART4_DMA_SendData((uint8_t *)pressure,2);
//											delay_xms(1);
//											UART4_DMA_SendData(rev_buff,rev_buff_len);
//											delay_xms(1);
//										}
										return REV_SUCCESSFUL;
								}
								else
								{
										return CHECK_SUM_ERR;
								}
						}
						break;
					
				}
			
		}
}

//uint8_t mx_serial_rev_run(uart_type_def com, mx_frame_rx_struct * mx_serial_rx)
//{
//		uint16_t length = 0;
//	
//		length = uart_rx_ringbuffer_pop_data(com, (uint8_t*)mx_serial_buff, MX_SERIAL_BUFF_SIZE);
//	
//		return mx_frame_rev(mx_serial_buff,length,mx_serial_rx);
//}

uint16_t mx_pack_frame(mx_frame_tx_struct * mx_serial_tx)
{
		uint8_t *tx_buff = mx_serial_tx->out_frame;
		uint16_t num = 0;
		uint16_t checksum = 0;
		
		tx_buff[num++] = (uint8_t)SERIAL_HEAD;
		tx_buff[num++] = SERIAL_HEAD>>8;
		tx_buff[num++] = mx_serial_tx->input_trantype;
		tx_buff[num++] = (mx_serial_tx->input_datalen + 6) & 0xff;
		tx_buff[num++] = (mx_serial_tx->input_datalen + 6) >> 8;
		tx_buff[num++] = mx_serial_tx->input_type;
		for(uint16_t loop = 0; loop < mx_serial_tx->input_datalen; loop++)
		{
			tx_buff[num++] = mx_serial_tx->input_data[loop];
		}
		
		checksum = CalChecksum(tx_buff, num);
		
		tx_buff[num++] = checksum & 0xff;
		tx_buff[num++] = checksum >> 8;
		
		mx_serial_tx->out_fram_len = num;
		
		return 1;
}

//uint8_t mx_send_frame(uart_type_def com, mx_frame_tx_struct * mx_serial_tx)
//{
//		mx_serial_tx->out_frame = mx_serial_buff;
//		mx_pack_frame(mx_serial_tx);
//		
//		bsp_usart_buffer_send_data(com, mx_serial_tx->out_frame,mx_serial_tx->out_fram_len);
//		return 1;
//}

//uint8_t mx_serial_rev(uart_type_def com)
//{
//		uint16_t serial_len;
//		uint16_t serial_head;
//		uint16_t check_sum;
//		uint16_t scan_max_number;
//		uint16_t usart_count;
//		uint16_t rx_buff_size = MX_SERIAL_BUFF_SIZE;
//		uint8_t  uart_com = com;
//		uint8_t  *rx_buff = mx_serial_buff;
//		uint8_t step;
//		uint8_t rev;
//			
//		usart_count  = uart_rx_ringbuffer_get_data_count(uart_com);
//		if(usart_count < 8) 
//		{
//			return DATA_SRC_ERR;
//		}
//		
//		scan_max_number = MIN(usart_count, 30);
//		
//		for(uint16_t i = 0; i <  scan_max_number; i++)
//		{
//				uart_rx_ringbuffer_read_data(uart_com, (uint8_t*)&serial_head, 2);
//				
//				if(usart_count <= 8) 
//				{
//						step = 0;
//						rev = DATA_SRC_ERR;
//						break;
//				}
//				else if(serial_head == SERIAL_HEAD)
//				{
//						step = 10;
//						break;
//				}
//				else
//				{
//						step = 0;
//						rev = SOF_ERR;
//						uart_rx_ringbuffer_drop_data(uart_com,1);
//						usart_count--;
//				}
//		}
//		
//		
//		while(1)
//		{
//			switch(step)
//			{
//				case 10:
//						uart_rx_ringbuffer_read_data(uart_com, (uint8_t*)rx_buff, 6);
//						serial_len = ((uint16_t)rx_buff[4]<<8) + rx_buff[3];
//						if(serial_len >= rx_buff_size)
//						{
//								rev = DATA_LEN_OVER;
//								step = 0;
//						}
//						else if(serial_len >= usart_count)
//						{
//								rev = DATA_LEN_LITTLE;
//								step = 0;
//						}
//						else
//						{
//								step = 20;
//						}
//						break;
//				case 20:
//						usart_count = usart_count - uart_rx_ringbuffer_pop_data(uart_com, (uint8_t*)rx_buff, serial_len + 2);
//						check_sum = ((uint16_t)rx_buff[serial_len+1]<<8) + rx_buff[serial_len];
//						if(check_sum == CalChecksum((uint8_t*)rx_buff, serial_len))
//						{
//								step = 0;
//								rev = REV_SUCCESSFUL;
//						}
//						else
//						{
//								step = 0;
//								rev = CHECK_SUM_ERR;
//						}
//						break;
//				case 0:
//				default :
//						uart_rx_ringbuffer_drop_data(uart_com,usart_count);
//						//printf("%d\r\n",rev);
//						return rev;
//			}
//			
//		}
//		
//	}

//	//mx_serials[mx_serial].rev_para.is_have_flag = mx_serials[mx_serial].rev_para.rev_fail_count > 2000/time_ms ? 0 : 1;
//uint16_t  mx_serial_rev_run(uart_type_def com, mx_serial_struct * mx_serial_rx)
//{
//		uint8_t rev = 0;
//		
//		rev = mx_serial_rev(com);

//		if(rev == REV_SUCCESSFUL)
//		{
//				 
//				mx_serial_rx->datalen = ((uint16_t)mx_serial_buff[4]<<8) + mx_serial_buff[3] - 6; 
//				mx_serial_rx->trantype = mx_serial_buff[2];
//				mx_serial_rx->type = mx_serial_buff[5];
//				mx_serial_rx->data = &mx_serial_buff[6];
//				
//		}
//		return rev;
//}


//uint8_t mx_send_frame(uart_type_def com,  mx_serial_struct * mx_serial_tx)
//{
//		uint16_t num = 0;
//		uint16_t loop = 0;
//		uint16_t checksum = 0;
//		uint8_t *tx_buff = mx_serial_buff;
//		uint16_t tx_buff_size = sizeof(mx_serial_buff);
//		
//		if(tx_buff == NULL || tx_buff_size <= mx_serial_tx->datalen + 8)
//		{
//				return 0;
//		}
//		tx_buff[num++] = (uint8_t)SERIAL_HEAD;
//		tx_buff[num++] = SERIAL_HEAD>>8;
//		tx_buff[num++] = mx_serial_tx->trantype;
//		tx_buff[num++] = (mx_serial_tx->datalen + 6) & 0xff;
//		tx_buff[num++] = (mx_serial_tx->datalen + 6) >> 8;
//		tx_buff[num++] = mx_serial_tx->type;
//		for(loop = 0; loop < mx_serial_tx->datalen; loop++)
//		{
//			tx_buff[num++] = mx_serial_tx->data[loop];
//		}
//		
//		checksum = CalChecksum(tx_buff, num);
//		
//		tx_buff[num++] = checksum & 0xff;
//		tx_buff[num++] = checksum >> 8;
//		bsp_usart_buffer_send_data(com, tx_buff,num);
//		return 1;
//}
