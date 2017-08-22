
#include "tudsApp_config.h"

#if APP_TD_STOPLINE_ENABLED //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
typedef void (*stopline_cb)(uint32_t val);    
uint32_t ma_stopline_init(stopline_cb);
uint32_t ma_stopline_deinit(void);
uint32_t ma_stopline_value(void);

uint32_t ma_stopline_wakeup_line_set(void);
    
#ifdef __cplusplus
}
#endif



#endif //APP_TD_STOPLINE_ENABLED //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
