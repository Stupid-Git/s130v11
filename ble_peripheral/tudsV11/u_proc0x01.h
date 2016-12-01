#ifndef __U_PROC0X01_H
#define __U_PROC0X01_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"

int32_t  proc01_checkPacketComplete(uint8_t *buf, uint16_t rp, uint16_t wp );

uint16_t get_checksum(uint8_t *buf, uint16_t rp, uint16_t wp );
    
#ifdef __cplusplus
}
#endif

#endif // __U_PROC0X01_H


