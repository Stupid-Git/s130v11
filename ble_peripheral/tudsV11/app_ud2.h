#ifndef __APP_TUDS_H
#define __APP_TUDS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"


#include "ble_ud2.h"

/* ----------------------------------------------------------------------------
*  TYPES
*/
/* Forward declaration of the ble_ud2_t type. */
struct app_ud2_s;
typedef struct app_ud2_s app_ud2_t;

typedef enum
{
    APP_TUDS_RX_START_PKT_0,
    APP_TUDS_RX_START_PKT_1,
    APP_TUDS_RX_PKT_0,        // Dcmd[1,1] ... packet received event
    APP_TUDS_RX_PKT_1,        // Dcmd[1,2] event
    APP_TUDS_RX_DONE_PKT_0,

    APP_TUDS_TX_DONE,
} app_ud2_evt_type_t;

typedef struct
{
    app_ud2_evt_type_t evt_type; /**< Type of event. */
    union
    {
        uint32_t error_communication; /**< Field used if evt_type is: APP_UART_COMMUNICATION_ERROR. This field contains the value in the ERRORSRC register for the UART peripheral. The UART_ERRORSRC_x defines from nrf5x_bitfields.h can be used to parse the error code. See also the \nRFXX Series Reference Manual for specification. */
        uint32_t error_code;          /**< Field used if evt_type is: NRF_ERROR_x. Additional status/error code if the error event type is APP_UART_FIFO_ERROR. This error code refer to errors defined in nrf_error.h. */
        uint8_t  value;               /**< Field used if evt_type is: NRF_ERROR_x. Additional status/error code if the error event type is APP_UART_FIFO_ERROR. This error code refer to errors defined in nrf_error.h. */
    } data;
} app_ud2_evt_t;







typedef struct app_ud2_init_s
{
    ble_ud2_t *p_ble_ud2;

} app_ud2_init_t;

uint32_t  app_ud2_init(app_ud2_t * p_app_ud2, const app_ud2_init_t * p_app_ud2_init);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
typedef enum eBlkDnState
{
    eBlkDn_WAIT_CMD,
    eBlkDn_WAIT_PACKET,
    eBlkDn_CFM_RE_PRESEND,
    eBlkDn_CFM_RE_SEND,
    eBlkDn_GOT_PACKET,
    eBlkDn_CFM_PRESEND, // set before CFM packet sent
    eBlkDn_CFM_OK_SEND, // set after CFM packet sent
    eBlkDn_CFM_NG_SEND, // set after CFM packet sent
    eBlkDn_CFM_SENT, // set after CFM packet sent (Write Done or ACK) received

    //eBlkDn_PROCESS_PACKET,
    //eBlkDn_SEND_CMD,
    //eBlkDn_SEND_DATA,
    //eBlkDn_SEND_WAITCFM,

} eBlkDnState_t;

typedef enum eBlkDn_EV
{
    eBlkDn_EV_RXDONE,
    eBlkDn_EV_CMD12,
} eBlkDn_EV_t;

typedef void (* blkDn_DnEventHandler_t)(app_ud2_t *p_app_ud2, eBlkDn_EV_t event, void *thing);



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
    eBlkUp_EV_TXDONE,
    eBlkUp_EV_TXFAILED,
} eBlkUp_EV_t;

typedef void (* blkUp_UpEventHandler_t)(app_ud2_t *p_app_ud2, eBlkUp_EV_t event, void *thing);

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------
#define BLK_DN_COUNT (128 + 8)

#define BLK_UP_COUNT (128 + 8)
typedef struct app_ud2_s
{


    ble_ud2_t *                p_ble_ud2;

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

    
}app_ud2_t;


void app_ud2_service_init(app_ud2_t *p_app_ud2, ble_ud2_t *p_ble_ud2);
uint32_t app_ud2_timer_init(void);

void app_ud2_on_ble_evt(app_ud2_t * p_app_ud2, ble_evt_t * p_ble_evt);

int app_ud2_U_StartSendPacket(app_ud2_t *p_app_ud2, uint8_t *pkt, uint16_t len);


























    


//BOGUS
/*
#undef dbgPrintf
#undef dbgPrint

#define dbgPrintf printf
#define dbgPrint  printf
*/
//BOGUS



#ifdef __cplusplus
}
#endif

#endif // __DEBUG_ETC_H

