

//#include "app_fifo_uart.h"
#include "app_uart.h"

#include "myapp.h"



//-----------------------------------------------------------------------------
// VARS
be_t *m_curr_beUtx;
volatile be_t *m_curr_beUrx;


#if( CORE_SLAVE == 1 )    
uint8_t buffer_UCs[1024];
uint8_t buffer_CUs[1024];
uint8_t buffer_BC[1024];
uint8_t buffer_CB[1024];

be_t be_UCs;
be_t be_CUs;
be_t be_BC;
be_t be_CB;

#endif

uint8_t buffer_UCm[1024];
uint8_t buffer_CUm[1024];

uint8_t buffer_UB[2048 + 7 + 64];
uint8_t buffer_BU[1024];

uint8_t buffer_Urx[128];
uint8_t buffer_Utx[128];

be_t be_UCm;
be_t be_CUm;
volatile be_t be_UB;
be_t be_BU;
be_t be_Urx;
be_t be_Utx;




//owner_t OwnerNONE;
owner_t OwnerBLE;
owner_t OwnerCoreM;
owner_t OwnerCoreS;

entity_t EntityUartTx;
entity_t EntityCoreS;


//-----------------------------------------------------------------------------
// CODE


//#############################################################################
//#############################################################################
//##### gInit_ ################################################################
//#############################################################################
//#############################################################################
//-----------------------------------------------------------------------------
void be_Init_be(volatile be_t *be, uint8_t * buffer, uint32_t capacity)
{
    be->buffer = buffer;
    be->capacity = capacity;
    be->length = 0;
    be->c0 = 0;
    be->c1 = 0;
    be->pktType = ePkt_Unknown;
}

void gInit_be()
{
#if( CORE_SLAVE == 1 )    
    be_Init_be(&be_UCs, buffer_UCs,sizeof(buffer_UCs));
    be_Init_be(&be_CUs, buffer_CUs,sizeof(buffer_CUs));
    be_Init_be(&be_BC, buffer_BC,sizeof(buffer_BC));
    be_Init_be(&be_CB, buffer_CB,sizeof(buffer_CB));

    be_CUs.vpSemaphore = semaphore_CUs;
    
#endif
    
    be_Init_be(&be_UCm, buffer_UCm,sizeof(buffer_UCm));
    be_Init_be(&be_CUm, buffer_CUm,sizeof(buffer_CUm));
    be_Init_be(&be_UB, buffer_UB,sizeof(buffer_UB));
    be_Init_be(&be_BU, buffer_BU,sizeof(buffer_BU));

    
    be_Init_be(&be_Urx, buffer_Urx,sizeof(buffer_Urx));
    be_Init_be(&be_Utx, buffer_Utx,sizeof(buffer_Utx));
    m_curr_beUrx = &be_Urx;
    m_curr_beUtx = &be_Utx;
}


bool gInit_All(void)
{
    bool bsts = false;
    static bool initDone = false;
    if( initDone )
        return(true);
    initDone = true;
    
   
    gInit_be();
    
    return(bsts);
}


//#############################################################################
//#############################################################################
//##### Entityes ###############################################################
//#############################################################################
//#############################################################################
//============================================================
bool gAquireEntity( owner_t *newOwner, entity_t *p_entity )
{
    if( p_entity->owner == 0)
    {
        p_entity->owner = newOwner;
        return(true);
    }
    return( false );
}

bool gReleaseEntity(owner_t *currentOwner, entity_t *p_entity)
{
dbgPrint("gReleaseEntity\r\n");
    if( currentOwner != p_entity->owner)
    {
dbgPrint("--- 0 XXXXX\r\n");
        return(false);
    }
    
dbgPrint("--- 1\r\n");

    p_entity->owner = 0;

    if(p_entity->wants1 != 0)
    {
dbgPrint("--- 2\r\n");
        p_entity->owner = p_entity->wants1;
        p_entity->wants1 = p_entity->wants2;
        p_entity->wants2 = p_entity->wants3;
        p_entity->wants3 = 0;

        //p_entity->owner->gotTheEntity( p_entity );
    }

    return(true);
}

bool gGoEntity(entity_t *p_entity)
{
    if(p_entity->owner != 0)
    {
        p_entity->owner->gotTheEntity( p_entity );
        return(true);
    }
    return(false);
}

void gWantEntity(owner_t *wanter, entity_t *p_entity)
{
    if( (p_entity->wants1 == wanter) ||
        (p_entity->wants2 == wanter) ||
        (p_entity->wants3 == wanter) 
        )
    {
        return;
    }
dbgPrint("fWantEntity\r\n");
    if(p_entity->wants1 == 0)
    {
        p_entity->wants1 = wanter;
dbgPrint("wants1 = wanter\r\n");
    } else
    if(p_entity->wants2 == 0)
        p_entity->wants2 = wanter;
    else
    if(p_entity->wants3 == 0)
        p_entity->wants3 = wanter;
    
}

void gUnwantEntity( owner_t *wanter, entity_t *p_entity)
{
//    owner_t *tmp;

    if(p_entity->wants1 == wanter){
        p_entity->wants1 = p_entity->wants2;
        p_entity->wants2 = p_entity->wants3;
        p_entity->wants3 = 0;
    }
    else
    if(p_entity->wants2 == wanter){
        p_entity->wants2 = p_entity->wants3;
        p_entity->wants3 = 0;
    }
    else
    if(p_entity->wants3 == wanter){
        p_entity->wants3 = 0;
    }
}







void Fake_bleEvent_onReqReceived(void)
{
}


bool bleMaster_StartRsp( be_t *pbe ) // be_UB or be_CsB
{
    return(true);
}
void Fake_bleEvent_onRspSent(void)
{
    uniEvent_t LEvt;
    LEvt.evtType = evt_bleMaster_BleRspDone;
    core_thread_QueueSend(&LEvt);
}

// This is called on gReleaseEntity( currentOwner, p_entity )
void bleMaster_gotTheEntity(entity_t *p_entity);
void coreMaster_gotTheEntity(entity_t *p_entity);

void gInit_Owners()
{
    OwnerBLE.gotTheEntity = (gotTheEntity_t)bleMaster_gotTheEntity;

    //OwnerCoreADC.gotTheEntity = C_Adc_gotTheEntity;

    OwnerCoreM.gotTheEntity = coreMaster_gotTheEntity;
}


void gInit_Entity( entity_t *p_entity)
{
    p_entity->i = 0;
    p_entity->owner = 0;
    p_entity->wants1 = 0;
    p_entity->wants2 = 0;
    p_entity->wants3 = 0;
}
void gInit_Entityes()
{
    gInit_Entity( &EntityUartTx );
    gInit_Entity( &EntityCoreS );
}




//#############################################################################
//#############################################################################
//##### State MAchines ? ######################################################
//#############################################################################
//#############################################################################

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// CUs State Machine
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if( CORE_SLAVE == 1 )


void be_UCs_Process_Req(void)
{
    if( be_UCs.buffer[1] == 0x42)
    {
        //C_CoreS_Process_0x42_Req();
    }
    else
    if( be_UCs.buffer[1] == 0x43)
    {
        //C_CoreS_Process_0x43_Req();
    }
    else
    {
        //TODO unhandled packet
    }        
    
}

#endif //( CORE_SLAVE == 1 )  


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// CUm State Machine
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    
typedef struct coreProcBlock_s
{
    bool pending;
    bool processing;
    int (*make_req)( be_t *be_Req );
    int (*proc_rsp)( be_t *be_Req,  be_t *be_Rsp );
    int (*proc_timeout)( be_t *be_Req,  be_t *be_Rsp );
    
} coreProcBlock_t;

coreProcBlock_t cpb_ADC;

coreProcBlock_t cpb_BLE;


int make_req_ADC( be_t *be_Req )
{
    return(0);
}
int proc_rsp_ADC( be_t *be_Req,  be_t *be_Rsp )
{
    dbgPrint("proc_rsp_ADC\r\n");
    return(0);
}
int proc_timeout_ADC( be_t *be_Req,  be_t *be_Rsp )
{
    dbgPrint("proc_timeout_ADC\r\n");
    return(0);
}











int proc01_9E_extractName(be_t *be, char * name);
int proc01_9E_makeCmd( be_t *be_Req );
int proc01_9E_proccessRsp( be_t *be_Req,  be_t *be_Rsp );

int proc01_9E_01_makeCmd( be_t *be_Req );
int proc01_9E_01_proccessRsp( be_t *be_Req,  be_t *be_Rsp );

coreProcBlock_t cpb_GBN;

int make_req_GBN( be_t *be_Req )
{
    proc01_9E_makeCmd(be_Req);
    return(0);
}
int proc_rsp_GBN( be_t *be_Req,  be_t *be_Rsp )
{
    proc01_9E_proccessRsp( be_Req,  be_Rsp );
    dbgPrint("proc_rsp_GBN\r\n");
    return(0);
}

int proc_timeout_GBN( be_t *be_Req,  be_t *be_Rsp )
{
    dbgPrint("proc_timeout_GBN\r\n");
    return(0);
}

int make_req_GBN_01( be_t *be_Req )
{
    proc01_9E_01_makeCmd(be_Req);
    return(0);
}
int proc_rsp_GBN_01( be_t *be_Req,  be_t *be_Rsp )
{
    proc01_9E_01_proccessRsp( be_Req,  be_Rsp );
    dbgPrint("proc_rsp_GBN_01\r\n");
    return(0);
}

int proc_timeout_GBN_01( be_t *be_Req,  be_t *be_Rsp )
{
    dbgPrint("proc_timeout_GBN_01\r\n");
    return(0);
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// BU State Machine
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include "block_proc.h"

int make_req_BLE( be_t *be_Req )
{
    uint16_t i;
    
    be_BU.rdPtr = 0;
    be_BU.wrPtr = m_blkDn_len - 2;
    be_BU.length = m_blkDn_len - 2;
    for( i=0 ; i<be_BU.length; i++)
    {
        be_BU.buffer[i] = m_blkDn_buf[i];
    }
    return(0);
}


int proc_rsp_BLE( be_t *be_Req,  be_t *be_Rsp )
{
    dbgPrint("proc_rsp_BLE\r\n");
    callThisWhenUartPacketForBleIsRecieved(); //TODO  BlkUp_Go( m_curr_beUrx->buffer, m_curr_beUrx->length); //be_UB
    return(0);
}

int proc_timeout_BLE( be_t *be_Req,  be_t *be_Rsp )
{
    dbgPrint("proc_timeout_BLE\r\n");
    return(0);
}




//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================


//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
//===== Indicate_BufSend_Started =====
//
#if( CORE_SLAVE == 1 )
void be_CUs_Indicate_BufSend_Started() { }
#endif
void be_BU_Indicate_BufSend_Started() { }
void be_CUm_Indicate_BufSend_Started() { }

void be_Cxx_Indicate_BufSend_Started()
{
    // m_curr_beUtx->vIndicateUse_Start(m_curr_beUtx);
#if( CORE_SLAVE == 1 )
    if( m_curr_beUtx == &be_CUs ) be_CUs_Indicate_BufSend_Started();
#endif
    if( m_curr_beUtx == &be_CUm ) be_CUm_Indicate_BufSend_Started();
    if( m_curr_beUtx == &be_BU  ) be_BU_Indicate_BufSend_Started();
}


//-----------------------------------------------------------------------------
//===== Indicate_BufSend_Done =====
//
#if( CORE_SLAVE == 1 )
void be_CUs_Indicate_BufSend_Done()
{

#endif
void be_BU_Indicate_BufSend_Done()
{
    uniEvent_t LEvt;
    if( EntityUartTx.owner != 0)
    {
        if( EntityUartTx.owner == &OwnerBLE)
        {                
            LEvt.evtType = evt_bleMaster_UartTxDone;
            core_thread_QueueSend(&LEvt);
        }
    }
}

void be_CUm_Indicate_BufSend_Done()
{
    uniEvent_t LEvt;

    if( EntityUartTx.owner != 0)
    {
        if( EntityUartTx.owner == &OwnerCoreM)
        {                
            LEvt.evtType = evt_coreMaster_UartTxDone;
            core_thread_QueueSend(&LEvt);
        }
    }
}

void be_Cxx_Indicate_BufSend_Done()
{
    //m_curr_beUtx->vIndicateUse_End(m_curr_beUtx);
#if( CORE_SLAVE == 1 )
    if( m_curr_beUtx == &be_CUs ) be_CUs_Indicate_BufSend_Done();
#endif
    if( m_curr_beUtx == &be_CUm ) be_CUm_Indicate_BufSend_Done();
    if( m_curr_beUtx == &be_BU  ) be_BU_Indicate_BufSend_Done();
}




//-----------------------------------------------------------------------------
//===== IndicateStartRecv =====
//
#if( CORE_SLAVE == 1 )
void be_UCs_IndicateStartRecv(void)
{
}
#endif
void be_UCm_IndicateStartRecv(void)
{
}
void be_UB_IndicateStartRecv(void)
{
}

//-----------------------------------------------------------------------------
//===== IndicateDoneRecv =====
//
#if( CORE_SLAVE == 1 )
void be_UCs_IndicateDoneRecv(void)
{
    uniEvent_t LEvt;
    LEvt.evtType = evt_core_UCs_Req;
    core_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
}
#endif
void be_UCm_IndicateDoneRecv(void)
{
    uniEvent_t LEvt;
    LEvt.evtType = evt_coreMaster_UartRxDone;
    core_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
}
void be_UB_IndicateDoneRecv(void)
{
    uniEvent_t LEvt;
    LEvt.evtType = evt_bleMaster_UartRxDone;
    core_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
}

//void be_Cxx_IndicateDoneRecv(void)
//{
//}













//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
void core_Init(void)
{
     //gInit_be();
}




void TEST_be_CUm_9E_1(void) //FFAA
{
    proc01_9E_makeCmd(&be_CUm);
    //be_CUm.buffer[0] = 0x01;
    //be_CUm.buffer[1] = 0x9E;
    //be_CUm.buffer[2] = 0x00;
    //be_CUm.buffer[3] = 0;
    //be_CUm.buffer[4] = 0;
    //be_CUm.buffer[5] = 0x9F;
    //be_CUm.buffer[6] = 0x00;
    //be_CUm.rdPtr = 0;
    //be_CUm.wrPtr = 7;
    //be_CUm.length = 7;
}


void startNextCoreEntityGrab(void)
{
    bool  bsts;

    bsts = gAquireEntity( &OwnerCoreM, &EntityUartTx );
    if( bsts )
    {
        OwnerCoreM.gotTheEntity(&EntityUartTx); //coreMaster_gotTheEntity
    }
    else
    {
        gWantEntity( &OwnerCoreM, &EntityUartTx);
        dbgPrint("gAquireEntity FAILED\r\n");
    }

}

void bleMaster_EntityUartTx_BITCHPROC(void)
{    
    //if( p_entity == &EntityUartTx )
    {
        if( cpb_BLE.pending == true) 
        {
            if( cpb_BLE.processing == true)
                return;
            cpb_BLE.processing = true;
            
            cpb_BLE.make_req( &be_BU ); //FFAA    

            // - debug padding on RX buffer -
            be_UB.rdPtr = 0;
            be_UB.wrPtr = 0;
            be_UB.length = 0;
            be_UB.buffer[0] = 0xcc;
            be_UB.buffer[1] = 0xcc;
            be_UB.buffer[2] = 0xcc;
            be_UB.buffer[3] = 0xcc;
            be_UB.buffer[4] = 0xcc;
            be_UB.buffer[5] = 0xcc;
            // - debug padding on RX buffer -
            
            m_curr_beUtx = &be_BU;

            ma_uart_Init();
            Uart_timer_stop(); //called from     bleMaster_gotTheEntity
            Uart_PacketTx_Start(); //called from     bleMaster_gotTheEntity
        }
    }
}




void coreMaster_EntityUartTx_BITCHPROC(void)
{
    //if( p_entity == &EntityUartTx )
    {
        if( cpb_ADC.pending == true) 
        {
            if( cpb_ADC.processing == true)
                return;
            cpb_ADC.processing = true;
            
            cpb_ADC.make_req( &be_CUm ); //FFAA    

            TEST_be_CUm_9E_1(); // Testing Override

            m_curr_beUtx = &be_CUm;

            ma_uart_Init(); //?!?!?!
            Uart_timer_stop();  //?!?!?! //called from     coreMaster_gotTheEntity
            Uart_PacketTx_Start(); //void coreMaster_gotTheEntity(entity_t *p_entity)
        }
        else
        if( cpb_GBN.pending == true) 
        {
            if( cpb_GBN.processing == true)
                return;
            cpb_GBN.processing = true;
            

            cpb_GBN.make_req( &be_CUm );
            m_curr_beUtx = &be_CUm;


            ma_uart_Init(); //?!?!?!
            Uart_timer_stop(); //?!?!?! //called from     coreMaster_gotTheEntity
            Uart_PacketTx_Start(); //void coreMaster_gotTheEntity(entity_t *p_entity)
        }
            

    }
}

void bleMaster_gotTheEntity(entity_t *p_entity)
{
    uniEvent_t LEvt;
    dbgPrint("bleMaster_gotTheEntity\r\n");
    //uniEvent_t LEvt;
    
    if( p_entity == &EntityUartTx )
    {
#if DIRECT_BITCH
        bleMaster_EntityUartTx_BITCHPROC();
#else
        LEvt.evtType = evt_bleMaster_startBle_proc;
        core_thread_QueueSend(&LEvt);
#endif
    }

#if( CORE_SLAVE == 1 )    
    if( p_entity == &EntityCoreS )
    {
        LEvt.evtType = evt_bleMaster_startCoreS_proc;
        core_thread_QueueSend(&LEvt);
    }
#endif
}

void coreMaster_gotTheEntity(entity_t *p_entity)
{
    uniEvent_t LEvt;
    dbgPrint("coreMaster_gotTheEntity\r\n");
    //uniEvent_t LEvt;
    //dbgPrint("1\r\n");

    if( p_entity == &EntityUartTx )
    {
#if DIRECT_BITCH
        coreMaster_EntityUartTx_BITCHPROC();
#else
        LEvt.evtType = evt_coreMaster_startCoreM_proc;
        core_thread_QueueSend(&LEvt);
#endif
    }
}






void FAKE_debugSetbleMasterResponse(void);




//#include "priority_scheduler.h"
#include "app_scheduler.h"

void core_thread_init()
{
   
    core_Init();

    //gInit_XXX();
    gInit_Owners();
    gInit_Entityes();
    cpb_ADC.pending = false;
    cpb_ADC.processing = false;

    //dbgPrint("core_thread : Running ... ... ... ...\r\n");
}


typedef enum eMpState
{
    eMpState_UNKNOWN,
    eMpState_WAKE_STARTED,
    eMpState_AWAKE,
    eMpState_SLEEP_STARTED,
    eMpState_ASLEEP,
} eMpState_t;

eMpState_t m_eMpState = eMpState_UNKNOWN; //eMpState_ASLEEP;

bool m_pinWakeUp_IsAsserted = false;

bool m_reTrigger_bleMaster_trigger = false;
bool m_reTrigger_core_GBN_trigger = false;
bool m_reTrigger_core_GBN_trigger_01 = false;
bool m_reTrigger_core_ADC_trigger = false;

static void core_thread_process_event(uniEvent_t *pEvt)
{
    bool bsts;
    uniEvent_t LEvt;
    bool localFlag = false;

    sprintf(m_s1,  "####  pEvt->evtType = %d\r\n", pEvt->evtType);
    dbgPrint(m_s1);
    
    switch( pEvt->evtType )
    {
        
    //-----------------------------------------------------
    //----- ConState -----
    case evt_ConState_Connected:
        m_bleIsConnected = true;
        break;

    case evt_ConState_Disconnected:

        m_bleIsConnected = false;
    
        /// if( m_bleIsConnected == false )
        /// {
        ///      LEvt.evtType = evt_MpState_ReleaseWakeup;
        ///      core_thread_QueueSend(&LEvt);
        /// }

        if( m_bleIsConnected == false )
        {
            LEvt.evtType = evt_core_GBN_trigger_00;
            core_thread_QueueSend(&LEvt);
        }
        
    /*
        //gap_device_name_set_again(bogusString);
        m_UpdatedAdvertisingName[0] = 'N';
        m_UpdatedAdvertisingName[1] = 'a';
        m_UpdatedAdvertisingName[2] = 'm';
        m_UpdatedAdvertisingName[3] = 'E';
        if( m_UpdatedAdvertisingName[4] == '0')
            m_UpdatedAdvertisingName[4] = '1';
        else
            m_UpdatedAdvertisingName[4] = '0';    

        m_UpdatedAdvertisingName[5] = 0;
        
        //strcpy(m_UpdatedAdvertisingName,"iuyuih");
     
        //m_doUpdateAdvertisingName = true;    
        m_doUpdateAdvertisingName = false;    
        gap_device_name_set_again(m_UpdatedAdvertisingName);
        advertising_init();
    */
        break;
    
    
    case UnUsed_evt_MpState_DriveWakeup_Start:
        /// pinWakeUp_Init();
        /// pinWakeUp_Assert(); m_pinWakeUp_IsAsserted = true;
        /// m_eMpState = eMpState_WAKE_STARTED;

        /// pinWkUp_Delay_timer_start( APP_TIMER_TICKS( 100, APP_TIMER_PRESCALER) );
        /// //pinWkUp_On_timer_start( APP_TIMER_TICKS( 12345, APP_TIMER_PRESCALER) );

        break;
    
    case evt_MpState_DriveWakeup_Done:
        m_eMpState = eMpState_AWAKE;
    
        //----- restart any pending requests ---
        if( m_reTrigger_bleMaster_trigger == true)
        {
            m_reTrigger_bleMaster_trigger = false;
            LEvt.evtType = evt_bleMaster_trigger;
            core_thread_QueueSend(&LEvt);
        }
        //----- restart any pending requests ---
        if( m_reTrigger_core_GBN_trigger == true)
        {
            m_reTrigger_core_GBN_trigger = false;
            LEvt.evtType = evt_core_GBN_trigger_00;
            core_thread_QueueSend(&LEvt);
        }
        //----- restart any pending requests ---
        if( m_reTrigger_core_GBN_trigger_01 == true)
        {
            m_reTrigger_core_GBN_trigger_01 = false;
            LEvt.evtType = evt_core_GBN_trigger_01;
            core_thread_QueueSend(&LEvt);
        }


        
        //----- restart any pending requests ---
        if( m_reTrigger_core_ADC_trigger == true)
        {
            m_reTrigger_core_ADC_trigger = false;
            LEvt.evtType = evt_core_ADC_trigger;
            core_thread_QueueSend(&LEvt);
        }
        
        break;
    
    
    case evt_MpState_ReleaseWakeup:
        m_eMpState = eMpState_SLEEP_STARTED;
        pinWakeUp_Release(); m_pinWakeUp_IsAsserted = false;
        pinWakeUp_Deinit();
        m_eMpState = eMpState_ASLEEP;
        break;
        
  

    //-----------------------------------------------------
    //----- Triggers ----- bleMaster -----
    case evt_bleMaster_trigger: //dbgPrint("evt_bleMaster_trigger\r\n");

        //----- if hardware not awake, reschedule on hardware wakeup -----
        if( m_eMpState != eMpState_AWAKE )
        {
            m_reTrigger_bleMaster_trigger = true;
            if( !m_pinWakeUp_IsAsserted )//!pinWakeUp_IsAsserted() )  //TODO which?
            {                
                pinWakeUp_Init();
                pinWakeUp_Assert();
                m_pinWakeUp_IsAsserted = true;
#if USE_OLD_PWU 
                pinWkUp_Delay_timer_start( APP_TIMER_TICKS( PINWAKEUP_TIMER_DELAY_MS, APP_TIMER_PRESCALER) );  //mg_3_wkUpDelayX1ms
#else
                pinWkUp_Delay_timer_start( APP_TIMER_TICKS( mg_3_wkUpDelayX1ms, APP_TIMER_PRESCALER) );  //
                if( mg_2_wkUpPerX50ms != 0)
                    pinWkUp_On_timer_start( APP_TIMER_TICKS( 50 * mg_2_wkUpPerX50ms, APP_TIMER_PRESCALER) );
#endif
            }
            break;
        }
    
        //----- if operation is already pending, then fall out -----
        if( cpb_BLE.pending == true) 
            break; // no next event set?!?!
        
        cpb_BLE.pending = true;
        cpb_BLE.processing = false;
        cpb_BLE.make_req = make_req_BLE;
        cpb_BLE.proc_rsp = proc_rsp_BLE;
        cpb_BLE.proc_timeout = proc_timeout_BLE;

        bsts = gAquireEntity( &OwnerBLE, &EntityUartTx ); //case evt_bleMaster_trigger:
        if( bsts )
        {
            OwnerBLE.gotTheEntity(&EntityUartTx); //bleMaster_gotTheEntity
        }
        else
        {
            gWantEntity( &OwnerBLE, &EntityUartTx);
        }
        break;
                
#if DIRECT_BITCH
#else
    case evt_bleMaster_startBle_proc:
        bleMaster_EntityUartTx_BITCHPROC();
        break;
    case evt_coreMaster_startCoreM_proc:
        coreMaster_EntityUartTx_BITCHPROC();
        break;
#endif

    //----- Triggers ----- core_ADC -----
    case evt_core_ADC_trigger: //dbgPrint("evt_core_ADC_trigger\r\n");

        //----- if hardware not awake, reschedule on hardware wakeup -----
        if( m_eMpState != eMpState_AWAKE )
        {
            m_reTrigger_core_ADC_trigger = true;
            if( !m_pinWakeUp_IsAsserted )//!pinWakeUp_IsAsserted() )  //TODO which?
            {
                pinWakeUp_Init();
                pinWakeUp_Assert(); m_pinWakeUp_IsAsserted = true;
#if USE_OLD_PWU 
                pinWkUp_Delay_timer_start( APP_TIMER_TICKS( PINWAKEUP_TIMER_DELAY_MS, APP_TIMER_PRESCALER) );  //mg_3_wkUpDelayX1ms
#else
                pinWkUp_Delay_timer_start( APP_TIMER_TICKS( mg_3_wkUpDelayX1ms, APP_TIMER_PRESCALER) );  //
                if( mg_2_wkUpPerX50ms != 0)
                    pinWkUp_On_timer_start( APP_TIMER_TICKS( 50 * mg_2_wkUpPerX50ms, APP_TIMER_PRESCALER) );
#endif
            }
            break;
        }

        //----- if operation is already pending, then fall out -----
        if( cpb_ADC.pending == true) 
            break;
        
        cpb_ADC.pending = true;
        cpb_ADC.processing = false;
        cpb_ADC.make_req = make_req_ADC;
        cpb_ADC.proc_rsp = proc_rsp_ADC;
        cpb_ADC.proc_timeout = proc_timeout_ADC;


        startNextCoreEntityGrab();
        break;
                
    //----- Triggers ----- core_GBN -----
    case evt_core_GBN_trigger_00: //dbgPrint("evt_core_GBN\r\n");

        if( !ma_gbn_isEnabled() )
        {
            break;
        }
        
        dbgPrint("---------- evt_core_GBN ----------\r\n");
        //----- if hardware not awake, reschedule on hardware wakeup -----
        if( m_eMpState != eMpState_AWAKE )
        {
dbgPrint("        && 0\r\n");
            m_reTrigger_core_GBN_trigger = true;
            if( !m_pinWakeUp_IsAsserted )//!pinWakeUp_IsAsserted() )  //TODO which?
            {
dbgPrint("        && 1\r\n");
                pinWakeUp_Init();
                pinWakeUp_Assert(); m_pinWakeUp_IsAsserted = true;
#if USE_OLD_PWU 
                pinWkUp_Delay_timer_start( APP_TIMER_TICKS( PINWAKEUP_TIMER_DELAY_MS, APP_TIMER_PRESCALER) );  //mg_3_wkUpDelayX1ms
#else
                pinWkUp_Delay_timer_start( APP_TIMER_TICKS( mg_3_wkUpDelayX1ms, APP_TIMER_PRESCALER) );  //
                if( mg_2_wkUpPerX50ms != 0)
                    pinWkUp_On_timer_start( APP_TIMER_TICKS( 50 * mg_2_wkUpPerX50ms, APP_TIMER_PRESCALER) );
#endif
            }
            break;
        }
        
        dbgPrint("        %% 0\r\n");
        //----- if operation is already pending, then fall out -----
        if( cpb_GBN.pending == true) 
            break;

        dbgPrint("        %% 12\r\n");

        cpb_GBN.pending = true;
        cpb_GBN.processing = false;
        cpb_GBN.make_req = make_req_GBN;
        cpb_GBN.proc_rsp = proc_rsp_GBN;
        cpb_GBN.proc_timeout = proc_timeout_GBN;
        dbgPrint("        %% 3\r\n");
        
        startNextCoreEntityGrab();
        dbgPrint("        %% 4\r\n");
        break;

    case evt_core_GBN_trigger_01: //dbgPrint("evt_core_GBN_trigger_01\r\n");

        if( !ma_gbn_isEnabled() )
        {
            break;
        }
        
        dbgPrint("---------- evt_core_GBN_trigger_01 ----------\r\n");
        //----- if hardware not awake, reschedule on hardware wakeup -----
        if( m_eMpState != eMpState_AWAKE )
        {
dbgPrint("        && 0\r\n");
            m_reTrigger_core_GBN_trigger_01 = true;
            if( !m_pinWakeUp_IsAsserted )//!pinWakeUp_IsAsserted() )  //TODO which?
            {
dbgPrint("        && 1\r\n");
                pinWakeUp_Init();
                pinWakeUp_Assert(); m_pinWakeUp_IsAsserted = true;
#if USE_OLD_PWU 
                pinWkUp_Delay_timer_start( APP_TIMER_TICKS( PINWAKEUP_TIMER_DELAY_MS, APP_TIMER_PRESCALER) );  //mg_3_wkUpDelayX1ms
#else
                pinWkUp_Delay_timer_start( APP_TIMER_TICKS( mg_3_wkUpDelayX1ms, APP_TIMER_PRESCALER) );  //
                if( mg_2_wkUpPerX50ms != 0)
                    pinWkUp_On_timer_start( APP_TIMER_TICKS( 50 * mg_2_wkUpPerX50ms, APP_TIMER_PRESCALER) );
#endif
            }
            break;
        }
        
        dbgPrint("        %% 0\r\n");
        //----- if operation is already pending, then fall out -----
        if( cpb_GBN.pending == true) 
            break;

        dbgPrint("        %% 12\r\n");

        cpb_GBN.pending = true;
        cpb_GBN.processing = false;
        cpb_GBN.make_req = make_req_GBN_01;
        cpb_GBN.proc_rsp = proc_rsp_GBN_01;
        cpb_GBN.proc_timeout = proc_timeout_GBN_01;
        dbgPrint("        %% 3\r\n");
        
        startNextCoreEntityGrab();
        dbgPrint("        %% 4\r\n");
        break;



    //-----------------------------------------------------
    // This is called after Uart has finished sending TxData
    case evt_bleMaster_UartTxDone:
    case evt_coreMaster_UartTxDone:
        //dbgPrint("UartTxDone\r\n");

        //----- cancel the uart timer (for TxTimeout) -----
        Uart_timer_stop(); //called from     case evt_bleMaster_UartTxDone, evt_coreMaster_UartTxDone

        //----- restart the uart timer (for RxTimeout) -----
        ma_Uart_timer_Reason = Reason_rxTimeout;  //Reason_none, Reason_rxTimeout, Reason_shutdown,
        Uart_timer_start( APP_TIMER_TICKS(500, APP_TIMER_PRESCALER) );
        break;
 

    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    case evt_xxxMaster_UartRxTimeout:

        Uart_timer_stop(); //?!? timer should be stopped if we get here?!? called from     case evt_xxxMaster_UartRxTimeout:  EntityUartTx.owner == &OwnerBLE
    
        if( EntityUartTx.owner == &OwnerCoreM)
        {
            if( (cpb_ADC.pending == true) && (cpb_ADC.processing == true) )
            {
                cpb_ADC.proc_timeout( &be_CUm, &be_UCm );
                cpb_ADC.pending = false;
                cpb_ADC.processing = false;
            }
            else
                if( (cpb_GBN.pending == true) && (cpb_GBN.processing == true) )
                {
                    cpb_GBN.proc_timeout( &be_CUm, &be_UCm );
                    cpb_GBN.pending = false;
                    cpb_GBN.processing = false;
                    if( m_bleIsConnected == false)
                    {
                        GBN_timer_start( APP_TIMER_TICKS( GBN_TIMER_WAIT_PERIOD_MS, APP_TIMER_PRESCALER) );
                    }
                }                    

            gReleaseEntity( &OwnerCoreM, &EntityUartTx );
            if( EntityUartTx.owner != 0 ) //we have a pending owner
            {
                gGoEntity( &EntityUartTx );
            }
            else
            {
                // Shutdown Uart
                ma_Uart_timer_Reason = Reason_shutdown;
                Uart_timer_start( APP_TIMER_TICKS( 5,  APP_TIMER_PRESCALER) );
                
                if( m_bleIsConnected == false )
                {
                    LEvt.evtType = evt_MpState_ReleaseWakeup;
                    core_thread_QueueSend(&LEvt);
                }
            }               
        }


        if( EntityUartTx.owner == &OwnerBLE)
        {

#if 0 //This is For Debug

            set_curr_beUrx_toStartPosition();

            sprintf(m_s1, "TO:  UB_len = %d \r\n", be_UB.length);  dbgPrint( m_s1 );
            sprintf(m_s1, "TO:  UB[0..3] = %02x %02x %02x %02x \r\n", be_UB.buffer[0], be_UB.buffer[1], be_UB.buffer[2], be_UB.buffer[3]);  dbgPrint( m_s1 );
            sprintf(m_s1, "TO:  UB[4..7] = %02x %02x %02x %02x \r\n", be_UB.buffer[4], be_UB.buffer[5], be_UB.buffer[6], be_UB.buffer[7]);  dbgPrint( m_s1 );
/*
            be_UB.buffer[0] = 0x01;
            be_UB.buffer[1] = 0x9F;
            be_UB.buffer[2] = 0x15;
            be_UB.buffer[3] = 0x00;
            be_UB.buffer[4] = 0x00;
            be_UB.buffer[5] = 0xB5;
            be_UB.buffer[6] = 0x00;
            be_UB.rdPtr = 0;
            be_UB.wrPtr = 7;
            be_UB.length = 7;            
*/
            
/**/
            be_UB.buffer[0] = 0x01;
            be_UB.buffer[1] = 0x9F;
            be_UB.buffer[2] = 0x15;
            be_UB.buffer[3] = 0x01;
            be_UB.buffer[4] = 0x00;
            be_UB.buffer[5] = 0x00;
            be_UB.buffer[6] = 0xB6;
            be_UB.buffer[7] = 0x00;
            be_UB.rdPtr = 0;
            be_UB.wrPtr = 8;
            be_UB.length = 8;            
/**/
            //bleMaster_process_rsp();  //TODO  BlkUp_Go( m_curr_beUrx->buffer, m_curr_beUrx->length); //be_UB            
            if( (cpb_BLE.pending == true) && (cpb_BLE.processing == true) )
            {
                cpb_BLE.proc_rsp( &be_BU, &be_UB ); //proc_rsp_BLE
                cpb_BLE.pending = false;
                cpb_BLE.processing = false;
            }

#else // This is for normal processing
            
            if( (cpb_BLE.pending == true) && (cpb_BLE.processing == true) )
            {
                cpb_BLE.proc_timeout( &be_BU, &be_UB );
                cpb_BLE.pending = false;
                cpb_BLE.processing = false;
            }                    

#endif // for debug

            
            gReleaseEntity( &OwnerBLE, &EntityUartTx );
            if( EntityUartTx.owner != 0 ) //we have a pending owner
            {
                gGoEntity( &EntityUartTx );
            }
            else
            {
                // Shutdown Uart
                ma_Uart_timer_Reason = Reason_shutdown;
                Uart_timer_start( APP_TIMER_TICKS( 5,  APP_TIMER_PRESCALER) );

                if( m_bleIsConnected == false )
                {
                    LEvt.evtType = evt_MpState_ReleaseWakeup;
                    core_thread_QueueSend(&LEvt);
                }
            }               
            
        }

        break;
     

    //-----------------------------------------------------------------
    //----- coreMaster Response Processing -----
    case evt_coreMaster_UartRxDone:
        Uart_timer_stop(); //called from     case evt_coreMaster_UartRxDone
        
        //TODO - sunabaInPlace0x010x9fPacketRspExtraction( &be_UB );
    
        if( (cpb_ADC.pending == true) && (cpb_ADC.processing == true) )
        {
            cpb_ADC.proc_rsp( &be_CUm, &be_UCm ); //proc_rsp_ADC
            cpb_ADC.pending = false;
            cpb_ADC.processing = false;
        }
        else
            if( (cpb_GBN.pending == true) && (cpb_GBN.processing == true) )
            {
                cpb_GBN.proc_rsp( &be_CUm, &be_UCm );
                cpb_GBN.pending = false;
                cpb_GBN.processing = false;
                
                if( cpb_GBN.make_req == make_req_GBN )        // New Function 9E_01
                {                                             // New Function 9E_01
                    LEvt.evtType = evt_core_GBN_trigger_01;   // New Function 9E_01
                    core_thread_QueueSend(&LEvt);             // New Function 9E_01
                    localFlag = true;                         // New Function 9E_01
                }                                             // New Function 9E_01
            }
            
        gReleaseEntity( &OwnerCoreM, &EntityUartTx );
        if( EntityUartTx.owner != 0 ) //we have a pending owner
        {
            gGoEntity( &EntityUartTx );
        }
        else
        {

            if( localFlag == true ) break;                    // New Function 9E_01
            // Shutdown Uart
            ma_Uart_timer_Reason = Reason_shutdown;
            Uart_timer_start( APP_TIMER_TICKS( 5,  APP_TIMER_PRESCALER) );

            if( m_bleIsConnected == false )
            {
                LEvt.evtType = evt_MpState_ReleaseWakeup;
                core_thread_QueueSend(&LEvt);
            }

        }
        break;
            
            
            
    //-----------------------------------------------------------------
    //----- bleMaster Response Processing -----
    case evt_bleMaster_UartRxDone: //dbgPrint("evt_bleMaster_UartRxDone\r\n");
    
        Uart_timer_stop(); //called from     case evt_bleMaster_UartRxDone
    
        if( (cpb_BLE.pending == true) && (cpb_BLE.processing == true) )
        {
            cpb_BLE.proc_rsp( &be_BU, &be_UB ); //proc_rsp_BLE
            cpb_BLE.pending = false;
            cpb_BLE.processing = false;
        }

        gReleaseEntity( &OwnerBLE, &EntityUartTx );
        if( EntityUartTx.owner != 0 ) //we have a pending owner
        {
            gGoEntity( &EntityUartTx );
        }
        else
        {
            // Shutdown Uart
            ma_Uart_timer_Reason = Reason_shutdown;
            Uart_timer_start( APP_TIMER_TICKS( 5,  APP_TIMER_PRESCALER) );

            if( m_bleIsConnected == false )
            {
                LEvt.evtType = evt_MpState_ReleaseWakeup;
                core_thread_QueueSend(&LEvt);
            }
        }
        break;


    //-----------------------------------------------------------------
    //----- Slave Request Processing -----
#if( CORE_SLAVE == 1 )
    case evt_core_UCs_Req:
        be_UCs_Process_Req();
        break;
#endif


    }
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
static void core_thread_proccess_queue_item(void * p_event_data, uint16_t event_size)
{
    //dbgPrint("PriorityThread -> ");
    //uniEvent_t *pE = (uniEvent_t *)p_event_data ;
    //sprintf( m_s1, "Event i = %d\r\n", pE->i );
    //dbgPrint( m_s1 );
    
    core_thread_process_event((uniEvent_t *)p_event_data);
}

void core_thread_QueueSend(uniEvent_t *pEvt)
{
    app_sched_event_put(pEvt, sizeof(uniEvent_t), core_thread_proccess_queue_item);
    //priority_sched_event_put(pEvt, sizeof(uniEvent_t), core_thread_proccess_queue_item);
}