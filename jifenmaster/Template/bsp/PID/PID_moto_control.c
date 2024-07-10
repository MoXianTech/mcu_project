#include "PID_moto_control.h"
#include "bsp_encoder.h"


PID_TypeDef motor_pid_4;
PID_TypeDef motor_pid_2;
uint8_t moto_thread_enable = 0;
void PID_thread_init(void)
{
    pid_init(&motor_pid_4);
    pid_init(&motor_pid_2);
    motor_pid_4.f_param_init(&motor_pid_4,
            PID_Position,
            400,
            150,
            0,
            0,
            0,
            0,
            70,
            0,
            0);
    motor_pid_2.f_param_init(&motor_pid_2,
            PID_Position,
            400,
            150,
            0,
            0,
            0,
            0,
            70,
            0,
            0);

}

void set_moto_thread_status(uint8_t enable)
{
    moto_thread_enable = enable;
    if (!enable)
    {
        set_moto_control(MOTO_4, 0);
        set_moto_control(MOTO_2, 0);
    }
}

void moto_PID_thread_run(void)
{
    if (moto_thread_enable)
    {
        motor_pid_4.f_cal_pid(&motor_pid_4, get_encoder_currentCount(MOTO_D));
        motor_pid_2.f_cal_pid(&motor_pid_2, get_encoder_currentCount(MOTO_B));
        set_moto_control(MOTO_4, motor_pid_4.output);
        set_moto_control(MOTO_2, motor_pid_2.output);
    } else {
        set_moto_control(MOTO_4, 0);
        set_moto_control(MOTO_2, 0);
    }
}

void set_moto_stop(MOTO_NUM moto_num)
{
    switch (moto_num)
    {
        case MOTO_2:
            set_moto_target(moto_num, motor_pid_2.measure);
            break;
        case MOTO_4:
            set_moto_target(moto_num, motor_pid_4.measure);
            break;

        default:
            break;
    }
}

void set_moto_target(MOTO_NUM moto_num, int16_t pos_value)
{
    switch(moto_num)
    {
        case MOTO_1:
            break;
        case MOTO_2:
            motor_pid_2.f_set_target(&motor_pid_2, pos_value);
            break;
        case MOTO_3:
            break;
        case MOTO_4:
            motor_pid_4.f_set_target(&motor_pid_4, pos_value);
            break;
defalut:
            break;
    }
}

