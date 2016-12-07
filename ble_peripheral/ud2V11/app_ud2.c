

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

app_ud2_event_handler_t    m_event_handler;

extern ble_ud2_t  m_ble_ud2;                                  // Structure to identify the BLKUP Service.


//-----------------------------------------------------------------------------
// -- app_ud2_d.h --
int32_t app_ud2_Dcmd_handler(app_ud2_t *p_app_ud2, uint8_t *buf, uint8_t len);
int32_t app_ud2_Ddat_handler(app_ud2_t *p_app_ud2, uint8_t *buf, uint8_t len);

int32_t app_ud2_OnWrittenComplete_Dcfm_handler(app_ud2_t *p_app_ud2,  uint8_t *buf, uint8_t len);

extern uint32_t BlkDn_timer_init(void);


//-----------------------------------------------------------------------------
// -- app_ud2_u.h --
int32_t app_ud2_Ucfm_handler(app_ud2_t *p_app_ud2, uint8_t *buf, uint8_t len);

int32_t app_ud2_OnWrittenComplete_Ucmd_handler(app_ud2_t *p_app_ud2,  uint8_t *buf, uint8_t len);
int32_t app_ud2_OnWrittenComplete_Udat_handler(app_ud2_t *p_app_ud2,  uint8_t *buf, uint8_t len);

uint32_t BlkUp_timer_init(void);


//-----------------------------------------------------------------------------
// -- app_ud2.h --
void ud2_Dcmd_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length);
void ud2_Ddat_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length);
void ud2_Ucfm_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length);
void ud2_tx_complete_handler(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt);




static app_ud2_t  m_app_ud2;

PUBLIC void ud2_Dcmd_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length)
{
    //int32_t r;
    //r = 
    app_ud2_Dcmd_handler(&m_app_ud2, p_data, length);
}

PUBLIC void ud2_Ddat_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length)
{
    //int32_t r;
    //r = 
    app_ud2_Ddat_handler(&m_app_ud2, p_data, length);
}

PUBLIC void ud2_tx_complete_handler(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt)
{
    app_ud2_OnWrittenComplete_Dcfm_handler(&m_app_ud2, 0, 0); // BlkDn_On_written_Dcfm(p_ud2, 0, 0);

    app_ud2_OnWrittenComplete_Ucmd_handler(&m_app_ud2, 0, 0); // BlkUp_On_written_Ucmd(p_ud2, 0, 0);
    app_ud2_OnWrittenComplete_Udat_handler(&m_app_ud2, 0, 0); // BlkUp_On_written_Udat(p_ud2, 0, 0);
}


// UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP
static void ud2_Ucfm_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length)
{
    app_ud2_Ucfm_handler(&m_app_ud2, p_data, length); // BlkUp_On_Ucfm(p_data, length);
    //
    //BlkUp_On_Ucfm(p_data, length);
}








//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint32_t  app_ud2_init(app_ud2_t * p_app_ud2, const app_ud2_init_t * p_app_ud2_init);
uint32_t  app_ud2_init(app_ud2_t * p_app_ud2, const app_ud2_init_t * p_app_ud2_init)
{
    //p_app_ud2->event_handler = p_app_ud2_init->event_handler;
    p_app_ud2->packet_handler = p_app_ud2_init->packet_handler;
    p_app_ud2->p_ble_ud2 = p_app_ud2_init->p_ble_ud2;
    
    return(0);    
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void main_app_ud2_packet_handler( app_ud2_t * p_ma_ud2, uint8_t * p_data, uint16_t length)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void main_app_ud2_event_handler(app_ud2_evt_t * p_app_ud2_event)
{
    callThisWhenBlePacketIsRecieved(p_app_ud2_event);
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
//#include "myapp.h"
#include "app_ud2.h"

//
//

void app_ud2_Init(void);
void app_ud2_on_ble_evt(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt);

PUBLIC uint32_t timers_init_ud2_part(void)
{
    uint32_t rv;
    
    rv = BlkDn_timer_init();
    if( rv == 0 )
    {
        rv = BlkUp_timer_init();
    }
    return(rv);
}


PUBLIC void services_init_ud2_part(void)
{
#if USE_TUDS_U
#endif
    uint32_t                   err_code;

    //--------------------------------------------------------
    ble_ud2_init_t   _ble_ud2_init;    
    memset(&_ble_ud2_init, 0, sizeof(_ble_ud2_init));

    _ble_ud2_init.Ddat_data_handler = ud2_Ddat_data_handler;
    _ble_ud2_init.Dcmd_data_handler = ud2_Dcmd_data_handler;
    _ble_ud2_init.tx_complete_handler = ud2_tx_complete_handler;

    _ble_ud2_init.Ucfm_data_handler = ud2_Ucfm_data_handler;
    
    err_code = ble_ud2_init(&m_ble_ud2, &_ble_ud2_init);
    
    APP_ERROR_CHECK(err_code);


    //--------------------------------------------------------
    m_event_handler = main_app_ud2_event_handler; // NOTE42: m_event_handler is used.
    
    app_ud2_init_t   _app_ud2_init;    
    memset(&_app_ud2_init, 0, sizeof(_app_ud2_init));

    _app_ud2_init.packet_handler = main_app_ud2_packet_handler;
    _app_ud2_init.event_handler = main_app_ud2_event_handler;    // NOTE42: this event_handler is not used, m_event_handler is used.
    _app_ud2_init.p_ble_ud2 = &m_ble_ud2;

    err_code = app_ud2_init( &m_app_ud2, &_app_ud2_init);    
    APP_ERROR_CHECK(err_code);
    
}

PUBLIC void application_timers_start_ud2_part(void)
{
#if USE_TUDS_U
#endif
}



