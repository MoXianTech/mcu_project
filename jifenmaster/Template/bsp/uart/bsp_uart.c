#include "bsp_uart.h"
#include <stdio.h>
#include <stdbool.h>
#include "bsp_switch.h"
#include "bsp_uart_ringbuffer.h"

// clang-format off

#define USART_TX_BUFFER_SIZE 2
#define USART_RX_BUFFER_SIZE 128
#define USART5_RX_BUFFER_SIZE 32
uint8_t usart_tx_buffer[COM_UART_MAX_NUM][USART_TX_BUFFER_SIZE] = {0,0};
uint8_t usart_rx_buffer[COM_UART_MAX_NUM][USART_RX_BUFFER_SIZE] = {0,0};
uint8_t usart5_rx_buffer[USART5_RX_BUFFER_SIZE] = {0};

static const uint32_t COM_UART[COM_UART_MAX_NUM] =
{
    COM_UART0,
    COM_UART1,
    COM_USART2,
    COM_USART5
};
static const IRQn_Type COM_UART_IRQn[COM_UART_MAX_NUM] =
{
    COM_UART0_IRQn,
    COM_UART1_IRQn,
    COM_USART2_IRQn,
    COM_USART5_IRQn
};
static const rcu_periph_enum COM_UART_CLK[COM_UART_MAX_NUM] =
{
    COM_UART0_CLK,
    COM_UART1_CLK,
    COM_USART2_CLK,
    COM_USART5_CLK
};
static const rcu_periph_enum COM_UART_TX_GPIO_CLK[COM_UART_MAX_NUM] =
{
    COM_UART0_TX_GPIO_CLK,
    COM_UART1_TX_GPIO_CLK,
    COM_USART2_TX_GPIO_CLK,
    COM_USART5_TX_GPIO_CLK
};
static const uint32_t COM_UART_TX_GPIO_AF[COM_UART_MAX_NUM] =
{
    COM_UART0_TX_GPIO_AF,
    COM_UART1_TX_GPIO_AF,
    COM_USART2_TX_GPIO_AF,
    COM_USART5_TX_GPIO_AF
};
static const uint32_t COM_UART_TX_PORT[COM_UART_MAX_NUM] =
{
    COM_UART0_TX_PORT,
    COM_UART1_TX_PORT,
    COM_USART2_TX_PORT,
    COM_USART5_TX_PORT
};
static const uint32_t COM_UART_TX_PIN[COM_UART_MAX_NUM] =
{
    COM_UART0_TX_PIN,
    COM_UART1_TX_PIN,
    COM_USART2_TX_PIN,
    COM_USART5_TX_PIN
};
static const rcu_periph_enum COM_UART_RX_GPIO_CLK[COM_UART_MAX_NUM] =
{
    COM_UART0_RX_GPIO_CLK,
    COM_UART1_RX_GPIO_CLK,
    COM_USART2_RX_GPIO_CLK,
    COM_USART5_RX_GPIO_CLK
};
static const uint32_t COM_UART_RX_GPIO_AF[COM_UART_MAX_NUM] =
{
    COM_UART0_RX_GPIO_AF,
    COM_UART1_RX_GPIO_AF,
    COM_USART2_RX_GPIO_AF,
    COM_USART5_RX_GPIO_AF
};
static const uint32_t COM_UART_RX_PORT[COM_UART_MAX_NUM] =
{
    COM_UART0_RX_PORT,
    COM_UART1_RX_PORT,
    COM_USART2_RX_PORT,
    COM_USART5_RX_PORT
};
static const uint32_t COM_UART_RX_PIN[COM_UART_MAX_NUM] =
{
    COM_UART0_RX_PIN,
    COM_UART1_RX_PIN,
    COM_USART2_RX_PIN,
    COM_USART5_RX_PIN
};

static const uint32_t COM_UART_BAUD[COM_UART_MAX_NUM] =
{
    COM_UART0_BAUD,
    COM_UART1_BAUD,
    COM_USART2_BAUD,
    COM_USART5_BAUD
};

static const uint32_t COM_UART_DMA_RCU[COM_UART_MAX_NUM] =
{
    0,
    0,
    COM_USART2_DMA_RCU,
    COM_USART5_DMA_RCU
};


static const uint32_t COM_UART_DMA[COM_UART_MAX_NUM] =
{
    0,
    0,
    COM_USART2_DMA,
    COM_USART5_DMA
};

static const uint32_t COM_UART_TX_DMA_CHANNEL[COM_UART_MAX_NUM] =
{
    0,
    0,
    COM_USART2_TX_DMA_CHANNEL,
    COM_USART5_TX_DMA_CHANNEL
};

static const uint32_t COM_UART_RX_DMA_CHANNEL[COM_UART_MAX_NUM] =
{
    0,
    0,
    COM_USART2_RX_DMA_CHANNEL,
    COM_USART5_RX_DMA_CHANNEL
};

static const uint32_t COM_UART_RX_DMA_SUBPERI[COM_UART_MAX_NUM] =
{
    0,
    0,
    COM_USART2_RX_DMA_SUBPERI,
    COM_USART5_RX_DMA_SUBPERI
};

static const uint32_t COM_UART_TX_DMA_SUBPERI[COM_UART_MAX_NUM] =
{
    0,
    0,
    COM_USART2_TX_DMA_SUBPERI,
    COM_USART5_TX_DMA_SUBPERI
};

static const uint32_t COM_UART_RX_DMA_IRQ[COM_UART_MAX_NUM] =
{
    0,
    0,
    COM_USART2_RX_DMA_IRQ,
    COM_USART5_RX_DMA_IRQ
};


// clang-format on
void bsp_uart_periph_init(uart_type_def uart_num)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable( COM_UART_TX_GPIO_CLK[uart_num]);
    rcu_periph_clock_enable( COM_UART_RX_GPIO_CLK[uart_num]);

    /* enable USART clock */
    rcu_periph_clock_enable(COM_UART_CLK[uart_num]);

    /* connect port to USARTx_Tx */
    gpio_af_set(COM_UART_TX_PORT[uart_num], COM_UART_TX_GPIO_AF[uart_num], COM_UART_TX_PIN[uart_num]);

    /* connect port to USARTx_Rx */
    gpio_af_set(COM_UART_RX_PORT[uart_num], COM_UART_RX_GPIO_AF[uart_num], COM_UART_RX_PIN[uart_num]);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(COM_UART_TX_PORT[uart_num], GPIO_MODE_AF, GPIO_PUPD_PULLUP,COM_UART_TX_PIN[uart_num]);
    gpio_output_options_set(COM_UART_TX_PORT[uart_num], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,COM_UART_TX_PIN[uart_num]);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(COM_UART_RX_PORT[uart_num], GPIO_MODE_AF, GPIO_PUPD_PULLUP,COM_UART_RX_PIN[uart_num]);
    gpio_output_options_set(COM_UART_RX_PORT[uart_num], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,COM_UART_RX_PIN[uart_num]);

    /* USART configure */
    usart_deinit(COM_UART[uart_num]);
    usart_baudrate_set(COM_UART[uart_num],COM_UART_BAUD[uart_num]);
    usart_receive_config(COM_UART[uart_num], USART_RECEIVE_ENABLE);
    usart_transmit_config(COM_UART[uart_num], USART_TRANSMIT_ENABLE);
    usart_enable(COM_UART[uart_num]);

#ifndef ENABLE_USART_DMA
    /* USART interrupt configuration */
    if (uart_num == COM_UART_NUM0)
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 2, 0);
    }
    else if (uart_num == COM_UART_NUM1)
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 2, 0);
    }
    else if (uart_num == COM_UART_NUM2)
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 0, 0);
    }
    else if (uart_num == COM_UART_NUM5)
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 1, 0);
    }
    else
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 2, 0);
    }

    usart_interrupt_enable(COM_UART[uart_num], USART_INT_RBNE);
#else
    /* USART interrupt configuration */
    if (uart_num == COM_UART_NUM0)
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 2, 0);
    }
    else if (uart_num == COM_UART_NUM1)
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 2, 0);
    }
    else if (uart_num == COM_UART_NUM2)
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 0, 0);
    }
    else if (uart_num == COM_UART_NUM5)
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 1, 0);
    }
    else
    {
        nvic_irq_enable(COM_UART_IRQn[uart_num], 2, 0);
    }

    usart_interrupt_enable(COM_UART[uart_num], USART_INT_IDLE);
#endif
}

void bsp_dma_usart_int(uart_type_def uart_num, bool dma_tx_enable, bool dma_rx_enable)
{
    dma_single_data_parameter_struct dma_init_struct;

    if (dma_tx_enable)
    {
        /* enable DMA1 */
        rcu_periph_clock_enable(COM_UART_DMA_RCU[uart_num]);
        /* deinitialize DMA */
        dma_deinit(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num]);
        dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
        dma_init_struct.memory0_addr = (uint32_t)usart_tx_buffer[uart_num];
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
        dma_init_struct.number = USART_TX_BUFFER_SIZE;
        dma_init_struct.periph_addr = (uint32_t)&USART_DATA(COM_UART[uart_num]);
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
        dma_single_data_mode_init(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num], &dma_init_struct);
        /* configure DMA mode */
        dma_circulation_disable(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num]);
        dma_channel_subperipheral_select(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num], COM_UART_TX_DMA_SUBPERI[uart_num]);

        /* close DMA TX interrupt */
        //nvic_irq_enable(DMA1_Channel7_IRQn, 0, 0);
        //dma_interrupt_disable(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num], DMA_CHXCTL_FTFIE);
    }

    if (dma_rx_enable)
    {
        rcu_periph_clock_enable(COM_UART_DMA_RCU[uart_num]);

        dma_deinit(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num]);
        dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;

        if (uart_num == COM_UART_NUM5)
            dma_init_struct.memory0_addr = (uint32_t)usart5_rx_buffer;
        else
            dma_init_struct.memory0_addr = (uint32_t)usart_rx_buffer[uart_num];

        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;

        if (uart_num == COM_UART_NUM5)
            dma_init_struct.number = USART5_RX_BUFFER_SIZE;
        else
            dma_init_struct.number = USART_RX_BUFFER_SIZE;

        dma_init_struct.periph_addr = (uint32_t)&USART_DATA(COM_UART[uart_num]);
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
        dma_single_data_mode_init(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], &dma_init_struct);
        /* configure DMA mode */
        dma_circulation_disable(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num]);
        dma_channel_subperipheral_select(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], COM_UART_RX_DMA_SUBPERI[uart_num]);

        /* close DMA RX interrupt */
        nvic_irq_enable(COM_UART_RX_DMA_IRQ[uart_num], 1, 0);
        dma_interrupt_enable(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], DMA_CHXCTL_FTFIE);

        dma_channel_enable(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num]);
        usart_dma_receive_config(COM_UART[uart_num], USART_RECEIVE_DMA_ENABLE);
    }
}

void bsp_uart_init(void)
{
    for (uart_type_def uart = COM_UART_NUM0; uart < COM_UART_MAX_NUM; uart++)
    {
        bsp_uart_periph_init(uart);
    }
}

void bsp_usart_dma_send_data(uart_type_def uart_num ,uint8_t *buffer, uint16_t buffer_size)
{
    dma_flag_clear(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num], DMA_FLAG_FTF);
    dma_memory_address_config(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num], DMA_MEMORY_0, (uint32_t)buffer);
    dma_transfer_number_config(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num], buffer_size);
    dma_channel_enable(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num]);
    usart_dma_transmit_config(COM_UART[uart_num], USART_TRANSMIT_DMA_ENABLE);
}

void bsp_usart_dma_recv_data(uart_type_def uart_num, uint8_t *buffer, uint16_t buffer_size)
{
    if (dma_flag_get(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], DMA_FLAG_FTF))
    {
        dma_flag_clear(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], DMA_FLAG_FTF);
        dma_channel_enable(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num]);
        bsp_usart_dma_send_data(COM_UART_NUM2, buffer, buffer_size);
    }
}

#if !defined(__MICROLIB)
//不使用微库的话就需要添加下面的函数
#if (__ARMCLIB_VERSION <= 6000000)
//如果编译器是AC5  就定义下面这个结构体
struct __FILE
{
    int handle;
};
#endif

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
#endif

void usart_thread_init(void)
{
#if 1
    bsp_uart_periph_init(COM_UART_NUM5);
    bsp_uart_periph_init(COM_UART_NUM2);
#ifndef ENABLE_USART_DMA
#else
    bsp_dma_usart_int(COM_UART_NUM5, true, true);
    bsp_dma_usart_int(COM_UART_NUM2, true, true);

    uart_ringbuffer_init(COM_UART_NUM5, USART5_RX_BUFFER_SIZE * 2);
    uart_ringbuffer_init(COM_UART_NUM2, USART_RX_BUFFER_SIZE * 2);
#endif
#else
    usart_dma_test(COM_UART_NUM5);
#endif
}

char txbuffer[20] = "dma test success";
void usart_dma_test(uart_type_def uart_num)
{
    dma_single_data_parameter_struct dma_init_struct;

    /* enable GPIO clock */
    rcu_periph_clock_enable( COM_UART_TX_GPIO_CLK[uart_num]);
    rcu_periph_clock_enable( COM_UART_RX_GPIO_CLK[uart_num]);

    /* enable USART clock */
    rcu_periph_clock_enable(COM_UART_CLK[uart_num]);

    /* connect port to USARTx_Tx */
    gpio_af_set(COM_UART_TX_PORT[uart_num], COM_UART_TX_GPIO_AF[uart_num], COM_UART_TX_PIN[uart_num]);

    /* connect port to USARTx_Rx */
    gpio_af_set(COM_UART_RX_PORT[uart_num], COM_UART_RX_GPIO_AF[uart_num], COM_UART_RX_PIN[uart_num]);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(COM_UART_TX_PORT[uart_num], GPIO_MODE_AF, GPIO_PUPD_PULLUP,COM_UART_TX_PIN[uart_num]);
    gpio_output_options_set(COM_UART_TX_PORT[uart_num], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,COM_UART_TX_PIN[uart_num]);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(COM_UART_RX_PORT[uart_num], GPIO_MODE_AF, GPIO_PUPD_PULLUP,COM_UART_RX_PIN[uart_num]);
    gpio_output_options_set(COM_UART_RX_PORT[uart_num], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,COM_UART_RX_PIN[uart_num]);

    /* USART configure */
    usart_deinit(COM_UART[uart_num]);
    usart_baudrate_set(COM_UART[uart_num],COM_UART_BAUD[uart_num]);
    usart_receive_config(COM_UART[uart_num], USART_RECEIVE_ENABLE);
    usart_transmit_config(COM_UART[uart_num], USART_TRANSMIT_ENABLE);
    usart_enable(COM_UART[uart_num]);

    while(1) {
        /* deinitialize DMA channel2 (USART0 RX) */
        dma_deinit(COM_UART_DMA[uart_num], COM_UART_TX_DMA_CHANNEL[uart_num]);
        usart_flag_clear(COM_UART[uart_num], USART_FLAG_RBNE);
        usart_dma_receive_config(COM_UART[uart_num], USART_RECEIVE_DMA_ENABLE);

        dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
        dma_init_struct.memory0_addr = (uint32_t)usart_rx_buffer;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.number = 2;
        dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(COM_UART[COM_UART_NUM5]));
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_single_data_mode_init(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], &dma_init_struct);
        /* configure DMA mode */
        dma_circulation_disable(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num]);
        dma_channel_subperipheral_select(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], DMA_SUBPERI5);
        /* enable DMA channel2 */
        dma_channel_enable(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num]);

        /* wait DMA channel transfer complete */
        while(RESET == dma_flag_get(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], DMA_FLAG_FTF)){};
            dma_flag_clear(COM_UART_DMA[uart_num], COM_UART_RX_DMA_CHANNEL[uart_num], DMA_FLAG_FTF);

        //usart_dma_receive_config(COM_UART[uart_num], USART_RECEIVE_DMA_DISABLE);
        //printf("\n\r%s\n\r", rxbuffer);
    }
}

uint8_t test_buffer[30] = {0};
void usart_thread_run(void)
{
#if 0
    sprintf((char *)test_buffer, "%d %d %d %d %d",
            get_switch_value(SWITCH_0),
            get_switch_value(SWITCH_1),
            get_switch_value(SWITCH_2),
            get_switch_value(SWITCH_3),
            get_switch_value(SWITCH_4)
           );
#endif
    if (uart_ringbuffer_pop_data(COM_UART_NUM2, test_buffer, 30) != 0)
    {
        bsp_usart_dma_send_data(COM_UART_NUM2, test_buffer, 30);
    }

    if (uart_ringbuffer_pop_data(COM_UART_NUM5, test_buffer, 30) != 0)
    {
        bsp_usart_dma_send_data(COM_UART_NUM5, test_buffer, 30);
    }

    //bsp_usart_dma_recv_data(COM_UART_NUM5, test_buffer, 30);
}

void DMA1_Channel7_IRQHandler()
{
    if(dma_interrupt_flag_get(DMA1, DMA_CH7, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(DMA1, DMA_CH7, DMA_INT_FLAG_FTF);
    }
}

/* usart5 RX dma IRQ */
void DMA1_Channel2_IRQHandler()
{
    if(dma_interrupt_flag_get(COM_UART_DMA[COM_UART_NUM5], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM5], DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(COM_UART_DMA[COM_UART_NUM5], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM5], DMA_INT_FLAG_FTF);
        dma_channel_enable(COM_UART_DMA[COM_UART_NUM5], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM5]);
    }
}

/* usart2 RX dma IRQ */
void DMA0_Channel1_IRQHandler()
{
    if(dma_interrupt_flag_get(COM_UART_DMA[COM_UART_NUM2], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM2], DMA_INT_FLAG_FTF))
    {
        dma_interrupt_flag_clear(COM_UART_DMA[COM_UART_NUM2], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM2], DMA_INT_FLAG_FTF);
        dma_channel_enable(COM_UART_DMA[COM_UART_NUM2], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM2]);
    }
}

void USART2_IRQHandler()
{
    uint16_t recv_num = 0;
    if((RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE)) &&
            (RESET != usart_flag_get(USART2, USART_FLAG_IDLE)))
    {
        USART_STAT0(USART2);
        USART_DATA(USART2);
        recv_num = USART_RX_BUFFER_SIZE - dma_transfer_number_get(COM_UART_DMA[COM_UART_NUM2], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM2]);
        uart_ringbuffer_push_data(COM_UART_NUM2, usart_rx_buffer[COM_UART_NUM2], recv_num);
        dma_channel_disable(COM_UART_DMA[COM_UART_NUM2], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM2]);
        dma_transfer_number_config(COM_UART_DMA[COM_UART_NUM2], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM2], USART_RX_BUFFER_SIZE);
        dma_channel_enable(COM_UART_DMA[COM_UART_NUM2], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM2]);

    }

}

void USART5_IRQHandler()
{
    uint16_t recv_num = 0;
    if((RESET != usart_interrupt_flag_get(USART5, USART_INT_FLAG_IDLE)) &&
            (RESET != usart_flag_get(USART5, USART_FLAG_IDLE)))
    {        
        USART_STAT0(USART5);
        USART_DATA(USART5);
        recv_num = USART5_RX_BUFFER_SIZE - dma_transfer_number_get(COM_UART_DMA[COM_UART_NUM5], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM5]);
        uart_ringbuffer_push_data(COM_UART_NUM5, usart5_rx_buffer, recv_num);
        dma_channel_disable(COM_UART_DMA[COM_UART_NUM5], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM5]);
        dma_transfer_number_config(COM_UART_DMA[COM_UART_NUM5], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM5], USART5_RX_BUFFER_SIZE);
        dma_channel_enable(COM_UART_DMA[COM_UART_NUM5], COM_UART_RX_DMA_CHANNEL[COM_UART_NUM5]);
    }
}


#if 1
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(COM_UART[COM_UART_NUM5], (uint8_t)ch);
    while(RESET == usart_flag_get(COM_UART[COM_UART_NUM5], USART_FLAG_TBE));
    return ch;
}

#endif

