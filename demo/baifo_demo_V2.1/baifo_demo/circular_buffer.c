/**
 * @file    circular_buffer.c
 * @brief   circular buffer
 * @author  Mark Xu<scmarxx@gmail.com>
 * @version 1.3
 * @date    2018-08-01
 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "circular_buffer.h"


/**
 * @brief     Init the Circular buffer with a array
 *
 * @param[in] CBuf      The circular buffer to initial
 * @param[in] Buff      the buffer for circular buffer to store data
 * @param[in] Size      the size of buffer
 *
 * @return      the Round Down(Power Of 2) size that the circular  buffer to be used
 */
int CircBuf_Init(CircBuf_t *CBuf, unsigned char *Buff, unsigned short Size) 
{
  CBuf->Buffer = Buff;
  CBuf->Size = Size;
  CBuf->Tailer = 0;
  CBuf->Header = 0;
  return Size;
}

/**
 * @brief     put data into the circular buffer
 *
 * @param[in] CBuf      the circular buffer that will store the data
 * @param[in] data      the data to store into circular buffer
 * @param[in] LenToPush  the length of data to store into circular buffer
 *
 * @return      the actual size stored into circular buffer
 */
unsigned int CircBuf_Push(CircBuf_t *CBuf, unsigned char *msg, unsigned short LenToPush) 
{
  unsigned int len;
  LenToPush = MIN(LenToPush, (CBuf->Size - (CBuf->Header - CBuf->Tailer)));
  len = MIN(LenToPush, CBuf->Size - (CBuf->Header & (CBuf->Size - 1)));
  memcpy(CBuf->Buffer + (CBuf->Header & CBuf->Size - 1), msg, len);
  memcpy(CBuf->Buffer, msg + len, LenToPush - len);
  CBuf->Header += LenToPush;
  return LenToPush;
}

/**
 * @brief     get data from circular buffer
 *
 * @param[in] CBuf      the circular buffer that stored data
 * @param[in] data      target buffer that will store the data that from circular buffer
 * @param[in] LenToPop  the length that wan't to get from circular buffer
 *
 * @return      actual length that get from circular buffer
 */
unsigned int CircBuf_Pop(CircBuf_t *CBuf, unsigned char *msg, unsigned short LenToPop)
{
  unsigned short len;
  LenToPop = MIN(LenToPop, CBuf->Header - CBuf->Tailer);
  len = MIN(LenToPop, CBuf->Size - (CBuf->Tailer & (CBuf->Size - 1)));
  memcpy(msg, CBuf->Buffer + (CBuf->Tailer & (CBuf->Size - 1)), len);
  memcpy(msg + len, CBuf->Buffer, LenToPop - len);
  CBuf->Tailer += LenToPop;
  return LenToPop;
}

/**
 * @brief     get one char from circular buffer
 *
 * @param[in] CBuf      the circular buffer that stored data
 * @param[n] data       target buffer that will store the data that from circular buffer
 *
 * @return              actual length that get from circular buffer
 */
unsigned int CircBuf_PopOneChar(CircBuf_t *CBuf, unsigned char *msg) 
{
  return CircBuf_Pop(CBuf, msg, 1);
}

/**
 * @brief     for access data at Tailer + offset
 *
 * @param[in] CBuf      the circular buffer that stored data
 * @param[in] offset    the offset of Tailer
 *
 * @return              the data at Buffer[Tailer + offset]
 */
unsigned char CircBuf_At(CircBuf_t *CBuf, unsigned short offset) 
{
  unsigned int index = (CBuf->Tailer + offset) & (CBuf->Size - 1);
  return CBuf->Buffer[index];
}


/**
 * @brief     drop the the size of data at tailer
 *
 * @param[in] CBuf          the circular buffer that stored data
 * @param[in] LenToDrop     the size of data at tailer of circular_buffer to drop
 */
void CircBuf_Drop(CircBuf_t *CBuf, unsigned short LenToDrop) 
{
  if((CBuf->Tailer + LenToDrop) <= CBuf->Header )
    CBuf->Tailer += LenToDrop;
  else
    CBuf->Tailer = CBuf->Header;
}


unsigned int CircBuf_GetUsedSize(CircBuf_t *CBuf) 
{
  return (CBuf->Header - CBuf->Tailer);
}

