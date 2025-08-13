/**
* @file                framework_sample.c
* @brief
* @author
* @version             V0.1.0
* @date                2020.7.1
* @details
* @copyright           (C) COPYRIGHT 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/
/** @addtogroup Framework层
  * @{
  */
/**
 * @defgroup AFE采样模块
 * @brief
 * @details
 * @{
 */
//hal
#include "hal_afe.h"
#include "hal_tk.h"

//framework
#include "framework_sample.h"
#include "framework_debug.h"
#include "framework_pmu.h"
#include "framework_reg.h"

//config
#include "user_config.h"

#define NEG(x)      ((x > 0) ? (-x) : (x))
#define ABS(x)      (((x) > 0) ? (x) : (-(x)))
#define MIN(x, y)   ((x > y) ? (y) : (x))
#define MAX(x, y)   ((x > y) ? (x) : (y))
#define RANGE(x, min, max)  MIN(MAX(x, min), max)
#define SQUARE(x)   ((x)*(x))

static int16_t fw_sample_rawdata[FW_SAMPLE_RAWDATA_NUM];
static uint16_t fw_noise_cal_num = 0;
static uint8_t flag_noise_cal_finish = 0;
static int16_t fw_rawdata_buf[FW_SAMPLE_RAWDATA_NUM];
static int16_t fw_noise_peak[FW_SAMPLE_RAWDATA_NUM];
static int32_t fw_noise_dev_sum[FW_SAMPLE_RAWDATA_NUM];

static int16_t fw_sample_rawdata_bak[FW_SAMPLE_RAWDATA_NUM][FW_SAMPLE_RAWDATA_SMOOTH_LEVEL];

static uint8_t flag_rawdata_bak_finish = 0;

static uint16_t sensor_connect_status[FW_SAMPLE_RAWDATA_NUM] = {0};

static int16_t fw_sample_filter_rawdata_bak[FW_SAMPLE_RAWDATA_NUM][2];

/**
* @fn static void fw_sample_rawdata_smooth(void)
* @brief rawdata smooth
* @param None
* @return None.
*/
static void fw_sample_rawdata_smooth(void)                  // rawdata 平滑处理
{
    char i, j;
    int32_t sum;

    if(FW_SAMPLE_RAWDATA_SMOOTH_LEVEL)                                  // 平滑级别不为0才生效
    {
        if(flag_rawdata_bak_finish == 0)                                // 判断是否已完成初始化备份
        {
            for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
            {
                for(j = 0; j < FW_SAMPLE_RAWDATA_SMOOTH_LEVEL; j++)
                {
                    fw_sample_rawdata_bak[i][j] = fw_sample_rawdata[i];
                }
            }
            flag_rawdata_bak_finish = 1;
        }

        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)                      // 对各通道进行数据平滑
        {
            sum = 0;

            for(j = 0; j < (FW_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j++)   // 计算备份值和
            {
                sum += fw_sample_rawdata_bak[i][j];
            }

            sum += fw_sample_rawdata[i];                                // 增加计算当前值和

            fw_sample_rawdata[i] = sum / FW_SAMPLE_RAWDATA_SMOOTH_LEVEL; // 计算平均值

            for(j = (FW_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j > 0; j--)            // 更新备份值
            {
                fw_sample_rawdata_bak[i][j] = fw_sample_rawdata_bak[i][j - 1];
            }

            fw_sample_rawdata_bak[i][0] = fw_sample_rawdata[i];
        }
    }
}

/**
* @fn static void fw_sample_rawdata_smooth(void)
* @brief rawdata smooth
* @param None
* @return None.
*/
static void fw_sample_rawdata_filter(void)                  // rawdata 平滑处理
{
    uint8_t i, j;
    int16_t temp_data;

    static uint8_t flag_start_finish = 0;

    if(flag_start_finish == 0)                              // 起始备份
    {
        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
        {
            for(j = 0; j < 2; j++)
            {
                fw_sample_filter_rawdata_bak[i][j] = fw_sample_rawdata[i];
            }
        }

        flag_start_finish = 1;
    }
    else
    {

        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
        {
            temp_data = fw_sample_rawdata[i];

            if((fw_sample_filter_rawdata_bak[i][0] > fw_sample_rawdata[i]) && (fw_sample_filter_rawdata_bak[i][0] > fw_sample_filter_rawdata_bak[i][1])) // 尖峰滤波
            {
                fw_sample_filter_rawdata_bak[i][0] = (fw_sample_rawdata[i] + fw_sample_filter_rawdata_bak[i][1]) / 2;
            }

            if((fw_sample_filter_rawdata_bak[i][0] < fw_sample_rawdata[i]) && (fw_sample_filter_rawdata_bak[i][0] < fw_sample_filter_rawdata_bak[i][1])) // 尖峰滤波
            {
                fw_sample_filter_rawdata_bak[i][0] = (fw_sample_rawdata[i] + fw_sample_filter_rawdata_bak[i][1]) / 2;
            }

            fw_sample_rawdata[i] = fw_sample_filter_rawdata_bak[i][0];                // 修正本次rawdata

            fw_sample_filter_rawdata_bak[i][1] = fw_sample_filter_rawdata_bak[i][0];        // 更新备份
            fw_sample_filter_rawdata_bak[i][0] = temp_data;
        }
    }
}

/**
* @fn static void fw_sample_rawdata_smooth(void)
* @brief rawdata smooth
* @param None
* @return None.
*/
static void fw_sample_rawdata_smooth2(void)                  // rawdata 平滑处理
{
    char i, j;
    //    int16_t temp_data;
    int32_t sum;

    if(FW_SAMPLE_RAWDATA_SMOOTH_LEVEL)                                  // 平滑级别不为0才生效
    {
        if(flag_rawdata_bak_finish == 0)                                // 判断是否已完成初始化备份
        {
            for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
            {
                for(j = 0; j < FW_SAMPLE_RAWDATA_SMOOTH_LEVEL; j++)
                {
                    fw_sample_rawdata_bak[i][j] = fw_sample_rawdata[i];
                }
            }
            flag_rawdata_bak_finish = 1;
        }

        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)                      // 对各通道进行数据平滑
        {
            for(j = (FW_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j > 0; j--)            // 更新备份值
            {
                fw_sample_rawdata_bak[i][j] = fw_sample_rawdata_bak[i][j - 1];
            }

            fw_sample_rawdata_bak[i][0] = fw_sample_rawdata[i];

            sum = 0;

            for(j = 0; j < (FW_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j++)   // 计算备份值和
            {
                sum += fw_sample_rawdata_bak[i][j];
            }

            sum += fw_sample_rawdata[i];                                // 增加计算当前值和

            fw_sample_rawdata[i] = sum / FW_SAMPLE_RAWDATA_SMOOTH_LEVEL; // 计算平均值
        }
    }
}

/**
* @fn static void fw_sample_sensor_connect_status(void)
* @brief sensor connect status sample
* @param None
* @return None.
*/
static void fw_sample_sensor_connect_status(void)                               // 采样通道通断状态
{
    uint8_t i;
    uint16_t temp;

    for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
    {
        temp = hal_get_sensor_state(i);

#if FW_SAMPLE_SENSOR_GET_WELL_SAMPLE
        // sensor恢复后继续执行采样
#else
        if(temp)
#endif
        {
            sensor_connect_status[i] = temp;             // 通道异常状态持续记录，需重新复位才会恢复正常
        }
    }
}

/**
* @fn uint16_t fw_sample_get_sensor_connect_status(uint8_t ch_id)
* @brief get sensor connect status
* @param None
* @return None.
*/
uint16_t fw_sample_get_sensor_connect_status(uint8_t ch_id)       // 获取通道通断状态
{
    if(ch_id < FW_SAMPLE_RAWDATA_NUM)
    {
        return sensor_connect_status[ch_id];
    }
    return 0xffff;
}

/**
* @fn uint8_t fw_sample_get_restart_status(void)
* @brief
* @param None
* @return None.
*/
uint8_t fw_sample_get_restart_status(void)               // 是否采样重新开始标记 0:无异常，1：重启动，此时缓存采样值需重新更新
{
    return hal_afe_get_adc_overflow();
}

/**
* @fn void fw_sample_init(void)
* @brief afe init
* @param None
* @return None.
*/
void fw_sample_init(void)                               // afe 初始化
{
    hal_afe_init();
}

/**
* @fn int16_t fw_sample_get_rawdata(uint8_t num)
* @brief get rawdata
* @param [in]num: nu
* @return rawdata , error :0xffff
*/
int16_t fw_sample_get_rawdata(uint8_t num)              // 获取采样rawdata
{
    if(num < FW_SAMPLE_RAWDATA_NUM)
    {
        return fw_sample_rawdata[num];
    }

    return 0xffff;
}

/**
* @fn void fw_sample_noise_cal_en(uint16_t count)
* @brief enable noise cal
* @param [in]count: count num
* @return None.
*/
void fw_sample_noise_cal_en(uint16_t count)             // 设置noise统计帧数，并开始计算
{
    char i;

    fw_noise_cal_num = count;
    flag_noise_cal_finish = 0;

    for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)          // 初始化备份rawdata及噪声值
    {
        fw_rawdata_buf[i] = fw_sample_rawdata[i];
        fw_noise_peak[i] = 0;
        fw_noise_dev_sum[i] = 0;
    }
}

/**
* @fn static void fw_sample_noise_cal(void)
* @brief noise cal
* @param None
* @return None.
*/
static void fw_sample_noise_cal(void)                               // noise 计算处理函数
{
    char i;
    int16_t diff;

    if(fw_noise_cal_num)                                            // 判断是否已使能noise 计算
    {

        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)                  // 计算rawdata 前后帧差值
        {
            diff =  fw_rawdata_buf[i] - fw_sample_rawdata[i];

            diff = ABS(diff);

            fw_noise_peak[i] = MAX(diff, fw_noise_peak[i]);         // 记录最大差分值

            fw_noise_dev_sum[i] += ((diff) * (diff));               // 计算方差值

            fw_rawdata_buf[i] = fw_sample_rawdata[i];               // 更新备份rawdata
        }

        fw_noise_cal_num--;                                         // 统计次数自减

        if(fw_noise_cal_num == 0)                                   // 判断统计次数是否达成
        {
            flag_noise_cal_finish = 1;                              // 统计次数达成，完成标记置位
        }

        fw_pmu_set_run_mode(PMU_ACTIVE_MODE);                       //设置avtive模式
    }
}

/**
* @fn uint8_t fw_sample_noise_get(int16_t *noise_peak, int32_t *noise_dev_sum, uint8_t num)
* @brief get noise data
* @param [out]noise_peak: point to noise peak data, [out]noise_dev_sum: point to noise dev sum data, [in]num: ch num
* @return result. 0: invalid, 1: valid
*/
uint8_t fw_sample_noise_get(int16_t *noise_peak, int32_t *noise_dev_sum, uint8_t num)   // 获取noise计算值
{
    char i;

    if(flag_noise_cal_finish)
    {
        for(i = 0; i < num; i++)
        {
            noise_peak[i] = fw_noise_peak[i];
            noise_dev_sum[i] = fw_noise_dev_sum[i];
        }
        flag_noise_cal_finish = 0;
        return 1;
    }
    return 0;
}

/**
* @fn void fw_sample_print(void)
* @brief iic print rawdata
* @param None
* @return None.
*/
extern char fw_debug_iic_printf_receive[FW_DEBUG_IIC_BUFFER_NUM];
static void fw_sample_print(void)
{
    uint8_t i;

    // debug
    static uint8_t count = 0;
    count++;

    fw_printf_iic(sprintf(fw_debug_iic_printf_receive, "R: "));                                 // 起始头标记

    for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
    {
        fw_printf_iic(sprintf(fw_debug_iic_printf_receive, "%d,", fw_sample_rawdata[i]));      // rawdata数据
    }
    fw_printf_iic(sprintf(fw_debug_iic_printf_receive, "\r\n"));                                // 换行符
}



/**
* @fn void fw_sample_proc(void)
* @brief sample system proc
* @param None
* @return None.
*/
//void fw_auto_calibration_proc(void);

void fw_sample_proc(void)                                   // 采样系统处理函数
{
    char i ;
    static uint32_t time_count = FW_SAMPLE_CHECK_SENSOR_TIME;

    time_count++;
    if(fw_pmu_get_run_mode() == PMU_SLEEP_MODE)             //sleep期间不采样
    {
        return;
    }
    hal_afe_sample();                                       // 采样rawdata

    for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)              // 获取rawdata
    {
      fw_sample_rawdata[i] = hal_afe_get_rawdata(i);      // sensor 连接状态正常，采样值有效
    }

    #if FW_SAMPLE_FILTER_ENABLE
        fw_sample_rawdata_filter();                             // 尖峰滤波处理
    #endif
    fw_sample_rawdata_smooth2();                            // rawdata平滑
    fw_sample_noise_cal();                                  // noise 计算

}
