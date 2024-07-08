#ifndef  __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__
#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b)       (((a) > (b)) ? (b) : (a))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

typedef struct circ_buf {
  uint16_t size;
  uint16_t header;
  uint16_t tailer;
  uint8_t *buffer;
} circbuf_t;

int16_t  circbuf_init(circbuf_t *cbuf, uint8_t *buff, uint16_t size);
int16_t  circbuf_alloc (circbuf_t *cbuf, uint16_t size);
void     circbuf_free (circbuf_t *cbuf);
uint16_t circbuf_push (circbuf_t *cbuf, uint8_t *data, uint16_t lentopush);
uint16_t circbuf_pop  (circbuf_t *cbuf, uint8_t *data, uint16_t lentopop);
uint16_t circbuf_poponechar (circbuf_t *cbuf, uint8_t *data);
uint8_t  circbuf_at(circbuf_t *cbuf, uint16_t offset);
uint16_t circbuf_read(circbuf_t *cbuf, uint8_t *data, uint16_t lentoread);
void     circbuf_drop(circbuf_t *cbuf, uint16_t lentodrop);
uint16_t circbuf_getavalaiblesize (circbuf_t *cbuf);
uint16_t circbuf_getusedsize (circbuf_t *cbuf);
uint16_t circbuf_isempty (circbuf_t *cbuf);
uint16_t circbuf_isfull (circbuf_t *cbuf);

#ifdef __cplusplus
}
#endif

#endif

