

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "nordic_common.h"
#include "app_timer.h"

#define DBGPRINTF_ALLOW 1
#include "dbg_etc.h"

#include "ble_tuds.h"

#include "ma_tuds.h"

#define PUBLIC 

//-----------------------------------------------------------------------------
// Defines
#define APP_TIMER_PRESCALER 0

//#define APP_TIMER_DEF(timer_id)                                  \
//    static app_timer_t timer_id##_data = { {0} };                  \
//    static const app_timer_id_t timer_id = &timer_id##_data
		
//static app_timer_id_t         m_Dn_timer_id;  //LNEW
// static app_timer_t m_Dn_timer_id_data = { {0} };  //LNEW
// static const app_timer_id_t m_Dn_timer_id = &m_Dn_timer_id_data;  //LNEW

//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
static void BlkDn_unlockStateMachine(ma_tuds_t *p_app_UD);


//=============================================================================
//=============================================================================
//=============================================================================
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//===== DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN ======
//=============================================================================
//=============================================================================
//=============================================================================




/* ----------------------------------------------------------------------------
*  BSS
*/



/* ----------------------------------------------------------------------------
*  FUNCTIONS
*/


//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
/*
typedef enum eBlkDnState
{
    eBlkDn_WAIT_CMD,
    eBlkDn_WAIT_PACKET,
    eBlkDn_GOT_PACKET,

    eBlkDn_CFM_OK_PRESEND, // set before CFM packet sent
    eBlkDn_CFM_NG_PRESEND,
    eBlkDn_CFM_RE_PRESEND,

    eBlkDn_CFM_OK_SEND, // set after CFM packet sent
    eBlkDn_CFM_NG_SEND, // set after CFM packet sent
    eBlkDn_CFM_RE_SEND,

    eBlkDn_CFM_OK_SENT, // set after CFM packet sent (Write Done or ACK) received

} eBlkDnState_t;


static eBlkDnState_t p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;
*/


//-----------------------------------------------------------------------------
static int32_t blk_dn_start(ma_tuds_t *p_app_UD, uint8_t *pkt )
{
    uint8_t  j;
    uint16_t i;

    p_app_UD->m_blkDn_rxBlkCnt = 0;
    p_app_UD->m_blkDn_blkCnt = 0;    
    
    p_app_UD->m_blkDn_len = pkt[2] | (pkt[3]<<8);
    if( p_app_UD->m_blkDn_len == 0)
        return(1);
    
    p_app_UD->m_blkDn_blkCnt = ((p_app_UD->m_blkDn_len - 1) / 16) + 1;
    
    
    for(i=0 ; i < p_app_UD->m_blkDn_blkCnt; i++)
    {
        p_app_UD->m_blkDn_chk[i] = 0x00;
        for(j=0 ; j < 16; j++)
        {
            p_app_UD->m_blkDn_buf[i*16 + j] = 0x00;
        }
    }
    
    return(0);
}


static int32_t blk_dn_add(ma_tuds_t *p_app_UD, uint8_t *pkt, uint16_t len )
{
    uint8_t  position;
    uint8_t  j;
//    uint16_t i;
    
    if(len != 20)
        return(1);
    
    position = pkt[0];
    if( position >= BLK_DN_COUNT)
        return(1);
    
    p_app_UD->m_blkDn_rxBlkCnt++;
    p_app_UD->m_blkDn_chk[position] += 1;
    for(j=0 ; j < 16; j++)
    {
        p_app_UD->m_blkDn_buf[position*16 + j] = pkt[4 + j];
    }
    return(0);
}

#define DN_CHK_OK 0
#define DN_CHK_CHKSUM_NG 3

static int32_t blk_dn_chk(ma_tuds_t *p_app_UD)
{
//    uint8_t  j;
    uint16_t i;
    uint16_t missing_blk_cnt;
    uint16_t cs_pkt;
    uint16_t cs_now;
 
    if(p_app_UD->m_blkDn_blkCnt == 0)
        return(1);

    if(p_app_UD->m_blkDn_rxBlkCnt < p_app_UD->m_blkDn_blkCnt)
        return(1);
    
    missing_blk_cnt = 0;
    for(i=0 ; i < p_app_UD->m_blkDn_blkCnt; i++)
    {
        if(p_app_UD->m_blkDn_chk[i] == 0x00)
            missing_blk_cnt++;
    }
    if(missing_blk_cnt>0)
        return(2);

    cs_pkt = p_app_UD->m_blkDn_buf[p_app_UD->m_blkDn_len - 2] | (p_app_UD->m_blkDn_buf[p_app_UD->m_blkDn_len - 1]<<8);
#if 0 // _CRC
    //cs_now = CRC_START_SEED; //0x0000;//0xFFFF;
    //c_now = crc16_compute( &p_app_UD->m_blkDn_buf[0], p_app_UD->m_blkDn_len - 2, cs_now);
#else    
    cs_now = 0;
    for(i=0 ; i < p_app_UD->m_blkDn_len - 2; i++)
    {
        cs_now += p_app_UD->m_blkDn_buf[i];
    }
#endif
    if( cs_now != cs_pkt)
        return( DN_CHK_CHKSUM_NG );
    
    return(DN_CHK_OK);
}

static int32_t blk_dn_missing_count(ma_tuds_t *p_app_UD)
{
    uint16_t i;
    uint16_t missing_blk_cnt;
 
    if(p_app_UD->m_blkDn_blkCnt == 0)
        return(0);
   
    missing_blk_cnt = 0;
    for(i=0 ; i < p_app_UD->m_blkDn_blkCnt; i++)
    {
        if(p_app_UD->m_blkDn_chk[i] == 0x00)
            missing_blk_cnt++;
    }
    return(missing_blk_cnt);
}

static int32_t blk_dn_get_missing(ma_tuds_t *p_app_UD, uint8_t* buf, uint8_t count)
{
    uint16_t i;
    uint16_t missing_blk_cnt;
 
    if(p_app_UD->m_blkDn_blkCnt == 0)
        return(0);
   
    missing_blk_cnt = 0;
    for(i=0 ; i < p_app_UD->m_blkDn_blkCnt; i++)
    {
        if(p_app_UD->m_blkDn_chk[i] == 0x00)
        {
            missing_blk_cnt++;
            if( missing_blk_cnt <= count )
            {
                buf[missing_blk_cnt-1] = (uint8_t)i;
            }
            if( missing_blk_cnt == count )
                break;
        }
    }
    return(missing_blk_cnt);
}



//-----------------------------------------------------------------------------
//
//  Timer
//
//-----------------------------------------------------------------------------
static uint32_t BlkDn_timer_start(ma_tuds_t *p_app_UD, uint32_t timeout_ticks); //, void* p_context);
static uint32_t BlkDn_timer_stop(void);
static void BlkDn_timeout_handler(void * p_context);


//-----------------------------------------------------------------------------
//
//  Controls
//
//-----------------------------------------------------------------------------

static uint32_t blk_dn_startSend_Dcfm_OK(ma_tuds_t *p_app_UD)
{
    uint32_t err_code;
    
    p_app_UD->m_Dcfm_buf[0] = 1;
    p_app_UD->m_Dcfm_buf[1] = 0; // 0 = OK
    p_app_UD->m_Dcfm_len = 2;
    
    // should be this mode p_app_UD->m_BlkDn_sm = eBlkDn_GOT_PACKET;
    
    err_code = ble_tuds_notify_Dcfm(p_app_UD->p_ble_tuds, p_app_UD->m_Dcfm_buf, &p_app_UD->m_Dcfm_len);
    if(err_code == NRF_SUCCESS)
    {
        dbgPrint("o");
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_OK_SEND;
        BlkDn_timer_start(p_app_UD, APP_TIMER_TICKS(222, APP_TIMER_PRESCALER) ); //Nofify(CFM_OK), Waiting for writeCompleted signal
    }
    else
    if(err_code == BLE_ERROR_NO_TX_PACKETS)
    {
        dbgPrint("n");
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_OK_PRESEND;
        BlkDn_timer_start(p_app_UD, APP_TIMER_TICKS(222, APP_TIMER_PRESCALER) ); //Nofify(CFM_OK), Had no free packets available for Nofify(CFM_OK)
    }
    else
    {
        dbgPrint("x");
        //TODO what do we do with this "fatal" error situation?
    }

    return(err_code);    
}

static uint32_t blk_dn_startSend_Dcfm_NG(ma_tuds_t *p_app_UD)
{
    uint32_t err_code;
    
    p_app_UD->m_Dcfm_buf[0] = 1;
    p_app_UD->m_Dcfm_buf[1] = 1; // 1 = NG (Checksum NG)
    p_app_UD->m_Dcfm_len = 2;
    
    // should be this mode p_app_UD->m_BlkDn_sm = eBlkDn_GOT_PACKET;
    
    err_code = ble_tuds_notify_Dcfm(p_app_UD->p_ble_tuds, p_app_UD->m_Dcfm_buf, &p_app_UD->m_Dcfm_len);
    if(err_code == NRF_SUCCESS)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_NG_SEND;
        BlkDn_timer_start(p_app_UD, APP_TIMER_TICKS(222, APP_TIMER_PRESCALER) ); //Nofify(CFM_NG), Waiting for writeCompleted signal
    }
    else
    if(err_code == BLE_ERROR_NO_TX_PACKETS)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_NG_PRESEND;
        BlkDn_timer_start(p_app_UD, APP_TIMER_TICKS(222, APP_TIMER_PRESCALER) ); //Nofify(CFM_NG), Had no free packets available for Nofify(CFM_NG)
    }
    else
    {
        dbgPrint("x");
        //TODO what do we do with this "fatal" error situation?
    }

    return(err_code);    
}

static uint32_t blk_dn_startSend_Dcfm_missing(ma_tuds_t *p_app_UD, int max_entries)
{
    uint32_t err_code;
    
    p_app_UD->m_Dcfm_buf[0] = 1;
    p_app_UD->m_Dcfm_buf[1] = 2; // 2 = Missing
    
    int n = blk_dn_missing_count(p_app_UD);
    if( n > max_entries )
        n = max_entries;
    p_app_UD->m_Dcfm_buf[2] = n;
    blk_dn_get_missing(p_app_UD, &p_app_UD->m_Dcfm_buf[3], n );
    p_app_UD->m_Dcfm_len = 3 + n;
    
    // should be this mode p_app_UD->m_BlkDn_sm = eBlkDn_GOT_PACKET;
    
    err_code = ble_tuds_notify_Dcfm(p_app_UD->p_ble_tuds, p_app_UD->m_Dcfm_buf, &p_app_UD->m_Dcfm_len);
    if(err_code == NRF_SUCCESS)
    {
        BlkDn_timer_stop();
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_RE_SEND;
        BlkDn_timer_start(p_app_UD, APP_TIMER_TICKS(222, APP_TIMER_PRESCALER) ); //Nofify(CFM_RE), Waiting for writeCompleted signal
    }
    else 
    if(err_code == BLE_ERROR_NO_TX_PACKETS)
    {
        BlkDn_timer_stop();
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_RE_PRESEND;
        BlkDn_timer_start(p_app_UD, APP_TIMER_TICKS(222, APP_TIMER_PRESCALER) ); //Nofify(CFM_RE), Had no free packets available for Nofify(CFM_RE)
    }
    else
    {
        dbgPrintf("ble_tuds_notify_Dcfm=%d\n\r", err_code);
        //TODO what do we do with this "fatal" error situation?
    }

    return(err_code);    
}

//-----------------------------------------------------------------------------
//
//  Events
//
//-----------------------------------------------------------------------------
void  BlkUart_Purge(void);
void  ma_tuds_BlkUp_Purge(ma_tuds_t *p_app_UD);
static void  BlkDn_Purge(ma_tuds_t *p_app_UD)
{
    p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;
    ma_tuds_BlkUp_Purge(p_app_UD);
#if USE_DRCT
    BlkUart_Purge();
#endif
}


//-----------------------------------------------------------------------------
//
//  Events
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PUBLIC int32_t ma_tuds_Dcmd_handler(ma_tuds_t *p_app_UD, uint8_t *buf, uint8_t len)
{
    int32_t r;
    uint8_t  byteZero;

    // First attempt at using Locked
    if(p_app_UD->m_BlkDn_stateMachineLocked == true)
    {
        return(42);
    }

    if( (buf[0] == 1) && (buf[1] == 1) )
    {
        p_app_UD->m_BlkDn_packetWaitTimeCount = 0;
        dbgPrint("S");
        r = blk_dn_start(p_app_UD, buf);
        p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_PACKET;

    }
    if( (buf[0] == 1) && (buf[1] == 2) )
    {
        byteZero = 0;
        
        p_app_UD->m_blkDn_len = 3;
        p_app_UD->m_blkDn_buf[0] = byteZero;
        p_app_UD->m_blkDn_buf[1] = 0x00; // totalCheckSum
        p_app_UD->m_blkDn_buf[2] = 0x00; // totalCheckSum
        
        p_app_UD->blkDn_DnEventHandler(p_app_UD, eBlkDn_EV_CMD12, 0);
    }

    if( (buf[0] == 1) && (buf[1] == 3) )
    {
        BlkDn_Purge(p_app_UD);
    }

    if( (buf[0] == 1) && (buf[1] == 42) )
    {
        ma_tuds_BlkUp_Go_Test(p_app_UD);    
    }

    return(r);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PUBLIC int32_t ma_tuds_Ddat_handler(ma_tuds_t *p_app_UD, uint8_t *buf, uint8_t len)
{
    int32_t r;

    // First attempt at using Locked
    if(p_app_UD->m_BlkDn_stateMachineLocked == true) return(0);
    
    // Reject Data packets unless we are in a state to accept them
    if( (p_app_UD->m_BlkDn_sm != eBlkDn_WAIT_PACKET) &&
        (p_app_UD->m_BlkDn_sm != eBlkDn_GOT_PACKET) && //??
        (p_app_UD->m_BlkDn_sm != eBlkDn_CFM_RE_SEND) &&
        (p_app_UD->m_BlkDn_sm != eBlkDn_CFM_RE_PRESEND) )
    {
        return(0);
    }

    
    BlkDn_timer_stop();
    p_app_UD->m_BlkDn_packetWaitTimeCount = 0;
    
    // Add the packet to the Down Block
    r = blk_dn_add(p_app_UD, buf, len);
    
    // Check for status of block
    r = blk_dn_chk(p_app_UD);
    if(r == DN_CHK_OK)
    {
        dbgPrint("O");
        p_app_UD->m_BlkDn_sm = eBlkDn_GOT_PACKET;
        blk_dn_startSend_Dcfm_OK(p_app_UD);
    }
    else
    if(r == DN_CHK_CHKSUM_NG)
    {
        dbgPrint("N");
        p_app_UD->m_BlkDn_sm = eBlkDn_GOT_PACKET;
        blk_dn_startSend_Dcfm_NG(p_app_UD);
    }
    else
    {
        dbgPrint("D");
        BlkDn_timer_start(p_app_UD, APP_TIMER_TICKS(222, APP_TIMER_PRESCALER) ); // On Ddat Packet, still waiting for all data
    }
    
    return(r);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void BlkDn_lockStateMachine(ma_tuds_t *p_app_UD)
{
    p_app_UD->m_BlkDn_stateMachineLocked = true;
}
static void BlkDn_unlockStateMachine(ma_tuds_t *p_app_UD)
{
    p_app_UD->m_BlkDn_stateMachineLocked = false;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void on_blk_dn_packetReceived(ma_tuds_t *p_app_UD)
{
    if(p_app_UD->blkDn_DnEventHandler)
    {
        BlkDn_lockStateMachine(p_app_UD);
        p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;
        p_app_UD->blkDn_DnEventHandler(p_app_UD, eBlkDn_EV_RXDONE, 0);
    }
    else
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;
        BlkDn_unlockStateMachine(p_app_UD);
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PUBLIC uint32_t ma_tuds_Dn_AllowNextPacket(ma_tuds_t *p_app_UD)
{
    p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;
    BlkDn_unlockStateMachine(p_app_UD);
    dbgPrintf("\r\n Allow Next Down process \r\n");
    return(0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PUBLIC uint32_t ma_tuds_RegCb_DnEventHandler(ma_tuds_t *p_app_UD, blkDn_DnEventHandler_t  cb)
{
    p_app_UD->blkDn_DnEventHandler = cb;
    return(0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int32_t ma_tuds_Dcfm_handler_OnWrittenComplete(ma_tuds_t *p_app_UD,  uint8_t *buf, uint8_t len) //int32_t BlkDn_On_written_Dcfm(ble_tuds_t *p_tuds,  uint8_t *buf, uint8_t len)
{
    int32_t r = 0;

    // Check if in proper state
    if( (p_app_UD->m_BlkDn_sm != eBlkDn_CFM_OK_SEND) &&
        (p_app_UD->m_BlkDn_sm != eBlkDn_CFM_NG_SEND) &&
        (p_app_UD->m_BlkDn_sm != eBlkDn_CFM_RE_SEND)   )
        return(0);

    dbgPrint("C");
    dbgPrint("C");

    BlkDn_timer_stop();
    
    // We had sent our OK,OK, so proceed with processing of our packet
    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_SEND)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_OK_SENT; 
        on_blk_dn_packetReceived(p_app_UD); // Proceed with processing of the recieved packet
    }
    
    // We have sent our NG, OK, so wait for next command.
    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_NG_SEND)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD; 
    }
    
    // We have sent our request, go back to waitig for packets with a new time-out
    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_SEND)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_PACKET;
        BlkDn_timer_start(p_app_UD, APP_TIMER_TICKS(222, APP_TIMER_PRESCALER) ); // CFM_OK_SEND->WrittenComplete
    }
    
    return(r);
}


//-----------------------------------------------------------------------------
//
//  Timer
//
//-----------------------------------------------------------------------------

APP_TIMER_DEF(m_BlkDn_timer_id);


PUBLIC uint32_t ma_tuds_BlkDn_timer_init(void)
{
    uint32_t err_code;

    // Create timer
    err_code = app_timer_create(&m_BlkDn_timer_id, APP_TIMER_MODE_SINGLE_SHOT, BlkDn_timeout_handler);
    if (err_code != NRF_SUCCESS)
    {
        ;
    }
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}

static uint32_t BlkDn_timer_start(ma_tuds_t *p_app_UD, uint32_t timeout_ticks)
{
    uint32_t err_code;

    err_code = app_timer_stop(m_BlkDn_timer_id);
    if( err_code != NRF_SUCCESS)
    {
        ;
    }
    err_code = app_timer_start(m_BlkDn_timer_id, timeout_ticks, p_app_UD);
    if (err_code != NRF_SUCCESS)
    {
        ;
    }

    //APP_ERROR_CHECK(err_code);
    return(err_code);
}

/*static*/ uint32_t BlkDn_timer_stop(void)
{
    uint32_t err_code;
    // Stop timer
    err_code = app_timer_stop(m_BlkDn_timer_id);
    if (err_code != NRF_SUCCESS)
    {
        ;
    }
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}


static void blk_dn_printState(ma_tuds_t *p_app_UD)
{
    if(p_app_UD->m_BlkDn_sm == eBlkDn_WAIT_CMD       ) dbgPrint("[W_C]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_WAIT_PACKET    ) dbgPrint("[W_P]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_PRESEND ) dbgPrint("[M_P]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_SEND    ) dbgPrint("[M_S]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_GOT_PACKET     ) dbgPrint("[G_P]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_PRESEND ) dbgPrint("[C_P]"); // set before CFM packet sent
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_SEND    ) dbgPrint("[C_S]"); // set after CFM packet sent
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_NG_SEND    ) dbgPrint("[C_S]"); // set after CFM packet sent
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_SENT    ) dbgPrint("[C_T]"); // set after CFM packet sent (Write Done or ACK) received
}

static void BlkDn_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    
    ma_tuds_t *p_app_UD;
    p_app_UD = p_context; //WWRONG
    
    dbgPrint("*");
    blk_dn_printState(p_app_UD);
    if(p_app_UD->m_BlkDn_sm == eBlkDn_WAIT_CMD) // Do nothing
    { } //Just waiting for a start up packet
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_WAIT_PACKET) // Should have got the next data packet by now
    {
        if(++p_app_UD->m_BlkDn_packetWaitTimeCount > 3)
        {
            p_app_UD->m_BlkDn_packetWaitTimeCount = 0;
            p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD; // Reset
        }
        else
        {
            dbgPrint("[WP]");
            blk_dn_startSend_Dcfm_missing(p_app_UD, 4);
        }
    }
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_GOT_PACKET) // Do nothing
    { } // Just got the packet so I should have cancelled the timer
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_PRESEND) // buffer was full last time, so try again
    {
            dbgPrint("[MPre]");
            blk_dn_startSend_Dcfm_missing(p_app_UD, 4);
    }
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_SEND) // we didn't get a write done
    {
            dbgPrint("[MSend]");
            blk_dn_startSend_Dcfm_missing(p_app_UD, 4);
    }
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_PRESEND) // buffer was full last time, so try again
    {
        blk_dn_startSend_Dcfm_OK(p_app_UD);
    }
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_NG_PRESEND) // buffer was full last time, so try again
    {
        blk_dn_startSend_Dcfm_NG(p_app_UD);
    }
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_SEND) // we didn't get a write done
    {
        blk_dn_startSend_Dcfm_OK(p_app_UD);
    }
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_NG_SEND) // we didn't get a write done
    {
        blk_dn_startSend_Dcfm_NG(p_app_UD);
    }
    else
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_SENT)
    {
    }
    else
    {
    }
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PUBLIC uint32_t ma_tuds_D_onBleConnect(ma_tuds_t *p_app_UD)
{
    return(0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PUBLIC uint32_t ma_tuds_D_onBleDisconnect(ma_tuds_t *p_app_UD)
{
    BlkDn_timer_stop();
    p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;
    p_app_UD->m_BlkDn_stateMachineLocked = false;
    return(0);
}

