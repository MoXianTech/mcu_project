#ifndef __WS2812_h
#define __WS2812_h
#include "main.h"

void ledAnimation(void);
void ws2812_process(void);
void setRGB(uint8_t red, uint8_t green, uint8_t blue);
void setSingleLEDColor(int index, uint8_t red, uint8_t green, uint8_t blue);

void setOneLEDColor(int index, uint8_t red, uint8_t green, uint8_t blue);
void WS2812_Send(void);
int median_filter(int channel, int new_sample);
#endif
