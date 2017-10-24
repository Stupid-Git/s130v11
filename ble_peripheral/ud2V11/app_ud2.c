
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#include "debug_etc.h"

#include "stdlib.h"
#include "nordic_common.h"
#include "app_timer.h"

#include "ble_ud2.h"
#include "app_ud2.h"

#define PUBLIC 
extern app_ud2_t m_app_ud2; //FOR DEBUG/PORT


// DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN
static void ud2_Dcmd_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length)
{
    app_ud2_t *p_app_ud2 = p_ud2->parentContext; 
    app_ud2_Dcmd_handler(p_app_ud2, p_data, length);
}

static void ud2_Ddat_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length)
{
    app_ud2_t *p_app_ud2 = p_ud2->parentContext; 
    app_ud2_Ddat_handler(p_app_ud2, p_data, length);
}

// UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP
static void ud2_Ucfm_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length)
{
    app_ud2_t *p_app_ud2 = p_ud2->parentContext; 
    app_ud2_Ucfm_handler(p_app_ud2, p_data, length);
}

// UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN
static void ud2_tx_complete_handler(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt)
{
    app_ud2_t *p_app_ud2 = p_ud2->parentContext; 
    app_ud2_Dcfm_handler_OnWrittenComplete(p_app_ud2, 0, 0); // BlkDn_On_written_Dcfm(p_ud2, 0, 0);

    app_ud2_Ucmd_handler_OnWrittenComplete(p_app_ud2, 0, 0); // BlkUp_On_written_Ucmd(p_ud2, 0, 0);
    app_ud2_Udat_handler_OnWrittenComplete(p_app_ud2, 0, 0); // BlkUp_On_written_Udat(p_ud2, 0, 0);
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


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PUBLIC uint32_t  app_ud2_init(app_ud2_t * p_app_ud2, const app_ud2_init_t * p_app_ud2_init)
{
//TBR    p_app_ud2->event_handler  = p_app_ud2_init->event_handler;
//TBR    p_app_ud2->packet_handler = p_app_ud2_init->packet_handler;
    p_app_ud2->p_ble_ud2 = p_app_ud2_init->p_ble_ud2;
    p_app_ud2->blkUp_UpEventHandler = 0;

    p_app_ud2->blkDn_DnEventHandler = 0;
    
    return(0);    
}

PUBLIC uint32_t app_ud2_timer_init(void)
{
    uint32_t rv;
    
    rv = app_ud2_BlkDn_timer_init();
    if( rv == 0 )
    {
        rv = app_ud2_BlkUp_timer_init();
    }
    return(rv);
}


//#include "ma_join.h"
//void join_Init(app_ud2_t *p_app_ud2); // BOGUS

PUBLIC void app_ud2_service_init(app_ud2_t *p_app_ud2, ble_ud2_t *p_ble_ud2)
{
    uint32_t                   err_code;

    p_app_ud2->m_BlkUp_sm = eBlkUp_IDLE;
    
    p_app_ud2->m_BlkDn_sm = eBlkDn_WAIT_CMD;
    p_app_ud2->m_BlkDn_packetWaitTimeCount= 0;
    
    //--------------------------------------------------------
    ble_ud2_init_t   _ble_ud2_init;    
    memset(&_ble_ud2_init, 0, sizeof(_ble_ud2_init));

    _ble_ud2_init.parentContext = p_app_ud2;
    _ble_ud2_init.Ddat_data_handler = ud2_Ddat_data_handler;
    _ble_ud2_init.Dcmd_data_handler = ud2_Dcmd_data_handler;
    _ble_ud2_init.tx_complete_handler = ud2_tx_complete_handler;

    _ble_ud2_init.Ucfm_data_handler = ud2_Ucfm_data_handler;
    
    err_code = ble_ud2_init(p_ble_ud2, &_ble_ud2_init);

    APP_ERROR_CHECK(err_code);


    //--------------------------------------------------------
    app_ud2_init_t   _app_ud2_init;    
    memset(&_app_ud2_init, 0, sizeof(_app_ud2_init));

    _app_ud2_init.p_ble_ud2 = p_ble_ud2;

    err_code = app_ud2_init( p_app_ud2, &_app_ud2_init);    
    APP_ERROR_CHECK(err_code);
    
    //join_Init(p_app_ud2); // BOGUS
}

//-----------------------------------------------------------------------------
// Here we want to add stuff to catch events such as DISCONNECT
//-----------------------------------------------------------------------------
void app_ud2_on_ble_evt(app_ud2_t * p_app_ud2, ble_evt_t * p_ble_evt)
{
    if ((p_app_ud2 == NULL) || (p_ble_evt == NULL))
        return;

  //dbgPrintf("\r\n evt = %d,%x: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id);
  //dbgPrintf("\r\n app_ud2_on_ble_evt");
  //dbgPrintf("\r\n revt = %d, 0x%x, s = %s: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id, get_ble_evt_str(p_ble_evt->header.evt_id) );

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            app_ud2_U_onBleConnect(p_app_ud2);
            app_ud2_D_onBleConnect(p_app_ud2);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            app_ud2_U_onBleDisconnect(p_app_ud2);
            app_ud2_D_onBleDisconnect(p_app_ud2);
            break;

        default:
            // No implementation needed.
            break;
    }
}


