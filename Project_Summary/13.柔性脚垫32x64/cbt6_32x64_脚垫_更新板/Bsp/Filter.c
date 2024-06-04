#include  "Filter.h"

 uint8_t mean_temp_buffer[MEAN_COUNT][MEAN_BUFFER_LEN] = {0};
 uint8_t mean_buffer[MEAN_BUFFER_LEN] = {0};


//========================================================================
// 函数: void get_filter_value(uint8_t *source_buffer,uint8_t *target_buffer,uint16_t buffer_len)
// 描述: 滤波函数。
// 参数: source_buffer,滤波前的buffer	target_buffer,滤波后的buffer		buffer_len,buffer的长度
// 返回: none.
//========================================================================

void get_filter_value(uint8_t *source_buffer,uint8_t *target_buffer,uint16_t buffer_len)
{
		uint32_t sum = 0;
		static uint32_t mean_loop = 0;
		uint8_t min_value = 0xFF;
		uint8_t max_value = 0;
		uint8_t filter_value = 1;
		uint8_t mint_hreshold_value = 0;//去除最小阈值
		uint16_t xy,i;

#if(MEAN_COUNT!=1)	
		if(mean_loop<MEAN_COUNT-1)
		{
			mean_loop++;
		}
		else
		{
			mean_loop = 0;
		}
#endif
		memcpy(&mean_temp_buffer[mean_loop],source_buffer,buffer_len);
		
	
		for(xy = 0; xy < buffer_len; xy++)
		{
			sum = 0;
			min_value = 0xFF;
			max_value = 0;
			for(i = 0; i < MEAN_COUNT; i++)
			{
				if(mean_temp_buffer[i][xy] > max_value)
				{
					max_value = mean_temp_buffer[i][xy];
				}
				if(mean_temp_buffer[i][xy] < min_value)
				{
					min_value = mean_temp_buffer[i][xy];
				}
				sum += (uint32_t)mean_temp_buffer[i][xy];

			}
			
#if (MEAN_COUNT==1)
			mean_buffer[xy] = mean_temp_buffer[0][xy];
#elif (MEAN_COUNT==3)
			mean_buffer[xy] = (sum-(max_value+min_value))>>(0);
#elif (MEAN_COUNT==4)
			mean_buffer[xy] = (sum-(max_value+min_value))>>(1);
#elif (MEAN_COUNT==6)
			mean_buffer[xy] = (sum-(max_value+min_value))>>(2);
#elif (MEAN_COUNT==10)
			mean_buffer[xy] = (sum-(max_value+min_value))>>(3);
#else
			MEAN_COUNT宏定义错误
#endif
			
			if(mean_buffer[xy] < 2)
			{
				target_buffer[xy] =mean_buffer[xy]; 
			}
			else if (abs(mean_buffer[xy] - target_buffer[xy]) > filter_value)
			{
				target_buffer[xy] =((uint32_t)mean_buffer[xy]*3 + (uint32_t)target_buffer[xy]*7)/10; 
			}
			if(target_buffer[xy] <= mint_hreshold_value)
				target_buffer[xy] = 0;
		}
}
