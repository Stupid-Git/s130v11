
#include "notiQ.h"


 /*
 * @retval ::NRF_SUCCESS Successfully queued a notification or indication for transmission, and optionally updated the attribute value.
 * @retval ::BLE_ERROR_INVALID_CONN_HANDLE Invalid Connection Handle.
 * @retval ::NRF_ERROR_INVALID_STATE Invalid Connection State or notifications and/or indications not enabled in the CCCD.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid parameter(s) supplied.
 * @retval ::BLE_ERROR_INVALID_ATTR_HANDLE Invalid attribute handle(s) supplied. Only attributes added directly by the application are available to notify and indicate.
 * @retval ::BLE_ERROR_GATTS_INVALID_ATTR_TYPE Invalid attribute type(s) supplied, only characteristic values may be notified and indicated.
 * @retval ::NRF_ERROR_NOT_FOUND Attribute not found.
 * @retval ::NRF_ERROR_DATA_SIZE Invalid data size(s) supplied.
 * @retval ::NRF_ERROR_BUSY Procedure already in progress.
 * @retval ::BLE_ERROR_GATTS_SYS_ATTR_MISSING System attributes missing, use @ref sd_ble_gatts_sys_attr_set to set them to a known value.
 * @retval ::BLE_ERROR_NO_TX_PACKETS  No available application packets for this connection, applies only to notifications.
 */
//SVCALL(SD_BLE_GATTS_HVX, uint32_t, sd_ble_gatts_hvx(uint16_t conn_handle, ble_gatts_hvx_params_t const *p_hvx_params));

 
uint16_t m_conn_handle = 0;

/**@brief Nordic UART Service event handler type. */
//typedef void (*ble_ud2_data_handler_t) (ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length);
//typedef void (*ble_ud2_tx_complete_handler_t) (ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt);

/*
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
*/

notiE_t notiQ[NOTIQ_LEN];

notiE_t *notiFREE[NOTIQ_LEN];
notiE_t *notiUSED[NOTIQ_LEN];

notiE_t *notiQ_rp;
notiE_t *notiQ_wp;

int notiQ_Init()
{
    int i;
    for(i=0; i<NOTIQ_LEN; i++)
    {
        notiFREE[i] = &notiQ[i];
        notiUSED[i] = 0;
    }

    notiQ_rp = &notiQ[0];
    notiQ_wp = &notiQ[0];
    return(0);    
}




int notiQ_count()
{
    return(0);    
}

int Uwp = 0;
int Urp = 0;
int Fwp = 0;
int Frp = 0;


notiE_t* notiQ_At(int pos)
{
    notiE_t *p_notiE;
    int theRp = 0;

    if( Urp == Uwp)
        return(0);
    
    theRp = Urp + pos;
    if(theRp >= NOTIQ_LEN)
        theRp -= NOTIQ_LEN;
    
    p_notiE = notiUSED[theRp];
    return(p_notiE);
}

notiE_t * notiQ_RemoveAt(int pos)
{
    notiE_t *p_notiE;
    p_notiE = 0;
    
    p_notiE = notiUSED[Urp];
    Urp++;
    if(Urp >= NOTIQ_LEN)
        Urp = 0;

    notiFREE[Fwp] = p_notiE;
    Fwp++;
    if(Fwp >= NOTIQ_LEN)
        Fwp = 0;
    
    return(p_notiE);
}


uint32_t sendQ()
{    
    uint32_t err_code;
    uint8_t  count;
    notiE_t *p_notiE;
    
    err_code = sd_ble_tx_packet_count_get(m_conn_handle, &count);
    if(err_code != NRF_SUCCESS)
        return(err_code);
    
    if(count==0)
    {
        return(BLE_ERROR_NO_TX_PACKETS);
    }
    
    while(count>0)
    {
        p_notiE = notiQ_At(0);
        if(p_notiE == 0)
            break;
        
        err_code = sd_ble_gatts_hvx(*(p_notiE->p_conn_handle), &p_notiE->hvx_params);
        if(err_code != NRF_SUCCESS)
            return(err_code);
        
        if(p_notiE->callback != 0 )            
            p_notiE->callback(p_notiE);
        
        notiQ_RemoveAt(0);
        count--;
        
    }
    
    return(err_code);
}


static notiE_t *notiQ_Get()
{
    notiE_t *p_notiE;
    p_notiE = 0;
    
    if(Fwp == Frp)
        return(0);
    
    p_notiE = notiFREE[Frp];
    Frp++;
    if(Frp>=NOTIQ_LEN)
        Frp = 0;
    
    return( p_notiE);
}

static void notiQ_Add(notiE_t *p_notiE)
{
    notiUSED[Uwp] = p_notiE;
    Uwp++;
    if(Uwp >= NOTIQ_LEN)
        Uwp = 0;
}

#include "string.h" //...-> memcpy
uint32_t notiQ_add( uint8_t *p_data, uint16_t len, uint16_t *p_char_handle, bool *p_enabled, notiE_callback_t callback)
{    
    uint32_t err_code;    
    notiE_t *p_notiE;
    
    p_notiE = notiQ_Get();
    if(p_notiE == 0)
        return(BLE_ERROR_NO_TX_PACKETS);
    
    p_notiE->callback = callback;
    p_notiE->p_srcData = p_data;
    
    p_notiE->p_char_handle = p_char_handle;
    p_notiE->p_enabled = p_enabled;
    
    memcpy(&p_notiE->buf, p_data, len);    
    p_notiE->len16 = len;

    p_notiE->hvx_params.handle = *p_char_handle;
    p_notiE->hvx_params.offset = 0;
    p_notiE->hvx_params.p_data = &p_notiE->buf[0];
    p_notiE->hvx_params.p_len = &p_notiE->len16;
    p_notiE->hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
    
    notiQ_Add(p_notiE);
    
    err_code = sendQ();
    
    return(err_code);
}

void notiQ_tx_done(int count)
{
    //uint32_t err_code;    
    notiE_t *p_notiE;
    
    while(count>0)
    {
        p_notiE = notiQ_At(0);
        if(p_notiE == 0)
            break;
        
        if(p_notiE->callback != 0 )            
            p_notiE->callback(p_notiE);
        
        notiQ_RemoveAt(0);
        count--;
        
    }    
}

