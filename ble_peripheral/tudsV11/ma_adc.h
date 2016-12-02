
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



#if USE_NADC
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// NADC API
typedef enum _eNADC_mode
{
    NADC_mode_VCHECK,   // do ADC but don't send via 9E_02
    NADC_mode_FORCE_1,  // after reading 9E_01 and timer non zero: Force first ADC->9E_02
    NADC_mode_FORCE_2,  // after reading 9E_01 and timer non zero: Force first ADC->9E_02
    NADC_mode_NORMAL,   // after FORCE_1, FORCE_2 just our normal mode
} eNADC_mode;

extern eNADC_mode NADC_mode;

typedef enum _eNADC_action
{
    NADC_action_RESET,        // 0
    NADC_action_NONE,         // 1
    NADC_action_CHECK,        // 2
    NADC_action_ONE_SECOND,   // 3
    NADC_action_WIDTH_TIMER,  // 4
    NADC_action_ADC_DONE,     // 5
    
    NADC_action_9E_TIMEOUT,   // 6
    NADC_action_9E_DONE       // 7

} eNADC_action;


/* not used*/
void NADC_set_ADC_L_GO(void);
void NADC_set_ADC_H_GO(void);
/*not used */

int NADC_proc(eNADC_action action);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#else
int adc_proc(int param);
int adc_proc_PRE(int param);
#endif



#endif //__M_ADC_H
