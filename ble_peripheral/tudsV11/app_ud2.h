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


typedef void (* app_ud2_event_handler_t) (app_ud2_evt_t * p_app_ud2_event);

typedef void (* app_ud2_packet_handler_t) (app_ud2_t * p_ma_ud2, uint8_t * p_data, uint16_t length);


typedef struct app_ud2_init_s
{
    ble_ud2_t *p_ble_ud2;
    app_ud2_packet_handler_t  packet_handler;    // Event handler to be called for handling received DCMD.
    app_ud2_event_handler_t   event_handler;

} app_ud2_init_t;


typedef struct app_ud2_s
{
    ble_ud2_t *                p_ble_ud2;
    app_ud2_packet_handler_t   packet_handler;

}app_ud2_t;

int callThisWhenUartPacketForBleIsRecieved(void); //ma_join.c
int callThisWhenBlePacketIsRecieved(app_ud2_evt_t * p_app_ud2_event);  //ma_join.c

    


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

