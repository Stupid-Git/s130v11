/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_uart_over_ble_main main.c
 * @{
 * @ingroup  ble_sdk_app_nus_eval
 * @brief    UART over BLE application main file.
 *
 * This file contains the source code for a sample application that uses the Nordic UART service.
 * This application uses the @ref srvlib_conn_params module.
 */
 
//#define USE_SDK_V11 1
 
#define USE_APPSH 1 //1

#define USE_GP_TIMER 1 //1

#define USE_UD2 1

#if USE_UD2
#endif

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"


#include "myapp.h"  //for uniEvent_t
#if USE_APPSH
#include "app_scheduler.h" // need to add nrflibraries/app_scheduler.c, app_timer_appsh.c
#include "softdevice_handler_appsh.h"
#include "app_timer_appsh.h"   //ks add lib app_scheduler, app_timer_with_app_scheduler, sd_handler_with_app_scheduler
#else
#include "app_timer.h"
#endif

#include "app_button.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp.h"
#include "bsp_btn_ble.h"

#if USE_UD2
#include "ble_ud2.h"
#endif

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                           /**< Include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#define CENTRAL_LINK_COUNT              0                                           /**<number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**<number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                     "XNordic_UART"                               /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#if USE_GP_TIMER
#define APP_TIMER_OP_QUEUE_SIZE         4 + 4  //karel                              /**< Size of timer operation queues. */
#else
#define APP_TIMER_OP_QUEUE_SIZE         4 + 4  //karel                              /**< Size of timer operation queues. */
#endif

// OK with Q10->Q100#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(11.75, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
// 20 seems stable #define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
//#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */

//#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
//#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */

//#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(8.75, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
//#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(8.75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(11.75, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(11.75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */

//#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
//#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */

#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define START_STRING                    "\nStart...\n"                                /**< The string that will be sent over the UART when the application starts. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */


#if USE_APPSH
#define SCHED_MAX_EVENT_DATA_SIZE_A      (MAX(APP_TIMER_SCHED_EVT_SIZE, sizeof(uniEvent_t)))
//#define SCHED_MAX_EVENT_DATA_SIZE        MAX(APP_TIMER_SCHED_EVT_SIZE   , BLE_STACK_HANDLER_SCHED_EVT_SIZE)  //APPSH  /**< Maximum size of scheduler events. */
#define SCHED_MAX_EVENT_DATA_SIZE        MAX(SCHED_MAX_EVENT_DATA_SIZE_A, BLE_STACK_HANDLER_SCHED_EVT_SIZE)  //APPSH  /**< Maximum size of scheduler events. */

#define SCHED_QUEUE_SIZE                 200//10    // 100 good at 7.5      14,20,30, 50,60 80=NG                            //APPSH    
#endif


//#define UART_TX_BUF_SIZE                256   //karel                                      /**< UART TX buffer size. */
//#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */
#define UART_TX_BUF_SIZE                4096   //karel                                      /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                4096                                         /**< UART RX buffer size. */

static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */


//karel
static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */
//karel -----
//#define BLE_UUID_UD2_SERVICE 0x0001                      /**< The UUID of the Nordic UART Service. */
//#define BLE_UD2_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
//#define UD2_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */
//
//static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_UD2_SERVICE, UD2_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */
//karel -----

//-----------------------------------------------------------------------------
#if USE_UD2
//static
ble_ud2_t                        m_ble_ud2;                                      /**< Structure to identify the UD2 Service. */
uint32_t timers_init_ud2_part(void);
void services_init_ud2_part(void);
void application_timers_start_ud2_part(void);
#endif
//-----------------------------------------------------------------------------



/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_pre_init(void) //karel
{
#if USE_APPSH
    // Initialize timer module, making it use the scheduler.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
#else
    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
#endif
}

//uint32_t P7_timer_init(void);
//uint32_t P8_timer_init(void);

#if USE_GP_TIMER
uint32_t GP_timer_init(void);
#endif


static void timers_init() //karel
{
#if USE_BAS
    //bas_timer_init();
#endif

    //GP_timer_init();
#if USE_GP_TIMER
    GP_timer_init();
#endif

    //ma_uart_timer_init(); // karel - Init the timer for Uart Shutdown timimg
    //ma_holdoff_timer_init();
    
    timers_init_ud2_part();  // USE_TUDS_U
    

    
    //P7_timer_init();
    //P8_timer_init();
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of 
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */

int pkt_count = 0;
int RXpkt_count = 0;
int RXpkt_max = 0;
uint8_t  RXdata_array[20]; 

#define RXLOGMAX 20
uint8_t RXlog[RXLOGMAX];
int RXlogIdx =0;
uint32_t RX_next()
{
    int i;
    uint32_t err_code;

    if(RXpkt_count>RXpkt_max)
        return(0xDEADBEEF);
    
    uint16_t length = 20;

    uint8_t c;
    
    c = (RXpkt_count % 10) + '0';
    
    for(i=0;i<20;i++) 
        RXdata_array[i] = c;//'0';

    err_code = ble_nus_string_send(&m_nus, RXdata_array, length);
    if( err_code == NRF_SUCCESS)
    {
        RXpkt_count++;
    }
    return(err_code);
}

void RX_start(int n)
{
    int i;
    for(i=0;i<20;i++) 
        RXdata_array[i] = '0';
    
    RXlogIdx = 0;
    RXpkt_count = 1;
    RXpkt_max = n;    

    if( RX_next() == NRF_SUCCESS)
    {
        if(RXlogIdx <RXLOGMAX)
            RXlog[RXlogIdx++] = 1;
    }

}

void RX_print()
{
    int i;

    //if(RXpkt_count>RXpkt_max)
    {
        for( i=0 ; i<RXlogIdx; i++)
        {
            printf("\n   RXlog[%d] = %d", i, RXlog[i]);
        }
        printf("\n");
    }

    
}


static void on_tx_complete(ble_nus_t * p_nus, ble_evt_t * p_ble_evt)
{
    //int i;
    //if( p_nus->rx_handles.value_handle == ...)
    
    RXlog[RXlogIdx] = 0;
#if 1 //SAD
    int i;
    for(i=0; i<2;i++)
    {
        if( RX_next() == NRF_SUCCESS)
        {
            RXlog[RXlogIdx] += 1;
        }
        else
            break;
    }
    
#else
    while( RX_next() == NRF_SUCCESS)
    {
        RXlog[RXlogIdx] += 1;
    }
#endif
    if(RXlogIdx <RXLOGMAX)
        RXlogIdx++;

    if(RXpkt_count>RXpkt_max)
    {
        //printf(".");
//        printf("\n RXlogIdx = %d", RXlogIdx);
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

const char * get_ble_evt_str( uint8_t evt_id);

void RX_next_ble_evt(ble_evt_t * p_ble_evt)
{
  //dbgPrintf("\n\revt = %d,%x: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id);
    //printf("ble_ud2_on_ble_evt\r\n");
    //printf("\n\revt = %d, 0x%x, s = %s: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id, get_ble_evt_str(p_ble_evt->header.evt_id) );


    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            //on_connect(p_ud2, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            //on_disconnect(p_ud2, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            //on_write(p_ud2, p_ble_evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
#if 0 // MOVED TO TDCTRLS
//            on_rw_authorize_request(p_ud2, p_ble_evt);
#endif // MOVED TO TDCTRLS
            break;
        
        case BLE_EVT_TX_COMPLETE:
            on_tx_complete(&m_nus, p_ble_evt);
            break;
        
        default:
            // No implementation needed.
            break;
    }
}


int RX_sendPktCount = 0;
static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
    int n=0;
    //karel
    if( (*(p_data+0) == '!') && (length==1))
    {
        printf("Cnt = %d\n", pkt_count);
        pkt_count = 0;
    }
    else
    if( (*(p_data+0) == '"') && (length==1))
    {
        RX_print();
        pkt_count = 0;
    }
    else
    if( (*(p_data+0) == '!') && (length>=2))
    {
        if(length==4)
        {
            n = 100 * ( (*(p_data+1) - '0') )
               + 10 * ( (*(p_data+2) - '0') )
               +  1 * ( (*(p_data+3) - '0') );
        }
        else
        if(length==3)
        {
            n = 10 * ( (*(p_data+1) - '0') )
               + 1 * ( (*(p_data+2) - '0') );
        }
        else
        if(length==2)
        {
            n =  1 * ( (*(p_data+1) - '0') );
        }
        else
        {
            n = 11;
        }
        RX_sendPktCount = n;
        printf("RX_start(%d)\n", RX_sendPktCount);
        
        RX_start(RX_sendPktCount);
    }
    else
    {
        pkt_count++;
    }
    
    if( (pkt_count % 100 ) == 0)
    {
        printf("Cnt = %d\n", pkt_count);
    }
    /*
    for (uint32_t i = 0; i < length; i++)
    {
        while(app_uart_put(p_data[i]) != NRF_SUCCESS);
    }
    while(app_uart_put('\n') != NRF_SUCCESS);
    */
}
/**@snippet [Handling the data received over BLE] */

#if USE_UD2
void ud2_Dcmd_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length);
void ud2_Ddat_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length);
void ud2_Ucfm_data_handler(ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length);
void ud2_tx_complete_handler(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt);
#endif


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t       err_code;
    
    //-----
#if USE_UD2
    services_init_ud2_part();                               //// ADVERTISING UUID128 This one is FIRST
/*
    ble_ud2_init_t ud2_init;
    memset(&ud2_init, 0, sizeof(ud2_init));

    ud2_init.Dcmd_data_handler = ud2_Dcmd_data_handler;
    ud2_init.Ddat_data_handler = ud2_Ddat_data_handler;
    //ud2_init._notify_enabled = false;
    
    ud2_init.Ucfm_data_handler = ud2_Ucfm_data_handler;

    ud2_init.tx_complete_handler = ud2_tx_complete_handler;    
    
    err_code = ble_ud2_init(&m_ble_ud2, &ud2_init);
    APP_ERROR_CHECK(err_code);
*/
#endif

    //-----
#if 1
    ble_nus_init_t nus_init;
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;
    
    err_code = ble_nus_init(&m_nus, &nus_init);             //// ADVERTISING UUID128 This one is second
    APP_ERROR_CHECK(err_code);
#endif
    

    
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
         printf("\n on_conn_params_evt: NG");
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }

    //karel    
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_SUCCEEDED)
    {
        printf("\n on_conn_params_evt: OK");
    }
    
    
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;
        default:
            break;
    }
}


/**@brief Function for the application's SoftDevice event handler.
 *
 * @param[in] p_ble_evt SoftDevice event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code;
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/*moved to debug_etc.c
const char * get_ble_evt_str( uint8_t evt_id)
{
    const char * s = "Unknown";
    
    switch( evt_id )
    {
    // ble.h BLE_EVT_BASE = 0x01
    case BLE_EVT_TX_COMPLETE:                          // 0x01 < Transmission Complete. @ref ble_evt_tx_complete_t 
        s = "BLE_EVT_TX_COMPLETE";
        break;
    case BLE_EVT_USER_MEM_REQUEST:                     // 0x02 < User Memory request. @ref ble_evt_user_mem_request_t 
        s = "BLE_EVT_USER_MEM_REQUEST";
        break;
    case BLE_EVT_USER_MEM_RELEASE:                     // 0x03 < User Memory release. @ref ble_evt_user_mem_release_t 
        s = "BLE_EVT_USER_MEM_RELEASE";
        break;
    
    // ble_gap.h BLE_GAP_EVT_BASE = 0x10
    case BLE_GAP_EVT_CONNECTED:                        // 0x10 < Connection established.                         \n See @ref ble_gap_evt_connected_t.            
        s = "BLE_GAP_EVT_CONNECTED";
        break;
    case BLE_GAP_EVT_DISCONNECTED:                     // 0x11 < Disconnected from peer.                         \n See @ref ble_gap_evt_disconnected_t.         
        s = "BLE_GAP_EVT_DISCONNECTED";
        break;
    case BLE_GAP_EVT_CONN_PARAM_UPDATE:                // 0x12 < Connection Parameters updated.                  \n See @ref ble_gap_evt_conn_param_update_t.    
        s = "BLE_GAP_EVT_CONN_PARAM_UPDATE";
        break;
    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:               // 0x13 < Request to provide security parameters.         \n Reply with @ref sd_ble_gap_sec_params_reply.  \n See @ref ble_gap_evt_sec_params_request_t. 
        s = "BLE_GAP_EVT_SEC_PARAMS_REQUEST";
        break;
    case BLE_GAP_EVT_SEC_INFO_REQUEST:                 // 0x14 < Request to provide security information.        \n Reply with @ref sd_ble_gap_sec_info_reply.    \n See @ref ble_gap_evt_sec_info_request_t.   
        s = "BLE_GAP_EVT_SEC_INFO_REQUEST";
        break;
    case BLE_GAP_EVT_PASSKEY_DISPLAY:                  // 0x15 < Request to display a passkey to the user.       \n See @ref ble_gap_evt_passkey_display_t.      
        s = "BLE_GAP_EVT_PASSKEY_DISPLAY";
        break;
    case BLE_GAP_EVT_AUTH_KEY_REQUEST:                 // 0x16 < Request to provide an authentication key.       \n Reply with @ref sd_ble_gap_auth_key_reply.    \n See @ref ble_gap_evt_auth_key_request_t.   
        s = "BLE_GAP_EVT_AUTH_KEY_REQUEST";
        break;
    case BLE_GAP_EVT_AUTH_STATUS:                      // 0x17 < Authentication procedure completed with status. \n See @ref ble_gap_evt_auth_status_t.          
        s = "BLE_GAP_EVT_AUTH_STATUS";
        break;
    case BLE_GAP_EVT_CONN_SEC_UPDATE:                  // 0x18 < Connection security updated.                    \n See @ref ble_gap_evt_conn_sec_update_t.      
        s = "BLE_GAP_EVT_CONN_SEC_UPDATE";
        break;
    case BLE_GAP_EVT_TIMEOUT:                          // 0x19 < Timeout expired.                                \n See @ref ble_gap_evt_timeout_t.              
        s = "BLE_GAP_EVT_TIMEOUT";
        break;
    case BLE_GAP_EVT_RSSI_CHANGED:                     // 0x1A < RSSI report.                                    \n See @ref ble_gap_evt_rssi_changed_t.         
        s = "BLE_GAP_EVT_RSSI_CHANGED";
        break;
    case BLE_GAP_EVT_ADV_REPORT:                       // 0x1B < Advertising report.                             \n See @ref ble_gap_evt_adv_report_t.           
        s = "BLE_GAP_EVT_ADV_REPORT";
        break;
    case BLE_GAP_EVT_SEC_REQUEST:                      // 0x1C < Security Request.                               \n See @ref ble_gap_evt_sec_request_t.          
        s = "BLE_GAP_EVT_SEC_REQUEST";
        break;    
    case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:        // 0x1D < Connection Parameter Update Request.            \n Reply with @ref sd_ble_gap_conn_param_update. \n See @ref ble_gap_evt_conn_param_update_request_t. 
        s = "BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST";
        break;    
    case BLE_GAP_EVT_SCAN_REQ_REPORT:                  // 0x1E < Scan request report.                            \n See @ref ble_gap_evt_scan_req_report_t.      
        s = "BLE_GAP_EVT_SCAN_REQ_REPORT";
        break;

    // "ble_gatts.h" BLE_GATTS_EVT_BASE = 0x50
    case BLE_GATTS_EVT_WRITE:                          // 0x50 < Write operation performed.                                           \n See @ref ble_gatts_evt_write_t.                 
        s = "BLE_GATTS_EVT_WRITE";
        break;
    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:           // 0x51 < Read/Write Authorization request.                                    \n Reply with @ref sd_ble_gatts_rw_authorize_reply. \n See @ref ble_gatts_evt_rw_authorize_request_t. 
        s = "BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST";
        break;
    case BLE_GATTS_EVT_SYS_ATTR_MISSING:               // 0x52 < A persistent system attribute access is pending.                     \n Respond with @ref sd_ble_gatts_sys_attr_set.     \n See @ref ble_gatts_evt_sys_attr_missing_t.     
        s = " BLE_GATTS_EVT_SYS_ATTR_MISSING";
        break;
    case BLE_GATTS_EVT_HVC:                            // 0x53 < Handle Value Confirmation.                                           \n See @ref ble_gatts_evt_hvc_t.                   
        s = " BLE_GATTS_EVT_HVC";
        break;
    case BLE_GATTS_EVT_SC_CONFIRM:                     // 0x54 < Service Changed Confirmation. No additional event structure applies.                                                    
        s = " BLE_GATTS_EVT_SC_CONFIRM";
        break;
    case BLE_GATTS_EVT_TIMEOUT:                 
        s = "BLE_GATTS_EVT_TIMEOUT";
        break;

    default:
        s = "Unknown";
        break;
    }
       
    return( s );
}
moved to debug_etc.c */

/**@brief Function for dispatching a SoftDevice event to all modules with a SoftDevice 
 *        event handler.
 *
 * @details This function is called from the SoftDevice event interrupt handler after a 
 *          SoftDevice event has been received.
 *
 * @param[in] p_ble_evt  SoftDevice event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    //printf("\n\revt = %d,%x: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id);
    if( (p_ble_evt->header.evt_id != 80) && (p_ble_evt->header.evt_id != 1))
    { 
        printf("\n\revt = %d, 0x%x, s = %s: ", p_ble_evt->header.evt_id, p_ble_evt->header.evt_id, get_ble_evt_str(p_ble_evt->header.evt_id) );
    }

    ble_conn_params_on_ble_evt(p_ble_evt);
    
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    RX_next_ble_evt(/*&m_nus,*/ p_ble_evt);

    ble_ud2_on_ble_evt(&m_ble_ud2, p_ble_evt);
    
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;
    

#if USE_APPSH
    // Initialize the SoftDevice handler module.
#if USE_SDK_V11
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    SOFTDEVICE_HANDLER_APPSH_INIT(&clock_lf_cfg, true); //APPSH
#else
    SOFTDEVICE_HANDLER_APPSH_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, true); //APPSH
#endif

#else
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);
#endif

    
    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);
        
    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
    
    ble_enable_params.common_enable_params.vs_uuid_count   = 3; //karel S130 enough room for 3 128bit UUIDs
    
    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}




#if USE_APPSH
/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void) //APPSH
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE); //APPSH
}
/* TODO
#include "priority_scheduler.h"
static void priority_scheduler_init(void)
{
    PRIORITY_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}
*/
#endif


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            err_code = ble_advertising_restart_without_whitelist();
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break;
    }
}


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to 
 *          a string. The string will be be sent over BLE when the last character received was a 
 *          'new line' i.e '\n' (hex 0x0D) or if the string has reached a length of 
 *          @ref NUS_MAX_DATA_LENGTH.
 */
/**@snippet [Handling the data received over UART] */
void BlkUp_Go_Test(void);//karel
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            
            //karel
            if(data_array[index]=='c')
                BlkUp_Go_Test();
            //karel
            index++;

            if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN)))
            {
                err_code = ble_nus_string_send(&m_nus, data_array, index);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
                
                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
static void uart_init(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_ENABLED,
        false,
        //UART_BAUDRATE_BAUDRATE_Baud38400 //karel
        UART_BAUDRATE_BAUDRATE_Baud115200 //karel
    };

    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);
    APP_ERROR_CHECK(err_code);
}
/**@snippet [UART Initialization] */


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    ble_advdata_t scanrsp;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = false;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]); //// ADVERTISING UUID128 !!!!!
    scanrsp.uuids_complete.p_uuids  = m_adv_uuids; //// ADVERTISING UUID128 !!!!! //karel changed from BLE_UUID_NUS_SERVICE to BLE_UUID_UD2_SERVICE 
                                                   //// ADVERTISING UUID128 !!!!!         BUT which UUID128 is sent depends only on which service
                                                   //// ADVERTISING UUID128 !!!!!         is defined first  search for this ADVERTISING UUID128

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, &scanrsp, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), 
                                 bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for placing the application in low power state while waiting for events.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


#if USE_GP_TIMER
#define  GP_TIMER_PERIOD_1000MS   1000
uint32_t GP_timer_start(uint32_t timeout_ticks);
#endif


//==============================================================================
//==============================================================================
// this function call is to set the settings for "sd_ble_opt_set" for BLE_CONN_BW_LO,MID,HIGH etc
// to see if the setting were too low by default and thus causing UP transfer to be throttled to 1packet per connection interval
// It however appears that the default setting, is for the highest bandwidth
void set_the_connection_bandwidth()
{    
    uint32_t err_code;
    //uint32_t opt_id;
    ble_opt_t ble_opt;
    
    memset(&ble_opt, 0x00, sizeof(ble_opt));
    uint8_t conn_bw;
    conn_bw = BLE_CONN_BW_NONE;
    //conn_bw = BLE_CONN_BW_LOW;
    //conn_bw = BLE_CONN_BW_MID;
    //conn_bw = BLE_CONN_BW_HIGH;
    
    ble_opt.common_opt.conn_bw.conn_bw.conn_bw_tx = conn_bw; //< Connection bandwidth configuration for transmission, see @ref BLE_CONN_BWS.
    ble_opt.common_opt.conn_bw.conn_bw.conn_bw_rx = conn_bw;  //< Connection bandwidth configuration for reception, see @ref BLE_CONN_BWS.
    ble_opt.common_opt.conn_bw.role = BLE_GAP_ROLE_PERIPH; //BLE_GAP_ROLE_CENTRAL; //#define BLE_GAP_ROLE_CENTRAL     0x2     Central Role. 
    err_code = sd_ble_opt_set(BLE_COMMON_OPT_CONN_BW, &ble_opt);
    printf("err_code = 0x%x\r\n", err_code);
}
//==============================================================================
//==============================================================================



/**@brief Application main function.
 */
int main(void)
{
    uint32_t err_code;
    bool erase_bonds;
    uint8_t  start_string[] = START_STRING;
    
    // Initialize.
//SAD    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
//SAD
    timers_pre_init(); //ks
    timers_init(); //ks

    uart_init();
    buttons_leds_init(&erase_bonds);

    
    ble_stack_init();

// this function call is to set the settings for "sd_ble_opt_set" for BLE_CONN_BW_LO,MID<HIGH 
//set_the_connection_bandwidth(); // this function call is to set the settings for "sd_ble_opt_set" for BLE_CONN_BW_LO,MID<HIGH
// this function call is to set the settings for "sd_ble_opt_set" for BLE_CONN_BW_LO,MID<HIGH
    
    
#if USE_APPSH
    scheduler_init();  // <> NOTE: KAREL Adjust Queue size etc as needed +++++++++++
    //TODOT priority_scheduler_init();  // <> NOTE: KAREL Adjust Queue size etc as needed +++++++++++
#else
#endif

    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();
    
    printf("%s",start_string);

    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
    
    
#if USE_GP_TIMER
    GP_timer_start( APP_TIMER_TICKS(GP_TIMER_PERIOD_1000MS, APP_TIMER_PRESCALER) );
#endif


    // Enter main loop.
    for (;;)
    {
#if USE_APPSH
        //TODOT priority_sched_execute();
        app_sched_execute();
#else

#endif

        power_manage();
    }
}


/** 
 * @}
 */
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//#include "myapp.h"

extern int app_sched_count(void);
extern int kmax; //karel
extern int kdelta; //karel

static void core_thread_process_queue_item(void * p_event_data, uint16_t event_size)
{
    //dbgPrint("PriorityThread -> ");
    //uniEvent_t *pE = (uniEvent_t *)p_event_data ;
    //dbgPrintf("Event i = %d\r\n", pE->i );
    
    //core_thread_process_event((uniEvent_t *)p_event_data);

    printf("####  pEvt->evtType = %d\r\n", ((uniEvent_t *)p_event_data)->evtType);

    printf("      app_sched_count = %d, (%d) Delta = %d\r\n", app_sched_count() , kmax, kdelta);
    kdelta = 0;

    //SS_thread_process_event((uniEvent_t *)p_event_data);
    //SS_process_event((uniEvent_t *)p_event_data);
}

//void ss_thread_QueueSend(uniEvent_t *pEvt)
void core_thread_QueueSend(uniEvent_t *pEvt)
{
    app_sched_event_put(pEvt, sizeof(uniEvent_t), core_thread_process_queue_item);
    //priority_sched_event_put(pEvt, sizeof(uniEvent_t), core_thread_process_queue_item);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#if USE_GP_TIMER
#endif


#if USE_GP_TIMER
void autoTimeout_onTick(void);

APP_TIMER_DEF(m_GP_timer_id);

static void GP_timeout_handler(void * p_context) // Effectively a 1 Second timer
{
    UNUSED_PARAMETER(p_context);
    uint32_t err_code;    

    //dbgPrint(".");
    //printf(".");
/*
    err_code = app_timer_stop(m_GP_timer_id);
    if( err_code != NRF_SUCCESS )
    {
        //sdoTE("GP: app_timer_stop NG\r\n");
        printf("GP: app_timer_stop NG\r\n");
        printf("GP: app_timer_stop NG err=code = %d\r\n", err_code);
    }
*/
    err_code = app_timer_start(m_GP_timer_id, APP_TIMER_TICKS(GP_TIMER_PERIOD_1000MS, APP_TIMER_PRESCALER), NULL);
    if (err_code == NRF_SUCCESS)
    {
        printf("."); //WTF?!?!?!? If I put this printf here I never see an X, but if I have the APP_ERROR_HANDLER I reset all the time.
        //APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    if( err_code != NRF_SUCCESS )
    {
        printf("@TG NG err=code = %d\r\n", err_code);
    }
    //if (err_code == NRF_ERROR_NO_MEM)
    //{
    //    printf("*"); //WTF?!?!?!? If I put this printf here I never see an X, but if I have the APP_ERROR_HANDLER I reset all the time.
    //    //APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    //}
    

    //dummy    
    uniEvent_t LEvt;
    LEvt.evtType = evt_bleMaster_UartTxDone;
    core_thread_QueueSend(&LEvt);

  
}


//static uint32_t m_app_ticks_per_100ms;
#define BSP_MS_TO_TICK(MS) (m_app_ticks_per_100ms * (MS / 100))

uint32_t GP_timer_init(void)
{
    uint32_t err_code;

    // Create timer
    err_code = app_timer_create(&m_GP_timer_id, APP_TIMER_MODE_SINGLE_SHOT, GP_timeout_handler);
    if( err_code != NRF_SUCCESS )
    {
        printf("GP: app_timer_create NG\r\n");
        ;//sdoTE("GP: app_timer_create NG\r\n");
    }
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}


uint32_t GP_timer_start(uint32_t timeout_ticks)
{
    uint32_t err_code;

    //err_code = app_timer_stop(m_GP_timer_id);
    //if( err_code != NRF_SUCCESS )
    //{
    //    //sdoTE("GP: app_timer_stop NG\r\n");
    //    printf("GP: app_timer_stop NG\r\n");
    //}

    // Start timer - Note: ignored if already started (freeRTOS)
    //err_code = app_timer_start(m_GP_timer_id, BSP_MS_TO_TICK(timeout_ticks), NULL);
    err_code = app_timer_start(m_GP_timer_id, timeout_ticks, NULL);
    if( err_code != NRF_SUCCESS )
    {
        //sdoTE("GP: app_timer_start NG\r\n");
        printf("GP: app_timer_start NG\r\n");
    }
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}

uint32_t GP_timer_stop(void)
{
    uint32_t err_code;
    // Stop timer
    err_code = app_timer_stop(m_GP_timer_id);
    if( err_code != NRF_SUCCESS )
    {
        //sdoTE("GP: app_timer_stop NG\r\n");
        printf("GP: app_timer_stop NG\r\n");
    }
    //APP_ERROR_CHECK(err_code);
    return(err_code);
}
#endif


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



