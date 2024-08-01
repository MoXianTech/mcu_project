#include "AHT20.h"

i2c_obj_t ATH20_master = {0};

/**
 * @brief       读取ATH20的16位IO值
 * @param       data:存储区
 * @param       reg :寄存器
 * @retval      ESP_OK:读取成功；其他:读取失败
 */
static esp_err_t ATH20_read_byte(uint8_t reg, uint8_t* data, uint16_t size)
{
    uint8_t reg_buf[1];

    reg_buf[0] = reg;

    i2c_buf_t buf[2] = {
        {.len = 1, .buf = reg_buf},
        {.len = size, .buf = data},
    };

    return i2c_transfer(&ATH20_master, ATH20_ADDR, 2, buf, I2C_FLAG_WRITE | I2C_FLAG_READ | I2C_FLAG_STOP);
}

/**
 * @brief       向ATH20写入16位IO值
 * @param       data:要写入的数据
 * @retval      ESP_OK:读取成功；其他:读取失败
 */
static esp_err_t ATH20_write_byte(uint8_t reg, uint8_t* data, uint16_t data_size)
{
    uint8_t reg_buf[1];

    reg_buf[0] = reg;

    i2c_buf_t buf[2] = {
        {.len = 1, .buf = reg_buf},
        {.len = data_size, .buf = data},
    };

    i2c_transfer(&ATH20_master, ATH20_ADDR, 2, buf, I2C_FLAG_STOP); /* 传输传感器地址 */
    return ESP_OK;
}

/**
 * @brief       读取ATH20的数据
 * @note        读取原始数据，包括ALS,PS和IR
 *              如果同时打开ALS,IR+PS的话两次数据读取的时间间隔要大于112.5ms
 * @param       temp  :温度传感器
 * @param       humi  :湿度传感器
 * @retval      无
 */
void ATH20_read_data(uint16_t *temp_value, uint16_t *humi_value)
{
    uint8_t buf[8] = {0};
    uint32_t temp = 0x00;
    uint32_t humi = 0x00;

    {
        buf[0] = 0x33;
        buf[1] = 0x00;
        ATH20_write_byte(ATH20_START_TEST, buf, 2);
        vTaskDelay(50);
    }

    {
        ATH20_read_byte(ATH20_START_STATE, buf, 8);
    }
    
    humi = (humi | buf[1]) << 8;
    humi = (humi | buf[2]) << 8;
    humi = (humi | buf[3]) >> 4;

    temp = (temp | (buf[3] & 0x0f)) << 8;
    temp = (temp | buf[4]) << 8;
    temp = (temp | buf[5]);

    *temp_value = (temp * 200 * 10) / 1024/1024 - 500;
    *humi_value = (humi * 1000) / 1024 / 1024 ;
}

/**
 * @brief       初始化ATH20
 * @param       无
 * @retval      0, 成功;
 1, 失败;
 */
uint8_t ATH20_config(void)
{
    uint8_t temp[10];

    temp[0] = 0x08;
    temp[1] = 0x00;

    ATH20_write_byte(ATH20_INIT, temp, 2); /* 复位ATH20 */
    vTaskDelay(5);

    ATH20_read_byte(ATH20_START_STATE, temp, 1);
    if (temp[0] == 0X18)
    {
        ESP_LOGI("ATH20", "ATH20 success!!!");
        return 0;                       /* ATH20正常 */
    }
    else
    {
        ESP_LOGE("ATH20", "ATH20 fail!!!");
        return 1;                       /* ATH20失败 */
    }
}

/**
 * @brief       初始化 ATH20
 * @param       无
 * @retval      无
 */
void ATH20_init(i2c_obj_t self)
{
    uint8_t value = 0;
    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_1);    /* 初始化IIC */
    }

    ATH20_master = self;

    while (ATH20_config())    /* 检测不到ATH20 */
    {
        ESP_LOGE("ATH20", "ATH20 init fail!!!");
        vTaskDelay(500);
    }
}


