#include "thread_init.h"
#include "temp_humi_thread.h"
#include "lwip_thread.h"

#define TEMP_HUMI_THREAD_PRIO      2                   /* 任务优先级 */
#define TEMP_HUMI_THREAD_STK_SIZE  5*1024              /* 任务堆栈大小 */
TaskHandle_t TEMP_HUMI_THREAD_Handler;  /* 任务句柄 */

#define LWIP_THREAD_PRIO      3              /* 任务优先级 */
#define LWIP_THREAD_STK_SIZE  10*1024              /* 任务堆栈大小 */
TaskHandle_t LWIP_THREAD_Handler;  /* 任务句柄 */

thread_pvparam_t thread_pvparam = {0, 0xff};

void thraed_work_init(void)
{

#if 1
    xTaskCreatePinnedToCore((TaskFunction_t )temp_humi_thread,
                            (const char *)  "TEMP_HUMI_THREAD",
                            (uint16_t)       TEMP_HUMI_THREAD_STK_SIZE,
                            (void *)         &thread_pvparam,
                            (UBaseType_t)    TEMP_HUMI_THREAD_PRIO,
                            (TaskHandle_t *) &TEMP_HUMI_THREAD_Handler,
                            (BaseType_t)     0);
#endif
    xTaskCreatePinnedToCore((TaskFunction_t )lwip_thread,
                            (const char *)  "LWIP_THREAD",
                            (uint16_t)       LWIP_THREAD_STK_SIZE,
                            (void *)         &thread_pvparam,
                            (UBaseType_t)    LWIP_THREAD_PRIO,
                            (TaskHandle_t *) &LWIP_THREAD_Handler,
                            (BaseType_t)     0);

}
