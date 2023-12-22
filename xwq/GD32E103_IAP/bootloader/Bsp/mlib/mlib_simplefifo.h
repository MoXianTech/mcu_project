//utf-8

#ifndef _MLIB_SIMPLEFIFO_H_
#define _MLIB_SIMPLEFIFO_H_

//主要引用了此文件的数据类型定义，也可以删除后自定义数据类型
#include "gd32e10x.h"



//FIFO单个内存单元类型
#define SIMPLEFIFO_TYPE             uint8_t
//设置FIFO长度
#define SIMPLEFIFO_BUFF_SIZE        1100


typedef enum {
    SIMPLEFIFO_ERROR_NONE,      //没有错误
    SIMPLEFIFO_ERROR_BUFF_FULL, //buff满
    SIMPLEFIFO_ERROR_BUFF_NULL  //buff空
}SIMPLEFIFO_ERROR;


//FIFO write
SIMPLEFIFO_ERROR simplefifo_buff_write(SIMPLEFIFO_TYPE value);
//FIFO cleare all
uint16_t  simplefifo_buff_cleare_all(void);
//FIFO read
SIMPLEFIFO_ERROR simplefifo_buff_pt_read(SIMPLEFIFO_TYPE *value);
//FIFO read
SIMPLEFIFO_TYPE simplefifo_buff_read(void);
uint16_t  simplefifo_buff_length_get(void);



#endif

