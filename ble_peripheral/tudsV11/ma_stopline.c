
#include "ma_stopline.h"


//#include "app_button.h"
//#include "app_timer.h"
//#include "app_error.h"
//#include "nrf_drv_gpiote.h"
//#include "nrf_assert.h"
#include "sdk_common.h" //VERIFY_SUCCESS



#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"


#if APP_TD_STOPLINE_ENABLED //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


//#include "core_cm0.h"  //NVIC_ClearPendingIRQ
#include "nrf51.h"

#include "debug_etc.h"


// [[1]]
void GPIOTE_IRQHandler(void);
// the GPIOTE_IRQHandler() in "nrf_drv_gpiote.c" will handle all gpio interrupts
// and wemust add a callback for the pin we want

// [[2]]
// D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\components\drivers_nrf\gpiote\nrf_drv_gpiote.c (502)
//                 handler(pin,polarity);

//__STATIC_INLINE void pin_in_use_by_te_set(uint32_t pin, uint32_t channel_id, nrf_drv_gpiote_evt_handler_t handler, bool is_channel)
//    m_cb.pin_assignments[pin] = channel_id;
//    m_cb.handlers[channel_id] = handler;

//static int8_t channel_port_alloc(uint32_t pin,nrf_drv_gpiote_evt_handler_t handler, bool channel)
//            pin_in_use_by_te_set(pin, i, handler, channel);

//ret_code_t nrf_drv_gpiote_in_init(nrf_drv_gpiote_pin_t pin, nrf_drv_gpiote_in_config_t const * p_config, nrf_drv_gpiote_evt_handler_t evt_handler)
//        int8_t channel = channel_port_alloc(pin, evt_handler, p_config->hi_accuracy);

// [[3]] 
// D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\components\libraries\button\app_button.c
//uint32_t app_button_init(app_button_cfg_t *             p_buttons, uint8_t                        button_count, uint32_t                       detection_delay)
//        err_code = nrf_drv_gpiote_in_init(p_btn->pin_no, &config, gpiote_event_handler);

// [[4]]
// 
//uint32_t bsp_init(uint32_t type, uint32_t ticks_per_100ms, bsp_event_callback_t callback)
//            err_code = app_button_init((app_button_cfg_t *)app_buttons, BUTTONS_NUMBER, ticks_per_100ms / 2);




/*
#ifdef BSP_BUTTON_0
    #define PIN_IN BSP_BUTTON_0
#endif
#ifndef PIN_IN
    #error "Please indicate input pin"
#endif

#ifdef BSP_LED_0
    #define PIN_OUT BSP_LED_0
#endif
//#ifdef BSP_LED_1
//    #define PIN_OUT BSP_LED_1
//#endif
#ifndef PIN_OUT
    #error "Please indicate output pin"
#endif


void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nrf_drv_gpiote_out_toggle(PIN_OUT);
}
*/

/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
/*
static void ref_gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

    err_code = nrf_drv_gpiote_out_init(PIN_OUT, &out_config);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(PIN_IN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(PIN_IN, true);
}
*/





//#define STOPLINES_NUMBER 2
//#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define STOPLINE_PIN_00   0  //P0.00
#define STOPLINE_TESTPIN  1  //P0.01

#define STOPLINE_PIN   STOPLINE_PIN_00




uint32_t ma_stopline_event_enable(void)
{
    nrf_drv_gpiote_in_event_enable( STOPLINE_PIN, true);
    return NRF_SUCCESS;
}

uint32_t ma_stopline_event_disable(void)
{
    nrf_drv_gpiote_in_event_disable( STOPLINE_PIN);
    return NRF_SUCCESS;
}

/*
uint32_t stopline_button_is_pushed(uint8_t button_id, bool * p_is_pushed) //uint32_t app_button_is_pushed(uint8_t button_id, bool * p_is_pushed)
{
    ASSERT(button_id <= m_button_count);
    ASSERT(mp_buttons != NULL);

    app_button_cfg_t * p_btn = &mp_buttons[button_id];
    bool is_set = nrf_drv_gpiote_in_is_set(p_btn->pin_no);

    *p_is_pushed = !(is_set^(p_btn->active_state == APP_BUTTON_ACTIVE_HIGH));
    
    return NRF_SUCCESS;
}
*/


static stopline_cb m_stopline_cb = 0;
uint32_t m_pin_state;
static void gpiote_stopline_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    //uint32_t err_code;
    uint32_t pin_mask = 1 << pin;

    if (nrf_drv_gpiote_in_is_set(pin))
    {
        if( m_stopline_cb != 0)
            m_stopline_cb (1);
        m_pin_state |= pin_mask;
        dbgPrintf("Pin HI \r\n");
    }
    else
    {
        if( m_stopline_cb != 0)
            m_stopline_cb (0);
        m_pin_state &= ~(pin_mask);
        dbgPrintf("Pin LO \r\n");
    }
}


uint32_t ma_stopline_init(stopline_cb _stopline_cb)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    in_config.pull = NRF_GPIO_PIN_PULLUP;
  
    err_code = nrf_drv_gpiote_in_init(STOPLINE_PIN, &in_config, gpiote_stopline_event_handler); //err_code = nrf_drv_gpiote_in_init(p_btn->pin_no, &config, gpiote_event_handler);
    VERIFY_SUCCESS(err_code);    

    nrf_drv_gpiote_in_event_enable(STOPLINE_PIN, true);

    m_stopline_cb = _stopline_cb;
    return NRF_SUCCESS;
}

uint32_t ma_stopline_deinit(void)
{
     //TODO   
    return NRF_SUCCESS;
}

uint32_t ma_stopline_value(void)
{
     //uint32_t pin_mask = 1 << STOPLINE_PIN;

    if (nrf_drv_gpiote_in_is_set(STOPLINE_PIN))
        return(1);
    
    return(0);    
}

//void ma_stopline_xxx()
//{
//     NVIC_ClearPendingIRQ(GPIOTE_IRQn);
//}


#endif //APP_TD_STOPLINE_ENABLED //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
