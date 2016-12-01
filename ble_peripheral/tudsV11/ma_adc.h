
#include <stdbool.h>

//-----------------------------------------------------------------------------
#ifndef __M_ADC_H
#define __M_ADC_H

#define USE_NEW_ADC 1

extern bool g_PRE;
extern float vPre_0;
extern float vPre_1;
extern float vPre_2;


//-----------------------------------------------------------------------------
#define ADC_PROC_INIT_TRIGGER   0
#define ADC_PROC_TIMER_TICK     1
#define ADC_PROC_TIMER_TICK_TWO 2 // The adcOn_timer delay for milliseconds to load the battery
#define ADC_PROC_UNPARK         3
#define ADC_PROC_ADC_TRIGGER    4
int adc_proc(int param);
extern bool   m_ADC_notEnabled;

int adc_proc_PRE(int param);

#endif //__M_ADC_H
