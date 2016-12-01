#include "debug_etc.h"

#include "ble_tuds.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"

#define BLE_UUID_TUDS_DCMD_CHARACTERISTIC 0x0002                       // The UUID of the Command Characteristic.
#define BLE_UUID_TUDS_DDAT_CHARACTERISTIC 0x0003                       // The UUID of the Data    Characteristic.
#define BLE_UUID_TUDS_DCFM_CHARACTERISTIC 0x0004                       // The UUID of the Confirm Characteristic.

#define BLE_UUID_TUDS_UCMD_CHARACTERISTIC 0x0005                       // The UUID of the Command Characteristic.
#define BLE_UUID_TUDS_UDAT_CHARACTERISTIC 0x0006                       // The UUID of the Data    Characteristic.
#define BLE_UUID_TUDS_UCFM_CHARACTERISTIC 0x0007                       // The UUID of the Confirm Characteristic.

//#define BLE_UUID_TUDS_WCTRL_CHARACTERISTIC 0x0008
//#define BLE_UUID_TUDS_RCTRL_CHARACTERISTIC 0x0009

#define BLE_TUDS_MAX_DCMD_CHAR_LEN     BLE_TUDS_MAX_DATA_LEN         // Maximum length of the Command Characteristic (in bytes).
#define BLE_TUDS_MAX_DDAT_CHAR_LEN     BLE_TUDS_MAX_DATA_LEN         // Maximum length of the Data    Characteristic (in bytes).
#define BLE_TUDS_MAX_DCFM_CHAR_LEN     BLE_TUDS_MAX_DATA_LEN         // Maximum length of the Confirm Characteristic (in bytes).

#define BLE_TUDS_MAX_UCMD_CHAR_LEN     BLE_TUDS_MAX_DATA_LEN         // Maximum length of the Command Characteristic (in bytes).
#define BLE_TUDS_MAX_UDAT_CHAR_LEN     BLE_TUDS_MAX_DATA_LEN         // Maximum length of the Data    Characteristic (in bytes).
#define BLE_TUDS_MAX_UCFM_CHAR_LEN     BLE_TUDS_MAX_DATA_LEN         // Maximum length of the Confirm Characteristic (in bytes).

//#define BLE_TUDS_MAX_WCTRL_CHAR_LEN    BLE_TUDS_MAX_DATA_LEN         // Maximum length of the Confirm Characteristic (in bytes).
//#define BLE_TUDS_MAX_RCTRL_CHAR_LEN    BLE_TUDS_MAX_DATA_LEN         // Maximum length of the Confirm Characteristic (in bytes).


//#define NUS_BASE_UUID                  {{0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */
#define  TUDS_BASE_UUID                  {{0x42, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */

/**@brief Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_tuds     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_connect(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    p_tuds->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the @ref BLE_GAP_EVT_DISCONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_tuds     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_disconnect(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_tuds->conn_handle = BLE_CONN_HANDLE_INVALID;
}



/**@brief Function for handling the @ref BLE_GATTS_EVT_WRITE event from the S110 SoftDevice.
 *
 * @param[in] p_tuds     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
#include "stdio.h"

#if 0 // MOVED TO TDCTRLS
uint8_t WctrlRctrl_BUF[20];
uint8_t RctrlWctrl_BUF[20];
#endif  // MOVED TO TDCTRLS


static void on_write(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    //char buf[10];
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

/*
    if( p_tuds->data_handler != NULL )
    {
        sprintf( buf, "0x%04x", p_evt_write->handle);
        p_tuds->data_handler(p_tuds, (uint8_t*)buf, 6);

        sprintf( buf, "0x%04x", p_tuds->cmd_handles.value_handle);
        p_tuds->data_handler(p_tuds, (uint8_t*)buf, 6);
    }
*/

    //=========================================================================
    //----- DCMD -----
    if(  (p_evt_write->handle == p_tuds->Dcmd_handles.value_handle)  &&
         (p_tuds->Dcmd_handler != NULL)                                 )
    {
        //p_evt_write->data[1] = '-';
        //ref tuds_data_handler(...)
        p_tuds->Dcmd_handler (p_tuds, p_evt_write->data, p_evt_write->len);
        //p_tuds->data_handler(p_tuds, p_evt_write->data, p_evt_write->len);
    }
    else
    //----- DDAT -----
    if(  (p_evt_write->handle == p_tuds->Ddat_handles.value_handle)  &&
         (p_tuds->Ddat_handler != NULL)                               )
    {
        p_tuds->Ddat_handler(p_tuds, p_evt_write->data, p_evt_write->len);
    }
    else
    //----- DCFM (Notify On/Off) -----
    if(  (p_evt_write->handle == p_tuds->Dcfm_handles.cccd_handle)  &&
         (p_evt_write->len == 2)                                      )
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
            p_tuds->is_DCFM_notify_enabled = true;
        else
            p_tuds->is_DCFM_notify_enabled = false;
    }
    else
    //=========================================================================
    //----- UCMD (Notify On/Off) -----
    if(  (p_evt_write->handle == p_tuds->Ucmd_handles.cccd_handle)  &&
         (p_evt_write->len == 2)                                      )
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
            p_tuds->is_UCMD_notify_enabled = true;
        else
            p_tuds->is_UCMD_notify_enabled = false;
    }
    else
    //----- UDAT (Notify On/Off) -----
    if(  (p_evt_write->handle == p_tuds->Udat_handles.cccd_handle)  &&
         (p_evt_write->len == 2)                                      )
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
            p_tuds->is_UDAT_notify_enabled = true;
        else
            p_tuds->is_UDAT_notify_enabled = false;
    }
    else
    //----- UCFM -----
    if(  (p_evt_write->handle == p_tuds->Ucfm_handles.value_handle)  &&
         (p_tuds->Ucfm_handler != NULL)                               )
    {
        p_tuds->Ucfm_handler(p_tuds, p_evt_write->data, p_evt_write->len);
    }
    else
#if 0 // MOVED TO TDCTRLS
    //=========================================================================
    //----- WCTRL -----
    if(  p_evt_write->handle == p_tuds->Wctrl_handles.value_handle) /*&& (p_tuds->Wctrl_handler != NULL) )*/   
    {
        WctrlRctrl_BUF[0] = p_evt_write->data[0];// p_evt_write->len);
    }
    else
    //----- RCTRL -----
    if(  p_evt_write->handle == p_tuds->Rctrl_handles.value_handle) 
    {
        WctrlRctrl_BUF[0] = p_evt_write->data[0];// p_evt_write->len);
        //RctrlWctrl_BUF[0] = p_evt_write->data[0];// p_evt_write->len);
    }
    else
#endif  // MOVED TO TDCTRLS
    {
        // Do Nothing. This event is not relevant for this service.
    }
}


static void on_tx_complete(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    if( p_tuds->tx_complete_handler != NULL)
    {
        p_tuds->tx_complete_handler(p_tuds, p_ble_evt);
    }
    else
    {
        dbgPrintf("NULL");
    }
    
    /* TODO
    uint32_t  r;
    on_tx_complete should be a global handler, because, no info regarding the 
    handle etc is fed back here, so we need to keep tabs on who sent what.
    
    p_ble_evt->evt.common_evt.conn_handle;
    p_ble_evt->evt.common_evt.params.tx_complete.count;
    
    p_tuds->Boguslen16 = 6;
    r = ble_tuds_notify_Dcfm(p_tuds, (uint8_t*)"AACCKK", &p_tuds->Boguslen16 );
    r = ble_tuds_notify_Ucmd(p_tuds, (uint8_t*)"buf", &p_tuds->Boguslen16 );
    r = ble_tuds_notify_Udat(p_tuds, (uint8_t*)"buf", &p_tuds->Boguslen16 );
    */
}

//=============================================================================
//=============================================================================
//===== DN ====================================================================
//=============================================================================
//=============================================================================

/**@brief Function for adding CMD characteristic.
 *
 * @param[in] p_tuds       BLKDN Service structure.
 * @param[in] p_tuds_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Dcmd_char_add(ble_tuds_t * p_tuds, const ble_tuds_init_t * UNUSED_p_tuds_init)
{
    ble_gatts_char_md_t char_md;
  //ble_gatts_attr_md_t cccd_md;                            //****
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

  //memset(&cccd_md, 0, sizeof(cccd_md));                   //****

  //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);     //****
  //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);    //****

  //cccd_md.vloc = BLE_GATTS_VLOC_STACK;                    //****


    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
  //char_md.char_props.notify = 1;              //*****
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
  //char_md.p_cccd_md         = &cccd_md;       //****
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_DCMD_CHARACTERISTIC;  //*****

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;


    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);  //==1
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_TUDS_MAX_DCMD_CHAR_LEN;  //*****

    return sd_ble_gatts_characteristic_add(p_tuds->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_tuds->Dcmd_handles); //****
}

/**@brief Function for adding TX characteristic.
 *
 * @param[in] p_tuds       Nordic UART Service structure.
 * @param[in] p_tuds_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Ddat_char_add(ble_tuds_t * p_tuds, const ble_tuds_init_t * UNUSED_p_tuds_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.write         = 1;
    char_md.char_props.write_wo_resp = 1;
    char_md.p_char_user_desc         = NULL;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_DDAT_CHARACTERISTIC; //###

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 1;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_TUDS_MAX_DDAT_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_tuds->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_tuds->Ddat_handles); //###
}
/**@brief Function for adding RX characteristic.
 *
 * @param[in] p_tuds       BLKDN Service structure.
 * @param[in] p_tuds_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Dcfm_char_add(ble_tuds_t * p_tuds, const ble_tuds_init_t * UNUSED_p_tuds_init)
{
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_DCFM_CHARACTERISTIC; //###

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_TUDS_MAX_DCFM_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_tuds->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_tuds->Dcfm_handles);
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
}

//=============================================================================
//=============================================================================
//===== UP ====================================================================
//=============================================================================
//=============================================================================

/**@brief Function for adding CMD characteristic.
 *
 * @param[in] p_tuds       BLKDN Service structure.
 * @param[in] p_tuds_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Ucmd_char_add(ble_tuds_t * p_tuds, const ble_tuds_init_t * UNUSED_p_tuds_init)
{
#if 1 //TODO
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_UCMD_CHARACTERISTIC; //###

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_TUDS_MAX_UCMD_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_tuds->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_tuds->Ucmd_handles); //###
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
#else
    return(0xEEEEEEEE);
#endif
}

/**@brief Function for adding TX characteristic.
 *
 * @param[in] p_tuds       Nordic UART Service structure.
 * @param[in] p_tuds_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Udat_char_add(ble_tuds_t * p_tuds, const ble_tuds_init_t * UNUSED_p_tuds_init)
{
#if 1 //TODO
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_UDAT_CHARACTERISTIC; //###

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_TUDS_MAX_UDAT_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_tuds->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_tuds->Udat_handles); //###
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
#else
    return(0xEEEEEEEE);
#endif
}
/**@brief Function for adding RX characteristic.
 *
 * @param[in] p_tuds       BLKDN Service structure.
 * @param[in] p_tuds_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Ucfm_char_add(ble_tuds_t * p_tuds, const ble_tuds_init_t *UNUSED_p_tuds_init)
{
#if 1 //TODO
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.write         = 1;
    char_md.char_props.write_wo_resp = 1;
    char_md.p_char_user_desc         = NULL;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_UCFM_CHARACTERISTIC; //###

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 1;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_TUDS_MAX_UCFM_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_tuds->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_tuds->Ucfm_handles); //###

#else
    return(0xEEEEEEEE);
#endif
}



//=============================================================================
//=============================================================================
//===== CTRL ==================================================================
//=============================================================================
//=============================================================================
#if 0 // MOVED TO TDCTRLS
/**@brief Function for adding xxxx characteristic.
 *
 * @param[in] p_tuds       BLKDN Service structure.
 * @param[in] p_tuds_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Wctrl_char_add(ble_tuds_t * p_tuds, const ble_tuds_init_t * UNUSED_p_tuds_init)
{
    ble_gatts_char_md_t char_md;
  //ble_gatts_attr_md_t cccd_md;                            //****
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

  //memset(&cccd_md, 0, sizeof(cccd_md));                   //****

  //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);     //****
  //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);    //****

  //cccd_md.vloc = BLE_GATTS_VLOC_STACK;                    //****


    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
  //char_md.char_props.notify = 1;              //*****
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
  //char_md.p_cccd_md         = &cccd_md;       //****
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_WCTRL_CHARACTERISTIC;  //*****

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_USER; //BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;


    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 16;//sizeof(uint8_t);  //==1
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_TUDS_MAX_WCTRL_CHAR_LEN;  //*****

    attr_char_value.p_value   = WctrlRctrl_BUF;

    return sd_ble_gatts_characteristic_add(p_tuds->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_tuds->Wctrl_handles); //****
}
static uint32_t Rctrl_char_add(ble_tuds_t * p_tuds, const ble_tuds_init_t * UNUSED_p_tuds_init)
{
    ble_gatts_char_md_t char_md;
  //ble_gatts_attr_md_t cccd_md;                            //****
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

  //memset(&cccd_md, 0, sizeof(cccd_md));                   //****

  //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);     //****
  //BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);    //****

  //cccd_md.vloc = BLE_GATTS_VLOC_STACK;                    //****


    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
  //char_md.char_props.notify = 1;              //*****
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
  //char_md.p_cccd_md         = &cccd_md;       //****
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_RCTRL_CHARACTERISTIC;  //*****

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_USER; //BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 20;//sizeof(uint8_t);  //==1
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_TUDS_MAX_RCTRL_CHAR_LEN;  //*****
    attr_char_value.p_value   = WctrlRctrl_BUF;


    return sd_ble_gatts_characteristic_add(p_tuds->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_tuds->Rctrl_handles); //****
}

/**@brief Read/write authorize request event handler.
 *
 * @param[in]   p_tuds      UD Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_rw_authorize_request(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_rw_authorize_request_t * evt_rw_auth = &p_ble_evt->evt.gatts_evt.params.authorize_request;
    
    uint16_t conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
    ble_gatts_rw_authorize_reply_params_t rw_authorize_reply_params;

    
    if(evt_rw_auth->type == BLE_GATTS_AUTHORIZE_TYPE_INVALID) //    0x00  /**< Invalid Type. */
    {
    }

    if(evt_rw_auth->type == BLE_GATTS_AUTHORIZE_TYPE_READ)    //   0x01  /**< Authorize a Read Operation. */
    {
        WctrlRctrl_BUF[0] = 4;
        WctrlRctrl_BUF[1] = 5;
        WctrlRctrl_BUF[2] = 6;
        WctrlRctrl_BUF[3] = 7;
        rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
        rw_authorize_reply_params.params.read.gatt_status = BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;//BLE_GATT_STATUS_SUCCESS;
        rw_authorize_reply_params.params.read.len = 13;
        rw_authorize_reply_params.params.read.offset = 0;
        rw_authorize_reply_params.params.read.p_data = WctrlRctrl_BUF;
        rw_authorize_reply_params.params.read.update = 1;
        
        sd_ble_gatts_rw_authorize_reply(conn_handle, &rw_authorize_reply_params);

        if(evt_rw_auth->request.read.handle == 42)
        {
        }
        if(evt_rw_auth->request.read.context.value_handle == 42)
        {
        }
        if(evt_rw_auth->request.read.offset == 42)
        {
        }
    }
    if(evt_rw_auth->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)   //   0x02  /**< Authorize a Write Request Operation. */
    {
        rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
        rw_authorize_reply_params.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
        //rw_authorize_reply_params.params.write.len = 13;
        //rw_authorize_reply_params.params.write.offset = 0;
        //rw_authorize_reply_params.params.write.p_data = WctrlRctrl_BUF;
        //rw_authorize_reply_params.params.write.update = 1;
        
        sd_ble_gatts_rw_authorize_reply(conn_handle, &rw_authorize_reply_params);
    }

    /*
    ble_hids_char_id_t                     char_id;

    if (evt_rw_auth->type != BLE_GATTS_AUTHORIZE_TYPE_READ)
    {
        // Unexpected operation
        return;
    }

    if (rep_value_identify(p_hids, evt_rw_auth->request.read.handle, &char_id))
    {
        on_report_value_read_auth(p_hids, &char_id, p_ble_evt);
    }
    */
}
#endif // MOVED TO TDCTRLS


/*
It is important to note that a notification will <b>consume an application buffer</b>, and will therefore 
 *          generate a @ref BLE_EVT_TX_COMPLETE event when the packet has been transmitted. An indication on the other hand will use the 
 *          standard server internal buffer and thus will only generate a @ref BLE_GATTS_EVT_HVC event as soon as the confirmation 
 *          has been received from the peer. Please see the documentation of @ref sd_ble_tx_buffer_count_get for more details.
*/


void ble_tuds_on_ble_evt(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt)
{
    
    if ((p_tuds == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

  //dbgPrintf("\r\n evt = %d,%x: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id);
    dbgPrintf("\r\n ble_tuds_on_ble_evt");
    dbgPrintf("\r\n revt = %d, 0x%x, s = %s: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id, get_ble_evt_str(p_ble_evt->header.evt_id) );


    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_tuds, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_tuds, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_tuds, p_ble_evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
#if 0 // MOVED TO TDCTRLS
            on_rw_authorize_request(p_tuds, p_ble_evt);
#endif // MOVED TO TDCTRLS
            break;
        
        case BLE_EVT_TX_COMPLETE:
            on_tx_complete(p_tuds, p_ble_evt);
            break;
        
        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_tuds_init(ble_tuds_t * p_tuds, const ble_tuds_init_t * p_tuds_init)
{
    uint32_t      err_code;
    ble_uuid_t    ble_uuid;
    ble_uuid128_t tuds_base_uuid = TUDS_BASE_UUID;

    if ((p_tuds == NULL) || (p_tuds_init == NULL))
    {
        return NRF_ERROR_NULL;
    }

    // Initialize the service structure.
    p_tuds->conn_handle              = BLE_CONN_HANDLE_INVALID;
    //===== DN =====
    p_tuds->Ddat_handler             = p_tuds_init->Ddat_handler;
    p_tuds->Dcmd_handler             = p_tuds_init->Dcmd_handler;
    p_tuds->is_DCFM_notify_enabled   = false;
    p_tuds->tx_complete_handler = p_tuds_init->tx_complete_handler;
    
    //===== UP =====
    p_tuds->is_UCMD_notify_enabled   = false;
    p_tuds->is_UDAT_notify_enabled   = false;
    p_tuds->Ucfm_handler             = p_tuds_init->Ucfm_handler;

    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&tuds_base_uuid, &p_tuds->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    ble_uuid.type = p_tuds->uuid_type;
    ble_uuid.uuid = BLE_UUID_TUDS_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_tuds->service_handle);
    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    //===== DN =====
    // Add the CMD Characteristic.
    err_code = Dcmd_char_add(p_tuds, p_tuds_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the DATA Down Characteristic.
    err_code = Ddat_char_add(p_tuds, p_tuds_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the CFM Characteristic.
    err_code = Dcfm_char_add(p_tuds, p_tuds_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }

    //===== UP =====
    // Add the CMD Characteristic.
    err_code = Ucmd_char_add(p_tuds, p_tuds_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the DATA Down Characteristic.
    err_code = Udat_char_add(p_tuds, p_tuds_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the CFM Characteristic.
    err_code = Ucfm_char_add(p_tuds, p_tuds_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }

#if 0 //MOVED TO TDCTRL
    //===== CTRL =====
    // Add the Wctrl Characteristic.
    err_code = Wctrl_char_add(p_tuds, p_tuds_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the Rctrl Characteristic.
    err_code = Rctrl_char_add(p_tuds, p_tuds_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
#endif //MOVED TO TDCTRL

    return NRF_SUCCESS;
}

uint32_t ble_tuds_string_send(ble_tuds_t * p_tuds, uint8_t * p_string, uint16_t length)
{
    ble_gatts_hvx_params_t hvx_params;

    if (p_tuds == NULL)
    {
        return NRF_ERROR_NULL;
    }

    if ((p_tuds->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_tuds->is_DCFM_notify_enabled))
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (length > BLE_TUDS_MAX_DATA_LEN)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_tuds->Dcfm_handles.value_handle;
    hvx_params.p_data = p_string;
    hvx_params.p_len  = &length;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION; //->BLE_EVT_TX_COMPLETE

    return sd_ble_gatts_hvx(p_tuds->conn_handle, &hvx_params);
}

uint32_t ble_tuds_notify_Dcfm(ble_tuds_t * p_tuds, uint8_t * buf, uint16_t* p_len16)
{
    ble_gatts_hvx_params_t hvx_params;
    uint32_t  r = 0;

  
    if (p_tuds == NULL)
        return NRF_ERROR_NULL;

    if ((p_tuds->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_tuds->is_DCFM_notify_enabled))
        return NRF_ERROR_INVALID_STATE;
        
    if (*p_len16 > BLE_TUDS_MAX_DATA_LEN) { return NRF_ERROR_INVALID_PARAM; }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_tuds->Dcfm_handles.value_handle;
    hvx_params.p_data = buf;
    hvx_params.p_len  = p_len16;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION; //->BLE_EVT_TX_COMPLETE

    r = sd_ble_gatts_hvx(p_tuds->conn_handle, &hvx_params);
    return(r);
}

uint32_t ble_tuds_notify_Ucmd(ble_tuds_t * p_tuds, uint8_t * buf, uint16_t* p_len16)
{
    ble_gatts_hvx_params_t hvx_params;
    uint32_t  r = 0;

    if (p_tuds == NULL)
        return NRF_ERROR_NULL;

    if ((p_tuds->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_tuds->is_UCMD_notify_enabled))
        return NRF_ERROR_INVALID_STATE;
        
    if (*p_len16 > BLE_TUDS_MAX_DATA_LEN) { return NRF_ERROR_INVALID_PARAM; }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_tuds->Ucmd_handles.value_handle;
    hvx_params.p_data = buf;
    hvx_params.p_len  = p_len16;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION; //->BLE_EVT_TX_COMPLETE

    r = sd_ble_gatts_hvx(p_tuds->conn_handle, &hvx_params);
    return(r);
}

uint32_t ble_tuds_notify_Udat(ble_tuds_t * p_tuds, uint8_t * buf, uint16_t* p_len16)
{
    ble_gatts_hvx_params_t hvx_params;
    uint32_t  r = 0;

    if (p_tuds == NULL)
        return NRF_ERROR_NULL;

    if ((p_tuds->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_tuds->is_UDAT_notify_enabled))
        return NRF_ERROR_INVALID_STATE;
        
    if (*p_len16 > BLE_TUDS_MAX_DATA_LEN) { return NRF_ERROR_INVALID_PARAM; }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_tuds->Udat_handles.value_handle;
    hvx_params.p_data = buf;
    hvx_params.p_len  = p_len16;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION; //->BLE_EVT_TX_COMPLETE

    r = sd_ble_gatts_hvx(p_tuds->conn_handle, &hvx_params);
    return(r);
}



