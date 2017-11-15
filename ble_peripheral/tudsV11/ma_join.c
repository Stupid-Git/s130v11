
#include "ma_join.h"

//#include "ma_tuds.h"

#define PUBLIC 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN DN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static ma_tuds_t *LOCAL_TMP_p_ma_tuds = 0;

PUBLIC int make_req_BLE( be_t *be_Req )
{
    uint16_t i;
    
    uint8_t  *p_buf = LOCAL_TMP_p_ma_tuds->m_blkDn_buf; //WWRONG
    uint16_t len    = LOCAL_TMP_p_ma_tuds->m_blkDn_len; //WWRONG
    
    // Copy the buffer and the length variable
    be_Req->rdPtr = 0;
    be_Req->wrPtr = len - 2;
    be_Req->length = len - 2;
    for( i=0 ; i<be_Req->length; i++)
    {
        be_Req->buffer[i] = p_buf[i];
    }
   
    //ma_tuds_Dn_AllowNextPacket(LOCAL_TMP_p_ma_tuds); //Have copied Data, so is now finished with BlkDn data
    
    return(0);
}


static void join_blkDn_DnEventHandler(ma_tuds_t *p_ma_tuds, eBlkDn_EV_t event, void* thing)
{
    switch(event)
    {
        case eBlkDn_EV_RXDONE:
            dbgPrint("\r\nOn_Dn_packetAvailable");
            //dbgPrintf("\r\nRxLen = %d\n", m_blkDn_len - 2);
        
            LOCAL_TMP_p_ma_tuds = p_ma_tuds; // we could init/update our local copy here

            //cpb_BLE.pending = true;
            //cpb_BLE.processing = false;
            /*
            if( cpb_BLE.pending == true) 
            {
                return;
            }
            */
        
            uniEvent_t LEvt;
            LEvt.evtType = evt_bleMaster_trigger;
            core_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )    
            
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
static void join_blkUp_UpEventHandler(ma_tuds_t *p_ma_tuds, eBlkUp_EV_t event, void* thing)
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

    ma_tuds_Dn_AllowNextPacket(LOCAL_TMP_p_ma_tuds); //UART TxRx Done, so is now finished with BlkDn data
    
    rv = ma_tuds_U_StartSendPacket(LOCAL_TMP_p_ma_tuds,  be_UB.buffer, be_UB.length);
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
    
    ma_tuds_Dn_AllowNextPacket(LOCAL_TMP_p_ma_tuds); //UART Timed Out, so is now finished with BlkDn data
    
    return(0);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT INIT
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Connect this 'join' section to the 'ma_tuds'
//-----------------------------------------------------------------------------
PUBLIC void join_Init(ma_tuds_t *p_ma_tuds)
{
    LOCAL_TMP_p_ma_tuds = p_ma_tuds;
    
    ma_tuds_RegCb_DnEventHandler(p_ma_tuds, join_blkDn_DnEventHandler);
    //p_ma_tuds->blkDn_DnEventHandler = join_blkDn_DnEventHandler;

    ma_tuds_RegCb_UpEventHandler(p_ma_tuds, join_blkUp_UpEventHandler);
    //p_ma_tuds->blkUp_UpEventHandler = join_blkUp_UpEventHandler;
}



