
#include "myapp.h"
#include "block_proc.h"
#include "block_up.h"
#include "app_tuds.h"


int callThisWhenBlePacketIsRecieved(app_tuds_evt_t * p_app_tuds_event)
{
    switch( p_app_tuds_event->evt_type )
    {
    case APP_TUDS_RX_PKT_0: // Dcmd[1,1] ... packet received event
            
        break;

    case APP_TUDS_RX_PKT_1: // Dcmd[1,2] event
            
        break;
        
    }
    
#if 0 //EE //GGG
    int       i;
    int32_t   r;
    uint16_t  tx_len;

    
    dbgPrint("On_Dn_packetAvailable\r\n");
    dbgPrintf("RxLen = %d\n", m_blkDn_len - 2);
/*
    for( i=0 ; i < m_blkDn_len - 2 ; i++)
    {
         sprintf(&m_s1[i*3], " %02x", m_blkDn_buf[i]);        
    }
    m_s1[i*3 + 0] = '\r';
    m_s1[i*3 + 1] = '\n';
    m_s1[i*3 + 2] = 0;
    dbgPrint( m_s1 );
*/
    
    //r = Bogus01_On_Rx0x01(m_blkDn_buf, m_blkDn_len - 2);
    //tx_len = r;

    m_blkDn_buf[0] = 0x01;
    m_blkDn_buf[1] = 0x58;
    m_blkDn_buf[2] = 0x06;
    m_blkDn_buf[3] = 0x04;
    m_blkDn_buf[4] = 0x00;

    m_blkDn_buf[5] = 0x01;
    m_blkDn_buf[6] = 0x02;
    m_blkDn_buf[7] = 0x03;
    m_blkDn_buf[8] = 0x04;

    m_blkDn_buf[9] = 0x6D;
    m_blkDn_buf[10] = 0x00;
    tx_len =11;
    
    
dbgPrintf("TxLen = %d\n", tx_len);
    
    BlkUp_Go(m_blkDn_buf, tx_len);

#else

dbgPrint("On_Dn_packetAvailable\r\n");
dbgPrintf("RxLen = %d\n", m_blkDn_len - 2);
   
//vTaskDelay(100);
    
    //BlkUart_directUartSend(m_blkDn_buf, m_blkDn_len - 2); // Added "- 2" because there is no need to send the totalCheckSum
    
    uniEvent_t LEvt;
    LEvt.evtType = evt_bleMaster_trigger;
    core_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
#endif

    BlkDn_unlockStateMachine(); // Tell the state machine its OK to receive the next command
    return(0);
}

void  NEWcallThisWhenBlePacketIsRecieved(void);
void  NEWcallThisWhenBlePacketIsRecieved(void)
{
    dbgPrint("On_Dn_packetAvailable\r\n");
    dbgPrintf("RxLen = %d\n", m_blkDn_len - 2);
   
    uniEvent_t LEvt;
    LEvt.evtType = evt_bleMaster_trigger;
    core_thread_QueueSend(&LEvt); 

    BlkDn_unlockStateMachine(); // Tell the state machine its OK to receive the next command
}


int callThisWhenUartPacketForBleIsRecieved(void)
{
    //BlkUp_Go( m_curr_beUrx->buffer, m_curr_beUrx->length); //be_UB
    dbgPrintf("UB_len = %d\r\n", be_UB.length);
    dbgPrintf("UB[0] = %02x %02x %02x %02x\r\n", be_UB.buffer[0], be_UB.buffer[1], be_UB.buffer[2], be_UB.buffer[3]);


    BlkUp_Go( be_UB.buffer, be_UB.length); //be_UB
    return(0);
}
