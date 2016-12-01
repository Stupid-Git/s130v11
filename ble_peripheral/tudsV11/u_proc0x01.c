
#include <stdint.h>

#include "u_proc0x01.h"

#include "myapp.h"

//#if _USE_CRC
//nerror
//#else
uint16_t get_checksum(uint8_t *buf, uint16_t rp, uint16_t wp ) //uint16_t crc16_compute(uint8_t * p_data, uint16_t size, uint16_t * p_crc)
{
    uint16_t cs = 0;
    uint16_t idx;
    
    for(idx = rp; idx < wp ; idx++)
        cs += buf[idx];        
    return(cs);
}
//#endif

//-----------------------------------------------------------------------------
// returns :  1 - Complete
//            0 - Not complete
//           -1 - Packet type error
//           -2 - CheckSum error
//-----------------------------------------------------------------------------
int32_t  proc01_checkPacketComplete(uint8_t *buf, uint16_t rp, uint16_t wp )
{
    int16_t size;
    uint16_t csum;
    uint16_t csum_calc;

    //printf("proc01_checkPacketComplete: buf = 0x%08x, rp = %d, wp = %d\n", (int)buf, rp, wp );

    if( rp == wp )
        return(0);
    if( buf[rp + 0] != 0x01 )
        return(-1);
    if( rp + 7 > wp )
        return(0);

//dbgPrintf("\r\nSSIIZZEE");
    
    size =  buf[rp + 4] << 8;
    size += buf[rp + 3];
    
    if( (rp + 5 + size + 2) > wp)
        return(0);

//dbgPrintf("\r\nAALLLL");

    
#if _USE_CRC
//nerror    
#if _ALLOW_REVERSE_CRC
    uint16_t csumR;
    csumR =  buf[rp + 5 + size + 1] << 8; // CRC LSB first !!!!!!!!!!!! REVERSE
    csumR += buf[rp + 5 + size + 0];
#endif
    csum =  buf[rp + 5 + size + 0] << 8; // CRC MSB first
    csum += buf[rp + 5 + size + 1];
    csum_calc = CRC_START_SEED; //0x0000;//0xFFFF;
    csum_calc = crc16_compute (&buf[rp+0], 0 + 5 + size, &csum_calc);
#else
    csum =  buf[rp + 5 + size + 1] << 8;
    csum += buf[rp + 5 + size + 0];
    csum_calc = get_checksum( buf, rp + 0, rp + 5 + size );
#endif
    
    if(csum == csum_calc)
        return(1);
#if _ALLOW_REVERSE_CRC
    if(csumR == csum_calc)
        return(1);
#endif

    dbgPrintf("\r\nBBAADD");
    dbgPrintf("\r\nrx   sum = 0x%04x", csum);
    dbgPrintf("\r\ncalc sum = 0x%04x", csum_calc);

    return(-2);
    //return(r);
}

