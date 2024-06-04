#include "led.h"

void led_gpio_init(void)
{
	  rcu_periph_clock_enable(RCU_GPIOB);
		gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4  | GPIO_PIN_5);
		GREEN_LED(0);
		RED_LED(0);
}


void led_spark(void)
{
		static bool led_flag = false;
		led_flag = !led_flag;
		GREEN_LED(led_flag);
		RED_LED(led_flag);
}
