
#include "debug_etc.h"

#include "ble_ud2.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"

#define BLE_UUID_UD2_DCMD_CHARACTERISTIC 0x0002                       // The UUID of the Command Characteristic.
#define BLE_UUID_UD2_DDAT_CHARACTERISTIC 0x0003                       // The UUID of the Data    Characteristic.
#define BLE_UUID_UD2_DCFM_CHARACTERISTIC 0x0004                       // The UUID of the Confirm Characteristic.

#define BLE_UUID_UD2_UCMD_CHARACTERISTIC 0x0005                       // The UUID of the Command Characteristic.
#define BLE_UUID_UD2_UDAT_CHARACTERISTIC 0x0006                       // The UUID of the Data    Characteristic.
#define BLE_UUID_UD2_UCFM_CHARACTERISTIC 0x0007                       // The UUID of the Confirm Characteristic.

//#define BLE_UUID_UD2_WCTRL_CHARACTERISTIC 0x0008
//#define BLE_UUID_UD2_RCTRL_CHARACTERISTIC 0x0009

#define BLE_UD2_MAX_DCMD_CHAR_LEN     BLE_UD2_MAX_DATA_LEN         // Maximum length of the Command Characteristic (in bytes).
#define BLE_UD2_MAX_DDAT_CHAR_LEN     BLE_UD2_MAX_DATA_LEN         // Maximum length of the Data    Characteristic (in bytes).
#define BLE_UD2_MAX_DCFM_CHAR_LEN     BLE_UD2_MAX_DATA_LEN         // Maximum length of the Confirm Characteristic (in bytes).

#define BLE_UD2_MAX_UCMD_CHAR_LEN     BLE_UD2_MAX_DATA_LEN         // Maximum length of the Command Characteristic (in bytes).
#define BLE_UD2_MAX_UDAT_CHAR_LEN     BLE_UD2_MAX_DATA_LEN         // Maximum length of the Data    Characteristic (in bytes).
#define BLE_UD2_MAX_UCFM_CHAR_LEN     BLE_UD2_MAX_DATA_LEN         // Maximum length of the Confirm Characteristic (in bytes).

//#define BLE_UD2_MAX_WCTRL_CHAR_LEN    BLE_UD2_MAX_DATA_LEN         // Maximum length of the Confirm Characteristic (in bytes).
//#define BLE_UD2_MAX_RCTRL_CHAR_LEN    BLE_UD2_MAX_DATA_LEN         // Maximum length of the Confirm Characteristic (in bytes).


//#define NUS_BASE_UUID                 {{0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */
//#define TUDS_BASE_UUID                {{0x42, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */
#define    UD2_BASE_UUID                {{0x02, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */

/**@brief Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_ud2     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_connect(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt)
{
    p_ud2->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the @ref BLE_GAP_EVT_DISCONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_ud2     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_disconnect(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_ud2->conn_handle = BLE_CONN_HANDLE_INVALID;
}



/**@brief Function for handling the @ref BLE_GATTS_EVT_WRITE event from the S110 SoftDevice.
 *
 * @param[in] p_ud2     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
#include "stdio.h"



static void on_write(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    //=========================================================================
    //----- DCMD -----
    if(  (p_evt_write->handle == p_ud2->Dcmd_handles.value_handle)  &&
         (p_ud2->Dcmd_data_handler != NULL)                                 )
    {
        p_ud2->Dcmd_data_handler (p_ud2, p_evt_write->data, p_evt_write->len);
    }
    else
    //----- DDAT -----
    if(  (p_evt_write->handle == p_ud2->Ddat_handles.value_handle)  &&
         (p_ud2->Ddat_data_handler != NULL)                               )
    {
        p_ud2->Ddat_data_handler(p_ud2, p_evt_write->data, p_evt_write->len);
    }
    else
    //----- DCFM (Notify On/Off) -----
    if(  (p_evt_write->handle == p_ud2->Dcfm_handles.cccd_handle)  &&
         (p_evt_write->len == 2)                                      )
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
            p_ud2->is_DCFM_notify_enabled = true;
        else
            p_ud2->is_DCFM_notify_enabled = false;
    }
    else

    //=========================================================================
    //----- UCMD (Notify On/Off) -----
    if(  (p_evt_write->handle == p_ud2->Ucmd_handles.cccd_handle)  &&
         (p_evt_write->len == 2)                                      )
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
            p_ud2->is_UCMD_notify_enabled = true;
        else
            p_ud2->is_UCMD_notify_enabled = false;
    }
    else
    //----- UDAT (Notify On/Off) -----
    if(  (p_evt_write->handle == p_ud2->Udat_handles.cccd_handle)  &&
         (p_evt_write->len == 2)                                      )
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
            p_ud2->is_UDAT_notify_enabled = true;
        else
            p_ud2->is_UDAT_notify_enabled = false;
    }
    else
    //----- UCFM -----
    if(  (p_evt_write->handle == p_ud2->Ucfm_handles.value_handle)  &&
         (p_ud2->Ucfm_data_handler != NULL)                               )
    {
        p_ud2->Ucfm_data_handler(p_ud2, p_evt_write->data, p_evt_write->len);
    }
    else
    {
        // Do Nothing. This event is not relevant for this service.
    }
}




static void on_tx_complete(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt)
{
    if( p_ud2->tx_complete_handler != NULL)
    {
        p_ud2->tx_complete_handler(p_ud2, p_ble_evt);
    }
    else
    {
        dbgPrintf("NULL");
    }
    
    //uint32_t  r;
    //on_tx_complete should be a global handler, because, no info regarding the 
    //handle etc is fed back here, so we need to keep tabs on who sent what.
    
    //p_ble_evt->evt.common_evt.conn_handle;
    //p_ble_evt->evt.common_evt.params.tx_complete.count;
    
    //p_ud2->Boguslen16 = 6;
    //r = ble_ud2_notify_Dcfm(p_ud2, (uint8_t*)"AACCKK", &p_ud2->Boguslen16 );
    //r = ble_ud2_notify_Ucmd(p_ud2, (uint8_t*)"buf", &p_ud2->Boguslen16 );
    //r = ble_ud2_notify_Udat(p_ud2, (uint8_t*)"buf", &p_ud2->Boguslen16 );

}

//=============================================================================
//=============================================================================
//===== DN ====================================================================
//=============================================================================
//=============================================================================

/**@brief Function for adding CMD characteristic.
 *
 * @param[in] p_ud2       BLKDN Service structure.
 * @param[in] p_ud2_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Dcmd_char_add(ble_ud2_t * p_ud2, const ble_ud2_init_t * UNUSED_p_ud2_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;


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

    ble_uuid.type = p_ud2->uuid_type;
    ble_uuid.uuid = BLE_UUID_UD2_DCMD_CHARACTERISTIC;  //*****

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
    attr_char_value.max_len   = BLE_UD2_MAX_DCMD_CHAR_LEN;  //*****

    return sd_ble_gatts_characteristic_add(p_ud2->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_ud2->Dcmd_handles); //****
}

/**@brief Function for adding TX characteristic.
 *
 * @param[in] p_ud2       Nordic UART Service structure.
 * @param[in] p_ud2_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Ddat_char_add(ble_ud2_t * p_ud2, const ble_ud2_init_t * UNUSED_p_ud2_init)
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

    ble_uuid.type = p_ud2->uuid_type;
    ble_uuid.uuid = BLE_UUID_UD2_DDAT_CHARACTERISTIC; //###

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
    attr_char_value.max_len   = BLE_UD2_MAX_DDAT_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_ud2->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_ud2->Ddat_handles); //###
}
/**@brief Function for adding RX characteristic.
 *
 * @param[in] p_ud2       BLKDN Service structure.
 * @param[in] p_ud2_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Dcfm_char_add(ble_ud2_t * p_ud2, const ble_ud2_init_t * UNUSED_p_ud2_init)
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

    ble_uuid.type = p_ud2->uuid_type;
    ble_uuid.uuid = BLE_UUID_UD2_DCFM_CHARACTERISTIC; //###

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
    attr_char_value.max_len   = BLE_UD2_MAX_DCFM_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_ud2->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_ud2->Dcfm_handles);
    /**@snippet [Adding proprietary characteristic to S110 SoftDevice] */
}


//=============================================================================
//=============================================================================
//===== UP ====================================================================
//=============================================================================
//=============================================================================

/**@brief Function for adding CMD characteristic.
 *
 * @param[in] p_ud2       BLKDN Service structure.
 * @param[in] p_ud2_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Ucmd_char_add(ble_ud2_t * p_ud2, const ble_ud2_init_t * UNUSED_p_ud2_init)
{
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

    ble_uuid.type = p_ud2->uuid_type;
    ble_uuid.uuid = BLE_UUID_UD2_UCMD_CHARACTERISTIC; //###

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
    attr_char_value.max_len   = BLE_UD2_MAX_UCMD_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_ud2->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_ud2->Ucmd_handles); //###
}

/**@brief Function for adding TX characteristic.
 *
 * @param[in] p_ud2       Nordic UART Service structure.
 * @param[in] p_ud2_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Udat_char_add(ble_ud2_t * p_ud2, const ble_ud2_init_t * UNUSED_p_ud2_init)
{
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

    ble_uuid.type = p_ud2->uuid_type;
    ble_uuid.uuid = BLE_UUID_UD2_UDAT_CHARACTERISTIC; //###

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
    attr_char_value.max_len   = BLE_UD2_MAX_UDAT_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_ud2->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_ud2->Udat_handles); //###
}

/**@brief Function for adding RX characteristic.
 *
 * @param[in] p_ud2       BLKDN Service structure.
 * @param[in] p_ud2_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t Ucfm_char_add(ble_ud2_t * p_ud2, const ble_ud2_init_t *UNUSED_p_ud2_init)
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

    ble_uuid.type = p_ud2->uuid_type;
    ble_uuid.uuid = BLE_UUID_UD2_UCFM_CHARACTERISTIC; //###

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
    attr_char_value.max_len   = BLE_UD2_MAX_UCFM_CHAR_LEN; //###

    return sd_ble_gatts_characteristic_add(p_ud2->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_ud2->Ucfm_handles); //###
}



/*  It is important to note that a notification will <b>consume an application buffer</b>, and will therefore 
 *  generate a @ref BLE_EVT_TX_COMPLETE event when the packet has been transmitted. An indication on the other hand will use the 
 *  standard server internal buffer and thus will only generate a @ref BLE_GATTS_EVT_HVC event as soon as the confirmation 
 *  has been received from the peer. Please see the documentation of @ref sd_ble_tx_buffer_count_get for more details.
*/

// https://devzone.nordicsemi.com/question/52032/s132-ble_evt_tx_complete-not-being-triggered/

int free_tx_buffers;

void sysinitsortathing()
{
    /*
    * @retval ::NRF_SUCCESS Number of application transmission packets retrieved successfully.
    * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
    * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
    */
    //SVCALL(SD_BLE_TX_PACKET_COUNT_GET, uint32_t, sd_ble_tx_packet_count_get(uint16_t conn_handle, uint8_t *p_count));
    
    //free_tx_buffers = 42;//sd_ble_tx_buffer_count_get();
    
    uint32_t err_code;
  
    uint8_t   count;
    uint16_t  conn_handle = 42;
    err_code = sd_ble_tx_packet_count_get(conn_handle, &count);
    if( err_code != NRF_SUCCESS)
    {
    }
}

void kjhjkhkjhkjh()
{
    //uint32_t err_code;
    /*
    err_code = sd_ble_gatts_hvx(p_sts->conn_handle, &hvx_params);
    if(err_code == NRF_SUCCESS)
    {
        (*p_sts->free_tx_buffers_p)--;        
        if(hvx_length != pkt_length)
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }
    }
    */
}

void ble_ud2_on_ble_evt(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt)
{
    
    if ((p_ud2 == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

  //dbgPrintf("\n\revt = %d,%x: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id);
    dbgPrintf("ble_ud2_on_ble_evt\r\n");
    dbgPrintf("\n\revt = %d, 0x%x, s = %s: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id, get_ble_evt_str(p_ble_evt->header.evt_id) );


    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_ud2, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_ud2, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_ud2, p_ble_evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
#if 0 // MOVED TO TDCTRLS
            on_rw_authorize_request(p_ud2, p_ble_evt);
#endif // MOVED TO TDCTRLS
            break;
        
        case BLE_EVT_TX_COMPLETE:
            free_tx_buffers += p_ble_evt->evt.common_evt.params.tx_complete.count;
            on_tx_complete(p_ud2, p_ble_evt);
            break;
        
        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_ud2_init(ble_ud2_t * p_ud2, const ble_ud2_init_t * p_ud2_init)
{
    uint32_t      err_code;
    ble_uuid_t    ble_uuid;
    ble_uuid128_t ud2_base_uuid = UD2_BASE_UUID;

    if ((p_ud2 == NULL) || (p_ud2_init == NULL))
    {
        return NRF_ERROR_NULL;
    }

    // Initialize the service structure.
    p_ud2->conn_handle              = BLE_CONN_HANDLE_INVALID;
    //===== DN =====
    p_ud2->Ddat_data_handler             = p_ud2_init->Ddat_data_handler;
    p_ud2->Dcmd_data_handler             = p_ud2_init->Dcmd_data_handler;
    p_ud2->is_DCFM_notify_enabled   = false;
    p_ud2->tx_complete_handler = p_ud2_init->tx_complete_handler;
    
    //===== UP =====
    p_ud2->is_UCMD_notify_enabled   = false;
    p_ud2->is_UDAT_notify_enabled   = false;
    p_ud2->Ucfm_data_handler             = p_ud2_init->Ucfm_data_handler;

    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&ud2_base_uuid, &p_ud2->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    ble_uuid.type = p_ud2->uuid_type;
    ble_uuid.uuid = BLE_UUID_UD2_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_ud2->service_handle);
    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    //===== DN =====
    // Add the CMD Characteristic.
    err_code = Dcmd_char_add(p_ud2, p_ud2_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the DATA Down Characteristic.
    err_code = Ddat_char_add(p_ud2, p_ud2_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the CFM Characteristic.
    err_code = Dcfm_char_add(p_ud2, p_ud2_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }

    //===== UP =====
    // Add the CMD Characteristic.
    err_code = Ucmd_char_add(p_ud2, p_ud2_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the DATA Down Characteristic.
    err_code = Udat_char_add(p_ud2, p_ud2_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the CFM Characteristic.
    err_code = Ucfm_char_add(p_ud2, p_ud2_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }

#if 0 //MOVED TO TDCTRL
    //===== CTRL =====
    // Add the Wctrl Characteristic.
    err_code = Wctrl_char_add(p_ud2, p_ud2_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
    // Add the Rctrl Characteristic.
    err_code = Rctrl_char_add(p_ud2, p_ud2_init);
    if (err_code != NRF_SUCCESS) { return err_code;  }
#endif //MOVED TO TDCTRL

    return NRF_SUCCESS;
}

uint32_t ble_ud2_string_send(ble_ud2_t * p_ud2, uint8_t * p_string, uint16_t length)
{
    ble_gatts_hvx_params_t hvx_params;

    if (p_ud2 == NULL)
    {
        return NRF_ERROR_NULL;
    }

    if ((p_ud2->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_ud2->is_DCFM_notify_enabled))
    {
        return NRF_ERROR_INVALID_STATE;
    }

    if (length > BLE_UD2_MAX_DATA_LEN)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_ud2->Dcfm_handles.value_handle;
    hvx_params.p_data = p_string;
    hvx_params.p_len  = &length;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION; //->BLE_EVT_TX_COMPLETE

    return sd_ble_gatts_hvx(p_ud2->conn_handle, &hvx_params);
}

uint32_t ble_ud2_notify_Dcfm(ble_ud2_t * p_ud2, uint8_t * buf, uint16_t* p_len16)
{
    ble_gatts_hvx_params_t hvx_params;
    uint32_t  r = 0;

  
    if (p_ud2 == NULL)
        return NRF_ERROR_NULL;

    if ((p_ud2->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_ud2->is_DCFM_notify_enabled))
        return NRF_ERROR_INVALID_STATE;
        
    if (*p_len16 > BLE_UD2_MAX_DATA_LEN) { return NRF_ERROR_INVALID_PARAM; }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_ud2->Dcfm_handles.value_handle;
    hvx_params.p_data = buf;
    hvx_params.p_len  = p_len16;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION; //->BLE_EVT_TX_COMPLETE

    r = sd_ble_gatts_hvx(p_ud2->conn_handle, &hvx_params);
    return(r);
}

uint32_t ble_ud2_notify_Ucmd(ble_ud2_t * p_ud2, uint8_t * buf, uint16_t* p_len16)
{
    ble_gatts_hvx_params_t hvx_params;
    uint32_t  r = 0;

    if (p_ud2 == NULL)
        return NRF_ERROR_NULL;

    if ((p_ud2->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_ud2->is_UCMD_notify_enabled))
        return NRF_ERROR_INVALID_STATE;
        
    if (*p_len16 > BLE_UD2_MAX_DATA_LEN) { return NRF_ERROR_INVALID_PARAM; }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_ud2->Ucmd_handles.value_handle;
    hvx_params.p_data = buf;
    hvx_params.p_len  = p_len16;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION; //->BLE_EVT_TX_COMPLETE

    r = sd_ble_gatts_hvx(p_ud2->conn_handle, &hvx_params);
    return(r);
}

uint32_t ble_ud2_notify_Udat(ble_ud2_t * p_ud2, uint8_t * buf, uint16_t* p_len16)
{
    ble_gatts_hvx_params_t hvx_params;
    uint32_t  r = 0;

    if (p_ud2 == NULL)
        return NRF_ERROR_NULL;

    if ((p_ud2->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_ud2->is_UDAT_notify_enabled))
        return NRF_ERROR_INVALID_STATE;
        
    if (*p_len16 > BLE_UD2_MAX_DATA_LEN) { return NRF_ERROR_INVALID_PARAM; }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_ud2->Udat_handles.value_handle;
    hvx_params.p_data = buf;
    hvx_params.p_len  = p_len16;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION; //->BLE_EVT_TX_COMPLETE

    r = sd_ble_gatts_hvx(p_ud2->conn_handle, &hvx_params);
    return(r);
}



