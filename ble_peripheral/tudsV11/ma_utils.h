#ifndef __MA_UTILS_H
#define __MA_UTILS_H

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif
	
    

int32_t cb_push( uint8_t data_byte, uint8_t* buffer, uint16_t rdPtr, uint16_t* pwrPtr, uint16_t capacity );
int32_t cb_pop( uint8_t* pdata_byte, uint8_t* buffer, uint16_t* prdPtr, uint16_t wrPtr, uint16_t capacity );
int32_t cb_peek( uint8_t* pdata_byte, uint8_t* buffer, uint16_t rdPtr, uint16_t wrPtr );
int32_t cb_count( uint16_t rdPtr, uint16_t wrPtr, uint16_t capacity );

int32_t cb_delete_range( uint8_t* buffer, uint16_t startPtr, uint16_t endPtr, uint16_t capacity );

struct cb16_s
{
    uint32_t capacity;
    uint32_t length;
    uint16_t rdPtr;
    uint16_t wrPtr;
    uint8_t* buffer;
    uint8_t  autoBuf[1];
};

typedef struct buf32_s
{
    uint32_t capacity;
    uint32_t length;
    uint16_t Unused_rdPtr;
    uint16_t Unused_wrPtr;
    uint8_t* buffer;
    uint8_t  autoBuf[1];
} buf32_t;


typedef struct cb16_s cb16_t;

int32_t cb16_available(cb16_t*cb);
int32_t cb16_push( cb16_t*cb, uint8_t data_byte );

int32_t cb16_push_n( cb16_t*cb, uint8_t* pD, int count);

int32_t cb16_pop( cb16_t*cb , uint8_t* pdata_byte );
int32_t cb16_peek( cb16_t*cb, uint8_t* pdata_byte );
int32_t cb16_count( cb16_t*cb );

int32_t cb16_delete_range( cb16_t*cb, uint16_t startPtr, uint16_t endPtr);
int32_t cb16_clear( cb16_t*cb );


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
buf32_t* buf32_Create(int byteCount);
void buf32_Destroy(buf32_t** p_pbuf);

void     buf32_zero      (buf32_t *P);;
void     buf32_join      (buf32_t *P, buf32_t *P2);
void     buf32_cpy       (buf32_t *P, void* p_bytes, int count);
uint32_t buf32_count     (buf32_t *P, uint8_t the_byte);
void     buf32_print     (char * s, buf32_t *P);
void     buf32_printShort(char * s, buf32_t *P);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
uint16_t crc16_compute(uint8_t * p_data, uint16_t size, uint16_t * p_crc);

#define CRC_START_SEED 0x0000 //everyone else uses 0xFFFF
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
void time_sleep(float time_in_seconds);
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
uint8_t * int32_to_bytes(uint32_t nr);
uint8_t * int16_to_bytes(uint32_t nr);
    

#if defined __cplusplus
}
#endif

#endif // __MA_UTILS_H

