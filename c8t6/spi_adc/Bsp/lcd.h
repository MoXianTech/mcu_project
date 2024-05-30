#ifndef _LCD_H
#define _LCD_H
#include "spi.h"
#include "DEV_Config.h"
#include "main.h"
#include "GUI_Paint.h"


#define LCD_1IN69_HEIGHT 280
#define LCD_1IN69_WIDTH 240

#define	PIXEL_WIDTH		3
#define PIXEL_HEIGHT	3

#define HORIZONTAL 0
#define VERTICAL   1

#define CS_Pin 								GPIO_PIN_12
#define DC_Pin		 						GPIO_PIN_11
//#define RST_Pin 							GPIO_PIN_2	//没有用到rst
#define DC_RST_CS_GPIO_Port 	GPIOB
#define DC_RST_CS_GPIO_Clock 	RCU_GPIOB

#define LCD_1IN69_CS_0  gpio_bit_reset(DC_RST_CS_GPIO_Port, CS_Pin)
#define LCD_1IN69_CS_1  gpio_bit_set(DC_RST_CS_GPIO_Port, CS_Pin)
                     
#define LCD_1IN69_RST_0 //gpio_bit_reset(DC_RST_CS_GPIO_Port, RST_Pin)//没接rst的gpio口
#define LCD_1IN69_RST_1 //gpio_bit_set(DC_RST_CS_GPIO_Port, RST_Pin)
                      
#define LCD_1IN69_DC_0  gpio_bit_reset(DC_RST_CS_GPIO_Port, DC_Pin)
#define LCD_1IN69_DC_1  gpio_bit_set(DC_RST_CS_GPIO_Port, DC_Pin)




typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_1IN69_ATTRIBUTES;
extern LCD_1IN69_ATTRIBUTES LCD_1IN69;

uint16_t RGB_out(RGB_Color rgb_color);
void LCD_init(void);
void LCD_1IN69_SendData_8Bit(UBYTE Data);
void LCD_1IN69_SendData_16Bit(UWORD Data);
void LCD_1IN69_SendCommand(UBYTE Reg);
void LCD_1IN69_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);
void LCD_1IN69_Init(UBYTE Scan_dir);
void LCD_1IN69_Clear(UWORD Color);
void LCD_1IN69_Display(UWORD *Image);
void LCD_1IN69_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN69_DrawPoint(UWORD X, UWORD Y, UWORD Color);
void LCD_1IN69_SetBackLight(UWORD Value);
void LCD_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color);
void LCD_ADC_display(uint8_t * adc_value);
void LCD_ADC_DMA_display(uint8_t * adc_value, uint8_t count);
#endif

