
#include "myapp.h"
#include "block_proc.h"
#include "block_up.h"
#include "app_tuds.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int callThisWhenBlePacketIsRecieved(app_tuds_evt_t * p_app_tuds_event)
{
    switch( p_app_tuds_event->evt_type )
    {
    case APP_TUDS_RX_PKT_0: // Dcmd[1,1] ... packet received event
        break;
    case APP_TUDS_RX_PKT_1: // Dcmd[1,2] event
        break;
    }
    
    dbgPrint("\r\nOn_Dn_packetAvailable");
    dbgPrintf("\r\nRxLen = %d\n", m_blkDn_len - 2);
   
   
    uniEvent_t LEvt;
    LEvt.evtType = evt_bleMaster_trigger;
    core_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )

    
    BlkDn_unlockStateMachine(); // Tell the state machine its OK to receive the next command
    return(0);
}



int make_req_BLE( be_t *be_Req )
{
    uint16_t i;
    
    be_Req->rdPtr = 0;
    be_Req->wrPtr = m_blkDn_len - 2;
    be_Req->length = m_blkDn_len - 2;
    for( i=0 ; i<be_Req->length; i++)
    {
        be_Req->buffer[i] = m_blkDn_buf[i];
    }
    
    //be_BU.rdPtr = 0;
    //be_BU.wrPtr = m_blkDn_len - 2;
    //be_BU.length = m_blkDn_len - 2;
    //for( i=0 ; i<be_BU.length; i++)
    //{
    //    be_BU.buffer[i] = m_blkDn_buf[i];
    //}
    return(0);
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int callThisWhenUartPacketForBleIsRecieved(void)
{
    //BlkUp_Go( m_curr_beUrx->buffer, m_curr_beUrx->length); //be_UB
    dbgPrintf("\r\nUB_len = %d", be_UB.length);
    dbgPrintf("\r\nUB[0] = %02x %02x %02x %02x", be_UB.buffer[0], be_UB.buffer[1], be_UB.buffer[2], be_UB.buffer[3]);


    BlkUp_Go( be_UB.buffer, be_UB.length); //be_UB
    return(0);
}

int proc_rsp_BLE( be_t *be_Req,  be_t *be_Rsp )
{
    //dbgPrint("\r\nproc_rsp_BLE");
    callThisWhenUartPacketForBleIsRecieved(); //TODO  BlkUp_Go( m_curr_beUrx->buffer, m_curr_beUrx->length); //be_UB
    return(0);
}

int proc_timeout_BLE( be_t *be_Req,  be_t *be_Rsp )
{
    //dbgPrint("\r\nproc_timeout_BLE");
    return(0);
}

