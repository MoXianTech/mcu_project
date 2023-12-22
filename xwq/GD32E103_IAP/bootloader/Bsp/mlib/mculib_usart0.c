//UTF-8
#include "mculib_usart0.h"

void mculib_usart0_gpio_init()
{
    //时钟配置
    rcu_periph_clock_enable(RCU_GPIOA);

		gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
		gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

}

#if MCULIB_USART0_SEND_MODE==1 || MCULIB_USART0_SEND_MODE==2
//DMA发送
void mculib_usart0_dma_send_init(void)
{
    dma_parameter_struct dma_init_struct;
    // enable DMA clock
    rcu_periph_clock_enable(MCULIB_USART0_SEND_DMA_RCU);

    // deinitialize DMA channel1
    dma_deinit(MCULIB_USART0_SEND_DMA_CH);
    //DMA方向 内存到外设
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    //内存地址
    dma_init_struct.memory_addr = 0;
    //内存地址自增
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    //内存传输宽度
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    //单次传输长度
    dma_init_struct.number = 0;
    //外设地址
    dma_init_struct.periph_addr = (uint32_t)(&USART_TDATA(MCULIB_USART0));//USART_TDATA(MCULIB_USART0);
    //外设地址自增 关闭
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    //外设地址宽度
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    //优先级
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    //配置 写入
    dma_init(MCULIB_USART0_SEND_DMA_CH,dma_init_struct);
    //关闭循环传输
    dma_circulation_disable(MCULIB_USART0_SEND_DMA_CH);
    //打开DMA传输
    //dma_channel_enable(MCULIB_USART0_SEND_DMA_CH);
    //
    dma_channel_disable(MCULIB_USART0_SEND_DMA_CH);
}

uint8_t mculib_usart0_dma_send_state()
{
    if( (DMA_CHCTL(MCULIB_USART0_SEND_DMA_CH)&0x0001)==0x0001)//当前DMA传输打开
    {
        if(dma_transfer_number_get(MCULIB_USART0_SEND_DMA_CH)!=0)//传输数据长度不为0
        {
            return 1;
        }
        if(RESET==dma_flag_get(MCULIB_USART0_SEND_DMA_CH,DMA_FLAG_FTF))
        {
            return 1;
        }
    }
    if(dma_flag_get(MCULIB_USART0_SEND_DMA_CH,DMA_FLAG_FTF))
    {
        dma_flag_clear(MCULIB_USART0_SEND_DMA_CH,DMA_FLAG_FTF);
    }
    return 0;
}


uint8_t mculib_usart0_dma_send_start(uint32_t *buff,uint32_t bufflen)
{
    if(bufflen==0)return 2;//发送数据长度不能为0
    if( (DMA_CHCTL(MCULIB_USART0_SEND_DMA_CH)&0x0001)==0x0001&&//当前DMA传输打开
        dma_transfer_number_get(MCULIB_USART0_SEND_DMA_CH)!=0   //传输数据长度不为0
    )
    {
        return 1;//有数据正在传输
    }
    dma_channel_disable(MCULIB_USART0_SEND_DMA_CH);
    
    dma_transfer_number_config(MCULIB_USART0_SEND_DMA_CH,bufflen);
    dma_memory_address_config(MCULIB_USART0_SEND_DMA_CH,(uint32_t)buff);
    
    dma_channel_enable(MCULIB_USART0_SEND_DMA_CH);
    return 0;
}


#endif



#if MCULIB_USART0_RECEIVE_MODE==0
    void mculib_usart0_init(uint32_t btl)
    {
#elif MCULIB_USART0_RECEIVE_MODE==1

//赋初值0，表示回调函数未指向具体的函数，在后面mculib_usart0_init指定函数
		MCULIB_USART0_RECEIVE_BACK MCULIB_USART0_receice_back_=0;
		
    void mculib_usart0_init(uint32_t btl,MCULIB_USART0_RECEIVE_BACK receive_back)
    {
        MCULIB_USART0_receice_back_=receive_back;	
#endif
        #if MCULIB_USART0_SEND_MODE!=0
            //使能DMA时钟
            rcu_periph_clock_enable(MCULIB_USART0_SEND_DMA_RCU);
        #endif
        //使能USART时钟
        rcu_periph_clock_enable(MCULIB_USART0_RCU);
        
        mculib_usart0_gpio_init();
        
        usart_deinit(MCULIB_USART0);
        usart_baudrate_set(MCULIB_USART0, btl);

        #if MCULIB_USART0_RECEIVE_MODE!=0
            usart_receive_config(MCULIB_USART0, USART_RECEIVE_ENABLE);//禁止接收
        #endif
        
        usart_transmit_config(MCULIB_USART0, USART_TRANSMIT_ENABLE);
        
        #if MCULIB_USART0_RECEIVE_MODE==1 || MCULIB_USART0_RECEIVE_MODE==2
            usart_interrupt_enable(MCULIB_USART0,USART_INT_RBNE);//打开接收中断
            nvic_irq_enable(USART0_IRQn, 0, 0);//打开中断
        #endif
        //
        usart_enable(MCULIB_USART0);
        
        #if MCULIB_USART0_SEND_MODE==1 || MCULIB_USART0_SEND_MODE==2
            mculib_usart0_dma_send_init();
            usart_dma_transmit_config(MCULIB_USART0, USART_DENT_ENABLE);
        #endif
        
    }

#if MCULIB_USART0_SEND_MODE==0
void mculib_usart0_send_byte(uint8_t ch)
{
    usart_data_transmit(MCULIB_USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(MCULIB_USART0, USART_FLAG_TBE));
}
void mculib_usart0_send_bytes(uint8_t *str,uint16_t len)
{
    for(int i=0;i<len;i++)
    {
        mculib_usart0_send_byte(str[i]);
    }
}
#endif

#if MCULIB_USART0_RECEIVE_MODE==1|| MCULIB_USART0_RECEIVE_MODE==2
void USART0_IRQHandler(void)
{
    if(RESET != usart_flag_get(MCULIB_USART0, USART_FLAG_ORERR))
    {
        //清除串口接收溢出中断，必须清除，否则可能导致死机，注意STM32和GD32清除该标志的方法不一样
        usart_flag_clear(MCULIB_USART0, USART_FLAG_ORERR);
    }
    else if(RESET != usart_interrupt_flag_get(MCULIB_USART0, USART_INT_FLAG_RBNE))
    {
        #if MCULIB_USART0_RECEIVE_MODE==1
			//下列任意参数未定义则返回
			if(MCULIB_USART0_receice_back_) 
            {
							//此回调函数在mculib_usart0_init()中定义
                MCULIB_USART0_receice_back_(usart_data_receive(MCULIB_USART0));
							//上面是通过回调函数的方式将收到的数据一个个写入到buff中
							//实际上功能和定义一个数组buff[size]将数据写入buff[i++]功能一样
            }
        #endif
        //mculib_usart0_send_byte(usart_data_receive(MCULIB_USART0));
    }
}
#endif

