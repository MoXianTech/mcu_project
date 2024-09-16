#include "lcd.h"
#include "image.h"

LCD_1IN69_ATTRIBUTES LCD_1IN69;

/******************************************************************************
function :  Hardware reset
parameter:
******************************************************************************/
static void LCD_1IN69_Reset(void)
{
    LCD_1IN69_RST_1;
    DEV_Delay_ms(100);
    LCD_1IN69_RST_0;
    DEV_Delay_ms(100);
    LCD_1IN69_RST_1;
    DEV_Delay_ms(100);
}

/******************************************************************************
function :  send command
parameter:
     Reg : Command register
******************************************************************************/
 void LCD_1IN69_SendCommand(UBYTE Reg)
{
		while(spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS) == SET){};
    LCD_1IN69_DC_0;
    LCD_1IN69_CS_0;
    DEV_SPI_WRITE(Reg);
    //LCD_1IN69_CS_1;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
 void LCD_1IN69_SendData_8Bit(UBYTE Data)
{
		while(spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS) == SET){};
    LCD_1IN69_DC_1;
    //LCD_1IN69_CS_0;
    DEV_SPI_WRITE(Data);
    //LCD_1IN69_CS_1;
}

/******************************************************************************
function :  send data
parameter:
    Data : Write data
******************************************************************************/
void LCD_1IN69_SendData_16Bit(UWORD Data)
{
		while(spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS) == SET){};
    LCD_1IN69_DC_1;
    LCD_1IN69_CS_0;
    DEV_SPI_WRITE((Data >> 8) & 0xFF);
    DEV_SPI_WRITE(Data & 0xFF);
		while(spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS) == SET){};
    LCD_1IN69_CS_1;
}

/******************************************************************************
function :  Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_1IN69_InitReg(void)
{
    LCD_1IN69_SendCommand(0x36);
    LCD_1IN69_SendData_8Bit(0x00);

    LCD_1IN69_SendCommand(0x3A);
    LCD_1IN69_SendData_8Bit(0x65);

    LCD_1IN69_SendCommand(0xB2);
    LCD_1IN69_SendData_8Bit(0x0B);
    LCD_1IN69_SendData_8Bit(0x0B);
    LCD_1IN69_SendData_8Bit(0x00);
    LCD_1IN69_SendData_8Bit(0x33);
    LCD_1IN69_SendData_8Bit(0x35);

    LCD_1IN69_SendCommand(0xB7);
    LCD_1IN69_SendData_8Bit(0x11);

    LCD_1IN69_SendCommand(0xBB);
    LCD_1IN69_SendData_8Bit(0x35);

    LCD_1IN69_SendCommand(0xC0);
    LCD_1IN69_SendData_8Bit(0x2C);

    LCD_1IN69_SendCommand(0xC2);
    LCD_1IN69_SendData_8Bit(0x01);

    LCD_1IN69_SendCommand(0xC3);
    LCD_1IN69_SendData_8Bit(0x0D);

    LCD_1IN69_SendCommand(0xC4);
    LCD_1IN69_SendData_8Bit(0x20);

    LCD_1IN69_SendCommand(0xC6);
    LCD_1IN69_SendData_8Bit(0x13);

    LCD_1IN69_SendCommand(0xD0);
    LCD_1IN69_SendData_8Bit(0xA4);
    LCD_1IN69_SendData_8Bit(0xA1);

    LCD_1IN69_SendCommand(0xD6);
    LCD_1IN69_SendData_8Bit(0xA1);

    LCD_1IN69_SendCommand(0xE0);
    LCD_1IN69_SendData_8Bit(0xF0);
    LCD_1IN69_SendData_8Bit(0x06);
    LCD_1IN69_SendData_8Bit(0x0B);
    LCD_1IN69_SendData_8Bit(0x0A);
    LCD_1IN69_SendData_8Bit(0x09);
    LCD_1IN69_SendData_8Bit(0x26);
    LCD_1IN69_SendData_8Bit(0x29);
    LCD_1IN69_SendData_8Bit(0x33);
    LCD_1IN69_SendData_8Bit(0x41);
    LCD_1IN69_SendData_8Bit(0x18);
    LCD_1IN69_SendData_8Bit(0x16);
    LCD_1IN69_SendData_8Bit(0x15);
    LCD_1IN69_SendData_8Bit(0x29);
    LCD_1IN69_SendData_8Bit(0x2D);

    LCD_1IN69_SendCommand(0xE1);
    LCD_1IN69_SendData_8Bit(0xF0);
    LCD_1IN69_SendData_8Bit(0x04);
    LCD_1IN69_SendData_8Bit(0x08);
    LCD_1IN69_SendData_8Bit(0x08);
    LCD_1IN69_SendData_8Bit(0x07);
    LCD_1IN69_SendData_8Bit(0x03);
    LCD_1IN69_SendData_8Bit(0x28);
    LCD_1IN69_SendData_8Bit(0x32);
    LCD_1IN69_SendData_8Bit(0x40);
    LCD_1IN69_SendData_8Bit(0x3B);
    LCD_1IN69_SendData_8Bit(0x19);
    LCD_1IN69_SendData_8Bit(0x18);
    LCD_1IN69_SendData_8Bit(0x2A);
    LCD_1IN69_SendData_8Bit(0x2E);

    LCD_1IN69_SendCommand(0xE4);
    LCD_1IN69_SendData_8Bit(0x25);
    LCD_1IN69_SendData_8Bit(0x00);
    LCD_1IN69_SendData_8Bit(0x00);

    LCD_1IN69_SendCommand(0x21);

    LCD_1IN69_SendCommand(0x11);
    //V_Delay_ms(120);
    LCD_1IN69_SendCommand(0x29);
}

/********************************************************************************
function:   Set the resolution and scanning method of the screen
parameter:
        Scan_dir:   Scan direction
********************************************************************************/
static void LCD_1IN69_SetAttributes(UBYTE Scan_dir)
{
    // Get the screen scan direction
    LCD_1IN69.SCAN_DIR = Scan_dir;
    UBYTE MemoryAccessReg = 0x00;

    // Get GRAM and LCD width and height
    if (Scan_dir == HORIZONTAL) {
        LCD_1IN69.HEIGHT = LCD_1IN69_WIDTH;
        LCD_1IN69.WIDTH = LCD_1IN69_HEIGHT;
        MemoryAccessReg = 0X70;
    }
    else {
        LCD_1IN69.HEIGHT = LCD_1IN69_HEIGHT;
        LCD_1IN69.WIDTH = LCD_1IN69_WIDTH;      
        MemoryAccessReg = 0X00;
    }

    // Set the read / write scan direction of the frame memory
    LCD_1IN69_SendCommand(0x36); // MX, MY, RGB mode
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
    LCD_1IN69_SendData_8Bit(MemoryAccessReg); // 0x08 set RGB
		__nop();
		__nop();
		__nop();
		__nop();
}

/********************************************************************************
function :  Initialize the lcd
parameter:
********************************************************************************/
void LCD_1IN69_Init(UBYTE Scan_dir)
{
    // Hardware reset
    LCD_1IN69_Reset();

    // Set the resolution and scanning method of the screen
    LCD_1IN69_SetAttributes(Scan_dir);

    // Set the initialization register
    LCD_1IN69_InitReg();
}

/********************************************************************************
function:   Sets the start position and size of the display area
parameter:
        Xstart  :   X direction Start coordinates
        Ystart  :   Y direction Start coordinates
        Xend    :   X direction end coordinates
        Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_1IN69_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    if (LCD_1IN69.SCAN_DIR == VERTICAL) {
				// set the X coordinates
        LCD_1IN69_SendCommand(0x2A);
        LCD_1IN69_SendData_8Bit(Xstart >> 8);
        LCD_1IN69_SendData_8Bit(Xstart);
        LCD_1IN69_SendData_8Bit((Xend-1) >> 8);
        LCD_1IN69_SendData_8Bit(Xend-1);

        // set the Y coordinates
        LCD_1IN69_SendCommand(0x2B);
        LCD_1IN69_SendData_8Bit((Ystart+20) >> 8);
        LCD_1IN69_SendData_8Bit(Ystart+20);
        LCD_1IN69_SendData_8Bit((Yend+20-1) >> 8);
        LCD_1IN69_SendData_8Bit(Yend+20-1);

    }
    else {
        // set the X coordinates
        LCD_1IN69_SendCommand(0x2A);
        LCD_1IN69_SendData_8Bit((Xstart+20) >> 8);
        LCD_1IN69_SendData_8Bit(Xstart+20);
        LCD_1IN69_SendData_8Bit((Xend+20-1) >> 8);
        LCD_1IN69_SendData_8Bit(Xend+20-1);

        // set the Y coordinates
        LCD_1IN69_SendCommand(0x2B);
        LCD_1IN69_SendData_8Bit((Ystart)>> 8);
        LCD_1IN69_SendData_8Bit(Ystart);
        LCD_1IN69_SendData_8Bit((Yend-1) >> 8);
        LCD_1IN69_SendData_8Bit(Yend-1);
    }
    
    LCD_1IN69_SendCommand(0X2C);
}

/******************************************************************************
function :  Clear screen
parameter:
******************************************************************************/
void LCD_1IN69_Clear(UWORD Color)
{
    UWORD i,j;
    
    LCD_1IN69_SetWindows(0, 0, LCD_1IN69.WIDTH, LCD_1IN69.HEIGHT);
		while(spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS) == SET){};
    LCD_1IN69_DC_1;
    
    for(i = 0; i < LCD_1IN69_WIDTH; i++) {
        for(j = 0; j < LCD_1IN69_HEIGHT; j++) {
            DEV_SPI_WRITE((Color>>8)&0xff);
            DEV_SPI_WRITE(Color);
        }
     }
}

/******************************************************************************
function :  Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_1IN69_Display(UWORD *Image)
{
    UWORD i,j;

    LCD_1IN69_SetWindows(0, 0, LCD_1IN69.WIDTH, LCD_1IN69.HEIGHT);
		while(spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS) == SET){};
    LCD_1IN69_DC_1;
    
    for(i = 0; i < LCD_1IN69_WIDTH; i++) {
        for(j = 0; j < LCD_1IN69_HEIGHT; j++) {
            DEV_SPI_WRITE((*(Image+i*LCD_1IN69_HEIGHT+j)>>8)&0xff);
            DEV_SPI_WRITE(*(Image+i*LCD_1IN69_WIDTH+j));
        }
    }

}

void LCD_1IN69_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;
    UWORD i,j;
    
    LCD_1IN69_SetWindows(Xstart, Ystart, Xend , Yend);
    LCD_1IN69_DC_1;
    
    for (i = Ystart; i < Yend - 1; i++) {
        Addr = Xstart + i * LCD_1IN69_WIDTH ;
        for(j=Xstart;j<Xend-1;j++) {
            DEV_SPI_WRITE((*(Image+Addr+j)>>8)&0xff);
            DEV_SPI_WRITE(*(Image+Addr+j));
        }
    }
}

void LCD_1IN69_DrawPoint(UWORD X, UWORD Y, UWORD Color)
{
    LCD_1IN69_SetWindows(X, Y, X, Y);
    LCD_1IN69_SendData_16Bit(Color);
}

void LCD_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)
{
		LCD_1IN69_SetWindows(Xstart, Ystart, Xend, Yend);
	 UWORD X, Y;
    for (Y = Ystart; Y < Yend; Y++) {
        for (X = Xstart; X < Xend; X++) {//8 pixel =  1 byte
            LCD_1IN69_DrawPoint(X, Y, Color);
        }
    }
}


/********* 以下是个人写的，不是官方库***********/

static void LCD_gpio_config(void)
{
		rcu_periph_clock_enable(DC_RST_CS_GPIO_Clock);

 		gpio_init(DC_RST_CS_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DC_Pin|CS_Pin);
		
		gpio_bit_set(DC_RST_CS_GPIO_Port, DC_Pin|CS_Pin);
}


void LCD_init(void)
{
		uint16_t x_number;
		uint16_t y_number;
		uint8_t pixel_width = PIXEL_WIDTH;//像素点的宽度等于屏幕宽分辨率/adc行的数目 = 240/72 = 3;由于屏幕有圆角，不是纯长方形所以得按实际能观察所有采样像素点来取值
		uint8_t pixel_height = PIXEL_HEIGHT;	//像素点的长度等于屏幕长分辨率/adc列的数目 = 280/80 = 3; 
		
		x_number = (240 - POS_X*pixel_width)/2;//X对称分布初始位置
		y_number = (280 - POS_Y*pixel_height)/2;//Y对称分布初始位置
	
		SPI_init();
		LCD_gpio_config();
	
		LCD_1IN69_Init(VERTICAL);
		Paint_SetClearFuntion(LCD_1IN69_Clear);
    Paint_SetDisplayFuntion(LCD_1IN69_DrawPoint);
		Paint_NewImage(LCD_1IN69_WIDTH, LCD_1IN69_HEIGHT, 0, WHITE);

		//设置开机图片
		LCD_1IN69_Clear(BLACK);//设置黑色背景
		Paint_DrawImage(gImage_image2, 72, 0, 95, 280);
		delay_1ms(2000);
	
		LCD_1IN69_Clear(BLACK);//设置黑色背景
		//LCD_ClearWindows(x_number, y_number, x_number + ADCLINE*pixel_width, y_number+ADCCOLUMN*pixel_height,BLACK);
		LCD_1IN69_SetWindows(x_number, y_number, x_number + POS_X*pixel_width, y_number+POS_Y*pixel_height);//设置屏幕adc采样显示范围
	
		//dma使能
		delay_1ms(100);
		LCD_1IN69_DC_1;
		SPI_DMA_init();
}


//把RGB565转为16位，R:5位、G:6位、B:5位
uint16_t RGB_out(RGB_Color rgb_color)
{
	uint16_t  color = 0;
	color = (uint16_t)rgb_color.b;
	color |= (uint16_t)rgb_color.g<<5;
	color |= (uint16_t)rgb_color.r<<11;
	
	return color;
}


//把adc采样值转为定制的颜色输出
uint16_t ADC_value_transition_color(uint8_t value)
{
	uint8_t tmep = 0;
	uint8_t min_value = 10;
	RGB_Color rgb_color = {0,0.0};
	
	if(value < min_value)
	{
		return 0x0000;
	}
	else
	{
		//94.0/(80.0f-2.0f)为色彩图与adc值的比例，80是adc最大值，2是最小值，94代表94种色彩配比
		tmep =  (uint32_t)(value-min_value) * 94.0/(65.0f-min_value)+0.5;
	}
	
	if(tmep<31)
	{
		rgb_color.r = 0;
		rgb_color.g = tmep;
		rgb_color.b =	31;
	}
	else if(tmep<31*2)
	{
		rgb_color.r = tmep%31;
		rgb_color.g = 31;
		rgb_color.b =	31 - (tmep%31);  
	}
	else if(tmep<31*3)
	{
		rgb_color.r = 31;
		rgb_color.g = 31 - (tmep%31);
		rgb_color.b =	0; 
	}
	else
	{
		rgb_color.r = 31;
		rgb_color.g = 0;
		rgb_color.b =	0; 
	}
	
	return RGB_out(rgb_color);
}

void LCD_ADC_DMA_display(uint8_t * adc_value, uint8_t count)
{
	uint16_t j = 0, k = 0,l = 0;
	uint16_t Color = 0;
	static uint32_t temp_cnt = 0;
	LCD_1IN69_CS_1;

	LCD_1IN69_DC_1;
	
	if(temp_cnt == ARRAYSIZE)
	{
		temp_cnt = 0;
	}
	
	
	for(l = 0; l < PIXEL_HEIGHT;l++)
	for(j = 0; j < POS_X; j++)
	{
		Color = ADC_value_transition_color(adc_value[count+j*POS_Y]);

		for(k = 0; k < PIXEL_WIDTH; k++)
		{
			spi1_send_array[temp_cnt] = (Color>>8)&0xff;
			spi1_send_array[temp_cnt+1] = Color;
			temp_cnt = temp_cnt + 2;
		}
	}
	
	LCD_1IN69_CS_0;
	__nop();
	__nop();

	
}
//显示adc
void LCD_ADC_display(uint8_t * adc_value)
{
	uint16_t i = 0, j = 0, k = 0,l = 0;
	uint16_t Color = 0;
	
	while(spi_i2s_flag_get(SPI1, SPI_FLAG_TRANS) == SET){};
	LCD_1IN69_DC_1;
	
	for(i = 0; i < POS_Y; i++)
	for(l = 0; l < PIXEL_HEIGHT;l++)
	for(j = 0; j < POS_X; j++)
	{
		Color = ADC_value_transition_color(adc_value[i+j*POS_Y]);

		for(k = 0; k < PIXEL_WIDTH; k++)
		{
			DEV_SPI_WRITE((Color>>8)&0xff);
			DEV_SPI_WRITE(Color);
		}
	}
	
}

