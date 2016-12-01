
#ifndef TUDS_NULL_H__
#define TUDS_NULL_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "stdio.h"

#include "stdarg.h"

#include "ble_tuds.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// http://www.cplusplus.com/reference/cstdio/printf/
// http://www.cplusplus.com/reference/cstdio/vprintf/

int dbgPrintf( const char * format, ... );
int dbgPrint( const char * str );
    
#define  PRINTF_OVER_UART 0
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    

extern bool m_bleIsConnected;

//=============================================================================
typedef enum eEvtType
{
    evt_ConState_Connected,
    evt_ConState_Disconnected,
    
    UnUsed_evt_MpState_DriveWakeup_Start,
    evt_MpState_DriveWakeup_Done,
    evt_MpState_ReleaseWakeup,
    
    // Event
    evt_Uart_RxReady,
    evt_Uart_TxEmpty,
    
    evt_core_ADC_trigger,
    evt_core_GBN_trigger,
    
//DFCT    evt_core_ADC_getbuf,
//DFCT    evt_core_UCm_Release,
    
    
#if( CORE_SLAVE == 1 )    

    // Core Slave
    evt_Core_CoreSOnBlePkt,     //n   // -> evt_bleMaster_CoreS_procDone
    evt_Core_CoreSSendBlePkt,   //n
 
    evt_Core_CoreSOnUartPkt,    //n  // -> evt_UartpacketFromCoreS //evt_uartMaster_CoreS_procDone
    evt_Core_CoreSSendUartPkt,  //n

    evt_core_UCs_Req,           //Y

    evt_bleMaster_startCoreS_proc,  //y
    evt_bleMaster_CoreSRspTimeout,  //n
    evt_bleMaster_CoreS_procDone    //n
#endif

#if DIRECT_BITCH
#else
    evt_bleMaster_startBle_proc,
    evt_coreMaster_startCoreM_proc,
#endif

    // Core Master
    evt_coreMaster_UartTxDone,
    evt_coreMaster_UartRxDone,

    evt_xxxMaster_UartRxTimeout,
    
    // bleMaster
    evt_bleMaster_trigger,
    evt_bleMaster_UartTxDone,
    evt_bleMaster_UartRxDone,
    evt_bleMaster_BleRspDone,



} eEvtType_t;

typedef struct uniEvent_s
{
    eEvtType_t evtType;
    int i;
} uniEvent_t;


void BlkDn_On_Dcmd(uint8_t * p_data, uint16_t length);
void BlkDn_On_Ddat(uint8_t * p_data, uint16_t length);
void BlkUp_On_Ucfm(uint8_t * p_data, uint16_t length);

void BlkDn_On_written_Dcfm(ble_tuds_t * p_tuds, int x, int y);
void BlkUp_On_written_Ucmd(ble_tuds_t * p_tuds, int x, int y);
void BlkUp_On_written_Udat(ble_tuds_t * p_tuds, int x, int y);

extern bool m_doUpdateAdvertisingName;
extern char m_UpdatedAdvertisingName[42];

//#define   PRIORITY_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE) 

extern uint32_t DUMB_counterA;



void sdo_Init(void);
void sdo_AppendText(char *S);

void ma_adc_config(void);
    
//void bas_timer_init(void);
void pinWakeup_timer_init(void);

void ADC_timer_init(void);  // karel - Init the timer for reading the ADC
void Uart_timer_init(void); // karel - Init the timer for Uart Shutdown timimg
void BlkDn_timer_init(void); // karel
void BlkUp_timer_init(void); // karel

uint32_t Fake_1_timer_init(void);
uint32_t Fake_1_timer_start(uint32_t timeout_ticks);
uint32_t Fake_1_timer_stop(void);

uint32_t GBN_timer_init(void);
uint32_t GBN_timer_start(uint32_t timeout_ticks);
uint32_t GBN_timer_stop(void);


void core_thread_init(void);
void gInit_All(void);
void priority_sched_execute(void);
void uart_thread_init(void);

  
void core_thread_QueueSend(uniEvent_t *pEvt); 
    
#ifdef __cplusplus
}
#endif

#endif // TUDS_NULL_H__

