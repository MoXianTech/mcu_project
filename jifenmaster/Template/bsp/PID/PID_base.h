#ifndef _PID_BASE_H
#define _PID_BASE_H

#include "stdint.h"
typedef enum
{
    PID_Position,
    PID_Speed
}PID_ID;

typedef struct _PID_TypeDef
{
    PID_ID id;

    int16_t target;							//Ä¿±êÖµ
    int16_t kp;
    float ki;
    int16_t kd;

    int16_t measure;					//²âÁ¿Öµ
    int16_t err;							//Îó²î
    int16_t last_err;      		//ÉÏ´ÎÎó²î

    int16_t pout;
    float iout;
    int16_t dout;

    int16_t output;						//±¾´ÎÊä³ö
    int16_t last_output;			//ÉÏ´ÎÊä³ö

    int16_t MaxOutput;				//Êä³öÏÞ·ù
    float IntegralLimit;		//»ý·ÖÏÞ·ù
    int16_t DeadBand;			  //ËÀÇø£¨¾ø¶ÔÖµ£©
    int16_t ControlPeriod;		//¿ØÖÆÖÜÆÚ
    int16_t  Max_Err;					//×î´óÎó²î

    void (*f_param_init)(struct _PID_TypeDef *pid,  //PID²ÎÊý³õÊ¼»¯
            PID_ID id,
            uint16_t maxOutput,
            uint16_t integralLimit,
            int16_t deadband,
            uint16_t controlPeriod,
            int16_t max_err,     
            int16_t  target,
            int16_t kp,
            float ki,
            int16_t kd);

    void (*f_pid_reset)(struct _PID_TypeDef *pid, int16_t kp, float ki, int16_t kd);		//pidÈý¸ö²ÎÊýÐÞ¸Ä
    int16_t (*f_cal_pid)(struct _PID_TypeDef *pid, int16_t measure);   //pid¼ÆËã
    void (*f_set_target)(struct _PID_TypeDef *pid, int16_t target);   //pid¼ÆËã

}PID_TypeDef;

void pid_init(PID_TypeDef* pid);
#endif
