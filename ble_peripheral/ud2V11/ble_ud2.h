#ifndef BLE_UD2_H__
#define BLE_UD2_H__

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>

#define BLE_UUID_UD2_SERVICE 0x0001                      /**< The UUID of the Nordic UART Service. */
#define BLE_UD2_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

/* Forward declaration of the ble_ud2_t type. */
typedef struct ble_ud2_s ble_ud2_t;

/**@brief Nordic UART Service event handler type. */
typedef void (*ble_ud2_data_handler_t) (ble_ud2_t * p_ud2, uint8_t * p_data, uint16_t length);
typedef void (*ble_ud2_tx_complete_handler_t) (ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt);
/**@brief Nordic UART Service initialization structure.
 *
 * @details This structure contains the initialization information for the service. The application
 * must fill this structure and pass it to the service using the @ref ble_ud2_init
 *          function.
 */
typedef struct
{
    void*                    parentContext;           // pointer to structure of upper service layer

    ble_ud2_data_handler_t   Dcmd_data_handler;            /**< Event handler to be called for handling received data. */
    ble_ud2_data_handler_t   Ddat_data_handler;            /**< Event handler to be called for handling received data. */
    ble_ud2_data_handler_t   Ucfm_data_handler;            /**< Event handler to be called for handling received data. */
    
    ble_ud2_tx_complete_handler_t  tx_complete_handler;
    
} ble_ud2_init_t;

/**@brief Nordic UART Service structure.
 *
 * @details This structure contains status information related to the service.
 */
struct ble_ud2_s
{
    uint8_t                  uuid_type;               /**< UUID type for UD2 Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of UD2 Service (as provided by the SoftDevice). */

    void*                      parentContext;           // pointer to structure of upper service layer
    
    ble_gatts_char_handles_t Dcmd_handles;            /**< Handles related to the Dcmd characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t Ddat_handles;            /**< Handles related to the Ddat characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t Dcfm_handles;            /**< Handles related to the Dcfm characteristic (as provided by the SoftDevice). */

    ble_gatts_char_handles_t Ucmd_handles;            /**< Handles related to the Ucmd characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t Udat_handles;            /**< Handles related to the Udat characteristic (as provided by the SoftDevice). */
    ble_gatts_char_handles_t Ucfm_handles;            /**< Handles related to the Ucfm characteristic (as provided by the SoftDevice). */
    
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    
    bool                     is_DCFM_notify_enabled;  /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    bool                     is_UCMD_notify_enabled;  /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    bool                     is_UDAT_notify_enabled;  /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
    ble_ud2_data_handler_t   Dcmd_data_handler;            /**< Event handler to be called for handling received data. */
    ble_ud2_data_handler_t   Ddat_data_handler;            /**< Event handler to be called for handling received data. */
    ble_ud2_data_handler_t   Ucfm_data_handler;            /**< Event handler to be called for handling received data. */
   
    ble_ud2_tx_complete_handler_t  tx_complete_handler;

};

/**@brief Function for initializing the Nordic UART Service.
 *
 * @param[out] p_ud2      Nordic UART Service structure. This structure must be supplied
 *                        by the application. It is initialized by this function and will
 *                        later be used to identify this particular service instance.
 * @param[in] p_ud2_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was successfully initialized. Otherwise, an error code is returned.
 * @retval NRF_ERROR_NULL If either of the pointers p_ud2 or p_ud2_init is NULL.
 */
uint32_t ble_ud2_init(ble_ud2_t * p_ud2, const ble_ud2_init_t * p_ud2_init);

/**@brief Function for handling the Nordic UART Service's BLE events.
 *
 * @details The Nordic UART Service expects the application to call this function each time an
 * event is received from the SoftDevice. This function processes the event if it
 * is relevant and calls the Nordic UART Service event handler of the
 * application if necessary.
 *
 * @param[in] p_ud2       Nordic UART Service structure.
 * @param[in] p_ble_evt   Event received from the SoftDevice.
 */
void ble_ud2_on_ble_evt(ble_ud2_t * p_ud2, ble_evt_t * p_ble_evt);

/**@brief Function for sending a string to the peer.
 *
 * @details This function sends the input string as an RX characteristic notification to the
 *          peer.
 *
 * @param[in] p_ud2       Pointer to the Nordic UART Service structure.
 * @param[in] p_string    String to be sent.
 * @param[in] length      Length of the string.
 *
 * @retval NRF_SUCCESS If the string was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ud2_string_send(ble_ud2_t * p_ud2, uint8_t * p_string, uint16_t length);




uint32_t ble_ud2_notify_Dcfm(ble_ud2_t * p_ud2, uint8_t * buf, uint16_t* p_len16);
uint32_t ble_ud2_notify_Ucmd(ble_ud2_t * p_ud2, uint8_t * buf, uint16_t* p_len16);
uint32_t ble_ud2_notify_Udat(ble_ud2_t * p_ud2, uint8_t * buf, uint16_t* p_len16);

#endif // BLE_UD2_H__

/** @} */
