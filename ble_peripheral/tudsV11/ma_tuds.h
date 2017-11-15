#ifndef __MA_TUDS_H
#define __MA_TUDS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"


#include "ble_tuds.h"

/* ----------------------------------------------------------------------------
*  TYPES
*/
/* Forward declaration of the ble_tuds_t type. */
struct ma_tuds_s;
typedef struct ma_tuds_s ma_tuds_t;
/*OLD
typedef enum
{
    MA_TUDS_RX_START_PKT_0,
    MA_TUDS_RX_START_PKT_1,
    MA_TUDS_RX_PKT_0,        // Dcmd[1,1] ... packet received event
    MA_TUDS_RX_PKT_1,        // Dcmd[1,2] event
    MA_TUDS_RX_DONE_PKT_0,

    MA_TUDS_TX_DONE,
} ma_tuds_evt_type_t;

typedef struct
{
    ma_tuds_evt_type_t evt_type; // Type of event.
    union
    {
        uint32_t error_communication; // < Field used if evt_type is: APP_UART_COMMUNICATION_ERROR. This field contains the value in the ERRORSRC register for the UART peripheral. The UART_ERRORSRC_x defines from nrf5x_bitfields.h can be used to parse the error code. See also the \nRFXX Series Reference Manual for specification. 
        uint32_t error_code;          // < Field used if evt_type is: NRF_ERROR_x. Additional status/error code if the error event type is APP_UART_FIFO_ERROR. This error code refer to errors defined in nrf_error.h. 
        uint8_t  value;               // < Field used if evt_type is: NRF_ERROR_x. Additional status/error code if the error event type is APP_UART_FIFO_ERROR. This error code refer to errors defined in nrf_error.h. 
    } data;
} ma_tuds_evt_t;
OLD*/






typedef struct ma_tuds_init_s
{
    ble_tuds_t *p_ble_tuds;
} ma_tuds_init_t;

uint32_t  ma_tuds_init(ma_tuds_t * p_ma_tuds, const ma_tuds_init_t * p_ma_tuds_init);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
typedef enum eBlkDnState
{
    eBlkDn_WAIT_CMD,

    eBlkDn_WAIT_PACKET,
    eBlkDn_GOT_PACKET,
    eBlkDn_CFM_RE_SEND,
    eBlkDn_CFM_RE_PRESEND,

    eBlkDn_CFM_OK_PRESEND, // set before CFM packet sent
    eBlkDn_CFM_NG_PRESEND,

    eBlkDn_CFM_OK_SEND, // set after CFM packet sent
    eBlkDn_CFM_NG_SEND, // set after CFM packet sent

    eBlkDn_CFM_OK_SENT, // set after CFM packet sent (Write Done or ACK) received

} eBlkDnState_t;

typedef enum eBlkDn_EV
{
    eBlkDn_EV_RXDONE,
    eBlkDn_EV_CMD12,
} eBlkDn_EV_t;

typedef void (* blkDn_DnEventHandler_t)(ma_tuds_t *p_ma_tuds, eBlkDn_EV_t event, void *thing);



typedef enum eBlkUpState
{
    eBlkUp_IDLE,
    eBlkUp_CMD_PRESEND,
    eBlkUp_CMD_SEND,
    eBlkUp_CMD_SENT,
    eBlkUp_DAT_PRESEND,
    eBlkUp_DAT_SEND,
    eBlkUp_DAT_SENT,
    
    eBlkUp_WAIT_CFM,
    
} eBlkUpState_t;


typedef enum eBlkUp_EV
{
    eBlkUp_EV_TXDONE,  // can release packet buffer
    eBlkUp_EV_TXFAILED,
} eBlkUp_EV_t;

typedef void (* blkUp_UpEventHandler_t)(ma_tuds_t *p_ma_tuds, eBlkUp_EV_t event, void *thing);

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------
#define BLK_DN_COUNT (128 + 8)

#define BLK_UP_COUNT (128 + 8)
typedef struct ma_tuds_s
{
    ble_tuds_t *                p_ble_tuds;

    //-------------------------------------------------------------------------
    // DN
    int      m_BlkDn_packetWaitTimeCount;// = 0;

    uint8_t  m_blkDn_buf[ 16 * BLK_DN_COUNT ]; // 2048 @ BLK_DN_COUNT = 128
    uint8_t  m_blkDn_chk[  1 * BLK_DN_COUNT ]; //  128 @ BLK_DN_COUNT = 128
    uint16_t m_blkDn_len;
    uint16_t m_blkDn_blkCnt;
    uint16_t m_blkDn_rxBlkCnt;
    
    uint8_t  m_Dcfm_buf[20];
    uint16_t m_Dcfm_len;
    
    eBlkDnState_t m_BlkDn_sm;// = eBlkDn_WAIT_CMD;

    blkDn_DnEventHandler_t  blkDn_DnEventHandler;
    bool     m_BlkDn_stateMachineLocked;
    
    //-------------------------------------------------------------------------
    // UP
    uint8_t* m_blkUp_p_buf;//[ 16 * BLK_UP_COUNT ]; // 2048 @ BLK_UP_COUNT = 128
    uint8_t  m_blkUp_chk[  1 * BLK_UP_COUNT ]; //  128 @ BLK_UP_COUNT = 128
    uint16_t m_blkUp_len;
    uint16_t m_blkUp_blkCnt;
    uint16_t m_blkUp_txBlkCnt;
    uint8_t  m_blkUp_chkSumLSB;
    uint8_t  m_blkUp_chkSumMSB;

    eBlkUpState_t m_BlkUp_sm;// = eBlkUp_IDLE;
    
    blkUp_UpEventHandler_t  blkUp_UpEventHandler;
    
}ma_tuds_t;


void ma_tuds_service_init(ma_tuds_t *p_ma_tuds, ble_tuds_t *p_ble_tuds);
uint32_t ma_tuds_timer_init(void);

void ma_tuds_on_ble_evt(ma_tuds_t * p_ma_tuds, ble_evt_t * p_ble_evt);



//-----------------------------------------------------------------------------
// -- ma_tuds_d.h --
uint32_t ma_tuds_BlkDn_timer_init(void);

uint32_t ma_tuds_D_onBleConnect(ma_tuds_t *p_ma_tuds);
uint32_t ma_tuds_D_onBleDisconnect(ma_tuds_t *p_ma_tuds);

int32_t  ma_tuds_Dcmd_handler(ma_tuds_t *p_ma_tuds, uint8_t *buf, uint8_t len);
int32_t  ma_tuds_Ddat_handler(ma_tuds_t *p_ma_tuds, uint8_t *buf, uint8_t len);

int32_t  ma_tuds_Dcfm_handler_OnWrittenComplete(ma_tuds_t *p_ma_tuds,  uint8_t *buf, uint8_t len);

uint32_t ma_tuds_RegCb_DnEventHandler(ma_tuds_t *p_ma_tuds, blkDn_DnEventHandler_t  cb);
uint32_t ma_tuds_Dn_AllowNextPacket(ma_tuds_t *p_ma_tuds); 


//-----------------------------------------------------------------------------
// -- ma_tuds_u.h --
uint32_t ma_tuds_BlkUp_timer_init(void);

uint32_t ma_tuds_U_onBleConnect(ma_tuds_t *p_ma_tuds);
uint32_t ma_tuds_U_onBleDisconnect(ma_tuds_t *p_ma_tuds);

int32_t  ma_tuds_Ucfm_handler(ma_tuds_t *p_ma_tuds, uint8_t *buf, uint8_t len);

int32_t  ma_tuds_Ucmd_handler_OnWrittenComplete(ma_tuds_t *p_ma_tuds,  uint8_t *buf, uint8_t len);
int32_t  ma_tuds_Udat_handler_OnWrittenComplete(ma_tuds_t *p_ma_tuds,  uint8_t *buf, uint8_t len);

int32_t  ma_tuds_U_StartSendPacket(ma_tuds_t *p_ma_tuds, uint8_t *pkt, uint16_t len);
uint32_t ma_tuds_RegCb_UpEventHandler(ma_tuds_t *p_app_UD, blkUp_UpEventHandler_t  cb);

void ma_tuds_BlkUp_Go_Test(ma_tuds_t *p_app_UD);

//-----------------------------------------------------------------------------
// -- ma_tuds.h --
//static void tuds_Dcmd_data_handler  (ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
//static void tuds_Ddat_data_handler  (ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
//static void tuds_Ucfm_data_handler  (ble_tuds_t * p_tuds, uint8_t * p_data, uint16_t length);
//static void tuds_tx_complete_handler(ble_tuds_t * p_tuds, ble_evt_t * p_ble_evt);



#ifdef __cplusplus
}
#endif

#endif // __DEBUG_ETC_H

