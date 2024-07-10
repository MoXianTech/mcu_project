#ifndef __CIRC_BUF__
#define __CIRC_BUF__

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b)       (((a) > (b)) ? (b) : (a))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

typedef struct circ_buf {
  unsigned int size;
  unsigned int header;
  unsigned int tailer;
  unsigned char *buffer;
} circbuf_t;

unsigned long long ispowerof2 (unsigned long long num);
unsigned long roundup_powerof2 (unsigned long num);
unsigned long rounddown_powerof2(unsigned long num);

int          circbuf_init(circbuf_t *cbuf, unsigned char *buff, unsigned int size);
int          circbuf_alloc (circbuf_t *cbuf, unsigned int size);
void         circbuf_free (circbuf_t *cbuf);
unsigned int circbuf_push (circbuf_t *cbuf, unsigned char *data, unsigned int lentopush);
unsigned int circbuf_pop  (circbuf_t *cbuf, unsigned char *data, unsigned int lentopop);
unsigned int circbuf_poponechar (circbuf_t *cbuf, unsigned char *data);
unsigned char circbuf_at(circbuf_t *cbuf, unsigned int offset);
unsigned int circbuf_read(circbuf_t *cbuf, unsigned char *data, unsigned int lentoread);
void         circbuf_drop(circbuf_t *cbuf, unsigned int lentodrop);
unsigned int circbuf_getavalaiblesize (circbuf_t *cbuf);
unsigned int circbuf_getusedsize (circbuf_t *cbuf);
unsigned int circbuf_isempty (circbuf_t *cbuf);
unsigned int circbuf_isfull (circbuf_t *cbuf);

#ifdef __cplusplus
}
#endif

#endif

