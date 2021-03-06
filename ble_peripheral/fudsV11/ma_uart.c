
#include "app_uart.h"
#include "myapp.h"



// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX RX
typedef enum
{
    ST_URX_STAGING,
    ST_URX_RECEIVING,
    ST_URX_TOSSING,
} eST_URX_t;

 eST_URX_t m_urx_state = ST_URX_STAGING;

//-----------------------------------------------------------------------------
static int32_t be_AddByte(uint8_t c)
{
    int32_t  r;
    if( m_curr_beUrx->length >= m_curr_beUrx->capacity)
        return(0);

    r = 1;
    m_curr_beUrx->buffer[m_curr_beUrx->length++] = c;
    return(r);
}


//-----------------------------------------------------------------------------
void serialSync_toss_0x01(uint8_t c)
{
    //TODO
    // work out when we get to the end of a packet
    // if( at the end) m_urx_state = ST_URX_STAGING;
}
void serialSync_toss_T2(uint8_t c)
{
    //TODO
    // work out when we get to the end of a packet
    // if( at the end) m_urx_state = ST_URX_STAGING;
}


void be_UCm_IndicateStartRecv(void);
void be_UB_IndicateStartRecv(void);

void be_UCm_IndicateDoneRecv(void);
void be_UB_IndicateDoneRecv(void);

void set_curr_beUrx_toStartPosition(void)
{
    m_curr_beUrx = &be_Urx;
    m_curr_beUrx->length = 0;
    m_curr_beUrx->c0 = 0;
    m_curr_beUrx->c1 = 0;
    m_curr_beUrx->pktType = ePkt_Unknown;
    m_urx_state = ST_URX_STAGING;
}

//-----------------------------------------------------------------------------
static int32_t serialSync_parseBytes( /*be_t *p_beUrx,*/ uint8_t *p_c, int len)
{
    volatile be_t *be_tmp;
    volatile be_t *be_new;
    int32_t r;
    uint8_t c = *p_c;
    r = 0;

    //=================
    //=====  BIG  =====
    //=================
    
    //dbgPrintf("[[%02x]]\r\n", c);

    
    if( m_curr_beUrx->pktType == ePkt_0x01 )
    {
        if( m_urx_state == ST_URX_TOSSING) {
            serialSync_toss_0x01(c); //DIFF
            return(42);
        }
        else
        if( m_urx_state == ST_URX_STAGING) {
            r = be_AddByte( c );
            if( m_curr_beUrx->length == 5 ) //DIFF  [0x01, CMD, SUB, L0, L1]
            {
                if(m_curr_beUtx == &be_CUm)
                {
                    //dbgPrint("RX UCm\r\n");
                    be_new = &be_UCm; // Set new buffer to Uart->CoreM buffer
                    //if(m_curr_beUrx->buffer[1] == m_curr_beUtx->buffer[1])
                    //{
                    //}
                    be_UCm_IndicateStartRecv();
                }
                else
                if(m_curr_beUtx == &be_BU)
                {
                    //dbgPrint("RX UB\r\n");
                    be_new = &be_UB; // Set new buffer to Uart->CoreM buffer
                    //if(m_curr_beUrx->buffer[1] == m_curr_beUtx->buffer[1])
                    //{
                    //}
                    be_UB_IndicateStartRecv();
                }
                /*TODO
                else // We have a RESPONSE
                if( EntityUartTx.owner == &OwnerBLE) // The sender was 
                {
                    be_new = &be_UB; // Set new buffer to Uart->Ble buffer
                }
                if( EntityUartTx.owner == &OwnerCoreM)
                {
                    be_new = &be_UCm; // Set new buffer to Uart->CoreM buffer
                    //if(m_curr_beUrx->buffer[1] == m_curr_beUtx->buffer[1])
                    //{
                    //}
                }
                TODO*/
                
                /*
                be_new = &be_UCs;
                be_new = &be_UCm;
                be_new = &be_UB;
                */
                be_tmp = m_curr_beUrx;
                m_curr_beUrx = be_new;
                m_curr_beUrx->pktType = be_tmp->pktType;
                m_curr_beUrx->length = be_tmp->length;
                memcpy( m_curr_beUrx->buffer, be_tmp->buffer, be_tmp->length );
                m_urx_state = ST_URX_RECEIVING;
            }
        }
        else
        if( m_urx_state == ST_URX_RECEIVING) {

            r = be_AddByte( c );
            r = proc01_checkPacketComplete( m_curr_beUrx->buffer, 0, m_curr_beUrx->length );
            if( r == 1)
            {
                if(m_curr_beUrx == &be_UB)
                {
                    be_UB_IndicateDoneRecv(); //TODO  BlkUp_Go( m_curr_beUrx->buffer, m_curr_beUrx->length); //be_UB
                }
                if(m_curr_beUrx == &be_UCm)
                {
                    be_UCm_IndicateDoneRecv();
                }
                set_curr_beUrx_toStartPosition(); //m_urx_state = ST_URX_STAGING;
            }
            if( r == 0) { } // Not a complete packet yet ... Continue
            if( r < 0) 
            {
                // -1 : Not a packet starting with 0x01
                // -2 : Checksum Error
            } 
        }
    }
    


    //=================
    //===== SMALL =====
    //=================
    if( m_curr_beUrx->pktType == ePkt_K2)
    {
//TODO        r = blk_uart_uartRxS_bufferPush( c );    
//TODO        r = procT2K2_checkPacketComplete( uartRxS_buffer, uartRxS_rp, uartRxS_wp, 'K' );
        //todo ...
    }
    if( m_curr_beUrx->pktType == ePkt_0xC0)
    {
//TODO        r = blk_uart_uartRxS_bufferPush( c );    
//TODO        r = procC0_checkPacketComplete( uartRxS_buffer, uartRxS_rp, uartRxS_wp );
        //todo ...
    }
    

    
    return(r);
}

static int32_t serialSync_parseByte( /*be_t *p_beUrx,*/ uint8_t b0)
{
    return( serialSync_parseBytes( &b0, 1) );
}

//-----------------------------------------------------------------------------
static int serialSync_data_event_process_uart_in(uint8_t b0)
{
    int sc = 0;

        
    if(m_curr_beUrx->pktType == ePkt_Unknown)
    {
        m_curr_beUrx->c1 = m_curr_beUrx->c0;
        m_curr_beUrx->c0 = b0;            
        if(                          m_curr_beUrx->c0 == 0x01  )
        {
            m_curr_beUrx->pktType = ePkt_0x01;
            serialSync_parseByte( 0x01 );
        }
/*
        if(                          m_curr_beUrx->c0 == 0xC0  )
        {
            m_curr_beUrx->pktType = ePkt_0xC0;
            serialSync_parseByte( 0xC0 );
        }
        if( (m_curr_beUrx->c1 == 'T') && (m_curr_beUrx->c0 == '2')  )
        {
            m_curr_beUrx->pktType = ePkt_T2;
            serialSync_parseByte( 'T' ); serialSync_parseByte( '2' );
        }
        if( (m_curr_beUrx->c1 == 'K') && (m_curr_beUrx->c0 == '2')  )
        {
            m_curr_beUrx->pktType = ePkt_K2;
            serialSync_parseByte( 'K' ); serialSync_parseByte( '2' );
        }
*/
        // otherwise toss
    }
    else
    {
        serialSync_parseByte( b0 );
    }

    return(sc);
}


int DEVT_uartRxReady()
{
    uint8_t  b0;
    uint32_t err_code;
    //printf("Debug: DEVT_uartRxReady\n");
    do {
        err_code = app_uart_get(&b0);
        if(err_code == NRF_SUCCESS)
        {
            serialSync_data_event_process_uart_in(b0);
        }
    } while(err_code == NRF_SUCCESS);

    return(0);
}



//-----------------------------------------------------------------------------
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX TX


#if USE_INT_BLOCK_UART

#include "nrf_drv_uart.h"
#include "nrf_assert.h"
#include "nordic_common.h"
#include "nrf_drv_common.h"
#include "nrf_gpio.h"
#include "app_util_platform.h"

//__STATIC_INLINE void interrupts_enable(uint8_t interrupt_priority)
__STATIC_INLINE void UART_INTS_ON()
{
    uint8_t interrupt_priority = APP_IRQ_PRIORITY_LOW;
    //CODE_FOR_UART
    //(
        nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);
        nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXTO);
        nrf_uart_int_enable(NRF_UART0, NRF_UART_INT_MASK_TXDRDY |
                                       NRF_UART_INT_MASK_RXTO);
    //)
    nrf_drv_common_irq_enable(UART0_IRQn, interrupt_priority);
}

//__STATIC_INLINE void interrupts_disable(void)
__STATIC_INLINE void UART_INTS_OFF()
{
    //CODE_FOR_UART
    //(
        nrf_uart_int_disable(NRF_UART0, NRF_UART_INT_MASK_RXDRDY |
                                        NRF_UART_INT_MASK_TXDRDY |
                                        NRF_UART_INT_MASK_ERROR  |
                                        NRF_UART_INT_MASK_RXTO);
    //)
    nrf_drv_common_irq_disable(UART0_IRQn);
}
#endif

void be_Cxx_Indicate_BufSend_Done(void);
void be_Cxx_Indicate_BufSend_Started(void);
static bool m_curr_beUtx_NowEmpty = false;

bool ma_uart_packetTx_start(void)
{
    uint32_t err_code;
    uint8_t c;

    //dbgPrint("\r\ma_uart_packetTx_start\r\n");

    if( m_curr_beUtx->rdPtr >= m_curr_beUtx->wrPtr )
        return(false);

    c = m_curr_beUtx->buffer[m_curr_beUtx->rdPtr];
    m_curr_beUtx->rdPtr++;
//dbgPrint("B11\r\n"); //vTaskDelay(100);

#if USE_INT_BLOCK_UART
    UART_INTS_OFF();  // see also: https://devzone.nordicsemi.com/question/68176/bug-in-app_uart_fifoc/
#endif
    err_code = app_uart_put( c );
#if USE_INT_BLOCK_UART
    UART_INTS_ON();
#endif

    //dbgPrint("B22\r\n"); //vTaskDelay(100);
    if( err_code != NRF_SUCCESS )
    {
        m_curr_beUtx->rdPtr--;
        dbgPrint("\r\napp_uart_put ERROR\r\n"); //vTaskDelay(100);
        return(false);
    }

    m_curr_beUtx_NowEmpty = false;

//dbgPrint("B44\r\n");
    be_Cxx_Indicate_BufSend_Started();
//dbgPrint("B55\r\n");
    
    return(true);
}



    




//-----------------------------------------------------------------------------
int DEVT_uartTxEmpty()
{
    uint32_t err_code;
    volatile uint8_t c;
    uint32_t fakeLimit;

    fakeLimit = 0;
    
    if( m_curr_beUtx_NowEmpty )
    {
        return( 0 );
    }
    
    // Uart TX is empty and m_curr_beUtx is empty
    if( m_curr_beUtx->rdPtr == m_curr_beUtx->wrPtr)
    {
        m_curr_beUtx_NowEmpty = true;
        be_Cxx_Indicate_BufSend_Done();
        //dbgPrint("1\r\n");
        return(42);
    }

    while( m_curr_beUtx->rdPtr < m_curr_beUtx->wrPtr )
    {
        c = m_curr_beUtx->buffer[m_curr_beUtx->rdPtr];
#if USE_INT_BLOCK_UART
        UART_INTS_OFF();  // see also: https://devzone.nordicsemi.com/question/68176/bug-in-app_uart_fifoc/
#endif
        err_code = app_uart_put( c );
#if USE_INT_BLOCK_UART
        UART_INTS_ON();
#endif
        if( err_code != NRF_SUCCESS )
        {
            dbgPrintf("app_uart_put ERROR = 0x%x\r\n", err_code );
            return(42);
        }
        else
        {
            m_curr_beUtx->rdPtr++;
            //karel for testing
            if( ++fakeLimit >= 42 )
            {
                break;
            }
        }
    }

    // if tx buffer has become empty.
    if( m_curr_beUtx->rdPtr == m_curr_beUtx->wrPtr)
    {
        //dbgPrint("*ALL*");
        //m_curr_beUtx_NowEmpty = true;
        // inform the other users that m_curr_beUtx is up for grabs
    }

    //dbgPrint("3\r\n");
    return(42);
}




//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#include "priority_scheduler.h"

void uart_thread_init()
{
}

static void uart_thread_process_event(uniEvent_t *pEvt)
{
    switch( pEvt->evtType )
    {
    case evt_Uart_RxReady:  //dbgPrint("evt_Uart_RxReady\r\n");
        DEVT_uartRxReady();
        break;

    case evt_Uart_TxEmpty:  //dbgPrint("evt_Uart_TxEmpty\r\n");
        DEVT_uartTxEmpty();
        break;
    } //swtich
}


static void uart_thread_process_queue_item(void * p_event_data, uint16_t event_size)
{
    //dbgPrint("PriorityThread -> ");
    //uniEvent_t *pE = (uniEvent_t *)p_event_data ;
    //dbgPrintf("Event i = %d\r\n", pE->i );
    
    uart_thread_process_event((uniEvent_t *)p_event_data);
}

void uart_thread_QueueSend(uniEvent_t *pEvt)
{
    priority_sched_event_put(pEvt, sizeof(uniEvent_t), uart_thread_process_queue_item);
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------


#include "myapp.h"
#include "app_uart.h"


bool m_uartIsDisabled = true;

void uart_event_handle(app_uart_evt_t * p_event)
{
    static uniEvent_t LEvt;

    if( m_uartIsDisabled )
    {
        dbgPrint("Uart Event while Uart is (being) closed");
        return;
    }

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            LEvt.evtType = evt_Uart_RxReady;
            uart_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
            break;

        //case APP_UART_COMMUNICATION_ERROR:
        //    APP_ERROR_HANDLER(p_event->data.error_communication);
        //    break;

        //case APP_UART_FIFO_ERROR:
        //    APP_ERROR_HANDLER(p_event->data.error_code);
        //    break;

        case APP_UART_TX_EMPTY:
            LEvt.evtType = evt_Uart_TxEmpty;
            uart_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
            break;
        
        default:
            break;
    }    
}

// must be a multiple of 2
//moved to myapp.h #define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
//moved to myapp.h #define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */
void ma_uart_Init(void)
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
        UART_BAUDRATE_BAUDRATE_Baud115200
        //UART_BAUDRATE_BAUDRATE_Baud38400
    };


    if( m_uartIsDisabled == false)
        return;

    
    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    //dbgPrint("ma_uart_Init\r\n");

    APP_ERROR_CHECK(err_code);
    m_uartIsDisabled = false;
}

void pinsUART_TX_NotFloating(void);
void pinsUART_RTS_NotFloating(void);

void ma_uart_Deinit() //karel
{

    //dbgPrint("ma_uart_Deinit\r\n");

    m_uartIsDisabled = true;
	app_uart_close();

    pinsUART_TX_NotFloating();
    pinsUART_RTS_NotFloating();


}


