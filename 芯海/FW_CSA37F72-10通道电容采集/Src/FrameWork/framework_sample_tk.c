/**
* @file                framework_sample_tk.c
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
 * @defgroup TK采样模块
 * @brief 采样tk数据
 * @details 
 * @{
 */
//hal
#include "hal_tk.h"

//framework
#include "framework_sample_tk.h"
#include "framework_debug.h"
#include "framework_pmu.h"
#include "framework_reg.h"
//config
#include "user_config.h"

#define NEG(x)		((x > 0) ? (-x) : (x))
#define ABS(x)		(((x) > 0) ? (x) : (-(x)))
#define MIN(x, y)	((x > y) ? (y) : (x))
#define MAX(x, y)	((x > y) ? (x) : (y))
#define RANGE(x, min, max)	MIN(MAX(x, min), max)
#define SQUARE(x)       ((x)*(x))

static int16_t fw_tk_sample_rawdata[FW_TK_SAMPLE_RAWDATA_NUM];
static uint16_t fw_tk_noise_cal_num = 0;
static uint8_t flag_tk_noise_cal_finish = 0;
static int16_t fw_tk_rawdata_buf[FW_TK_SAMPLE_RAWDATA_NUM];
static int16_t fw_tk_noise_peak[FW_TK_SAMPLE_RAWDATA_NUM];
static int32_t fw_tk_noise_dev_sum[FW_TK_SAMPLE_RAWDATA_NUM];

static int16_t fw_tk_sample_rawdata_bak[FW_TK_SAMPLE_RAWDATA_NUM][FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL];

static uint8_t flag_tk_rawdata_bak_finish = 0;

static int16_t fw_tk_sample_filter_rawdata_bak[FW_TK_SAMPLE_RAWDATA_NUM][2];

//static char  s_printf_buf[256];
/**
* @fn static void fw_tk_sample_rawdata_smooth(void)
* @brief rawdata smooth
* @param None
* @return None.
*/
static void fw_tk_sample_rawdata_smooth(void)                  // tk rawdata 平滑处理
{
    char i, j;
    int32_t sum;

    if(FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL)                                  // 平滑级别不为0才生效
    {
        if(flag_tk_rawdata_bak_finish == 0)                                // 判断是否已完成初始化备份
        {
            for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
            {
                for(j = 0; j < FW_SAMPLE_RAWDATA_SMOOTH_LEVEL; j++)
                {
                    fw_tk_sample_rawdata_bak[i][j] = fw_tk_sample_rawdata[i];
                }
            }
            flag_tk_rawdata_bak_finish = 1;
        }

        for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)                      // 对各通道进行数据平滑
        {
            sum = 0;

            for(j = 0; j < (FW_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j++)   // 计算备份值和
            {
                sum += fw_tk_sample_rawdata_bak[i][j];
            }

            sum += fw_tk_sample_rawdata[i];                                // 增加计算当前值和

            fw_tk_sample_rawdata[i] = sum / FW_SAMPLE_RAWDATA_SMOOTH_LEVEL; // 计算平均值

            for(j = (FW_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j > 0; j--)            // 更新备份值
            {
                fw_tk_sample_rawdata_bak[i][j] = fw_tk_sample_rawdata_bak[i][j - 1];
            }

            fw_tk_sample_rawdata_bak[i][0] = fw_tk_sample_rawdata[i];
        }
    }
}

/**
* @fn static void fw_sample_rawdata_smooth(void)
* @brief rawdata smooth
* @param None
* @return None.
*/
static void fw_tk_sample_rawdata_smooth2(void)                  // rawdata 平滑处理
{
    char i, j;
    //    int16_t temp_data;
    int32_t sum;

    if(FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL)                                  // 平滑级别不为0才生效
    {
        if(flag_tk_rawdata_bak_finish == 0)                                // 判断是否已完成初始化备份
        {
            for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)
            {
                for(j = 0; j < FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL; j++)
                {
                    fw_tk_sample_rawdata_bak[i][j] = fw_tk_sample_rawdata[i];
                }
            }
            flag_tk_rawdata_bak_finish = 1;
        }

        for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)                      // 对各通道进行数据平滑
        {
            for(j = (FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j > 0; j--)            // 更新备份值
            {
                fw_tk_sample_rawdata_bak[i][j] = fw_tk_sample_rawdata_bak[i][j - 1];
            }

            fw_tk_sample_rawdata_bak[i][0] = fw_tk_sample_rawdata[i];


            sum = 0;

            for(j = 0; j < (FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j++)   // 计算备份值和
            {
                sum += fw_tk_sample_rawdata_bak[i][j];
            }

            sum += fw_tk_sample_rawdata[i];                                // 增加计算当前值和

            fw_tk_sample_rawdata[i] = sum / FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL; // 计算平均值
        }
    }
}

/**
* @fn static void fw_sample_rawdata_filter(void)
* @brief rawdata smooth
* @param None
* @return None.
*/
static void fw_tk_sample_rawdata_filter(void)                  // // 尖峰滤波处理
{
    uint8_t i, j;
    int16_t temp_data;

    static uint8_t flag_tk_start_finish = 0;

    if(flag_tk_start_finish == 0)                              // 起始备份
    {
        for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)
        {
            for(j = 0; j < 2; j++)
            {
                fw_tk_sample_filter_rawdata_bak[i][j] = fw_tk_sample_rawdata[i];
            }
        }

        flag_tk_start_finish = 1;
    }
    else
    {

        for(i = 0; i < FW_SAMPLE_RAWDATA_NUM; i++)
        {
            temp_data = fw_tk_sample_rawdata[i];

            if((fw_tk_sample_filter_rawdata_bak[i][0] > fw_tk_sample_rawdata[i]) && (fw_tk_sample_filter_rawdata_bak[i][0] > fw_tk_sample_filter_rawdata_bak[i][1])) // 尖峰滤波
            {
                fw_tk_sample_filter_rawdata_bak[i][0] = (fw_tk_sample_rawdata[i] + fw_tk_sample_filter_rawdata_bak[i][1]) / 2;
            }

            if((fw_tk_sample_filter_rawdata_bak[i][0] < fw_tk_sample_rawdata[i]) && (fw_tk_sample_filter_rawdata_bak[i][0] < fw_tk_sample_filter_rawdata_bak[i][1])) // 尖峰滤波
            {
                fw_tk_sample_filter_rawdata_bak[i][0] = (fw_tk_sample_rawdata[i] + fw_tk_sample_filter_rawdata_bak[i][1]) / 2;
            }

            fw_tk_sample_rawdata[i] = fw_tk_sample_filter_rawdata_bak[i][0];                // 修正本次rawdata

            fw_tk_sample_filter_rawdata_bak[i][1] = fw_tk_sample_filter_rawdata_bak[i][0];        // 更新备份
            fw_tk_sample_filter_rawdata_bak[i][0] = temp_data;
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

    if(FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL)                                  // 平滑级别不为0才生效
    {
        if(flag_tk_rawdata_bak_finish == 0)                                // 判断是否已完成初始化备份
        {
            for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)
            {
                for(j = 0; j < FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL; j++)
                {
                    fw_tk_sample_rawdata_bak[i][j] = fw_tk_sample_rawdata[i];
                }
            }
            flag_tk_rawdata_bak_finish = 1;
        }

        for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)                      // 对各通道进行数据平滑
        {
            for(j = (FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j > 0; j--)            // 更新备份值
            {
                fw_tk_sample_rawdata_bak[i][j] = fw_tk_sample_rawdata_bak[i][j - 1];
            }

            fw_tk_sample_rawdata_bak[i][0] = fw_tk_sample_rawdata[i];


            sum = 0;

            for(j = 0; j < (FW_TK_SAMPLE_RAWDATA_SMOOTH_LEVEL - 1); j++)   // 计算备份值和
            {
                sum += fw_tk_sample_rawdata_bak[i][j];
            }

            sum += fw_tk_sample_rawdata[i];                                // 增加计算当前值和

            fw_tk_sample_rawdata[i] = sum / FW_SAMPLE_RAWDATA_SMOOTH_LEVEL; // 计算平均值


        }
    }
}


/**
* @fn int16_t fw_tk_sample_get_rawdata(uint8_t num)
* @brief get rawdata
* @param [in]num: nu
* @return rawdata , error :0xffff
*/
int16_t fw_tk_sample_get_rawdata(uint8_t num)              // 获取采样TK rawdata
{
    if(num < FW_TK_SAMPLE_RAWDATA_NUM)
    {
        return fw_tk_sample_rawdata[num];
    }

    return 0xffff;
}

/**
* @fn void fw_tk_sample_noise_cal_en(uint16_t count)
* @brief enable noise cal
* @param [in]count: count num
* @return None.
*/
void fw_tk_sample_noise_cal_en(uint16_t count)             // 设置noise统计帧数，并开始计算
{
    char i;

    fw_tk_noise_cal_num = count;
    flag_tk_noise_cal_finish = 0;

    for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)          // 初始化备份rawdata及噪声值
    {
        fw_tk_rawdata_buf[i] = fw_tk_sample_rawdata[i];
        fw_tk_noise_peak[i] = 0;
        fw_tk_noise_dev_sum[i] = 0;
    }
}

/**
* @fn static void fw_sample_noise_cal(void)
* @brief noise cal
* @param None
* @return None.
*/
static void fw_tk_sample_noise_cal(void)                   // noise 计算处理函数
{
    char i;
    int16_t diff;

    if(fw_tk_noise_cal_num)                                // 判断是否已使能noise 计算
    {

        for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)                  // 计算rawdata 前后帧差值
        {
            diff =  fw_tk_rawdata_buf[i] - fw_tk_sample_rawdata[i];

            diff = ABS(diff);

            fw_tk_noise_peak[i] = MAX(diff, fw_tk_noise_peak[i]);         // 记录最大差分值

            fw_tk_noise_dev_sum[i] += ((diff) * (diff));               // 计算方差值

            fw_tk_rawdata_buf[i] = fw_tk_sample_rawdata[i];               // 更新备份rawdata
        }

        fw_tk_noise_cal_num--;                                         // 统计次数自减

        if(fw_tk_noise_cal_num == 0)                                   // 判断统计次数是否达成
        {
            flag_tk_noise_cal_finish = 1;                              // 统计次数达成，完成标记置位
        }

        fw_pmu_set_run_mode(PMU_ACTIVE_MODE);   //设置avtive模式
    }
}

/**
* @fn uint8_t fw_sample_noise_get(int16_t *noise_peak, int32_t *noise_dev_sum, uint8_t num)
* @brief get noise data
* @param [out]noise_peak: point to noise peak data, [out]noise_dev_sum: point to noise dev sum data, [in]num: ch num
* @return result. 0: invalid, 1: valid
*/
uint8_t fw_tk_sample_noise_get(int16_t *noise_peak, int32_t *noise_dev_sum, uint8_t num)   // 获取noise计算值
{
    char i;

    if(flag_tk_noise_cal_finish)
    {
        for(i = 0; i < num; i++)
        {
            noise_peak[i] = fw_tk_noise_peak[i];
            noise_dev_sum[i] = fw_tk_noise_dev_sum[i];
        }

        flag_tk_noise_cal_finish = 0;
        return 1;
    }

    return 0;
}

///**
//* @fn void fw_tk_sample_print(void)
//* @brief iic print rawdata
//* @param None
//* @return None.
//*/
//static void fw_tk_sample_print(void)
//{
//    uint8_t i;
//    // debug
//	int offset = 0;
//	strcpy(s_printf_buf, "");
//	offset = strlen(s_printf_buf);
//	for(i = 0; i<FW_TK_SAMPLE_RAWDATA_NUM; i++)
//	{
//		offset += sprintf(s_printf_buf+offset, "%d,", fw_tk_sample_rawdata[i]);
//	}
//	offset = strlen(s_printf_buf);
//	strcpy(s_printf_buf+offset, "\r\n");
//	printf("%s", s_printf_buf);
//}

#define TK_AVERAGE_NUM       8
/**
* @fn void fw_sample_proc(void)
* @brief sample system proc
* @param None
* @return None.
*/
void fw_tk_sample_proc(void)                                    // 采样系统处理函数
{
    char i ;
    static uint32_t time_count = FW_SAMPLE_CHECK_SENSOR_TIME;

    time_count++;
	
    hal_tk_sample();                                            // 采样rawdata 单通道 97.4us

    for(i = 0; i < FW_TK_SAMPLE_RAWDATA_NUM; i++)               // 获取rawdata
    {
        fw_tk_sample_rawdata[i] = hal_tk_sample_get_rawdata(i); 
    }

}

/**
* @fn void fw_sample_tk_init(void)
* @brief tk init
* @param None
* @return None.
*/
void fw_sample_tk_init(void)                               // tk 初始化
{
    hal_tk_init();
}

/**
* @fn void fw_sample_proc(void)
* @brief sample system proc
* @param None
* @return None.
*/
int16_t get_fw_tk_rawdata(uint8_t ch)                                   // 采样系统处理函数
{
    return fw_tk_sample_rawdata[ch];
}

/**
  * @}
  */

/**
  * @}
  */
