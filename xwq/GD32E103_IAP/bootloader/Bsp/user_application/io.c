#include "io.h"

void io_init(void)
{
		rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
		gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
		gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15 );
}

void ledG_on(void)
{
    gpio_bit_reset(GPIOA, GPIO_PIN_15);
}
void ledG_off(void)
{
    gpio_bit_set(GPIOA, GPIO_PIN_15);
}
void ledG_not(void)
{
    gpio_bit_write(GPIOA,GPIO_PIN_15,(bit_status)(!gpio_input_bit_get(GPIOA, GPIO_PIN_15)));
}





