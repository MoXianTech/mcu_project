#include "bsp_power_check.h"
#include "board.h"
#include "bsp_led.h"
#include "bsp_flash_operate.h"
#include "bsp_encoder.h"
#include "PID_moto_control.h"

#define WRITE_ADDRESS_BYTE          ((uint32_t)0x0807f800)

typedef struct {
    int16_t moto_pos_info[4];
    int8_t moto_flag[4];
} power_save_t;

void power_save_thread(void)
{
		power_save_t power_save_1 = {0};
    lock_encoder_currentCount(MOTO_MAX_NUM);

    power_save_1.moto_pos_info[MOTO_A] = get_encoder_currentCount(MOTO_A);
    power_save_1.moto_pos_info[MOTO_B] = get_encoder_currentCount(MOTO_B);
    power_save_1.moto_pos_info[MOTO_C] = get_encoder_currentCount(MOTO_C);
    power_save_1.moto_pos_info[MOTO_D] = get_encoder_currentCount(MOTO_D);

    power_save_1.moto_pos_info[MOTO_D] = 40;
    printf("%s MOTO_B %d  %d\n", __FUNCTION__,
                            get_encoder_currentCount(MOTO_B),
                            power_save_1.moto_pos_info[MOTO_B]
                          );

    fmc_write_8bit_data(WRITE_ADDRESS_BYTE, sizeof(power_save_1), (int8_t *)&power_save_1);

    printf("%s %d %d %d %d\n", __FUNCTION__,
                          power_save_1.moto_pos_info[MOTO_A],
                          power_save_1.moto_pos_info[MOTO_B],
                          power_save_1.moto_pos_info[MOTO_C],
                          power_save_1.moto_pos_info[MOTO_D]
                          );
}

void power_load_thread(void)
{
    power_save_t power_save = {0};

    lock_encoder_currentCount(MOTO_MAX_NUM);

    fmc_read_8bit_data(WRITE_ADDRESS_BYTE, sizeof(power_save), (int8_t *)&power_save);

#if 0
    set_encoder_currentCount(MOTO_A, power_save.moto_pos_info[MOTO_A]);
    set_encoder_currentCount(MOTO_B, power_save.moto_pos_info[MOTO_B]);
    set_encoder_currentCount(MOTO_C, power_save.moto_pos_info[MOTO_C]);
    set_encoder_currentCount(MOTO_D, power_save.moto_pos_info[MOTO_D]);
#endif
    printf("%s %d %d %d %d\n", __FUNCTION__,
                          power_save.moto_pos_info[0],
                          power_save.moto_pos_info[1],
                          power_save.moto_pos_info[2],
                          power_save.moto_pos_info[3]
                          );

    unlock_encoder_currentCount(MOTO_MAX_NUM);
}


void powercheck_thread_init(void)
{
    rcu_periph_clock_enable(RCU_POWER_CHECK);
    gpio_mode_set(PORT_POWER_CHECK, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_POWER_CHECK);

#ifdef ENABLE_EXTI_POWERCHECK
    nvic_irq_enable(EXTI10_15_IRQn, 3U, 3U);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOE, EXTI_SOURCE_PIN13);
    exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_enable(EXTI_13);
    exti_interrupt_flag_clear(EXTI_13);
#endif
}

void powercheck_thread_run(void)
{
    static uint8_t save_flag = 1;
    if(save_flag == 1 && GET_POWER_CHECK_IN == 0)
    {
        bsp_led_off(LED1);
        save_flag = 0;
        power_load_thread();
        set_moto_thread_status(1);

    }

    if(save_flag == 0 && GET_POWER_CHECK_IN == 1)
    {
        bsp_led_on(LED1);
        save_flag = 1;
        //set_moto_thread_status(0);
        power_save_thread();
    }
}
