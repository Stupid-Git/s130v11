#include "stdint.h"
#include "nrf_error.h"
#include "ble_err.h"


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
typedef enum eBlkUpState
{
    eBlkUp_IDLE,
    eBlkUp_CMD_PRESEND,
    eBlkUp_CMD_SEND,
    eBlkUp_CMD_SENT,
    eBlkUp_DAT_PRESEND,
    eBlkUp_DAT_SEND,
    eBlkUp_DAT_SENT,
    
    eBlkUp_WAIT_CFM,

} eBlkUpState_t;

typedef enum
{
    UP_START,
    UP_TO,
    TXDONE,
    
} e_trig_t;


uint32_t startTO()
{
    return(0);
}
uint32_t stopTO()
{
    return(0);
}

uint32_t N_CMD()
{
    return(0);
}
uint32_t N_DAT()
{
    return(0);
}


eBlkUpState_t m_upsm = 0;

int dosm(int trig);

void start()
{

    stopTO();
    m_upsm = eBlkUp_CMD_PRESEND;
    dosm(UP_START);
    
}



int dosm(int trig)
{
    uint32_t err_code;
    
    switch(m_upsm)
    {
        //---------------------------------------------------------------------
        case eBlkUp_IDLE:
            break;
        
        //---------------------------------------------------------------------
        case eBlkUp_CMD_PRESEND:
            if(trig==UP_START)
            {
                stopTO();
                err_code = N_CMD();
                if(err_code == NRF_SUCCESS)
                {
                    m_upsm = eBlkUp_CMD_SEND;
                    startTO();
                }else
                if(err_code == BLE_ERROR_NO_TX_PACKETS) //no packets
                {
                    m_upsm = eBlkUp_CMD_PRESEND;
                    startTO();
                }else
                {
                    //fatal?
                }
            }

            if(trig==UP_TO)
            {
                err_code = N_CMD(); //retry
                if(err_code == NRF_SUCCESS)
                {
                    m_upsm = eBlkUp_CMD_SEND;
                    startTO();
                } else
                if(err_code == BLE_ERROR_NO_TX_PACKETS) //no packets
                {
                    m_upsm = eBlkUp_CMD_PRESEND;
                    startTO();
                }else
                {
                    //fatal?
                }
            }
            if(trig==TXDONE) // others have been done, so maybe we have a chance
            {
                stopTO();
                err_code = N_CMD();
                if(err_code == NRF_SUCCESS)
                {
                    m_upsm = eBlkUp_CMD_SEND;
                    startTO();
                }else
                if(err_code == BLE_ERROR_NO_TX_PACKETS) //no packets
                {
                    m_upsm = eBlkUp_CMD_PRESEND;
                    startTO();
                }else
                {
                    //fatal?
                }
            }
            break;
        
        //---------------------------------------------------------------------
        case eBlkUp_CMD_SEND:
            if(trig==UP_TO)
            {
                err_code = N_CMD(); //retry
                if(err_code == NRF_SUCCESS)
                {
                    //m_upsm = eBlkUp_CMD_SEND;
                    startTO();
                } else
                if(err_code == BLE_ERROR_NO_TX_PACKETS) //no packets
                {
                    //m_upsm = eBlkUp_CMD_PRESEND;
                    startTO();
                }else
                {
                    //fatal?
                }
            }

            if(trig==TXDONE)
            {
                stopTO();
                m_upsm = eBlkUp_CMD_SENT; 
                //m_current_blk_No = 0;
                err_code  = N_DAT(); //(m_current_blk_No);
                if(err_code == NRF_SUCCESS)
                {
                    m_upsm = eBlkUp_DAT_SEND;
                    startTO();
                } else
                if(err_code == BLE_ERROR_NO_TX_PACKETS) //no packets
                {
                    m_upsm = eBlkUp_DAT_PRESEND;
                    startTO();
                }else
                {
                    //fatal?
                }            
            }

            break;
        
        //---------------------------------------------------------------------
        case eBlkUp_CMD_SENT:
            break;
        
        //---------------------------------------------------------------------
        case eBlkUp_DAT_PRESEND:
            break;
        
        //---------------------------------------------------------------------
        case eBlkUp_DAT_SEND:
            break;
        
        //---------------------------------------------------------------------
        case eBlkUp_DAT_SENT:
            break;
        
        //---------------------------------------------------------------------
        case eBlkUp_WAIT_CFM:
            break;
    }

    return(0);
}

