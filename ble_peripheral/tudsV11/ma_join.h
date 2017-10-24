#ifndef __MA_JOIN_H
#define __MA_JOIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#include "myapp.h" // for be_t

int make_req_BLE( be_t *be_Req );
int proc_rsp_BLE( be_t *be_Req,  be_t *be_Rsp );
int proc_timeout_BLE( be_t *be_Req,  be_t *be_Rsp );
    
void join_Init(app_tuds_t *p_app_tuds);

#ifdef __cplusplus
}
#endif

#endif // __DEBUG_ETC_H



