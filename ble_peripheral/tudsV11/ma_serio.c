
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
    
    sprintf(m_s1, "[[%02x]]\r\n", c);  dbgPrint( m_s1 );

    
    if( m_curr_beUrx->pktType == ePkt_0x01 )
    {
        if( m_urx_state == ST_URX_TOSSING) {
            serialSync_toss_0x01(c); //DIFF
            return(42);
        }
        else
        if( m_urx_state == ST_URX_STAGING) {
            r = be_AddByte( c );
            if( m_curr_beUrx->length == 5 ) //DIFF
            {
                if(m_curr_beUtx == &be_CUm)
                {
dbgPrint("RX UCm\r\n");
                    be_new = &be_UCm; // Set new buffer to Uart->CoreM buffer
                    //if(m_curr_beUrx->buffer[1] == m_curr_beUtx->buffer[1])
                    //{
                    //}
//dbgPrint("1\r\n");
                    be_UCm_IndicateStartRecv();
                }
                else
                if(m_curr_beUtx == &be_BU)
                {
dbgPrint("RX UB\r\n");
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
//dbgPrint("3\r\n");

            r = be_AddByte( c );
            r = proc01_checkPacketComplete( m_curr_beUrx->buffer, 0, m_curr_beUrx->length );
            if( r == 1)
            {
                if(m_curr_beUrx == &be_UB)
                {
dbgPrint("4_UB\r\n");
                    be_UB_IndicateDoneRecv(); //TODO  BlkUp_Go( m_curr_beUrx->buffer, m_curr_beUrx->length); //be_UB
                }
                if(m_curr_beUrx == &be_UCm)
                {
dbgPrint("4_UCm\r\n");
                    be_UCm_IndicateDoneRecv();
                }
                
                set_curr_beUrx_toStartPosition();
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

/*
void FAKE_debugSetbleMasterResponse(void)
{
    uint8_t buffer[20];
    uint8_t i;
    
    /////
    buffer[0] = 0x01;
    buffer[1] = 0x31;
    buffer[2] = 0x00;
    buffer[3] = 0x00;
    buffer[4] = 0x00;
    buffer[5] = 0x32;
    buffer[6] = 0x00;

    /////
    buffer[0] = 0x01;
    buffer[1] = 0x31;
    buffer[2] = 0x06;
    buffer[3] = 0x04;
    buffer[4] = 0x00;

    buffer[5] = 0x00;
    buffer[6] = 0x00;
    buffer[7] = 0x00;
    buffer[8] = 0x00;

    buffer[9] = 0x3C;
    buffer[10] = 0x00;


    m_curr_beUrx = &be_Urx;
    m_curr_beUrx->length = 0;
    m_curr_beUrx->c0 = 0;
    m_curr_beUrx->c1 = 0;
    m_curr_beUrx->pktType = ePkt_Unknown;
    m_urx_state = ST_URX_STAGING;
    

    for(i=0; i<11; i++)
    {
        serialSync_data_event_process_uart_in( buffer[i] );
    }
    
}
*/

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


void be_Cxx_Indicate_BufSend_Done(void);
void be_Cxx_Indicate_BufSend_Started(void);
static bool m_curr_beUtx_NowEmpty = false;

bool Uart_PacketTx_Start(void)
{
    uint32_t err_code;
    uint8_t c;

dbgPrint("\r\nUart_PacketTx_Start\r\n");

    if( m_curr_beUtx->rdPtr >= m_curr_beUtx->wrPtr )
        return(false);

    c = m_curr_beUtx->buffer[m_curr_beUtx->rdPtr];
    m_curr_beUtx->rdPtr++;
//dbgPrint("B11\r\n"); //vTaskDelay(100);
    err_code = app_uart_put( c );
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
//    static uniEvent_t LEvt;
    uint32_t err_code;
    volatile uint8_t c;
    uint32_t fakeLimit;

    //printf("Debug: DEVT_uartTxEmpty\n");
dbgPrint("DEVT_uartTxEmpty - "); //\r\n");
//vTaskDelay(100);

    fakeLimit = 0;
    
    if( m_curr_beUtx_NowEmpty )
    {
        dbgPrint("0\r\n");
        return( 0 );
    }
    
    // Uart TX is empty and m_curr_beUtx is empty
    if( m_curr_beUtx->rdPtr == m_curr_beUtx->wrPtr)
    {
        m_curr_beUtx_NowEmpty = true;
//dbgPrint("*EMP*");
        be_Cxx_Indicate_BufSend_Done();
        dbgPrint("1\r\n");
        return(42);
    }

    while( m_curr_beUtx->rdPtr < m_curr_beUtx->wrPtr )
    {
        c = m_curr_beUtx->buffer[m_curr_beUtx->rdPtr];
//sprintf(m_s1, "<<%02x>>\r\n", c);
//dbgPrint( m_s1 );

        err_code = app_uart_put( c );
        if( err_code != NRF_SUCCESS )
        {
            dbgPrint("*X*");
            //printf("\napp_uart_put ERROR\n");
            dbgPrint("2\r\n");
            return(42);
        }
        else
        {
            m_curr_beUtx->rdPtr++;

            //karel for testing
            if( ++fakeLimit >= 42 )
            {
            //    XXX.i = 42;
            //    xQueueSend( xQueueEvents, &XXX, 0);
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
    dbgPrint("3\r\n");
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


static void uart_thread_proccess_queue_item(void * p_event_data, uint16_t event_size)
{
    //dbgPrint("PriorityThread -> ");
    //uniEvent_t *pE = (uniEvent_t *)p_event_data ;
    //sprintf( m_s1, "Event i = %d\r\n", pE->i );
    //dbgPrint( m_s1 );
    
    uart_thread_process_event((uniEvent_t *)p_event_data);
}

void uart_thread_QueueSend(uniEvent_t *pEvt)
{
    priority_sched_event_put(pEvt, sizeof(uniEvent_t), uart_thread_proccess_queue_item);
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
