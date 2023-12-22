//UTF-8
#ifndef _MCULIB_USART0_H_
#define _MCULIB_USART0_H_


#include "gd32e10x_usart.h"

#define MCULIB_USART0                 USART0
#define MCULIB_USART0_IRQn            USART0_IRQn
#define MCULIB_USART0_RCU             RCU_USART0


//0=NONE_RECEIVE(不接收)
//1=interrupt+backfunction(中断+接收回调函数)
//2=interrupt+buff(中断+缓冲区)
//3=dma+buff(dma+缓冲区)
#define MCULIB_USART0_RECEIVE_MODE      1

//0=普通阻塞发送
//1=DMA send(普通DMA发送)
//2=DMA buff SEND(发送缓冲+DMA发送)
#define MCULIB_USART0_SEND_MODE         0

#if MCULIB_USART0_SEND_MODE==1||MCULIB_USART0_SEND_MODE==2

#define MCULIB_USART0_SEND_DMA_CH       DMA_CH1
#define MCULIB_USART0_SEND_DMA_RCU      RCU_DMA


#endif



#if MCULIB_USART0_RECEIVE_MODE==0

    void mculib_usart0_init(uint32_t btl);

#elif MCULIB_USART0_RECEIVE_MODE==1

    typedef void(*MCULIB_USART0_RECEIVE_BACK)(uint8_t byte);//回调函数注册
    void mculib_usart0_init(uint32_t btl,MCULIB_USART0_RECEIVE_BACK receive_back);

#endif

#if MCULIB_USART0_SEND_MODE==0

    void mculib_usart0_send_byte(uint8_t ch);
    void mculib_usart0_send_bytes(uint8_t *str,uint16_t len);
    
#elif MCULIB_USART0_SEND_MODE==1||MCULIB_USART0_SEND_MODE==2

    uint8_t mculib_usart0_dma_send_state();
    uint8_t mculib_usart0_dma_send_start(uint32_t *buff,uint32_t bufflen);
    
#endif











#endif
