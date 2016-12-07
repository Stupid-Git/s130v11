#ifndef NOTIQ_H__
#define NOTIQ_H__

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>

#define BLE_UUID_UD2_SERVICE 0x0001                      /**< The UUID of the Nordic UART Service. */
#define BLE_UD2_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

/* Forward declaration of the ble_ud2_t type. */
typedef struct notiE_s notiE_t;

typedef void (*notiE_callback_t)(notiE_t *);

struct notiE_s
{
    void      *p_srcData;
    uint16_t  *p_conn_handle;
 
    uint16_t  *p_char_handle;
    bool      *p_enabled;

    uint16_t len16;
    uint8_t buf[20];

    //const ble_gatts_hvx_params_t hvx_params;
    ble_gatts_hvx_params_t hvx_params;
    
    notiE_callback_t callback;
    //void (*callback)(notiE_t *);
};

#define NOTIQ_LEN 10
//extern notiE_t notiQ[NOTIQ_LEN];

uint32_t notiQ_add( uint8_t *p_data, uint16_t len, uint16_t *p_conn_handle, bool *p_enabled, notiE_callback_t callback );

int notiQ_count(void);


#endif // NOTIQ_H__

/** @} */
