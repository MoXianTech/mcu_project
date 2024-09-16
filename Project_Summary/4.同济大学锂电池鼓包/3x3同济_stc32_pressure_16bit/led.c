#include "led.h"




void led_GPIO_config(void)
{
	
	P2_MODE_OUT_PP(GPIO_Pin_3);
	
}
