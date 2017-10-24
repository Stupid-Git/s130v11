
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


// DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN
static void tuds_Dcmd_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    app_tuds_t *p_app_tuds = p_tuds->parentContext; 
    app_tuds_Dcmd_handler(p_app_tuds, p_data, length);
}

static void tuds_Ddat_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    app_tuds_t *p_app_tuds = p_tuds->parentContext; 
    app_tuds_Ddat_handler(p_app_tuds, p_data, length);
}

// UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP
static void tuds_Ucfm_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    app_tuds_t *p_app_tuds = p_tuds->parentContext; 
    app_tuds_Ucfm_handler(p_app_tuds, p_data, length);
}

// UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN
static void tuds_tx_complete_handler(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    app_tuds_t *p_app_tuds = p_tuds->parentContext; 
    app_tuds_Dcfm_handler_OnWrittenComplete(p_app_tuds, 0, 0);

    app_tuds_Ucmd_handler_OnWrittenComplete(p_app_tuds, 0, 0);
    app_tuds_Udat_handler_OnWrittenComplete(p_app_tuds, 0, 0);
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
PUBLIC uint32_t  app_tuds_init(app_tuds_t * p_app_tuds, const app_tuds_init_t * p_app_tuds_init)
{
//TBR    p_app_tuds->event_handler  = p_app_tuds_init->event_handler;
//TBR    p_app_tuds->packet_handler = p_app_tuds_init->packet_handler;
    p_app_tuds->p_ble_tuds = p_app_tuds_init->p_ble_tuds;
    p_app_tuds->blkUp_UpEventHandler = 0;

    p_app_tuds->blkDn_DnEventHandler = 0;
    
    return(0);    
}

PUBLIC uint32_t app_tuds_timer_init(void)
{
    uint32_t rv;
    
    rv = app_tuds_BlkDn_timer_init();
    if( rv == 0 )
    {
        rv = app_tuds_BlkUp_timer_init();
    }
    return(rv);
}


#include "ma_join.h"
//void join_Init(app_tuds_t *p_app_tuds); // BOGUS

PUBLIC void app_tuds_service_init(app_tuds_t *p_app_tuds, ble_tuds_t *p_ble_tuds)
{
    uint32_t                   err_code;

    p_app_tuds->m_BlkUp_sm = eBlkUp_IDLE;
    
    p_app_tuds->m_BlkDn_sm = eBlkDn_WAIT_CMD;
    p_app_tuds->m_BlkDn_packetWaitTimeCount= 0;
    
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
    app_tuds_init_t   _app_tuds_init;    
    memset(&_app_tuds_init, 0, sizeof(_app_tuds_init));

    _app_tuds_init.p_ble_tuds = p_ble_tuds;

    err_code = app_tuds_init(p_app_tuds, &_app_tuds_init);    
    APP_ERROR_CHECK(err_code);
    
    join_Init(p_app_tuds); // BOGUS
}

//-----------------------------------------------------------------------------
// Here we want to add stuff to catch events such as DISCONNECT
//-----------------------------------------------------------------------------
void app_tuds_on_ble_evt(app_tuds_t * p_app_tuds, ble_evt_t * p_ble_evt)
{
    if ((p_app_tuds == NULL) || (p_ble_evt == NULL))
        return;

  //dbgPrintf("\r\n evt = %d,%x: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id);
  //dbgPrintf("\r\n app_tuds_on_ble_evt");
  //dbgPrintf("\r\n revt = %d, 0x%x, s = %s: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id, get_ble_evt_str(p_ble_evt->header.evt_id) );

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            app_tuds_U_onBleConnect(p_app_tuds);
            app_tuds_D_onBleConnect(p_app_tuds);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            app_tuds_U_onBleDisconnect(p_app_tuds);
            app_tuds_D_onBleDisconnect(p_app_tuds);
            break;

        default:
            // No implementation needed.
            break;
    }
}


