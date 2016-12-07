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

#include "myapp.h"


#if USE_UD2  //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

#define PUBLIC 

//-----------------------------------------------------------------------------
#define APP_TIMER_PRESCALER 0                               //  APP_TIMER_PRESCALER 0 is usually defined in main (used for time calculations)
#define BLE_ERROR_NO_TX_BUFFERS BLE_ERROR_NO_TX_PACKETS     //  SDK differences, different naming

//-----------------------------------------------------------------------------
extern ble_ud2_t  m_ble_ud2;                                //  used here for sending Notifications


static uint8_t  m_Dcfm_buf[20];
static uint16_t m_Dcfm_len;


//-----------------------------------------------------------------------------
/*static*/ void BlkDn_unlockStateMachine(void); //karel <----Undefined symbol BlkDn_unlockStateMachine (referred from ma_join.o).


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
	  //karel
    return dnDataDelay_ticks;
	  //karel
	
    if(local_conn_interval == m_current_conn_params.max_conn_interval)
        return dnDataDelay_ticks;
    
    local_conn_interval = m_current_conn_params.max_conn_interval;
    
    dnDataDelay_ms = 16 * 1.25 * m_current_conn_params.max_conn_interval;
    dnDataDelay_ticks = APP_TIMER_TICKS(dnDataDelay_ms, APP_TIMER_PRESCALER);
    
    dnTxCompleteDelay_ms = 16 * 1.25 * m_current_conn_params.max_conn_interval;
    dnTxCompleteDelay_ticks = APP_TIMER_TICKS(dnTxCompleteDelay_ms, APP_TIMER_PRESCALER);

    dbgPrintf("dnDataDelay_ticks       = %d\r\n", dnDataDelay_ticks);
    dbgPrintf("dnTxCompleteDelay_ticks = %d\r\n", dnTxCompleteDelay_ticks);

    return dnDataDelay_ticks;
}

static inline uint32_t get_dnTxCompleteDelay(void)
{
	  //karel
    return dnTxCompleteDelay_ticks;
	  //karel
	
    if(local_conn_interval == m_current_conn_params.max_conn_interval)
        return dnTxCompleteDelay_ticks;
    
    local_conn_interval = m_current_conn_params.max_conn_interval;
    
    dnDataDelay_ms = 16 * 1.25 * m_current_conn_params.max_conn_interval;
    dnDataDelay_ticks = APP_TIMER_TICKS(dnDataDelay_ms, APP_TIMER_PRESCALER);
    
    dnTxCompleteDelay_ms = 16 * 1.25 * m_current_conn_params.max_conn_interval;
    dnTxCompleteDelay_ticks = APP_TIMER_TICKS(dnTxCompleteDelay_ms, APP_TIMER_PRESCALER);

    dbgPrintf("dnDataDelay_ticks       = %d\r\n", dnDataDelay_ticks);
    dbgPrintf("dnTxCompleteDelay_ticks = %d\r\n", dnTxCompleteDelay_ticks);

    return dnTxCompleteDelay_ticks;
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

#define BLK_DN_COUNT (128 + 8)

/*static*/ uint8_t  m_blkDn_buf[ 16 * BLK_DN_COUNT ]; // 2048 @ BLK_DN_COUNT = 128          // <----Undefined symbol m_blkDn_buf (referred from ss_thread.o).
static uint8_t  m_blkDn_chk[  1 * BLK_DN_COUNT ]; //  128 @ BLK_DN_COUNT = 128
/*static*/ uint16_t m_blkDn_len;         // <---- Undefined symbol m_blkDn_len (referred from ma_join.o).
static uint16_t m_blkDn_blkCnt;
static uint16_t m_blkDn_rxBlkCnt;

extern app_ud2_event_handler_t    m_event_handler;

//app_ud2_t                  m_app_ud2;
//app_ud2_evt_t

//int callThisWhenUartPacketForBleIsRecieved(void){ return(-1);} //ma_join.c
//int callThisWhenBlePacketIsRecieved(app_ud2_evt_t * p_app_ud2_event){return(-1);}  //ma_join.c

/* ----------------------------------------------------------------------------
*  FUNCTIONS
*/

//void      ma_ud2_on_ble_evt(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt); //wrapper for ble_ud2_on_ble_evt

//uint32_t  ma_ud2_send_packet(app_ud2_t * p_ud2, uint8_t * buf, uint16_t* p_len16);

//void unused_function_calls_m_event_handler()
//{
//    app_ud2_evt_t app_ud2_event;
//    
//    m_event_handler(&app_ud2_event);
//}
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

typedef enum eBlkDnState
{
    eBlkDn_WAIT_CMD,
    eBlkDn_WAIT_PACKET,
    eBlkDn_MISSING_PRESEND,
    eBlkDn_MISSING_SEND,
    eBlkDn_GOT_PACKET,
    eBlkDn_CFM_PRESEND, // set before CFM packet sent
    eBlkDn_CFM_SEND, // set after CFM packet sent
    eBlkDn_CFM_SENT, // set after CFM packet sent (Write Done or ACK) received

    //eBlkDn_PROCESS_PACKET,
    //eBlkDn_SEND_CMD,
    //eBlkDn_SEND_DATA,
    //eBlkDn_SEND_WAITCFM,

} eBlkDnState_t;


static eBlkDnState_t m_BlkDn_sm = eBlkDn_WAIT_CMD;



//-----------------------------------------------------------------------------
static int32_t blk_dn_start( uint8_t *pkt )
{
    uint8_t  j;
    uint16_t i;

    m_blkDn_rxBlkCnt = 0;
    m_blkDn_blkCnt = 0;    
    
    m_blkDn_len = pkt[2] | (pkt[3]<<8);
    if( m_blkDn_len == 0)
        return(1);
    
    m_blkDn_blkCnt = ((m_blkDn_len - 1) / 16) + 1;
    
    
    for(i=0 ; i < m_blkDn_blkCnt; i++)
    {
        m_blkDn_chk[i] = 0x00;
        for(j=0 ; j < 16; j++)
        {
            m_blkDn_buf[i*16 + j] = 0x00;
        }
    }
    
    return(0);
}


static int32_t blk_dn_add( uint8_t *pkt, uint16_t len )
{
    uint8_t  position;
    uint8_t  j;
//    uint16_t i;
    
    if(len != 20)
        return(1);
    
    position = pkt[0];
    if( position >= BLK_DN_COUNT)
        return(1);
    
    m_blkDn_rxBlkCnt++;
    m_blkDn_chk[position] += 1;
    for(j=0 ; j < 16; j++)
    {
        m_blkDn_buf[position*16 + j] = pkt[4 + j];
    }
    return(0);
}

#define DN_CHK_OK 0
#define DN_CHK_CHKSUM_NG 3

static int32_t blk_dn_chk()
{
//    uint8_t  j;
    uint16_t i;
    uint16_t missing_blk_cnt;
    uint16_t cs_pkt;
    uint16_t cs_now;
 
    if(m_blkDn_blkCnt == 0)
        return(1);

    if(m_blkDn_rxBlkCnt < m_blkDn_blkCnt)
        return(1);
    
    missing_blk_cnt = 0;
    for(i=0 ; i < m_blkDn_blkCnt; i++)
    {
        if(m_blkDn_chk[i] == 0x00)
            missing_blk_cnt++;
    }
    if(missing_blk_cnt>0)
        return(2);

    cs_pkt = m_blkDn_buf[m_blkDn_len - 2] | (m_blkDn_buf[m_blkDn_len - 1]<<8);
#if 0 // _USE_CRC
    cs_now = CRC_START_SEED; //0x0000;//0xFFFF;
    c_now = crc16_compute( &m_blkDn_buf[0], m_blkDn_len - 2, cs_now);
#else    
    cs_now = 0;
    for(i=0 ; i < m_blkDn_len - 2; i++)
    {
        cs_now += m_blkDn_buf[i];
    }
#endif    
    if( cs_now != cs_pkt)
        return( DN_CHK_CHKSUM_NG );
    
    return(DN_CHK_OK);
}

static int32_t blk_dn_missing_count()
{
    uint16_t i;
    uint16_t missing_blk_cnt;
 
    if(m_blkDn_blkCnt == 0)
        return(0);
   
    missing_blk_cnt = 0;
    for(i=0 ; i < m_blkDn_blkCnt; i++)
    {
        if(m_blkDn_chk[i] == 0x00)
            missing_blk_cnt++;
    }
    return(missing_blk_cnt);
}

static int32_t blk_dn_get_missing(uint8_t* buf, uint8_t count)
{
    uint16_t i;
    uint16_t missing_blk_cnt;
 
    if(m_blkDn_blkCnt == 0)
        return(0);
   
    missing_blk_cnt = 0;
    for(i=0 ; i < m_blkDn_blkCnt; i++)
    {
        if(m_blkDn_chk[i] == 0x00)
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
//PUBLIC uint32_t BlkDn_timer_init(void);
static uint32_t BlkDn_timer_start(uint32_t timeout_ticks, void* p_context);
static uint32_t BlkDn_timer_stop(void);
static void BlkDn_timeout_handler(void * p_context);


//-----------------------------------------------------------------------------
//
//  Controls
//
//-----------------------------------------------------------------------------
static char S1[] = "S1";
static char S2[] = "S2";

static uint32_t blk_dn_startSend_Dcfm_OK()
{
    uint32_t err_code;
    
    m_Dcfm_buf[0] = 1;
    m_Dcfm_buf[1] = 0; // 0 = OK
    m_Dcfm_len = 2;
    
    // should be this mode m_BlkDn_sm = eBlkDn_GOT_PACKET;
    
    err_code = ble_ud2_notify_Dcfm( &m_ble_ud2, m_Dcfm_buf, &m_Dcfm_len);
    if(err_code == NRF_SUCCESS)
    {
        dbgPrint("o");
        m_BlkDn_sm = eBlkDn_CFM_SEND;
        BlkDn_timer_start( get_dnTxCompleteDelay()/*APP_TIMER_TICKS(222, APP_TIMER_PRESCALER)*/, S1); // In Send_Dcfm_OK. wait for tx_complete, if timeout try Dcfm_OK again
    }
    else
    if(err_code == BLE_ERROR_NO_TX_PACKETS)
    {
        dbgPrint("n");
        // should be this mode m_BlkDn_sm = eBlkDn_GOT_PACKET;
        BlkDn_timer_start( get_dnDataDelay()/*APP_TIMER_TICKS(222, APP_TIMER_PRESCALER)*/, S2); // In Send_Dcfm_OK. wait for next/another time when more buffers may be available, then try Dcfm_OK again
    }
    else
    {
        dbgPrint("x");
    }

    return(err_code);    
}

static char S3[] = "S3";
static char S4[] = "S4";
static uint32_t blk_dn_startSend_Dcfm_NG()
{
    uint32_t err_code;
    
    m_Dcfm_buf[0] = 1;
    m_Dcfm_buf[1] = 1; // 1 = NG (Checksum NG)
    m_Dcfm_len = 2;
    
    // should be this mode m_BlkDn_sm = eBlkDn_GOT_PACKET;
    
    err_code = ble_ud2_notify_Dcfm( &m_ble_ud2, m_Dcfm_buf, &m_Dcfm_len);
    if(err_code == NRF_SUCCESS)
    {
        m_BlkDn_sm = eBlkDn_CFM_SEND;
        BlkDn_timer_start( get_dnTxCompleteDelay()/*APP_TIMER_TICKS(222, APP_TIMER_PRESCALER)*/, S3); // In Send_Dcfm_NG. wait for tx_complete, if timeout try Dcfm_NG again
    }
    if(err_code == BLE_ERROR_NO_TX_BUFFERS)
    {
        // should be this mode m_BlkDn_sm = eBlkDn_GOT_PACKET;
        BlkDn_timer_start( get_dnDataDelay()/*APP_TIMER_TICKS(222, APP_TIMER_PRESCALER)*/, S4); // In Send_Dcfm_NG. wait for next/another time when more buffers may be available, then try Dcfm_NG again
    }

    return(err_code);    
}

static char S5[] = "S5";
static char S6[] = "S6";
static uint32_t blk_dn_startSend_Dcfm_missing(int max_entries)
{
    uint32_t err_code;
    
    m_Dcfm_buf[0] = 1;
    m_Dcfm_buf[1] = 2; // 2 = Missing
    
    int n = blk_dn_missing_count();
    if( n > max_entries )
        n = max_entries;
    m_Dcfm_buf[2] = n;
    blk_dn_get_missing( &m_Dcfm_buf[3], n );
    m_Dcfm_len = 3 + n;
    
    // should be this mode m_BlkDn_sm = eBlkDn_GOT_PACKET;
    
    err_code = ble_ud2_notify_Dcfm( &m_ble_ud2, m_Dcfm_buf, &m_Dcfm_len);
    if(err_code == NRF_SUCCESS)
    {
        dbgPrint("mo");
        //BlkDn_timer_stop();
        m_BlkDn_sm = eBlkDn_MISSING_SEND;
        BlkDn_timer_start( get_dnTxCompleteDelay()/*APP_TIMER_TICKS(222, APP_TIMER_PRESCALER)*/, S5); // In Send_Dcfm_missing. wait for tx_complete, if timeout try Dcfm_missing again
    }
    else
    if(err_code == BLE_ERROR_NO_TX_BUFFERS)
    {
        dbgPrint("mn");
        // should be this mode m_BlkDn_sm = eBlkDn_GOT_PACKET;
        //BlkDn_timer_stop();
        m_BlkDn_sm = eBlkDn_MISSING_PRESEND;
        BlkDn_timer_start( get_dnDataDelay()/*APP_TIMER_TICKS(222, APP_TIMER_PRESCALER)*/, S6); // In Send_Dcfm_missing. wait for next/another time when more buffers may be available, then try Dcfm_missing again
    }
    else
    {
        dbgPrint("mx");
    }

    return(err_code);    
}

//-----------------------------------------------------------------------------
//
//  Events
//
//-----------------------------------------------------------------------------
void BlkUp_Go_Test(void);

void  BlkUart_Purge(void);
void  BlkUp_Purge(void);
void  BlkDn_Purge(void)
{
    m_BlkDn_sm = eBlkDn_WAIT_CMD;
    BlkUp_Purge();
#if USE_DRCT
    BlkUart_Purge();
#endif
}
//-----------------------------------------------------------------------------
//
//  Events
//
//-----------------------------------------------------------------------------

static int m_BlkDn_packetWaitTimeCount = 0;

int32_t app_ud2_Dcmd_handler(app_ud2_t *p_app_ud2, uint8_t *buf, uint8_t len)
{
    int32_t r;
    uint8_t  byteZero;
    app_ud2_evt_t app_ud2_event;

    if( (buf[0] == 1) && (buf[1] == 1) )
    {
        m_BlkDn_packetWaitTimeCount = 0;
        dbgPrint("S");
        r = blk_dn_start(buf);
        m_BlkDn_sm = eBlkDn_WAIT_PACKET;

    }
    if( (buf[0] == 1) && (buf[1] == 2) )
    {
        byteZero = 0;
        
        m_blkDn_len = 3;
        m_blkDn_buf[0] = byteZero;
        m_blkDn_buf[1] = 0x00; // totalCheckSum
        m_blkDn_buf[2] = 0x00; // totalCheckSum
        
    
        app_ud2_event.evt_type = APP_TUDS_RX_PKT_1; // Dcmd[1,2] event
        app_ud2_event.data.value = 42;//dummy
        m_event_handler(&app_ud2_event);    
        
        //callThisWhenBlePacketIsRecieved();
        //m_app_ud2.OnEvent();
        //BlkUart_directUartSend(m_blkDn_buf, m_blkDn_len - 2); // Added "- 2" because there is no need to send the totalCheckSum
        //BlkUp_Go_Test();    
    }
    if( (buf[0] == 1) && (buf[1] == 3) )
    {
        BlkDn_Purge();
    }

    if( (buf[0] == 1) && (buf[1] == 42) )
    {
        BlkUp_Go_Test();    
    }

    return(r);
}

static char S7[] = "S7";
int32_t app_ud2_Ddat_handler(app_ud2_t *p_app_ud2, uint8_t *buf, uint8_t len)
{
    int32_t r;

    BlkDn_timer_stop();
    m_BlkDn_packetWaitTimeCount = 0;
    //dbgPrint("-");
    r = blk_dn_add( buf, len);
    
    r = blk_dn_chk();
    if(r == DN_CHK_OK)
    {
        dbgPrint("O");
        //dbgPrint("O");
        m_BlkDn_sm = eBlkDn_GOT_PACKET;
        blk_dn_startSend_Dcfm_OK();
    }
    else
    if(r == DN_CHK_CHKSUM_NG)
    {
        dbgPrint("N");
        //dbgPrint("N");
        m_BlkDn_sm = eBlkDn_GOT_PACKET;
        blk_dn_startSend_Dcfm_NG();
    }
    else
    {
        //dbgPrint("%%");
        //karel dbgPrint("D");
        BlkDn_timer_start( get_dnDataDelay()/*APP_TIMER_TICKS(222, APP_TIMER_PRESCALER)*/, S7); // Data Ddat arrived, but still more to come. This timeout in case packets stop comming -> send a Dcfm_mising
    }
    
    return(r);
}

bool m_BlkDn_stateMachineLocked = false;


void BlkDn_lockStateMachine(void)
{
    m_BlkDn_stateMachineLocked = true;
}
void BlkDn_unlockStateMachine(void)
{
    m_BlkDn_stateMachineLocked = false;
}

void  NEWcallThisWhenBlePacketIsRecieved(void);

static void on_blk_dn_packetReceived()
{
    //app_ud2_evt_t app_ud2_event;

    // Finally, let the SM accept new packets
    BlkDn_lockStateMachine();
    m_BlkDn_sm = eBlkDn_WAIT_CMD;

#if 0 //testing use 1 karel
    //karel BlkUp_Go_Test();
    dbgPrint("\nPackets RX DONE\n");
   
    BlkDn_unlockStateMachine();
#else
    //OLD app_ud2_event.evt_type = APP_TUDS_RX_PKT_0; // Dcmd[1,1] ... packet received event
    //OLD app_ud2_event.data.value = 42;//dummy
    //OLD m_event_handler(&app_ud2_event);    
    //OLD //callThisWhenBlePacketIsRecieved();
    NEWcallThisWhenBlePacketIsRecieved();
#endif
}

static char S8[] = "S8";
int32_t app_ud2_OnWrittenComplete_Dcfm_handler(app_ud2_t *p_app_ud2,  uint8_t *buf, uint8_t len)
{
    int32_t r;

    // Check if in proper state
    if( (m_BlkDn_sm != eBlkDn_CFM_SEND) &&  (m_BlkDn_sm != eBlkDn_MISSING_SEND)   )
        return(0);

    if( m_BlkDn_sm == eBlkDn_CFM_SEND)
    {
        dbgPrint(" CFM Send ");
    }
    if(m_BlkDn_sm == eBlkDn_MISSING_SEND) 
    {
        dbgPrint(" MissSend ");
    }
    //dbgPrint("C");
    //dbgPrint("C");

    BlkDn_timer_stop();
    if( m_BlkDn_sm == eBlkDn_CFM_SEND)
    {
        m_BlkDn_sm = eBlkDn_CFM_SENT; 
        on_blk_dn_packetReceived();
    }
    if( m_BlkDn_sm == eBlkDn_MISSING_SEND)
    {
        m_BlkDn_sm = eBlkDn_WAIT_PACKET;
        BlkDn_timer_start( get_dnDataDelay()/*APP_TIMER_TICKS(222, APP_TIMER_PRESCALER)*/, S8); // In tx_complete, if Dcfm_missing was what wa sent, this timeout to wait for more data
    }
    return(r);
}


//-----------------------------------------------------------------------------
//
//  Timer
//
//-----------------------------------------------------------------------------

APP_TIMER_DEF(m_BlkDn_timer_id);


//static uint32_t m_app_ticks_per_100ms;
//#define BSP_MS_TO_TICK(MS) (m_app_ticks_per_100ms * (MS / 100))

PUBLIC uint32_t BlkDn_timer_init(void)
{
    uint32_t err_code;

    // Create timer
    err_code = app_timer_create(&m_BlkDn_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT, //APP_TIMER_MODE_REPEATED,
                                BlkDn_timeout_handler);
    if (err_code != NRF_SUCCESS)
    {
        ;
    }
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}

static uint32_t BlkDn_timer_start(uint32_t timeout_ticks, void* p_context)
{
    uint32_t err_code;

//SAD
    err_code = app_timer_stop(m_BlkDn_timer_id);
    if( err_code != NRF_SUCCESS )
    {
        dbgPrintf("@DS NG err=code = %d\r\n", err_code);
    }
    err_code = app_timer_start(m_BlkDn_timer_id, timeout_ticks,  p_context); //NULL);
    if( err_code != NRF_SUCCESS )
    {
        dbgPrintf("@DG NG err=code = %d\r\n", err_code);
    }

    //dbgPrint("@D");
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
        dbgPrintf("@DS NG err=code = %d\r\n", err_code);
    }
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}

static void blk_dn_printState(void)
{
    if(m_BlkDn_sm == eBlkDn_WAIT_CMD       ) dbgPrint("(W_C)");
    if(m_BlkDn_sm == eBlkDn_WAIT_PACKET    ) dbgPrint("(W_P)");
    if(m_BlkDn_sm == eBlkDn_MISSING_PRESEND) dbgPrint("(M_P)");
    if(m_BlkDn_sm == eBlkDn_MISSING_SEND   ) dbgPrint("(M_S)");
    if(m_BlkDn_sm == eBlkDn_GOT_PACKET     ) dbgPrint("(G_P)");
    if(m_BlkDn_sm == eBlkDn_CFM_PRESEND    ) dbgPrint("(C_P)"); // set before CFM packet sent
    if(m_BlkDn_sm == eBlkDn_CFM_SEND       ) dbgPrint("(C_S)"); // set after CFM packet sent
    if(m_BlkDn_sm == eBlkDn_CFM_SENT       ) dbgPrint("(C_T)"); // set after CFM packet sent (Write Done or ACK) received
}

static void BlkDn_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
#if 0
    dbgPrint("*");
#else    
    dbgPrint("*");
    blk_dn_printState();
    
    //dbgPrint((char*)p_context);
    
    if(m_BlkDn_sm == eBlkDn_WAIT_CMD) // Do nothing
    { } //Just waiting for a start up packet
    else
    if(m_BlkDn_sm == eBlkDn_WAIT_PACKET) // Should have got the next data packet by now
    {
        if(++m_BlkDn_packetWaitTimeCount > 3)
        {
            m_BlkDn_packetWaitTimeCount = 0;
            m_BlkDn_sm = eBlkDn_WAIT_CMD; // Reset
        }
        else
        {
            dbgPrint("(WP)");
            blk_dn_startSend_Dcfm_missing(4);
        }
    }
    else
    if(m_BlkDn_sm == eBlkDn_GOT_PACKET) // Do nothing
    { } // Just got the packet so I should have cancelled the timer
    else
    if(m_BlkDn_sm == eBlkDn_MISSING_PRESEND) // buffer was full last time, so try again
    {
            dbgPrint("(MPre)");
            blk_dn_startSend_Dcfm_missing(4);
    }
    else
    if(m_BlkDn_sm == eBlkDn_MISSING_SEND) // we didn't get a write done
    {
            dbgPrint("(MSend)");
            blk_dn_startSend_Dcfm_missing(4);
    }
    else
    if(m_BlkDn_sm == eBlkDn_CFM_PRESEND) // buffer was full last time, so try again
    {
        blk_dn_startSend_Dcfm_OK();
    }
    else
    if(m_BlkDn_sm == eBlkDn_CFM_SEND) // we didn't get a write done
    {
        blk_dn_startSend_Dcfm_OK();
    }
    else
    if(m_BlkDn_sm == eBlkDn_CFM_SENT)
    {
    }
    else
    {
    }
#endif
}

#else   //#if USE_UD2  //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
#endif //#if USE_UD2  //''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

