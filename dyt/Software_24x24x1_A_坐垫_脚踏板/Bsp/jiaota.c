#include <stdio.h>  
  
// 假设 image 是一个二维 unsigned char 数组，表示灰度图像  
// 假设 image 的尺寸足够大，至少有 24 行和 10 列  

#define MIN_VALUE 2
unsigned char image[24][10]; // 这只是一个示例大小，实际大小可能不同  
  
int cal_front_side(unsigned char image[24][10]) {  
    int right_side = 0;  
    int left_side = 0;  
    int i;  
  
    // 计算右边（第1列）的白色像素数量  
    for (i = 5; i < 24; i++) {  
        if (image[i][0] > MIN_VALUE) {  
            right_side++;  
        }  
    }  
  
    // 计算左边（第10列）的白色像素数量  
    for (i = 5; i < 24; i++) {  
        if (image[i][9] > MIN_VALUE) { // 注意 C 语言中数组索引从0开始，所以第10列是索引9  
            left_side++;  
        }  
    }  
  
    // 返回白色像素数量之和  
    return right_side + left_side;  
}  

int cal_back_side(unsigned char image[24][10]) {  
    int right_side = 0;  
    int left_side = 0; 
    int i;  
  
    // 计算右边（第1列）的白色像素数量  
    for (i = 0; i < 5; i++) {  

            right_side += image[i][0] > MIN_VALUE;  
            left_side += image[i][9] > MIN_VALUE; 

    } 
		return right_side + left_side;  
}
    
int cal_rear_side(unsigned char image[24][10]) {

	  int right_side = 0;  
    int left_side = 0; 
		int rear_side = 0;
		int i;
  
    for (int i = 2; i < 8; i++) {
        rear_side += image[0][i] > MIN_VALUE;
        rear_side += image[1][i] > MIN_VALUE;
    }
  
    // 返回白色像素数量之和  
    return rear_side;  
}  


int sum_of_elements(unsigned char matrix[][10], int rows) {  
    int sum = 0;  
    for (int i = 0; i < rows; i++) {  
        for (int j = 0; j < 10; j++) {  
            sum += matrix[i][j];  
        }  
    }  
    return sum;  
} 

int count_positive_elements(unsigned char matrix[][10], int rows) {  
    int count = 0;  
    for (int i = 0; i < rows; i++) {  
        for (int j = 0; j < 10; j++) {  
            if (matrix[i][j] > MIN_VALUE) {  
                count++;  
            }  
        }  
    }  
    return count;  
}  

int FR = 0,BR = 0,RR = 0,W = 0,N = 0;
int foot_alarm(unsigned char image[24][10]) {  
    // 这里假设 image 已经被填充了数据  
    // 调用函数并打印结果  
    FR = cal_front_side(image);
    BR = cal_back_side(image);
    RR = cal_rear_side(image);
        // 计算所有元素的和  
    W = sum_of_elements(image, 24);  
  
    // 计算大于0的元素数量  
    N = count_positive_elements(image, 24);  
    
    if(N > 110 && (FR + BR) > 22)
    {
    	return 2;
    }
    else if(N > 85 && (RR+BR) > 8 && FR > 18)  
    {
        return 2;  
    }
    else if(W > 400)
    {
        return 1;
    } else 
		{
				return 0;
		}

}

