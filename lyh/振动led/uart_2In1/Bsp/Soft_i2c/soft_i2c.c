#include "soft_i2c.h"
#include "systick.h"

// 假设的设备地址和寄存器地址
typedef uint8_t tws_chip_index_e;
typedef uint8_t TWS_U8;
typedef uint16_t TWS_U16;
typedef uint32_t TWS_U32;
typedef int8_t TWS_S8;
typedef enum {
    TWS_CMD_START,
    TWS_CMD_COMP,
    TWS_CMD_ENTER_SLEEP,
    TWS_CMD_EXITS_SLEEP,
    TWS_CMD_IDLE,
    TWS_CMD_SCAN,
    TWS_CMD_RESET
} enum_tws_cmd_e;

#define TWS_RET_OK 0

// 全局回调结构体
struct {
    int (*tws_I2c_Write_f)(tws_chip_index_e, TWS_U16, TWS_U8*, uint8_t);
} g_tws_cb;


// 简单延时，用于模拟I2C时序
static void i2c_delay(void) {
    for(volatile uint32_t i = 0; i < 100; i++);
}

// 初始化 I2C 引脚为开漏输出
void i2c_init(void) {
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(I2C_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, SCL_PIN | SDA_PIN);
    SCL_HIGH();
    SDA_HIGH();
}

// I2C 起始信号
void i2c_start(void) {
    SDA_OUT();      // 设置为输出
    SDA_HIGH();
    SCL_HIGH();
    i2c_delay();
    SDA_LOW();
    i2c_delay();
    SCL_LOW();
}

// I2C 停止信号
void i2c_stop(void) {
    SDA_OUT();
    SDA_LOW();
    SCL_HIGH();
    i2c_delay();
    SDA_HIGH();
    i2c_delay();
}

// 发送一个字节并读取 ACK
uint8_t i2c_send_byte(uint8_t byte) {
    SDA_OUT();
    for(uint8_t i = 0; i < 8; i++) {
        if(byte & 0x80) SDA_HIGH();
        else SDA_LOW();
        byte <<= 1;

        i2c_delay();
        SCL_HIGH();
        i2c_delay();
        SCL_LOW();
    }

    // 接收 ACK
    SDA_IN();       // 设置为输入
    i2c_delay();
    SCL_HIGH();
    uint8_t ack = SDA_READ();
    i2c_delay();
    SCL_LOW();
    SDA_OUT();      // 设置回输出
    return ack;
}

// 接收一个字节
uint8_t i2c_receive_byte(uint8_t ack) {
    uint8_t byte = 0;
    SDA_IN();  // 设置为输入
    for(uint8_t i = 0; i < 8; i++) {
        byte <<= 1;
        SCL_HIGH();
        i2c_delay();
        if(SDA_READ()) byte |= 0x01;
        SCL_LOW();
        i2c_delay();
    }

    SDA_OUT(); // 设置为输出
    if(ack) SDA_LOW();    // 发送ACK
    else SDA_HIGH();      // 发送NACK

    i2c_delay();
    SCL_HIGH();
    i2c_delay();
    SCL_LOW();
    SDA_HIGH();  // 释放 SDA
    return byte;
}
void i2c_write_bytes(uint8_t device_addr, uint8_t reg_addr, uint8_t *buf, uint8_t len) {
    i2c_start();
    i2c_send_byte(device_addr << 1 | 0);  // 写命令
    i2c_send_byte(reg_addr);             // 起始寄存器
    for(uint8_t i = 0; i < len; i++) {
        i2c_send_byte(buf[i]);
    }
    i2c_stop();
}
void i2c_read_bytes(uint8_t device_addr, uint8_t reg_addr, uint8_t *buf, uint8_t len) {
    i2c_start();
    i2c_send_byte(device_addr << 1 | 0);  // 写命令
    i2c_send_byte(reg_addr);             // 寄存器地址

    i2c_start();                         // 重启
    i2c_send_byte(device_addr << 1 | 1); // 读命令

    for(uint8_t i = 0; i < len; i++) {
        buf[i] = i2c_receive_byte(1);  // 最后一个字节发送NACK
    }

    i2c_stop();
}

void cvt213x_write_bytes(uint16_t reg_addr, uint8_t *buf, uint8_t len)
{
    uint8_t reg_addr_h = (reg_addr >> 8) & 0xFF;
    uint8_t reg_addr_l = reg_addr & 0xFF;

    // 起始信号 + 写设备地址
    i2c_start();
    i2c_send_byte(CVT213X_I2C_8BITS_ADDRESS_W);  // 发送写地址 0x50
    
    // 发送寄存器地址（16位）
    i2c_send_byte(reg_addr_h);                   // 地址高位
    i2c_send_byte(reg_addr_l);                   // 地址低位

    // 写入数据字节
    for (uint8_t i = 0; i < len; i++) {
        i2c_send_byte(buf[i]);
    }

    // 停止信号
    i2c_stop();
}
void cvt213x_read_bytes(uint16_t reg_addr, uint8_t *buf, uint8_t len)
{
    uint8_t reg_addr_h = (reg_addr >> 8) & 0xFF;
    uint8_t reg_addr_l = reg_addr & 0xFF;

    // 起始信号 + 写设备地址
    i2c_start();
    i2c_send_byte(CVT213X_I2C_8BITS_ADDRESS_W);  // 发送写地址 0x50
    i2c_send_byte(reg_addr_h);                   // 地址高位
    i2c_send_byte(reg_addr_l);                   // 地址低位

    // 重启 + 读设备地址
    i2c_start();
    i2c_send_byte(CVT213X_I2C_8BITS_ADDRESS_R);  // 发送读地址 0x51

    // 读取 len 个字节，每个都发 ACK（符合 CVT213x 需求）
    for (uint8_t i = 0; i < len; i++) {
        uint8_t should_ack = (i < len - 1);
				buf[i] = i2c_receive_byte(should_ack);
    }

    // 停止信号
    i2c_stop();
}

void write_register_init(){
	// 起始信号 + 写设备地址
    i2c_start();
    i2c_send_byte(CVT213X_I2C_8BITS_ADDRESS_W);  // 发送写地址 0x50
		i2c_send_byte(0x40);
		i2c_send_byte(0x08);
		i2c_send_byte(0x01);
		i2c_stop();
	
	
	
}

int32_t read_capacitance(uint8_t phase) {
    uint8_t reg_addr = PH0_DATA_REG + (phase * 0x100); // 计算寄存器地址
    uint8_t raw_data[4] = {0}; // CVT213x 电容数据为 3 字节

    // 读取 3 字节原始数据
    cvt213x_read_bytes(reg_addr, raw_data, 4);

    // 解析为 24bit 有符号整数（小端格式）
    int32_t cap_value = (raw_data[2] << 16) | (raw_data[1] << 8) | raw_data[0];
    if (raw_data[2] & 0x10) { // 处理负数（最高位为符号位）
        cap_value -= 0x1000000;
    }

    return cap_value;
}
/**
 * @brief 扫描所有电容通道并返回数据
 * @param cap_values 输出数组，用于存储各通道电容值
 * @note 数组长度需 >= NUM_PHASES
 */
void scan_all_capacitance(int32_t cap_values[]) {
    for (uint8_t phase = 0; phase < NUM_PHASES; phase++) {
        cap_values[phase] = read_capacitance(phase);
    }
}

void HXf72_write_bytes(uint8_t reg_addr, uint8_t *buf, uint8_t len)
{
    

    // 起始信号 + 写设备地址
    i2c_start();
    i2c_send_byte(CVT213X_I2C_8BITS_ADDRESS_W);  // 发送写地址 0x50
    
    // 发送寄存器地址（8位）
    i2c_send_byte(reg_addr);                   // 地址高位

    // 写入数据字节
    for (uint8_t i = 0; i < len; i++) {
        i2c_send_byte(buf[i]);
    }

    // 停止信号
    i2c_stop();
}

void HXf72_read_bytes(uint8_t reg_addr, uint8_t *buf, uint8_t len)
{
		
    // 起始信号 + 写设备地址
    i2c_start();
    i2c_send_byte(CVT213X_I2C_8BITS_ADDRESS_W);  // 发送写地址 0x50
    i2c_send_byte(reg_addr);                   // 地址高位

    // 重启 + 读设备地址
    i2c_start();
    i2c_send_byte(CVT213X_I2C_8BITS_ADDRESS_R);  // 发送读地址 0x51

    // 读取 len 个字节，每个都发 ACK（符合 CVT213x 需求）
    for (uint8_t i = 0; i < len; i++) {
        uint8_t should_ack = (i < len - 1);
				buf[i] = i2c_receive_byte(should_ack);
		
    }

    // 停止信号
    i2c_stop();
	
}

void get_date_hx(uint8_t *buf,uint8_t *buf2){
		HXf72_write_bytes(0x61, (uint8_t[]){0x00}, 1);
    int counter = 0;
    // 2. 启用调试模式（设置寄存器0x60为0x01）
    HXf72_write_bytes(0x60, (uint8_t[]){0xf4}, 1);
//		while(1){
//			HXf72_read_bytes(0x61,(uint8_t *)buf2,1);
//			if(buf2[counter] == 0){
//				break;
//			}
//			else
//				counter++;
//		}
    HXf72_read_bytes(0x62,(uint8_t *)buf,30);
    // 3. 等待数据就绪（可能需要延时或轮询状态）
    // delay_ms(10); // 根据传感器需求调整延时
		HXf72_write_bytes(0x60, (uint8_t[]){0x00}, 1);
}
