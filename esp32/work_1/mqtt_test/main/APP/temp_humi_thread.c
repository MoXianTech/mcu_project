#include "temp_humi_thread.h"
#include "iic.h"
#include "AHT20.h"

void temp_humi_thread(void *pvparams)
{
    thread_pvparam_t *thread_pvparam = (thread_pvparam_t *)pvparams; 
    i2c_obj_t i2c1_master = {0};
    queue_temp_humi_t queue_temp_humi = {0};

    i2c1_master = iic_init(I2C_NUM_1);
    ATH20_init(i2c1_master);
    uint16_t temp = 0x00, humi = 0x00;
    
    thread_pvparam->queue_temp_humi = xQueueCreate(1, sizeof(queue_temp_humi_t));
    while (1)
    {
        ATH20_read_data(&temp, &humi);
        queue_temp_humi.temp_value = temp / 10.0;
        queue_temp_humi.humi_value = humi / 10.0;
        xQueueSend(thread_pvparam->queue_temp_humi, &queue_temp_humi, 100);

        ESP_LOGI(__FUNCTION__, "temp %0.1f, humi %0.1f tags 0x%X",
                                temp / 10.0, humi / 10.0, thread_pvparam->tags);
        vTaskDelay(1000);
    }

}
