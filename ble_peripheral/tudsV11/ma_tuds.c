
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#define DBGPRINTF_ALLOW 1
#include "dbg_etc.h"

#include "stdlib.h"
#include "nordic_common.h"
#include "app_timer.h"

#include "ble_tuds.h"
#include "ma_tuds.h"

#define PUBLIC 


// DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN
static void tuds_Dcmd_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    ma_tuds_t *p_ma_tuds = p_tuds->parentContext; 
    ma_tuds_Dcmd_handler(p_ma_tuds, p_data, length);
}

static void tuds_Ddat_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    ma_tuds_t *p_ma_tuds = p_tuds->parentContext; 
    ma_tuds_Ddat_handler(p_ma_tuds, p_data, length);
}

// UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP
static void tuds_Ucfm_data_handler(ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length)
{
    ma_tuds_t *p_ma_tuds = p_tuds->parentContext; 
    ma_tuds_Ucfm_handler(p_ma_tuds, p_data, length);
}

// UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN UP DN
static void tuds_tx_complete_handler(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    ma_tuds_t *p_ma_tuds = p_tuds->parentContext; 
    ma_tuds_Dcfm_handler_OnWrittenComplete(p_ma_tuds, 0, 0);

    ma_tuds_Ucmd_handler_OnWrittenComplete(p_ma_tuds, 0, 0);
    ma_tuds_Udat_handler_OnWrittenComplete(p_ma_tuds, 0, 0);
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
PUBLIC uint32_t  ma_tuds_init(ma_tuds_t * p_ma_tuds, const ma_tuds_init_t * p_ma_tuds_init)
{
//TBR    p_ma_tuds->event_handler  = p_ma_tuds_init->event_handler;
//TBR    p_ma_tuds->packet_handler = p_ma_tuds_init->packet_handler;
    p_ma_tuds->p_ble_tuds = p_ma_tuds_init->p_ble_tuds;
    p_ma_tuds->blkUp_UpEventHandler = 0;

    p_ma_tuds->blkDn_DnEventHandler = 0;    
    p_ma_tuds->m_BlkDn_stateMachineLocked = false;
    
    return(0);    
}

PUBLIC uint32_t ma_tuds_timer_init(void)
{
    uint32_t rv;
    
    rv = ma_tuds_BlkDn_timer_init();
    if( rv == 0 )
    {
        rv = ma_tuds_BlkUp_timer_init();
    }
    return(rv);
}


#include "ma_join.h"
//void join_Init(ma_tuds_t *p_ma_tuds); // BOGUS

PUBLIC void ma_tuds_service_init(ma_tuds_t *p_ma_tuds, ble_tuds_t *p_ble_tuds)
{
    uint32_t                   err_code;

    p_ma_tuds->m_BlkUp_sm = eBlkUp_IDLE;
    
    p_ma_tuds->m_BlkDn_sm = eBlkDn_WAIT_CMD;
    p_ma_tuds->m_BlkDn_packetWaitTimeCount= 0;
    
    //--------------------------------------------------------
    ble_tuds_init_t   tuds_init;    
    memset(&tuds_init, 0, sizeof(tuds_init));

    tuds_init.parentContext = p_ma_tuds;
    tuds_init.Ddat_handler = tuds_Ddat_data_handler;
    tuds_init.Dcmd_handler = tuds_Dcmd_data_handler;
    tuds_init.tx_complete_handler = tuds_tx_complete_handler;

    tuds_init.Ucfm_handler = tuds_Ucfm_data_handler;
    
    err_code = ble_tuds_init(p_ble_tuds, &tuds_init);

    APP_ERROR_CHECK(err_code);


    //--------------------------------------------------------
    ma_tuds_init_t   _ma_tuds_init;    
    memset(&_ma_tuds_init, 0, sizeof(_ma_tuds_init));

    _ma_tuds_init.p_ble_tuds = p_ble_tuds;

    err_code = ma_tuds_init(p_ma_tuds, &_ma_tuds_init);    
    APP_ERROR_CHECK(err_code);
    
    join_Init(p_ma_tuds); // BOGUS
}

//-----------------------------------------------------------------------------
// Here we want to add stuff to catch events such as DISCONNECT
//-----------------------------------------------------------------------------
void ma_tuds_on_ble_evt(ma_tuds_t * p_ma_tuds, ble_evt_t * p_ble_evt)
{
    if ((p_ma_tuds == NULL) || (p_ble_evt == NULL))
        return;

  //dbgPrintf("\r\n evt = %d,%x: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id);
  //dbgPrintf("\r\n ma_tuds_on_ble_evt");
  //dbgPrintf("\r\n revt = %d, 0x%x, s = %s: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id, get_ble_evt_str(p_ble_evt->header.evt_id) );

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            ma_tuds_U_onBleConnect(p_ma_tuds);
            ma_tuds_D_onBleConnect(p_ma_tuds);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            ma_tuds_U_onBleDisconnect(p_ma_tuds);
            ma_tuds_D_onBleDisconnect(p_ma_tuds);
            break;

        default:
            // No implementation needed.
            break;
    }
}


