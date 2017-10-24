

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "nordic_common.h"
#include "app_timer.h"

#include "debug_etc.h"

#include "ble_ud2.h"

#include "app_ud2.h"

#define PUBLIC 

//-----------------------------------------------------------------------------
// Defines
#define APP_TIMER_PRESCALER 0                               //  APP_TIMER_PRESCALER 0 is usually defined in main (used for time calculations)

//-----------------------------------------------------------------------------
extern ble_ud2_t  m_ble_ud2;                                //  used here for sending Notifications




//-----------------------------------------------------------------------------
static void BlkDn_unlockStateMachine(void);


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//  uint16_t min_conn_interval;         /**< Minimum Connection Interval in 1.25 ms units, see @ref BLE_GAP_CP_LIMITS.*/
//  uint16_t max_conn_interval;         /**< Maximum Connection Interval in 1.25 ms units, see @ref BLE_GAP_CP_LIMITS.*/
//  uint16_t slave_latency;             /**< Slave Latency in number of connection events, see @ref BLE_GAP_CP_LIMITS.*/
//  uint16_t conn_sup_timeout;          /**< Connection Supervision Timeout in 10 ms units, see @ref BLE_GAP_CP_LIMITS.*/

extern ble_gap_conn_params_t  m_current_conn_params;

static uint16_t local_conn_interval = 0xffff;

static int dnDataDelay_ms = 222;
static int dnTxCompleteDelay_ms = 222;
static uint32_t dnDataDelay_ticks = APP_TIMER_TICKS(222, APP_TIMER_PRESCALER);;
static uint32_t dnTxCompleteDelay_ticks = APP_TIMER_TICKS(222, APP_TIMER_PRESCALER);


static inline uint32_t get_dnDataDelay(void)
{
    if(local_conn_interval == m_current_conn_params.max_conn_interval)
        return dnDataDelay_ticks;
    
    local_conn_interval = m_current_conn_params.max_conn_interval;
    
    dnDataDelay_ms = 16 * 1.25 * m_current_conn_params.max_conn_interval;
    dnDataDelay_ticks = APP_TIMER_TICKS(dnDataDelay_ms, APP_TIMER_PRESCALER);
    
    dnTxCompleteDelay_ms = 16 * 1.25 * m_current_conn_params.max_conn_interval;
    dnTxCompleteDelay_ticks = APP_TIMER_TICKS(dnTxCompleteDelay_ms, APP_TIMER_PRESCALER);

    printf("dnDataDelay_ticks       = %d\r\n", dnDataDelay_ticks);
    printf("dnTxCompleteDelay_ticks = %d\r\n", dnTxCompleteDelay_ticks);

    return dnDataDelay_ticks;
}

static inline uint32_t get_dnTxCompleteDelay(void)
{
    if(local_conn_interval == m_current_conn_params.max_conn_interval)
        return dnTxCompleteDelay_ticks;
    
    local_conn_interval = m_current_conn_params.max_conn_interval;
    
    dnDataDelay_ms = 16 * 1.25 * m_current_conn_params.max_conn_interval;
    dnDataDelay_ticks = APP_TIMER_TICKS(dnDataDelay_ms, APP_TIMER_PRESCALER);
    
    dnTxCompleteDelay_ms = 16 * 1.25 * m_current_conn_params.max_conn_interval;
    dnTxCompleteDelay_ticks = APP_TIMER_TICKS(dnTxCompleteDelay_ms, APP_TIMER_PRESCALER);

    printf("dnDataDelay_ticks       = %d\r\n", dnDataDelay_ticks);
    printf("dnTxCompleteDelay_ticks = %d\r\n", dnTxCompleteDelay_ticks);

    return dnTxCompleteDelay_ticks;
}

void printConnectionInfo(void)
{
    printf("\r\n");
    printf("  Connection Interval     = %d\r\n", m_current_conn_params.max_conn_interval);
    
    get_dnDataDelay();
    get_dnTxCompleteDelay();
    
    printf("  dnDataDelay_ms          = %d\r\n", dnDataDelay_ms);
    printf("  dnTxCompleteDelay_ms    = %d\r\n", dnTxCompleteDelay_ms);
    printf("  dnDataDelay_ticks       = %d\r\n", dnDataDelay_ticks);
    printf("  dnTxCompleteDelay_ticks = %d\r\n", dnTxCompleteDelay_ticks);
    printf("\r\n");
  
}

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

    eBlkDn_CFM_SENT, // set after CFM packet sent (Write Done or ACK) received

} eBlkDnState_t;


static eBlkDnState_t p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;
*/


//-----------------------------------------------------------------------------
static int32_t blk_dn_start(app_ud2_t *p_app_UD, uint8_t *pkt )
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


static int32_t blk_dn_add(app_ud2_t *p_app_UD, uint8_t *pkt, uint16_t len )
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

static int32_t blk_dn_chk(app_ud2_t *p_app_UD)
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

static int32_t blk_dn_missing_count(app_ud2_t *p_app_UD)
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

static int32_t blk_dn_get_missing(app_ud2_t *p_app_UD, uint8_t* buf, uint8_t count)
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
static uint32_t BlkDn_timer_start(app_ud2_t *p_app_UD, uint32_t timeout_ticks, void* p_context);
static uint32_t BlkDn_timer_stop(void);
static void BlkDn_timeout_handler(void * p_context);


//-----------------------------------------------------------------------------
//
//  Controls
//
//-----------------------------------------------------------------------------
static char S1[] = "S1";
static char S2[] = "S2";

static uint32_t blk_dn_startSend_Dcfm_OK(app_ud2_t *p_app_UD)
{
    uint32_t err_code;
    
    p_app_UD->m_Dcfm_buf[0] = 1;
    p_app_UD->m_Dcfm_buf[1] = 0; // 0 = OK
    p_app_UD->m_Dcfm_len = 2;
    
    // should be this mode p_app_UD->m_BlkDn_sm = eBlkDn_GOT_PACKET;
    
    err_code = ble_ud2_notify_Dcfm(p_app_UD->p_ble_ud2, p_app_UD->m_Dcfm_buf, &p_app_UD->m_Dcfm_len);
    if(err_code == NRF_SUCCESS)
    {
        printf("o");
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_OK_SEND;
        BlkDn_timer_start(p_app_UD, get_dnTxCompleteDelay(), S1); // In Send_Dcfm_OK. wait for tx_complete, if timeout try Dcfm_OK again
    }
    else
    if(err_code == BLE_ERROR_NO_TX_PACKETS)
    {
        printf("n");
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_OK_PRESEND;
        BlkDn_timer_start(p_app_UD, get_dnDataDelay(), S2); // In Send_Dcfm_OK. wait for next/another time when more buffers may be available, then try Dcfm_OK again
    }
    else
    {
        dbgPrint("x");
    }

    return(err_code);    
}

static char S3[] = "S3";
static char S4[] = "S4";
static uint32_t blk_dn_startSend_Dcfm_NG(app_ud2_t *p_app_UD)
{
    uint32_t err_code;
    
    p_app_UD->m_Dcfm_buf[0] = 1;
    p_app_UD->m_Dcfm_buf[1] = 1; // 1 = NG (Checksum NG)
    p_app_UD->m_Dcfm_len = 2;
    
    // should be this mode p_app_UD->m_BlkDn_sm = eBlkDn_GOT_PACKET;
    
    err_code = ble_ud2_notify_Dcfm(p_app_UD->p_ble_ud2, p_app_UD->m_Dcfm_buf, &p_app_UD->m_Dcfm_len);
    if(err_code == NRF_SUCCESS)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_NG_SEND;
        BlkDn_timer_start(p_app_UD, get_dnTxCompleteDelay(), S3); // In Send_Dcfm_NG. wait for tx_complete, if timeout try Dcfm_NG again
    }
    if(err_code == BLE_ERROR_NO_TX_PACKETS)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_NG_PRESEND;
        BlkDn_timer_start(p_app_UD, get_dnDataDelay(), S4); // In Send_Dcfm_NG. wait for next/another time when more buffers may be available, then try Dcfm_NG again
    }

    return(err_code);    
}

static char S5[] = "S5";
static char S6[] = "S6";

static uint32_t blk_dn_startSend_Dcfm_missing(app_ud2_t *p_app_UD, int max_entries)
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
    
    err_code = ble_ud2_notify_Dcfm(p_app_UD->p_ble_ud2, p_app_UD->m_Dcfm_buf, &p_app_UD->m_Dcfm_len);
    if(err_code != NRF_SUCCESS)
    {
        dbgPrint("ble_ud2_notify_Dcfm=<TODO not OK>\n\r");
        //dbgPrint("ble_ud2_notify_Dcfm=%d\n\r", err_code);
    }

    if(err_code == NRF_SUCCESS)
    {
        printf("mo");
        //BlkDn_timer_stop();
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_RE_SEND;
        BlkDn_timer_start(p_app_UD, get_dnTxCompleteDelay(), S5); // In Send_Dcfm_missing. wait for tx_complete, if timeout try Dcfm_missing again
    }    else
    if(err_code == BLE_ERROR_NO_TX_PACKETS)
    {
        printf("mn");
        //BlkDn_timer_stop();
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_RE_PRESEND;
        BlkDn_timer_start(p_app_UD, get_dnDataDelay(), S6); // In Send_Dcfm_missing. wait for next/another time when more buffers may be available, then try Dcfm_missing again
    }    else
    {
        printf("mx");
    }

    return(err_code);    
}

//-----------------------------------------------------------------------------
//
//  Events
//
//-----------------------------------------------------------------------------
void app_ud2_BlkUp_Go_Test(app_ud2_t *p_app_UD);

void  BlkUart_Purge(void);
void  app_ud2_BlkUp_Purge(app_ud2_t *p_app_UD);
static void  BlkDn_Purge(app_ud2_t *p_app_UD)
{
    p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;
    app_ud2_BlkUp_Purge(p_app_UD);
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
int32_t app_ud2_Dcmd_handler(app_ud2_t *p_app_UD, uint8_t *buf, uint8_t len)
{
    int32_t r;
    uint8_t  byteZero;

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
        
        //OLD app_ud2_event.evt_type = APP_TUDS_RX_PKT_1; // Dcmd[1,2] event
        //OLD app_ud2_event.data.value = 42;//dummy
        //OLD m_event_handler(&app_ud2_event);    
        p_app_UD->blkDn_DnEventHandler(p_app_UD, eBlkDn_EV_CMD12, 0);
        
        //callThisWhenBlePacketIsRecieved();
        //m_app_ud2.OnEvent();
        //BlkUart_directUartSend(m_blkDn_buf, m_blkDn_len - 2); // Added "- 2" because there is no need to send the totalCheckSum
        //app_ud2_BlkUp_Go_Test(p_app_UD);    
    }

    if( (buf[0] == 1) && (buf[1] == 3) )
    {
        BlkDn_Purge(p_app_UD);
    }

    if( (buf[0] == 1) && (buf[1] == 42) )
    {
        printConnectionInfo();
        app_ud2_BlkUp_Go_Test(p_app_UD);    
    }

    return(r);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static char S7[] = "S7";
int32_t app_ud2_Ddat_handler(app_ud2_t *p_app_UD, uint8_t *buf, uint8_t len)
{
    int32_t r;

    BlkDn_timer_stop();
    p_app_UD->m_BlkDn_packetWaitTimeCount = 0;
    r = blk_dn_add(p_app_UD, buf, len);
    
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
        BlkDn_timer_start(p_app_UD,  get_dnDataDelay(), S7); // Data Ddat arrived, but still more to come. This timeout in case packets stop comming -> send a Dcfm_mising
    }
    
    return(r);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

static bool m_BlkDn_stateMachineLocked = false;

static void BlkDn_lockStateMachine(void)
{
    m_BlkDn_stateMachineLocked = true;
}
void BlkDn_unlockStateMachine(void)
{
    m_BlkDn_stateMachineLocked = false;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void on_blk_dn_packetReceived(app_ud2_t *p_app_UD)
{
    // Finally, let the SM accept new packets
    BlkDn_lockStateMachine();
    p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD;

#if 1 //testing use 1 karel
    //karel app_ud2_BlkUp_Go_Test(p_app_UD);
    printf("\nPackets RX DONE\n");
   
    BlkDn_unlockStateMachine();
#else
    if(p_app_UD->blkDn_DnEventHandler)
        p_app_UD->blkDn_DnEventHandler(p_app_UD, eBlkDn_EV_RXDONE, 0);
#endif
}

static char S8[] = "S8";
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int32_t app_ud2_Dcfm_handler_OnWrittenComplete(app_ud2_t *p_app_UD,  uint8_t *buf, uint8_t len) //int32_t BlkDn_On_written_Dcfm(ble_ud2_t *p_tuds,  uint8_t *buf, uint8_t len)
{
    int32_t r;

    // Check if in proper state
    if( (p_app_UD->m_BlkDn_sm != eBlkDn_CFM_OK_SEND) &&
        (p_app_UD->m_BlkDn_sm != eBlkDn_CFM_NG_SEND) &&
        (p_app_UD->m_BlkDn_sm != eBlkDn_CFM_RE_SEND)   )
        return(0);

    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_SEND)   dbgPrint(" CFM OK Send ");
    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_NG_SEND)   dbgPrint(" CFM NG Send ");
    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_SEND)   dbgPrint(" CFM RE Send ");

    //dbgPrint("C");
    //dbgPrint("C");

    BlkDn_timer_stop();
    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_SEND)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_CFM_SENT; 
        on_blk_dn_packetReceived(p_app_UD);
    }
    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_NG_SEND)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_CMD; 
    }
    if( p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_SEND)
    {
        p_app_UD->m_BlkDn_sm = eBlkDn_WAIT_PACKET;
        BlkDn_timer_start(p_app_UD, get_dnDataDelay(), S8); // In tx_complete, if Dcfm_missing was what wa sent, this timeout to wait for more data
    }
    return(r);
}


//-----------------------------------------------------------------------------
//
//  Timer
//
//-----------------------------------------------------------------------------

APP_TIMER_DEF(m_BlkDn_timer_id);


PUBLIC uint32_t app_ud2_BlkDn_timer_init(void)
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

static uint32_t BlkDn_timer_start(app_ud2_t *p_app_UD, uint32_t timeout_ticks, void* p_context)
{
    uint32_t err_code;

//SAD
    err_code = app_timer_stop(m_BlkDn_timer_id);
    if( err_code != NRF_SUCCESS )
    {
        printf("@DS NG err_code = %d\r\n", err_code);
    }
    err_code = app_timer_start(m_BlkDn_timer_id, timeout_ticks, p_app_UD); //p_context);
    if( err_code != NRF_SUCCESS )
    {
        printf("@DG NG err_code = %d\r\n", err_code);
    }

    //printf("@D");
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}

static uint32_t BlkDn_timer_stop(void)
{
    uint32_t err_code;
    // Stop timer
    err_code = app_timer_stop(m_BlkDn_timer_id);
    if( err_code != NRF_SUCCESS )
    {
        printf("@DS NG err_code = %d\r\n", err_code);
    }
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}


static void blk_dn_printState(app_ud2_t *p_app_UD)
{
    if(p_app_UD->m_BlkDn_sm == eBlkDn_WAIT_CMD       ) dbgPrint("[W_C]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_WAIT_PACKET    ) dbgPrint("[W_P]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_PRESEND ) dbgPrint("[M_P]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_RE_SEND    ) dbgPrint("[M_S]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_GOT_PACKET     ) dbgPrint("[G_P]");
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_PRESEND ) dbgPrint("[C_P]"); // set before CFM packet sent
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_OK_SEND    ) dbgPrint("[C_S]"); // set after CFM packet sent
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_NG_SEND    ) dbgPrint("[C_S]"); // set after CFM packet sent
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_SENT       ) dbgPrint("[C_T]"); // set after CFM packet sent (Write Done or ACK) received
}

static void BlkDn_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    
    app_ud2_t *p_app_UD;
    p_app_UD = p_context; //WWRONG
    
    dbgPrint("*");
    blk_dn_printState(p_app_UD);
    
    //printf((char*)p_context);
    
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
    if(p_app_UD->m_BlkDn_sm == eBlkDn_CFM_SENT)
    {
    }
    else
    {
    }
}


