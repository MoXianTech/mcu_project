#include <stdlib.h>
#include <string.h>
#include "circular_buffer.h"
#include "priv_malloc.h"

/**
 * @brief     init the circular buffer with a array
 *
 * @param[in] cbuf      the circular buffer to initial
 * @param[in] buff      the buffer for circular buffer to store data
 * @param[in] size      the size of buffer
 *
 * @return      the round down(power of 2) size that the circular  buffer to be used
 */
int16_t circbuf_init(circbuf_t *cbuf, uint8_t *buff, uint16_t size) {
    cbuf->buffer = buff;

    cbuf->size = size;
    cbuf->tailer = 0;
    cbuf->header = 0;

    return size;
}

/**
 * @brief     circular buffer initialization
 *
 * @param[in] cbuf      the circular buffer to initialization
 * @param[in] size      size of the circular buffer
 *
 * @return    1 if memery allocation success
 *            0 if fail
 */
int16_t circbuf_alloc(circbuf_t *cbuf, uint16_t size) {
    int16_t result = 0;

    cbuf->buffer = (uint8_t *) board_calloc(size);    // buffer will set to 0

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
 * @param[in] cbuf  the circular buffer to delete
 */
void circbuf_free(circbuf_t *cbuf) {
    free(cbuf->buffer);
    cbuf = NULL;
}

/**
 * @brief     put data into the circular buffer
 *
 * @param[in] cbuf      the circular buffer that will store the data
 * @param[in] data      the data to store into circular buffer
 * @param[in] lentopush  the length of data to store into circular buffer
 *
 * @return      the actual size stored into circular buffer
 */
uint16_t circbuf_push(circbuf_t *cbuf, uint8_t *data, uint16_t lentopush) {
    uint16_t len;

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
 * @param[in] cbuf      the circular buffer that stored data
 * @param[in] data      target buffer that will store the data that from circular buffer
 * @param[in] lentopop  the length that wan't to get from circular buffer
 *
 * @return      actual length that get from circular buffer
 */
uint16_t circbuf_pop(circbuf_t *cbuf, uint8_t *data, uint16_t lentopop) {
    uint16_t len;

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
 * @param[in] cbuf      the circular buffer that stored data
 * @param[n] data       target buffer that will store the data that from circular buffer
 *
 * @return              actual length that get from circular buffer
 */
uint16_t circbuf_poponechar(circbuf_t *cbuf, uint8_t *data) {
    return circbuf_pop(cbuf, data, 1);
}

/**
 * @brief     for access data at tailer + offset
 *
 * @param[in] cbuf      the circular buffer that stored data
 * @param[in] offset    the offset of tailer
 *
 * @return              the data at buffer[tailer + offset]
 */
uint8_t circbuf_at(circbuf_t *cbuf, uint16_t offset) {
    uint16_t index = (cbuf->tailer + offset) & (cbuf->size - 1);
    return cbuf->buffer[index];
}

/**
 * @brief     get data from circular buffer
 *
 * @param[in] cbuf      the circular buffer that stored data
 * @param[in] data      target buffer that will store the data that from circular buffer
 * @param[in] lentoread  the length that wan't to get from circular buffer
 *
 * @return      actual length that get from circular buffer
 */
uint16_t circbuf_read(circbuf_t *cbuf, uint8_t *data, uint16_t lentoread) {
    uint16_t len;

    lentoread = MIN(lentoread, cbuf->header - cbuf->tailer);

    len = MIN(lentoread, cbuf->size - (cbuf->tailer & (cbuf->size - 1)));

    memcpy(data, cbuf->buffer + (cbuf->tailer & (cbuf->size - 1)), len);
    memcpy(data + len, cbuf->buffer, lentoread - len);

    return lentoread;
}

/**
 * @brief     drop the the size of data at tailer
 *
 * @param[in] cbuf          the circular buffer that stored data
 * @param[in] lentodrop     the size of data at tailer of circular_buffer to drop
 */
void circbuf_drop(circbuf_t *cbuf, uint16_t lentodrop) {
    if((cbuf->tailer + lentodrop) <= cbuf->header )
        cbuf->tailer += lentodrop;
    else
        cbuf->tailer = cbuf->header;
}

/**
 * @brief     get the available memery size of circular buffer
 *
 * @param[in] cbuf  the circular buffer to get size
 *
 * @return          available size of the circular buffer
 */
uint16_t circbuf_getavalaiblesize(circbuf_t *cbuf) {
    return ((cbuf->size > 0) ? (cbuf->size - (cbuf->header - cbuf->tailer)) : 0);
}

/**
 * @brief     get the used memery size of circular buffer
 *
 * @param[in] cbuf  the circular buffer to get size
 *
 * @return          used size of the circular buffer
 */
uint16_t circbuf_getusedsize(circbuf_t *cbuf) {
    return (cbuf->header - cbuf->tailer);
}

/**
 * @brief     check if the circular buffer is empty
 *
 * @param[in] cbuf  the circular buffer to check
 *
 * @return          1   if no data stored in the circular buffer
 *                  0   if the size of circular buffer equals to 0
 *                      or some data stored in the circular buffer
 */
uint16_t circbuf_isempty(circbuf_t *cbuf) {
    return ((cbuf->size > 0) && (cbuf->header == cbuf->tailer));
}

/**
 * @brief     check if the circular buffer is full
 *
 * @param[in] cbuf  the circular buffer to check
 *
 * @return      1 if the size of circular buffer equals to 0
 *                  or no available space of circular buffer
 */
uint16_t circbuf_isfull(circbuf_t *cbuf) {
    return ((cbuf->size == 0) || (cbuf->size == (cbuf->header - cbuf->tailer)));
}
