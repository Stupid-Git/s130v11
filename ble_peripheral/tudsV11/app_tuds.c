
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#include "debug_etc.h"

#include "stdlib.h"
#include "nordic_common.h"
#include "app_timer.h"

#include "ble_tuds.h"
#include "app_tuds.h"

#define PUBLIC 

#define APP_TIMER_PRESCALER 0
#define BLE_ERROR_NO_TX_BUFFERS BLE_ERROR_NO_TX_PACKETS

app_tuds_event_handler_t    m_event_handler;

//-----------------------------------------------------------------------------
// -- app_tuds_d.h --
int32_t app_tuds_Dcmd_handler(app_tuds_t *p_app_tuds, uint8_t *buf, uint8_t len);
int32_t app_tuds_Ddat_handler(app_tuds_t *p_app_tuds, uint8_t *buf, uint8_t len);

int32_t app_tuds_OnWrittenComplete_Dcfm_handler(app_tuds_t *p_app_tuds,  uint8_t *buf, uint8_t len);

uint32_t BlkDn_timer_init(void);


//-----------------------------------------------------------------------------
// -- app_tuds_u.h --
int32_t app_tuds_Ucfm_handler(app_tuds_t *p_app_tuds, uint8_t *buf, uint8_t len);

int32_t app_tuds_OnWrittenComplete_Ucmd_handler(app_tuds_t *p_app_tuds,  uint8_t *buf, uint8_t len);
int32_t app_tuds_OnWrittenComplete_Udat_handler(app_tuds_t *p_app_tuds,  uint8_t *buf, uint8_t len);

uint32_t BlkUp_timer_init(void);


//-----------------------------------------------------------------------------
// -- app_tuds.h --
void tuds_Dcmd_data_handler  (ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
void tuds_Ddat_data_handler  (ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
void tuds_Ucfm_data_handler  (ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
void tuds_tx_complete_handler(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt);


//extern app_tuds_t          m_app_tuds;

static void tuds_Dcmd_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    //int32_t r;
    //r = 
    app_tuds_t *p_app_tuds = p_tuds->parentContext; 
    app_tuds_Dcmd_handler(p_app_tuds, p_data, length); //BlkDn_On_Dcmd(p_data, length);
}

static void tuds_Ddat_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    //int32_t r;
    //r = 
    app_tuds_t *p_app_tuds = p_tuds->parentContext; 
    app_tuds_Ddat_handler(p_app_tuds, p_data, length); //BlkDn_On_Ddat(p_data, length);
}

static void tuds_tx_complete_handler(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    app_tuds_t *p_app_tuds = p_tuds->parentContext; 
    app_tuds_OnWrittenComplete_Dcfm_handler(p_app_tuds, 0, 0); // BlkDn_On_written_Dcfm(p_tuds, 0, 0);

    app_tuds_OnWrittenComplete_Ucmd_handler(p_app_tuds, 0, 0); // BlkUp_On_written_Ucmd(p_tuds, 0, 0);
    app_tuds_OnWrittenComplete_Udat_handler(p_app_tuds, 0, 0); // BlkUp_On_written_Udat(p_tuds, 0, 0);
}


// UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP
static void tuds_Ucfm_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    app_tuds_t *p_app_tuds = p_tuds->parentContext; 
    app_tuds_Ucfm_handler(p_app_tuds, p_data, length); // BlkUp_On_Ucfm(p_data, length);
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

PUBLIC uint32_t timers_init_tuds_part(void)
{
    uint32_t rv;
    
    rv = BlkDn_timer_init();
    if( rv == 0 )
    {
        rv = BlkUp_timer_init();
    }
    return(rv);
}


PUBLIC void services_init_tuds_part(app_tuds_t *p_app_tuds, ble_tuds_t *p_ble_tuds)
{
    uint32_t                   err_code;

    //--------------------------------------------------------
    ble_tuds_init_t   tuds_init;    
    memset(&tuds_init, 0, sizeof(tuds_init));

    tuds_init.parentContext = p_app_tuds;
    tuds_init.Ddat_handler = tuds_Ddat_data_handler;
    tuds_init.Dcmd_handler = tuds_Dcmd_data_handler;
    tuds_init.tx_complete_handler = tuds_tx_complete_handler;

    tuds_init.Ucfm_handler = tuds_Ucfm_data_handler;
    
    err_code = ble_tuds_init(p_ble_tuds, &tuds_init);

    APP_ERROR_CHECK(err_code);


    //--------------------------------------------------------
    m_event_handler = main_app_tuds_event_handler; // NOTE42: m_event_handler is used.
    
    app_tuds_init_t   _tuds_init;    
    memset(&_tuds_init, 0, sizeof(_tuds_init));

    _tuds_init.packet_handler = main_app_tuds_packet_handler;
    _tuds_init.event_handler = main_app_tuds_event_handler;    // NOTE42: this event_handler is not used, m_event_handler is used.
    _tuds_init.p_ble_tuds = p_ble_tuds;

    err_code = app_tuds_init(p_app_tuds, &_tuds_init);    
    APP_ERROR_CHECK(err_code);
    
}

//void application_timers_start_tuds_part(void)
//{
//}

