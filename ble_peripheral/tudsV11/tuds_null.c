


#include "tuds_null.h"

//void BlkDn_On_Dcmd(uint8_t * p_data, uint16_t length) {}
//void BlkDn_On_Ddat(uint8_t * p_data, uint16_t length) {}
//void BlkUp_On_Ucfm(uint8_t * p_data, uint16_t length) {}

//void BlkDn_On_written_Dcfm(ble_tuds_t * p_tuds, int x, int y) {}
//void BlkUp_On_written_Ucmd(ble_tuds_t * p_tuds, int x, int y) {}
//void BlkUp_On_written_Udat(ble_tuds_t * p_tuds, int x, int y) {}


bool m_doUpdateAdvertisingName = false;
char m_UpdatedAdvertisingName[42];

//uint32_t DUMB_counterA;
    
void sdo_Init(void) {}
void sdo_AppendText(char *S) {}

void ma_adc_config(void) {}
    
//void bas_timer_init(void) {}
void pinWakeup_timer_init(void) {}
void ADC_timer_init(void) {}
void Uart_timer_init(void) {}
//void BlkDn_timer_init(void) {}
//void BlkUp_timer_init(void) {}

    



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// http://www.cplusplus.com/reference/cstdio/printf/
// http://www.cplusplus.com/reference/cstdio/vprintf/

#if PRINTF_OVER_UART

int dbgPrintf( const char * format, ... )
{
    int r;
    va_list args;
    va_start (args, format);
    r = vprintf (format, args);
    va_end (args);
    return(r);
}


#define dbgPrint(x) sdo_AppendText(x)
/*
int dbgPrint( const char *str )
{
    int r = 0;
    return(r);
}
*/

#else
int dbgPrintf( const char * format, ... )
{
    int r = 0;
    return(r);
}
int dbgPrint( const char *str )
{
    int r = 0;
    return(r);
}
#endif
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


uint32_t Fake_1_timer_init(void) { return(0); }
uint32_t Fake_1_timer_start(uint32_t timeout_ticks) { return(0); }
uint32_t Fake_1_timer_stop(void) { return(0); }

uint32_t GBN_timer_init(void) { return(0); }
uint32_t GBN_timer_start(uint32_t timeout_ticks) { return(0); }
uint32_t GBN_timer_stop(void) { return(0); }



//void core_thread_init(void){}
//void core_thread_QueueSend(uniEvent_t *pEvt) {} 

//void gInit_All(void){}
//bool m_bleIsConnected;
//void priority_sched_execute(void) {}
void uart_thread_init(void) {}
