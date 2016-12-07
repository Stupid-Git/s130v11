
#include <stdint.h>

#include "u_proc0x01.h"
//#include "u_uni.h"



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////// common /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
#include "myapp.h"

#if _USE_CRC

//nerror

#else
uint16_t get_checksum(uint8_t *buf, uint16_t rp, uint16_t wp ) //uint16_t crc16_compute(uint8_t * p_data, uint16_t size, uint16_t * p_crc)
{
    uint16_t cs = 0;
    uint16_t idx;
    
    for(idx = rp; idx < wp ; idx++)
        cs += buf[idx];        
    return(cs);
}
#endif

#if 0 // UNUSED now
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////// proc01 /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int32_t proc01_GetLength(frame01_t* pF)
{
    uint16_t  size;
    uint16_t  dataLen;

    memcpy((void*)&dataLen, &pF->rx.length1, 2);
    size = 5 + dataLen + 2;
    return( size );
}

//-----------------------------------------------------------------------------
int32_t proc01_AddTxCheckSum(frame01_t* pF)
{
    int32_t r;
    int32_t   i;
    uint16_t  size;
    uint16_t  dataLen;
    uint16_t  sum = 0;

    r = 0;

    memcpy((void*)&dataLen, &pF->tx.length1, 2);
    size = dataLen + 7;

    for(i=0; i<size-2; i++)
        sum = sum + pF->byte[i] ;

    //memcpy(&pF->byte[size-2], (void*)&sum, 2) ;
    memcpy(&pF->tx.sum, (void*)&sum, 2) ;

    pF->tx.data[dataLen + 0] = (sum >> 0) & 0x00ff;
    pF->tx.data[dataLen + 1] = (sum >> 8) & 0x00ff;

    return( r );
}

//bool  g_Flag_frame01_rx_done = false;
 bool  g_Flag_frame01_tx_ready; // = false;
uint32_t serial_no = 0x11223344;

extern  int32_t cmd_01_44(frameUni_t* F);
extern  int32_t cmd_01_45_XXX(frameUni_t* F);
extern  int32_t cmd_01_F5_EmptyFull(frameUni_t* F);

//-----------------------------------------------------------------------------
int32_t proc01_process_rxframe(frameUni_t* FUin, frameUni_t* FUout)
{
    int32_t i;
    int32_t r;
    uint8_t cmd;
    uint8_t subcmd;
    frame01_t* pF;
    frame01_t* pFo;

    uint16_t _size;
    
    pF = &FUin->tbd.frame01;
    pFo = &FUout->tbd.frame01;

    r = -1;
    cmd = pF->rx.command1;
    subcmd = pF->rx.command2;

 
    // try and process command
    switch( cmd )
    {

    case READ_SERIAL_NO1_COMMAND1_SERIAL:  //0x58
    case READ_SERIAL_NO2_COMMAND1_SERIAL:  //0xB3

        pFo->tx.response = ACK_TX_FRAME_SERIAL;
        pFo->tx.length1 = 0x04;
        pFo->tx.length2 = 0x00;
        pFo->tx.data[0] = (uint8_t) (serial_no      );
        pFo->tx.data[1] = (uint8_t) (serial_no >>  8);
        pFo->tx.data[2] = (uint8_t) (serial_no >> 16);
        pFo->tx.data[3] = (uint8_t) (serial_no >> 24);

        pFo->tx.soh     = SOH_TX_FRAME_SERIAL;
        pFo->tx.command = pF->rx.command1;

        proc01_AddTxCheckSum(pFo); // pF->tx.sum = ...;
        g_Flag_frame01_tx_ready = true;
        FUout->wrPtr = proc01_GetLength(pFo);
        break;

        
    case CMD_01_0x44:
//TODO        cmd_01_44(FUin);

        pFo->tx.soh     = SOH_TX_FRAME_SERIAL;
        pFo->tx.command = pF->rx.command1;
        proc01_AddTxCheckSum(pFo); // pF->tx.sum = ...;
        g_Flag_frame01_tx_ready = true;
        FUout->wrPtr = proc01_GetLength(pFo);
        break;
    case CMD_01_0x45:
//TODO        cmd_01_45_XXX(FUin);

        pFo->tx.soh     = SOH_TX_FRAME_SERIAL;
        pFo->tx.command = pF->rx.command1;
        proc01_AddTxCheckSum(pFo); // pF->tx.sum = ...;
        g_Flag_frame01_tx_ready = true;
        FUout->wrPtr = proc01_GetLength(pFo);
        break;


    case CMD_01_0xF5:  // EmptyFull
//TODO        cmd_01_F5_EmptyFull(FUin);

        pFo->tx.soh     = SOH_TX_FRAME_SERIAL;
        pFo->tx.command = pF->rx.command1;
        proc01_AddTxCheckSum(pFo); // pFo->tx.sum = ...;
        g_Flag_frame01_tx_ready = true;
        FUout->wrPtr = proc01_GetLength(pFo);
        break;


    case CMD_01_0xF8:  //0xF8

        pFo->tx.response = ACK_TX_FRAME_SERIAL;
        pFo->tx.length1 = 0x42;
        pFo->tx.length2 = 0x00;
        for( i = 0 ; i < 0x42 ; i++)
            pFo->tx.data[i] = (uint8_t) (i);

        pFo->tx.soh     = SOH_TX_FRAME_SERIAL;
        pFo->tx.command = pF->rx.command1;

        proc01_AddTxCheckSum(pFo); // pF->tx.sum = ...;
        g_Flag_frame01_tx_ready = true;
        FUout->wrPtr = proc01_GetLength(pFo);
        break;


    case CMD_01_0xF9:  //0xF9
        _size =  (pF->rx.data[1]) * 256;
        _size += (pF->rx.data[0]);
        printf("d1   =  %d, %02x\n",pF->rx.data[1], pF->rx.data[1] );
        printf("d0   =  %d, %02x\n",pF->rx.data[0], pF->rx.data[0] );
        printf("size =  %d, %04x\n", _size, _size);

        pFo->tx.response = ACK_TX_FRAME_SERIAL;
        pFo->tx.length1 = (_size>>0) & 0x00FF;
        pFo->tx.length2 = (_size>>8) & 0x00FF;


        for( i = 0 ; i < _size ; i++)
            pFo->tx.data[i] = (uint8_t) (i & 0x000000FF);

        pFo->tx.soh     = SOH_TX_FRAME_SERIAL;
        pFo->tx.command = pF->rx.command1;

        proc01_AddTxCheckSum(pFo); // pF->tx.sum = ...;
        g_Flag_frame01_tx_ready = true;
        FUout->wrPtr = proc01_GetLength(pFo);
        break;


    default:
        // prepare default Nack
        pFo->tx.soh     = SOH_TX_FRAME_SERIAL;
        pFo->tx.command = pF->rx.command1;
        pFo->tx.response = NAK_TX_FRAME_SERIAL;
        pFo->tx.length1 = 0x00;
        pFo->tx.length2 = 0x00;

        proc01_AddTxCheckSum(pFo); // pF->tx.sum = ...;
        g_Flag_frame01_tx_ready = true;
        FUout->wrPtr = proc01_GetLength(pFo);
        break;
    }
    
    return( r );
}
#endif // 0 UNUSED now


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

    if( rp ==  wp )
        return(0);
    if( buf[rp + 0] != 0x01 )
        return(-1);
    if( rp + 7 > wp )
        return(0);

//dbgPrintf("SSIIZZEE\r\n");
    
    size =  buf[rp + 4] << 8;
    size += buf[rp + 3];
    
    if( (rp + 5 + size + 2) > wp)
        return(0);

    dbgPrintf("AALLLL\r\n");

    
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

    dbgPrintf("BBAADD\r\n");
    dbgPrintf("rx   sum = 0x%04x\r\n", csum);
    dbgPrintf("calc sum = 0x%04x\r\n", csum_calc);

    return(-2);
    //return(r);
}

