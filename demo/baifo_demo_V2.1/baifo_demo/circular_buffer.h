/**
 * @file    circular_buffer.h
 * @brief   circular buffer
 * @author  Mark Xu<scmarxx@gmail.com>
 * @version 1.3
 * @date    2018-08-01
 */
#ifndef __CIRC_BUF__
#define __CIRC_BUF__

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b)       (((a) > (b)) ? (b) : (a))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

typedef struct CIRC_BUF 
{
  unsigned int Size;
  unsigned int Header;
  unsigned int Tailer;
  unsigned char *Buffer;
} CircBuf_t;

int          CircBuf_Init(CircBuf_t *CBuf, unsigned char *Buff, unsigned short Size);
unsigned int CircBuf_Push (CircBuf_t *CBuf, unsigned char *msg, unsigned short LenToPush);
unsigned int CircBuf_Pop  (CircBuf_t *CBuf, unsigned char *msg, unsigned short LenToPop);
unsigned int CircBuf_PopOneChar (CircBuf_t *CBuf, unsigned char *msg);
unsigned char CircBuf_At(CircBuf_t *CBuf, unsigned short offset);
void         CircBuf_Drop(CircBuf_t *CBuf, unsigned short LenToDrop);
unsigned int CircBuf_GetUsedSize (CircBuf_t *CBuf);
#ifdef __cplusplus
}
#endif

#endif

