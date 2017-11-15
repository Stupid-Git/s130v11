#ifndef __DEBUG_ETC_H
#define __DEBUG_ETC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

#include "stdarg.h"

// http://www.cplusplus.com/reference/cstdio/printf/
// http://www.cplusplus.com/reference/cstdio/vprintf/

#include "tudsapp_config.h" //for #define  APP_TD_SPIDGB_ENABLED  1/0
    

//------------------------- NO Debug -------------------------
#if (APP_TD_SPIDGB_ENABLED==0)
static inline int dbgPrintf( const char * format, ... )
{
    return(0);
}

#define dbgPrint( X )
#define dbgPrint_Init() 
#define get_ble_evt_str(X) (const char *)""
    
//------------------------- YES Debug -------------------------
#else

void dbgSpi_Init(void);
void dbgSpi_Deinit(void);
int  dbgSpi_AppendText(const char * str);

#define dbgPrint_Init dbgSpi_Init

#if(DBGPRINTF_ALLOW==0) //-------------------------------------
static inline int dbgPrintf( const char * format, ... )
{
    return(0);
}
#define dbgPrint( X )
#else //-------------------------------------------------------
int     dbgPrintf( const char * format, ... );
#define dbgPrint(x) dbgSpi_AppendText(x)
#endif //------------------------------------------------------


const char * get_ble_evt_str( uint8_t evt_id);

#endif
//------------------------- End Debug -------------------------
        
    
#ifdef __cplusplus
}
#endif

#endif // __DEBUG_ETC_H

