
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#include "debug_etc.h"

#include <stdlib.h>
#include "nordic_common.h"
#include "app_timer.h"

#include "ble_tuds.h"
#include "app_tuds.h"


#define APP_TIMER_PRESCALER 0
#define BLE_ERROR_NO_TX_BUFFERS BLE_ERROR_NO_TX_PACKETS

app_tuds_event_handler_t    m_event_handler;

extern ble_tuds_t  m_ble_tuds;                                  // Structure to identify the BLKUP Service.



// -- app_tuds_d.h --
int32_t app_tuds_Dcmd_handler(app_tuds_t *p_app_tuds, uint8_t *buf, uint8_t len);
int32_t app_tuds_Ddat_handler(app_tuds_t *p_app_tuds, uint8_t *buf, uint8_t len);

int32_t app_tuds_OnWrittenComplete_Dcfm_handler(app_tuds_t *p_app_tuds,  uint8_t *buf, uint8_t len);

uint32_t BlkDn_timer_init(void);


// -- app_tuds_u.h --
int32_t app_tuds_Ucfm_handler(app_tuds_t *p_app_tuds, uint8_t *buf, uint8_t len);

int32_t app_tuds_OnWrittenComplete_Ucmd_handler(app_tuds_t *p_app_tuds,  uint8_t *buf, uint8_t len);
int32_t app_tuds_OnWrittenComplete_Udat_handler(app_tuds_t *p_app_tuds,  uint8_t *buf, uint8_t len);

uint32_t BlkUp_timer_init(void);


// -- app_tuds.h --
void tuds_Dcmd_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
void tuds_Ddat_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
void tuds_Ucfm_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
void tuds_tx_complete_handler(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt);




app_tuds_t          m_app_tuds;

static void tuds_Dcmd_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    //int32_t r;
    //r = 
    app_tuds_Dcmd_handler(&m_app_tuds, p_data, length); //BlkDn_On_Dcmd(p_data, length);
    //
    //BlkDn_On_Dcmd(p_data, length);
}

static void tuds_Ddat_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    //int32_t r;
    //r = 
    app_tuds_Ddat_handler(&m_app_tuds, p_data, length); //BlkDn_On_Ddat(p_data, length);
    //
    //BlkDn_On_Ddat(p_data, length);
}

static void tuds_tx_complete_handler(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    app_tuds_OnWrittenComplete_Dcfm_handler(&m_app_tuds, 0, 0); // BlkDn_On_written_Dcfm(p_tuds, 0, 0);

    app_tuds_OnWrittenComplete_Ucmd_handler(&m_app_tuds, 0, 0); // BlkUp_On_written_Ucmd(p_tuds, 0, 0);
    app_tuds_OnWrittenComplete_Udat_handler(&m_app_tuds, 0, 0); // BlkUp_On_written_Udat(p_tuds, 0, 0);
}


// UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP
static void tuds_Ucfm_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    app_tuds_Ucfm_handler(&m_app_tuds, p_data, length); // BlkUp_On_Ucfm(p_data, length);
    //
    //BlkUp_On_Ucfm(p_data, length);
}








//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint32_t  app_tuds_init(app_tuds_t * p_app_tuds, const app_tuds_init_t * p_app_tuds_init);
uint32_t  app_tuds_init(app_tuds_t * p_app_tuds, const app_tuds_init_t * p_app_tuds_init)
{
    //p_app_tuds->event_handler = p_app_tuds_init->event_handler;
    p_app_tuds->packet_handler = p_app_tuds_init->packet_handler;
    p_app_tuds->p_ble_tuds = p_app_tuds_init->p_ble_tuds;
    
    return(0);    
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void main_app_tuds_packet_handler( app_tuds_t * p_ma_tuds, uint8_t * p_data, uint16_t length)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void main_app_tuds_event_handler(app_tuds_evt_t * p_app_tuds_event)
{
    callThisWhenBlePacketIsRecieved(p_app_tuds_event);
}


//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//
//     PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC PUBLIC
//
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
#include "myapp.h"
#include "app_tuds.h"



void app_tuds_Init(void);
void app_tuds_on_ble_evt(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt);

uint32_t timers_init_tuds_part(void)
{
    uint32_t rv;
    
    rv = BlkDn_timer_init();
    if( rv == 0 )
    {
        rv = BlkUp_timer_init();
    }
    return(rv);
}


void services_init_tuds_part(void)
{

    uint32_t                   err_code;

    //--------------------------------------------------------
    ble_tuds_init_t   tuds_init;    
    memset(&tuds_init, 0, sizeof(tuds_init));

    tuds_init.Ddat_handler = tuds_Ddat_data_handler;
    tuds_init.Dcmd_handler = tuds_Dcmd_data_handler;
    tuds_init.tx_complete_handler = tuds_tx_complete_handler;

    tuds_init.Ucfm_handler = tuds_Ucfm_data_handler;
    
    err_code = ble_tuds_init(&m_ble_tuds, &tuds_init);

    //rn_BlkDn_timer_Init();//APP_TIMER_TICKS(100, APP_TIMER_PRESCALER));
    //rn_BlkUp_timer_Init();//APP_TIMER_TICKS(100, APP_TIMER_PRESCALER));
    APP_ERROR_CHECK(err_code);


    //--------------------------------------------------------
    m_event_handler = main_app_tuds_event_handler; // NOTE42: m_event_handler is used.
    
    app_tuds_init_t   _tuds_init;    
    memset(&_tuds_init, 0, sizeof(_tuds_init));

    _tuds_init.packet_handler = main_app_tuds_packet_handler;
    _tuds_init.event_handler = main_app_tuds_event_handler;    // NOTE42: this event_handler is not used, m_event_handler is used.
    _tuds_init.p_ble_tuds = &m_ble_tuds;

    err_code = app_tuds_init( &m_app_tuds, &_tuds_init);    
    APP_ERROR_CHECK(err_code);
    
}
/*
void tuds_service_init()
{
    uint32_t                   err_code;

    ble_tuds_init_t   tuds_init;    
    memset(&tuds_init, 0, sizeof(tuds_init));

    tuds_init.Ddat_handler = tuds_Ddat_data_handler;
    tuds_init.Dcmd_handler = tuds_Dcmd_data_handler;
    tuds_init.tx_complete_handler = tuds_tx_complete_handler;

    tuds_init.Ucfm_handler = tuds_Ucfm_data_handler;
    
    err_code = ble_tuds_init(&m_ble_tuds, &tuds_init);

    //rn_BlkDn_timer_Init();//APP_TIMER_TICKS(100, APP_TIMER_PRESCALER));
    //rn_BlkUp_timer_Init();//APP_TIMER_TICKS(100, APP_TIMER_PRESCALER));
    
    //APP_ERROR_CHECK(err_code);
}
*/
void application_timers_start_tuds_part(void)
{
}




/*
void ma_tuds_init(void)
{
    uint32_t                   err_code;

    app_tuds_init_t   ma_tuds_init;    
    memset(&ma_tuds_init, 0, sizeof(ma_tuds_init));

    tuds_service_init();
    
    //ma_tuds_init.p_tuds = &m_ble_tuds;
    ma_tuds_init.packet_handler = 0;
    
    err_code = app_tuds_init(&m_app_tuds, &ma_tuds_init);

    //APP_ERROR_CHECK(err_code);
}

void ma_tuds_on_ble_evt(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    ble_tuds_on_ble_evt(p_tuds, p_ble_evt); // ble_tuds_on_ble_evt(&m_ble_tuds, p_ble_evt);
}

void ma_tuds_set_packetHandler( app_tuds_t *p_app_tuds, app_tuds_packet_handler_t app_tuds_ph )
{
    p_app_tuds->packet_handler = app_tuds_ph;
}
    


void appcode_tuds_packet_handler (app_tuds_t * p_ma_tuds, uint8_t * p_data, uint16_t length)
{
}

void appcode1()
{
    ma_tuds_set_packetHandler(&m_app_tuds, appcode_tuds_packet_handler );
}



void tuds_event_handle(app_tuds_evt_t * p_event)
{
    static uniEvent_t LEvt;

    switch (p_event->evt_type)
    {
        case APP_TUDS_RX_START_PKT_0:
           // app_tuds_send_noBufferBusy(m_app_tuds);
            break;
        
        case APP_TUDS_RX_DONE_PKT_0:
            // TODO PKT_0 goes to UART
        //TODO    LEvt.evtType = evt_GHJGJHGHGH_trigger;
            //LEvt.evtType = evt_Uart_RxReady;
            //uart_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
        
        
            break;

        case APP_TUDS_TX_DONE:
            //LEvt.evtType = evt_Uart_TxEmpty;
            //uart_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
            break;
        
        default:
            break;
    }    
}


void ma_app_tuds_Init(void)
{
    m_app_tuds.packet_handler = tuds_event_handle;
    m_app_tuds.p_ble_tuds = &m_ble_tuds;
    
}

void ma_app_tuds_Deinit() //karel
{
    //dbgPrint("***** ma_app_tuds_Deinit *****\r\n");

	//app_tuds_close();
}


app_tuds_event_handler_t    m_event_handler;

uint32_t app_tuds_init__uarttype(//const app_uart_comm_params_t * p_comm_params,
                       //      app_uart_buffers_t *     p_buffers,
                             app_tuds_event_handler_t event_handler
                       //      app_irq_priority_t       irq_priority
                      )
{
    uint32_t err_code;

    m_event_handler = event_handler;

    return 0;
}
*/

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


