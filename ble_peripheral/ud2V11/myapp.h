#ifndef __MYAPP_H
#define __MYAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

    
#define FIRMWARE_REV_LSB  0x91
#define FIRMWARE_REV_MSB  0x00 // e.g. 0x0100 -> Version 1.00

//---------------------------------------------------------
#define  USE_SDK_V11 1
#if USE_SDK_V11
#endif


    
/*TODOT
    
#define USE_SS 1    
    
#include "debug_etc.h"

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
//#include "ble_bas.h"
//#include "ble_hrs.h"
//#include "ble_dis.h"
#include "ble_conn_params.h"
#include "boards.h"
//#include "sensorsim.h"
#include "softdevice_handler.h"
#include "app_timer.h"
//#include "device_manager.h"
#include "pstorage.h"
#include "app_trace.h"
#include "bsp.h"
#include "bsp_btn_ble.h"

#define WAKE_ACTIVE_HIGH  1//0


#define USE_FAKE_9E 0
#define USE_FUDGE_9E 0

#define ADC_debug1  0


//#define  ADC_TIMER_PERIOD_10000_MS     10000
//#define  ADC_TIMER_PERIOD_1000_MS       1000


#define  GP_TIMER_PERIOD_1000MS   1000

#define AUTOTIMEOUT_STDTIME 60
#define AUTOTIMEOUT_NONE 0
void autoTimeout_Start( uint32_t timeout );

//-----------------------------------------------------------------------------
#ifndef __M_ADC_H
#define __M_ADC_H

#define USE_NEW_ADC 1

extern bool g_PRE;
extern float vPre_0;
extern float vPre_1;
extern float vPre_2;


//-----------------------------------------------------------------------------
#define ADC_PROC_INIT_TRIGGER   0
#define ADC_PROC_TIMER_TICK     1
#define ADC_PROC_UNPARK         2
#define ADC_PROC_ADC_TRIGGER    3
int adc_proc(int param);
extern bool   m_ADC_notEnabled;

int adc_proc_PRE(int param);

#endif //__M_ADC_H


//-----------------------------------------------------------------------------
#define BLN_PROC_INIT_TRIGGER    0
#define BLN_PROC_TIMER_TICK      1
#define BLN_PROC_UNPARK          2
#define BLN_PROC_FORCE_TRIGGER   3
void bln_proc(int param);

//-----------------------------------------------------------------------------
#define BLP_PROC_INIT_TRIGGER    0
#define BLP_PROC_TIMER_TICK      1
#define BLP_PROC_UNPARK          2
#define BLP_PROC_FORCE_TRIGGER   3
void blp_proc(int param);


//=============================================================================
// TYPES

typedef enum epktType
{
    ePkt_Unknown,
    ePkt_0xC0,
    ePkt_0x01,
    ePkt_T2,
    ePkt_K2,
} epktType_t;


struct be_s;
typedef struct be_s be_t;

typedef struct be_s
{
    void      (*vIndicateUse_Start)(be_t* this_be);
    void      (*vIndicateUse_End)(be_t* this_be);
    void*      vpSemaphore;
    epktType_t pktType;

    uint8_t  c0;
    uint8_t  c1;
    uint8_t  *buffer;
    uint32_t capacity;
    uint32_t rdPtr;
    uint32_t wrPtr;
    uint32_t length;
} be_t;


//typedef struct frameUni_s frameUni_t;

extern bool m_bleIsConnected;

TODOT*/

//=============================================================================
typedef enum eEvtType
{
    evt_ConState_Connected,                         // 0
    evt_ConState_Disconnected,                      // 1
    
    // Event
    evt_Uart_RxReady,                               // 2
    evt_Uart_TxEmpty,                               // 3
    
    evt_core_BLN_trigger,                           // 4
    evt_core_BLP_trigger,                           // 5
    evt_core_ADC_trigger,                           // 6
    
    // Core Master
    evt_coreMaster_UartTxDone,                      // 7
    evt_coreMaster_UartRxDone,                      // 8

    evt_xxxMaster_UartRxTimeout,                    // 9
    
    // bleMaster
    evt_bleMaster_trigger,                          // 10
    evt_bleMaster_UartTxDone,                       // 11
    evt_bleMaster_UartRxDone,                       // 12
    evt_bleMaster_BleRspDone,                       // 13

    // SS
    evt_SSQ_Check_Queue,                            // 14
    evt_SSQ_StartNext_TxRx,                         // 15
    evt_SSQ_TxRxOrTimeout_Done,                     // 16
    


} eEvtType_t;

typedef struct uniEvent_s
{
    eEvtType_t evtType;
    int i;
} uniEvent_t;

/* TODOT
//=============================================================================
// predefines
// Buffers

#if( CORE_SLAVE == 1 )  
extern be_t be_CUs;
extern be_t be_UCs;
#endif

extern be_t be_CUm;
extern be_t be_UCm;
extern be_t be_BU;
extern volatile be_t be_UB;
extern be_t be_Urx;

extern uint32_t DUMB_counterA;

extern volatile be_t *m_curr_beUrx;
extern be_t *m_curr_beUtx;


void core_thread(void * arg);
void uart_thread(void * arg);




//TODOT #include "ma_timers.h"
// rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_
// rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_



// rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_
// rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_ rn_


// ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_
// ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_


void ma_adc_config(void);
    
    
void ma_uart_Init(void);
void ma_uart_Deinit(void);


// ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_
// ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_ ma_
    

int DEVT_uartRxReady(void);
int DEVT_uartTxEmpty(void);
    

//void sdo_Init(void);
//void sdo_AppendText( char *);

// dbgPrint
// sdo_AppendText_
// sdo_Deinit
// sdo_Init
// sdo_master_0_event_handler
// sdo_next
// sdo_test




//TODOT #include "u_proc0x01.h"


    
bool gInit_All(void);

bool ma_uart_packetTx_start(void);

extern char m_s1[256];

//-----------------------------------------------------------------------------
// ma_serio.c
void uart_thread_init(void);
void uart_thread_QueueSend(uniEvent_t *pEvt);

void set_curr_beUrx_toStartPosition(void);

//-----------------------------------------------------------------------------
// ma_thread.c
void core_thread_init(void);
void core_thread_QueueSend(uniEvent_t *pEvt);

//-----------------------------------------------------------------------------
// ss_thread.c
void ss_thread_init(void);
void ss_thread_QueueSend(uniEvent_t *pEvt);

//-----------------------------------------------------------------------------
// ma_constate.c

#define USE_OLD_PWU 1

void pinWakeUp_Init(void);
void pinWakeUp_Assert(void);
void pinWakeUp_Release(void);
void pinWakeUp_Deinit(void);
bool pinWakeUp_IsAsserted(void);
 

extern uint8_t mg_5_dengen;        // system power 1:On ,  0:Off
extern uint8_t mg_6_advX50ms;      // advertising period x 50ms (1~255) -> 50ms ~ 12.75sec
extern uint8_t mg_7_wkUpPerX50ms;  // period for Wake Signal x 50ms (1~255) -> 50ms ~ 12.75sec
extern uint8_t mg_8_wkUpDelayX1ms; // delay to SOH for Wake Signal x 1ms (1~255) -> 1ms ~ 255ms
extern uint8_t mg_9_ADC_rate;      // do Battery power measurement  0:Off, every 1~255 seconds
extern uint8_t mg_10_loadADC;      // do Battery Loaded measurement 0:Off, every 1~255 minutes
extern int8_t mg_11_power;        // set output power level (signed byte) e.g. -40, -30, -20, -16, -1, -8, -4, 0 , +4 dBm
extern uint8_t mg_12_9E00_rate;    // the rate to read (9E_00) and update Advertising Info (0-once only after reset, 0x01~0x3F:1~63minutes, 0x81~0xBF: 1~63seconds) 

extern uint32_t mg_6_advX50ms_inTicks;

//-----------------------------------------------------------------------------
void advertising_init_mg_new(uint32_t param_APP_ADV_INTERVAL); //mg_6_advX50ms_inTicks


extern uint8_t mg_ManufacturerSpecific_rsp26[26];
extern uint8_t mg_ShortenedName_rsp26[26 + 2]; // Added 2 bytes for two NULLS
void mg_ManufacturerSpecific_rsp26_setInitialValue(void);
void mg_ShortenedName_rsp26_set(char * device_name);


//DEFUNCT? extern bool m_doUpdateAdvertisingName;
//DEFUNCT? extern char m_UpdatedAdvertisingName[];
void gap_device_name_only_set(char * nextName);
//-----------------------------------------------------------------------------

 TODOT*/

#ifdef __cplusplus
}
#endif

#endif // __MYAPP_H

