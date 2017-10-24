
#include "ma_join.h"

//#include "app_tuds.h"
void BlkDn_unlockStateMachine(void);

#define PUBLIC 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static app_tuds_t *LOCAL_TMP_p_app_tuds = 0;

PUBLIC int make_req_BLE( be_t *be_Req )
{
    uint16_t i;
    
    uint8_t  *p_buf = LOCAL_TMP_p_app_tuds->m_blkDn_buf; //WWRONG
    uint16_t len    = LOCAL_TMP_p_app_tuds->m_blkDn_len; //WWRONG
    
    // Copy the buffer and the length variable
    be_Req->rdPtr = 0;
    be_Req->wrPtr = len - 2;
    be_Req->length = len - 2;
    for( i=0 ; i<be_Req->length; i++)
    {
        be_Req->buffer[i] = p_buf[i];
    }
    return(0);
}


static void join_blkDn_DnEventHandler(app_tuds_t *p_app_tuds, eBlkDn_EV_t event, void* thing)
{
    switch(event)
    {
        case eBlkDn_EV_RXDONE:
            dbgPrint("\r\nOn_Dn_packetAvailable");
            //dbgPrintf("\r\nRxLen = %d\n", m_blkDn_len - 2);
        
            LOCAL_TMP_p_app_tuds = p_app_tuds; // we could init/update our local copy here
        
            uniEvent_t LEvt;
            LEvt.evtType = evt_bleMaster_trigger;
            core_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )    
    
            BlkDn_unlockStateMachine(); // Tell the state machine its OK to receive the next command
            break;
        
        case eBlkDn_EV_CMD12:
            break;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP UP
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void join_blkUp_UpEventHandler(app_tuds_t *p_app_tuds, eBlkUp_EV_t event, void* thing)
{
    switch(event)
    {
        // The Up Machine has finished sending the packet we got from the UART
        // so we can release the packet
        case eBlkUp_EV_TXDONE:
            // TODO? Free up  be_UB.buffer  so uart can write to it again
            break;
        
        // The Up Machine has failed to send the packet we got from the UART
        case eBlkUp_EV_TXFAILED:
            break;
    }
}

//-----------------------------------------------------------------------------
// Callbacks from Uart for requests from BLE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// process the response from the UART
//-----------------------------------------------------------------------------
PUBLIC int proc_rsp_BLE( be_t *be_Req,  be_t *be_Rsp )
{
    //dbgPrint("\r\nproc_rsp_BLE");
    int rv;
    dbgPrintf("\r\nUB_len = %d", be_UB.length);
    dbgPrintf("\r\nUB[0] = %02x %02x %02x %02x", be_UB.buffer[0], be_UB.buffer[1], be_UB.buffer[2], be_UB.buffer[3]);

    app_tuds_Dn_AllowNextPacket(LOCAL_TMP_p_app_tuds);
    rv = app_tuds_U_StartSendPacket(LOCAL_TMP_p_app_tuds,  be_UB.buffer, be_UB.length);
    if( rv != 0)
    {
        // Failed, but just ignore failures
    }
    return(0);
}

//-----------------------------------------------------------------------------
// process the UART wait for response timeout event
//-----------------------------------------------------------------------------
PUBLIC int proc_timeout_BLE( be_t *be_Req,  be_t *be_Rsp )
{
    //dbgPrint("\r\nproc_timeout_BLE");
    // Since we have no data to send, just return and continue.
    //
    //   The Application on the host machine (our client) will have to timeout
    // and retry the command again.
    app_tuds_Dn_AllowNextPacket(LOCAL_TMP_p_app_tuds);
    return(0);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Connect this 'join' section to the 'app_tuds'
//-----------------------------------------------------------------------------
PUBLIC void join_Init(app_tuds_t *p_app_tuds)
{
    LOCAL_TMP_p_app_tuds = p_app_tuds;
    
    app_tuds_RegCb_DnEventHandler(p_app_tuds, join_blkDn_DnEventHandler);
    //p_app_tuds->blkDn_DnEventHandler = join_blkDn_DnEventHandler;

    app_tuds_RegCb_UpEventHandler(p_app_tuds, join_blkUp_UpEventHandler);
    //p_app_tuds->blkUp_UpEventHandler = join_blkUp_UpEventHandler;
}



