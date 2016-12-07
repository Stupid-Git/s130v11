
#include "myapp.h" // #include "nrf_soc.h" -> NRF_APP_PRIORITY_HIGH
#include "nrf_adc.h"
#include "nrf_delay.h"

#include "ma_adc.h"


volatile int32_t adc_sample;


void pinsBatt_Init(void);
void pinsBatt_ON_Assert(void);
void pinsBatt_LD_Assert(void);
void pinsBatt_ON_Release(void);
void pinsBatt_LD_Release(void);


//-------------------------------------
static void BattPins_ON_LO(void)
{  
    pinsBatt_Init();
    pinsBatt_ON_Assert();
}

static void BattPins_ON_HI(void)
{  
    //pinsBatt_Init();
    //pinsBatt_ON_Assert();
    pinsBatt_LD_Assert();
}
static void BattPins_OFF(void)
{
    pinsBatt_ON_Release();
    pinsBatt_LD_Release();
}

#if USE_V11
#include "app_util_platform.h"
#include "nrf_drv_config.h"
#endif

void ma_adc_config(void)
{
    const nrf_adc_config_t nrf_adc_config = NRF_ADC_CONFIG_DEFAULT;

    // Initialize and configure ADC
    nrf_adc_configure( (nrf_adc_config_t *)&nrf_adc_config);
    nrf_adc_input_select(NRF_ADC_CONFIG_INPUT_2);
    nrf_adc_int_enable(ADC_INTENSET_END_Enabled << ADC_INTENSET_END_Pos);
#if USE_V11
    NVIC_SetPriority(ADC_IRQn, APP_IRQ_PRIORITY_HIGH);
#else
    NVIC_SetPriority(ADC_IRQn, NRF_APP_PRIORITY_HIGH);
#endif
    NVIC_EnableIRQ(ADC_IRQn);
   
}
/*
    See also Zip file attached to
        https://devzone.nordicsemi.com/question/1771/high-sample-rate-with-adc-and-softdevice/
*/


void ADC_IRQHandler(void)
{
    nrf_adc_conversion_event_clean();

    adc_sample = nrf_adc_result_get();

    adc_proc(ADC_PROC_ADC_TRIGGER);    
    adc_proc_PRE(ADC_PROC_ADC_TRIGGER);    
}

/**/
typedef enum 
{
    ADC_COUNTING,
    ADC_PARKED,   
    ADC_LOADSEQUENCE,
} adc_state_t;
static adc_state_t adc_sm = ADC_PARKED;

typedef enum 
{
    ADCADC_ON,
    ADCADC_S0,
    ADCADC_S1,
    ADCADC_S2,
    ADCADC_S3,
} adcADC_state_t;
static adcADC_state_t adcADC_sm = ADCADC_S0;
/**/


//PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE 
//PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE 
bool g_PRE = true;
typedef enum 
{
    ADC_COUNTING_PRE,
    ADC_PARKED_PRE,   
    //ADC_LOADSEQUENCE_PRE,
} adc_state_PRE_t;
static adc_state_PRE_t adc_sm_PRE = ADC_PARKED_PRE;

typedef enum 
{
    ADCADC_S0_PRE,
    ADCADC_S1_PRE,
//    ADCADC_S2_PRE,
    ADCADC_S3_PRE,
} adcADC_state_PRE_t;
static adcADC_state_PRE_t adcADC_sm_PRE = ADCADC_S0_PRE;
//PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE 
//PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE PRE 






//mg_9_ADC_rate = be_Rsp->buffer[9];
//mg_10_loadADC = be_Rsp->buffer[10];
static uint8_t cnt_9 = 0;
static uint8_t cnt_10s = 0;
static uint8_t cnt_10m = 0;
static float dv_old = 3.0;
static float dv_new = 3.0;
static uint16_t batStatus78;
static uint16_t batVoltage78;


bool   m_ADC_notEnabled = true;
    
static float voltsFromAdcValue(int v)
{
    float dv = 0.0;
    
    //dv = 0.006349 * v + 0.240635;
    dv = 0.007914 * v - 0.1741;
#if USE_FUD
    dv = 3.0;
#endif	
    return(dv);
}

static int load_cnt;

int adc_count()
{
    int adcMeasureType = 0;
    if( cnt_9 < mg_9_ADC_rate )
        cnt_9++;
    
    if( cnt_10m < mg_10_loadADC )
    {
        cnt_10s++;
        if( cnt_10s >= 60 )
        {
            cnt_10s = 0;
            if( cnt_10m < mg_10_loadADC ) 
                cnt_10m++;
        }
    }
    
    adcMeasureType = 0;
    if( cnt_9 >= mg_9_ADC_rate )
    {
        adcMeasureType = 1;
        if( cnt_10m >= mg_10_loadADC ) 
        {
            adcMeasureType = 2;
        }
    }
    return(adcMeasureType);
}

int   vPreCount = 0;
float vPre_0;
float vPre_1;
float vPre_2;
float vPre_Limit;
float vPre_Sunaba;

int adc_proc_PRE(int param)
{
    if(g_PRE == false)
        return(0);
    
    int adcMeasureType = 0;
    if( param == ADC_PROC_INIT_TRIGGER ) // Basically called on power up
    {
        vPreCount = 0;
        vPre_0 = 0.0;
        vPre_1 = 0.0;
        vPre_2 = 0.0;
        vPre_Limit = 2.5;
        vPre_Sunaba = 1.8; // 1.16 [169]

        //mg_9_ADC_rate = 0;
        
        adc_sm_PRE = ADC_COUNTING_PRE;
        //load_cnt = 0;
        //dv_old = 3.0;
        //dv_new = 3.0;
    }
    
    
    if( param == ADC_PROC_TIMER_TICK_TWO ) // The adcOn_timer delay for milliseconds to load the battery
    {
    }
    if( param == ADC_PROC_TIMER_TICK ) // 1 Second Tick
    {
        adcMeasureType = 1;
        switch(adc_sm_PRE)
        {        
            case ADC_COUNTING_PRE:

                if (adcMeasureType == 1)
                {
                    BattPins_ON_LO();
                    batStatus78 = 0x0000;

                    dv_old = dv_new;
                    //adcADC_sm_PRE = ADCADC_S2_PRE;
                    adcADC_sm_PRE = ADCADC_S3_PRE;
                    nrf_adc_start();
                }

                //if (adcMeasureType != 0) // != 0 means doing an ADC
                //{
                //    adc_sm_PRE = ADC_PARKED_PRE;
                //}                
                break;
                
            case ADC_PARKED_PRE:
                //if(mg_9_ADC_rate == 0) // Don't do ADC
                //   break;
                //adcMeasureType = adc_count();            
                break;

                
            //case ADC_LOADSEQUENCE_PRE:
            //    break;
        
        } 
    }
    
    if( param == ADC_PROC_UNPARK ) // unpark the sm
    {
        if (adc_sm_PRE == ADC_PARKED_PRE)
            adc_sm_PRE = ADC_COUNTING_PRE;
        adcADC_sm_PRE = ADCADC_S0_PRE; // should be here at ADCADC_S0 anyway
    }
    
    if( param == ADC_PROC_ADC_TRIGGER ) // ADC conversion Done
    {
        
        switch(adcADC_sm_PRE)
        {        
            case ADCADC_S0_PRE:
                // just sit and wait for Force to another state
                break;
            
            case ADCADC_S1_PRE:
                //adcADC_sm_PRE = ADCADC_S2_PRE;
                adcADC_sm_PRE = ADCADC_S3_PRE;
                //break;
  
            /*            
            case ADCADC_S2_PRE: // ADC done event
                dv_new = voltsFromAdcValue(adc_sample); 
            
                vPre_0 = vPre_1;
                vPre_1 = vPre_2;
                vPre_2 = dv_new;
                if(vPreCount < 3)
                    vPreCount++;
                if(vPreCount >= 3)
                {
                    if( (vPre_0 > vPre_Limit) && (vPre_1 > vPre_Limit) && (vPre_2 > vPre_Limit) )
                    {
                        g_PRE = false;
                    }
                }
                dbgPrintf(" _PRE Va = %f [%d]\r\n", dv_new,  adc_sample);

                adcADC_sm_PRE = ADCADC_S3_PRE;
                nrf_adc_start();        
                break;
            */
            case ADCADC_S3_PRE: // ADC done event
                BattPins_OFF();
                dv_new = voltsFromAdcValue(adc_sample);

                //----
                vPre_0 = vPre_1;
                vPre_1 = vPre_2;
                vPre_2 = dv_new;
                if(vPreCount < 3)
                    vPreCount++;
                if(vPreCount >= 3)
                {
                    if( (vPre_0 > vPre_Limit) && (vPre_1 > vPre_Limit) && (vPre_2 > vPre_Limit) )
                    {
                        g_PRE = false;
                    }
                    if( (vPre_0 < vPre_Sunaba) && (vPre_1 < vPre_Sunaba) && (vPre_2 < vPre_Sunaba) )
                    {
                        g_PRE = false;
                    }
                }
                //----
            
                dbgPrintf(" _PRE Vb = %f [%d]\r\n", dv_new,  adc_sample);
                adcADC_sm_PRE = ADCADC_S0_PRE;

                adc_sm_PRE = ADC_COUNTING_PRE;
                adcADC_sm_PRE = ADCADC_S0_PRE; // should be here at ADCADC_S0 anyway
                break;
        }
    }
    
    return(0);
}


int adc_proc(int param)
{
#if 1 // _PRE debug
    
//    if(g_PRE == true)
//        return(0);

    int adcMeasureType = 0;
    if( param == ADC_PROC_INIT_TRIGGER ) // Basically called on power up
    {
        mg_9_ADC_rate = 0;
        
        adc_sm = ADC_COUNTING;
        load_cnt = 0;
        dv_old = 3.0;
        dv_new = 3.0;
    }
    
    if(g_PRE == true)
        return(0);

#if USE_ADCON_TIMER
    if( param == ADC_PROC_TIMER_TICK_TWO ) // The adcOn_timer delay for milliseconds to load the battery
    {
        //State should be adcADC_sm == ADCADC_ON;
        adcADC_sm = ADCADC_S1;
        adcADC_sm = ADCADC_S2;
        nrf_adc_start();
    }
#endif
    if( param == ADC_PROC_TIMER_TICK ) // 1 Second Tick
    {
        switch(adc_sm)
        {        
            case ADC_COUNTING:
                if(mg_9_ADC_rate == 0) // Don't do ADC
                    break;
                adcMeasureType = adc_count();            

                if (adcMeasureType == 1)
                {
                    BattPins_ON_LO();
                    batStatus78 = 0x0000;

                    dv_old = dv_new;
#if USE_ADCON_TIMER
                    adcADC_sm = ADCADC_ON;
                    //adcOn_timer_start( 18 );//10 );//20 ); // 6 >500us 12 700uS 18 900us
                    adcOn_timer_start( 51 ); // 2000uS?
                    //  T = 300us + n * (100us)/3
                    // (2000us - 300us) *3  / 100us = n
#else                    
                    adcADC_sm = ADCADC_S2;
                    nrf_adc_start();
#endif

                    cnt_9 = 0;
                }
                if (adcMeasureType == 2)
                {
                    BattPins_ON_LO();
                    BattPins_ON_HI();
                    batStatus78 = 0x8000;
                    
                    dv_old = dv_new;
#if USE_ADCON_TIMER
                    adcADC_sm = ADCADC_ON;
                    adcOn_timer_start( 10 );//20 );
#else                    
                    adcADC_sm = ADCADC_S1;
                    nrf_adc_start();
#endif
                    cnt_9 = 0;
                    cnt_10s = 0;
                    cnt_10m = 0;                    
                }               
                if (adcMeasureType != 0) // != 0 means doing an ADC
                {
                    adc_sm = ADC_PARKED;
                }                
                break;
                
            case ADC_PARKED:
                if(mg_9_ADC_rate == 0) // Don't do ADC
                    break;
                adcMeasureType = adc_count();            
                break;

                
            case ADC_LOADSEQUENCE:
                if( 
                    (load_cnt ==  0) ||    //  0- 1 ON,  1-11 OFF
                    (load_cnt == 11) ||    // 11-12 ON, 12-22 OFF
                    (load_cnt == 22)    )  // 22-23 ON, 23+   OFF
                {
                    BattPins_ON_LO();
                    BattPins_ON_HI();
                    dbgPrint("ADC_LOADSEQUENCE: LOAD ON\r\n");
                }
                else
                {
                    BattPins_OFF();
                    dbgPrint("ADC_LOADSEQUENCE: LOAD OFF\r\n");
                }
                if(load_cnt == 23)
                {
                    load_cnt = 0;
                    adc_sm = ADC_COUNTING;
                }
                else
                {
                    load_cnt++;
                }
                break;
        
        } 
    }
    
    if( param == ADC_PROC_UNPARK ) // unpark the sm
    {
        if (adc_sm == ADC_PARKED)
            adc_sm = ADC_COUNTING;
        adcADC_sm = ADCADC_S0; // should be here at ADCADC_S0 anyway
    }
    
    if( param == ADC_PROC_ADC_TRIGGER ) // ADC conversion Done
    {
        
        switch(adcADC_sm)
        {        
            case ADCADC_S0:
                // just sit and wait for Force to another state
                break;
            
            case ADCADC_S1:
                adcADC_sm = ADCADC_S2;
                //break; commented means fall through to ADCADC_S2.
            
            case ADCADC_S2: // ADC done event
                dv_new = voltsFromAdcValue(adc_sample); 
                dbgPrintf(" Va = %f [%d]\r\n", dv_new,  adc_sample);
                adcADC_sm = ADCADC_S3;
                nrf_adc_start();        
                break;

            case ADCADC_S3: // ADC done event
                BattPins_OFF();
                dv_new = voltsFromAdcValue(adc_sample);
            
                dbgPrintf(" Vb = %f [%d]\r\n", dv_new,  adc_sample);
                adcADC_sm = ADCADC_S0;

                batVoltage78 = (uint16_t)(dv_new * 1000.0); 
                batVoltage78 |= batStatus78;
    
                uniEvent_t LEvt;
                LEvt.evtType = evt_core_ADC_trigger;
                core_thread_QueueSend(&LEvt); // ..._QueueSendFromISR( ... )
            
                if( (dv_new > 2.9) && (dv_old < 2.5) )            
                {
                    load_cnt = 0;
                    adc_sm = ADC_LOADSEQUENCE;
                }
                break;
        }
    }
#endif
    return(0);
}


int proc01_9E_02_timeout( be_t *be_Req,  be_t *be_Rsp )
{
    dbgPrint("proc01_9E_02_timeout\r\n");
    adc_proc(ADC_PROC_UNPARK);
    return(0);
}
int proc01_9E_02_processRsp( be_t *be_Req,  be_t *be_Rsp )
{
    dbgPrint("proc01_9E_02_processRsp\r\n");
    adc_proc(ADC_PROC_UNPARK);
    return(0);
}
int proc01_9E_02_makeCmd( be_t *be_Req )
{
    be_Req->buffer[0] = 0x01;
    be_Req->buffer[1] = 0x9E;
    be_Req->buffer[2] = 0x02;
    be_Req->buffer[3] = 4; //len LSB
    be_Req->buffer[4] = 0; //len MSB
    be_Req->buffer[5] = 0x91;
    be_Req->buffer[6] = 0x00; // 0x0100 -> Version 1.00
    be_Req->buffer[7] = (batVoltage78 >> 0) & 0x00ff;
    be_Req->buffer[8] = (batVoltage78 >> 8) & 0x00ff;
    
    uint16_t cs;
#if _USE_CRC
    //nerror
    cs = CRC_START_SEED; //0x0000;//0xFFFF;
    cs = crc16_compute (be_Req->buffer, 9, &cs);
    be_Req->buffer[ 9] = (cs >> 8) & 0x00ff; // CRC MSB first
    be_Req->buffer[10] = (cs >> 0) & 0x00ff;
#else
    cs = get_checksum  (be_Req->buffer, 0, 9 );
    be_Req->buffer[ 9] = (cs >> 0) & 0x00ff;
    be_Req->buffer[10] = (cs >> 8) & 0x00ff;
#endif

    be_Req->rdPtr = 0;
    be_Req->wrPtr = 11;
    be_Req->length = 11;

    /* DEBUG*/
    dbgPrintf("===============================================================\r\n");
    int i;
    for(i=0; i<11;i++)
    {
        if( i==7 ) dbgPrintf("<%02x ", be_Req->buffer[i] ); else
        if( i==8 ) dbgPrintf("%02x> ", be_Req->buffer[i] ); else
        dbgPrintf("%02x ", be_Req->buffer[i] );
    }
    dbgPrintf("\r\n");
    dbgPrintf("===============================================================\r\n");

    // 01 9e 02 04 00 00 01 f0 0a a0 01
    /**/
    
    return(0);
}

