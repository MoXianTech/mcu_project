#include "stdint.h"


typedef struct {
    uint8_t x_max;  //原始阵列横轴点位数量
    uint8_t y_max;  //原始阵列纵轴点位数量

    uint8_t *matrix_real;  //原始输入阵列
    uint8_t *matrix_stab;  //算法缓冲区，需要预留与原始阵列一样大的空间
    uint8_t *matrix_creep; //算法缓冲区，需要预留与原始阵列一样大的空间

    uint8_t *matrix_display; //输出阵列，输出消除蠕变后的阵列
} cal_creep_resistance_t;

void cal_creep_resistance_init(uint8_t creep_strength, uint8_t creep_level);
void cal_creep_resistance(cal_creep_resistance_t *cal_creep_resistance);


