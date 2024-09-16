#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "circular_buffer.h"
#include "priv_malloc.h"

/**
 * @brief     Check if Num is power of 2
 *
 * @param[in] Num   the number to check
 *
 * @return          1 if Num is power of 2
 */
unsigned long long ispowerof2(unsigned long long num) {
  return (num > 0 && !(num & (num - 1)));
}

/**
 * @brief     calculate the minimum number that round up to the next power of 2
 *
 * @param[in] Num   the number to calculate
 *
 * @return          the number that round up to the next power of 2 (0x100 if Num is 0xf0, 0x81, 0xa3 ... )
 */
unsigned long roundup_powerof2(unsigned long num) {
  unsigned long result = 1;

  if (ispowerof2(num) || num == 0)
    return num;
  else if (num > LONG_MAX)
    return (LONG_MAX ^ ULONG_MAX);   // WARN: if Num biger than (LONG_MAX+1) then result will equals to (LONG_MAX+1)

  while (num) {
    num >>= 1;
    result <<= 1;
  }

  return result;
}

/**
 * @brief     calculate the minimum number that round down to the next power of 2
 *
 * @param[] Num the number to check
 *
 * @return    the number that round up to the last power of 2 (4 if Num is 5,6,7, 8 if Num is 9,10,11 ... )
 */
unsigned long rounddown_powerof2(unsigned long num) {
  unsigned long result = 1;

  if (ispowerof2(num) || num == 0)
    return num;
  else if (num > LONG_MAX)
    return (LONG_MAX ^ ULONG_MAX);   // WARN: if Num biger than (LONG_MAX+1) then result will equals to (LONG_MAX+1)

  while (num) {
    num >>= 1;
    result <<= 1;
  }

  return result >> 1;
}

/**
 * @brief     Init the Circular buffer with a array
 *
 * @param[in] CBuf      The circular buffer to initial
 * @param[in] Buff      the buffer for circular buffer to store data
 * @param[in] Size      the size of buffer
 *
 * @return      the Round Down(Power Of 2) size that the circular  buffer to be used
 */
int circbuf_init(circbuf_t *cbuf, unsigned char *buff, unsigned int size) {
  cbuf->buffer = buff;

  if(!ispowerof2(size)) {
    if (size > INT_MAX)
      size = (INT_MAX ^ UINT_MAX);
    else
      size = (int) rounddown_powerof2(size);
  }

  cbuf->size = size;
  cbuf->tailer = 0;
  cbuf->header = 0;

  return size;
}

/**
 * @brief     Circular Buffer initialization
 *
 * @param[in] CBuf      the circular buffer to initialization
 * @param[in] Size      size of the circular buffer
 *
 * @return    1 if memery allocation success
 *            0 if fail
 */
int circbuf_alloc(circbuf_t *cbuf, unsigned int size) {
  int result = 0;

  if(!ispowerof2(size)) {
    if(size > INT_MAX)
      size = (INT_MAX ^ UINT_MAX);
    else
      size = (int)roundup_powerof2(size);
  }
  cbuf->buffer = (unsigned char *) board_calloc(size);    // buffer will set to 0

  cbuf->tailer = 0;
  cbuf->header = 0;

  if(cbuf->buffer != NULL) {
    cbuf->size = size;
    result = 1;
  }

  return result;
}

/**
 * @brief     delete circular buffer and release the memery
 *
 * @param[in] CBuf  the circular buffer to delete
 */
void circbuf_free(circbuf_t *cbuf) {
  board_free(cbuf->buffer);
  cbuf->tailer = 0;
  cbuf->header = 0;
  cbuf = NULL;
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
unsigned int circbuf_push(circbuf_t *cbuf, unsigned char *data, unsigned int lentopush) {
  unsigned int len;

  lentopush = MIN(lentopush, (cbuf->size - (cbuf->header - cbuf->tailer)));

  len = MIN(lentopush, cbuf->size - (cbuf->header & (cbuf->size - 1)));

  memcpy(cbuf->buffer + (cbuf->header & cbuf->size - 1), data, len);
  memcpy(cbuf->buffer, data + len, lentopush - len);

  cbuf->header += lentopush;

  return lentopush;
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
unsigned int circbuf_pop(circbuf_t *cbuf, unsigned char *data, unsigned int lentopop) {
  unsigned int len;

  lentopop = MIN(lentopop, cbuf->header - cbuf->tailer);

  len = MIN(lentopop, cbuf->size - (cbuf->tailer & (cbuf->size - 1)));

  memcpy(data, cbuf->buffer + (cbuf->tailer & (cbuf->size - 1)), len);
  memcpy(data + len, cbuf->buffer, lentopop - len);

  cbuf->tailer += lentopop;

  return lentopop;
}

/**
 * @brief     get one char from circular buffer
 *
 * @param[in] CBuf      the circular buffer that stored data
 * @param[n] data       target buffer that will store the data that from circular buffer
 *
 * @return              actual length that get from circular buffer
 */
unsigned int circbuf_poponechar(circbuf_t *cbuf, unsigned char *data) {
  return circbuf_pop(cbuf, data, 1);
}

/**
 * @brief     for access data at Tailer + offset
 *
 * @param[in] CBuf      the circular buffer that stored data
 * @param[in] offset    the offset of Tailer
 *
 * @return              the data at Buffer[Tailer + offset]
 */
unsigned char circbuf_at(circbuf_t *cbuf, unsigned int offset) {
  unsigned int index = (cbuf->tailer + offset) & (cbuf->size - 1);
  return cbuf->buffer[index];
}

/**
 * @brief     get data from circular buffer
 *
 * @param[in] CBuf      the circular buffer that stored data
 * @param[in] data      target buffer that will store the data that from circular buffer
 * @param[in] LenToRead  the length that wan't to get from circular buffer
 *
 * @return      actual length that get from circular buffer
 */
unsigned int circbuf_read(circbuf_t *cbuf, unsigned char *data, unsigned int lentoread) {
  unsigned int len;

  lentoread = MIN(lentoread, cbuf->header - cbuf->tailer);

  len = MIN(lentoread, cbuf->size - (cbuf->tailer & (cbuf->size - 1)));

  memcpy(data, cbuf->buffer + (cbuf->tailer & (cbuf->size - 1)), len);
  memcpy(data + len, cbuf->buffer, lentoread - len);

  return lentoread;
}

/**
 * @brief     drop the the size of data at tailer
 *
 * @param[in] CBuf          the circular buffer that stored data
 * @param[in] LenToDrop     the size of data at tailer of circular_buffer to drop
 */
void circbuf_drop(circbuf_t *cbuf, unsigned int lentodrop) {
  if((cbuf->tailer + lentodrop) <= cbuf->header )
    cbuf->tailer += lentodrop;
  else
    cbuf->tailer = cbuf->header;
}

/**
 * @brief     get the Available memery size of circular buffer
 *
 * @param[in] CBuf  the circular buffer to get size
 *
 * @return          Available size of the circular buffer
 */
unsigned int circbuf_getavalaiblesize(circbuf_t *cbuf) {
  return ((cbuf->size > 0) ? (cbuf->size - (cbuf->header - cbuf->tailer)) : 0);
}

/**
* @brief     get the used memery size of circular buffer
*
* @param[in] CBuf  the circular buffer to get size
*
* @return          used size of the circular buffer
*/
unsigned int circbuf_getusedsize(circbuf_t *cbuf) {
  return (cbuf->header - cbuf->tailer);
}

/**
 * @brief     check if the circular buffer is empty
 *
 * @param[in] CBuf  the circular buffer to check
 *
 * @return          1   if no data stored in the circular buffer
 *                  0   if the size of circular buffer equals to 0
 *                      or some data stored in the circular buffer
 */
unsigned int circbuf_isempty(circbuf_t *cbuf) {
  return ((cbuf->size > 0) && (cbuf->header == cbuf->tailer));
}

/**
 * @brief     check if the circular buffer is full
 *
 * @param[in] CBuf  the circular buffer to check
 *
 * @return      1 if the size of circular buffer equals to 0
 *                  or no Available space of circular buffer
 */
unsigned int circbuf_isfull(circbuf_t *cbuf) {
  return ((cbuf->size == 0) || (cbuf->size == (cbuf->header - cbuf->tailer)));
}
