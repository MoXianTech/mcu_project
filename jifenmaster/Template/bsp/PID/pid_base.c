/**
 ******************************************************************************
 * @file    pid.c
 * @author  Ginger
 * @version V1.0.0
 * @date    2015/11/14
 * @brief   ??ÿһ??pid?ṹ?嶼Ҫ?Ƚ??к?????��?ӣ??ٽ??г?ʼ??
 ******************************************************************************
 * @attention Ӧ?????ö??ײ???(d)??̨???????ȶ?
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "PID_base.h"

#define ABS(x)		((x>0)? x: -x)

PID_TypeDef pid_pitch,pid_pithch_speed,pid_roll,pid_roll_speed,pid_yaw_speed;

/*??????ʼ??--------------------------------------------------------------*/
static void pid_param_init(
        PID_TypeDef * pid,
        PID_ID   id,
        uint16_t maxout,
        uint16_t intergral_limit,
        int16_t deadband,
        uint16_t period,
        int16_t  max_err,
        int16_t  target,

        int16_t 	kp,
        float   	ki,
        int16_t 	kd)
{
    pid->id = id;

    pid->ControlPeriod = period;             //û?õ?
    pid->DeadBand = deadband;
    pid->IntegralLimit = intergral_limit;
    pid->MaxOutput = maxout;
    pid->Max_Err = max_err;
    pid->target = target;

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->output = 0;
}

/*??;???Ĳ????趨--------------------------------------------------------------*/
static void pid_reset(PID_TypeDef * pid, int16_t kp, float ki, int16_t kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

/*pid????-----------------------------------------------------------------------*/


static int16_t pid_calculate(PID_TypeDef* pid, int16_t measure)//, int16_t target)
{

    pid->measure = measure;
    //	pid->target = target;

    pid->last_err  = pid->err;
    pid->last_output = pid->output;

    pid->err = pid->target - pid->measure;

    //?Ƿ?????????
    if((ABS(pid->err) > pid->DeadBand))
    {
        pid->pout = pid->kp * pid->err;
        pid->iout += (pid->ki * pid->err);


        pid->dout =  pid->kd * (pid->err - pid->last_err);

        //?????Ƿ񳬳?????
        if(pid->iout > pid->IntegralLimit)
            pid->iout = pid->IntegralLimit;
        if(pid->iout < - pid->IntegralLimit)
            pid->iout = - pid->IntegralLimit;

        //pid??????
        pid->output = pid->pout + pid->iout + pid->dout;


        pid->output = pid->output * 0.3f + pid->last_output * 0.7f;  //?˲???
        if(pid->output>pid->MaxOutput)
        {
            pid->output = pid->MaxOutput;
        }
        if(pid->output < -(pid->MaxOutput))
        {
            pid->output = -(pid->MaxOutput);
        }

    }


    return pid->output;
}

void pid_set_target(PID_TypeDef *pid, int16_t target)
{
    pid->target = target;
}

/*pid?ṹ????ʼ????ÿһ??pid??????Ҫ????һ??-----------------------------------------------------*/
void pid_init(PID_TypeDef* pid)
{
    pid->f_param_init = pid_param_init;
    pid->f_pid_reset = pid_reset;
    pid->f_cal_pid = pid_calculate;
    pid->f_set_target = pid_set_target;
}
