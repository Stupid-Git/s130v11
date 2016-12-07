#ifndef __BLOCK_PROC_H
#define __BLOCK_PROC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#include "debug_etc.h"

    
//#include "memory.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"

struct def_tr_frame{
  uint8_t  soh;      /* SOH 1Byte */
  uint8_t  command;    /* コマンド 1Byte */
  uint8_t  response;    /* 応答コード 1Byte*/
  uint8_t  length1;    /* データ長 2Byte */
  uint8_t  length2;
  uint8_t  data[1024];    /* データ部 1024Byte */
  uint16_t  sum;      /* SUM 2Byte */
};
union def_tr{
  struct def_tr_frame ir;
  struct def_tr_frame usb;
  struct def_tr_frame serial;
};
struct def_rcv_frame{
  uint8_t  soh;      /* SOH 1Byte */
  uint8_t  command1;    /* コマンド1 1Byte */
  uint8_t  command2;    /* コマンド2 1Byte*/
  uint8_t  length1;    /* データ長 2Byte */
  uint8_t  length2;
  uint8_t  data[1024];    /* データ部 1024Byte */
  uint16_t  sum;      /* SUM 2Byte */
};
union def_rcv{
  struct def_rcv_frame ir;
  struct def_rcv_frame usb;
  struct def_rcv_frame serial;
};

union def_frame_uartch0{
  uint8_t byte[1031];
  union def_tr tr;
  union def_rcv rcv;
};

//union def_frame_uartch0 frame_uartch0;
//union def_frame_uartch0* p_fr_uart;

#define CMD1  p_fr_uart->rcv.serial.command1

#define SET_SERIAL_COM 1
#define CLR_SERIAL_COM 0
#define ADD_LENGTH_TX_FRAME_SERIAL 7
#define SOH_TX_FRAME_SERIAL 0x01
#define ACK_TX_FRAME_SERIAL 0x06
#define NAK_TX_FRAME_SERIAL 0x15
#define TIMEOVER_TX_FRAME_SERIAL 0x35
#define REC_TX_FRAME_SERIAL      0x09 // INVALID_TX_FRAME_SERIAL
#define INVALID_TX_FRAME_SERIAL  0x09 // REC_TX_FRAME_SERIAL
#define SOH_RX_FRAME_SERIAL 0x01
#define LENGTH_HEADER_RX_FRAME_SERIAL 5

#define  READ_SERIAL_NO1_COMMAND1_SERIAL 0x58
#define  READ_SERIAL_NO2_COMMAND1_SERIAL 0xB3



//*****************************************************************************
//*****************************************************************************
//********** GLOBALS **********************************************************
//*****************************************************************************
//*****************************************************************************
#ifdef BLOCK_PROC_C
#define _EXT
#else
#define _EXT extern
#endif

#include "ble_tuds.h"

void BlkDn_unlockStateMachine(void);

uint32_t BlkDn_timer_init(void);
int32_t BlkDn_On_Dcmd(/*ble_tuds_t *p_tuds,*/ uint8_t *buf, uint8_t len);
int32_t BlkDn_On_Ddat(/*ble_tuds_t *p_tuds,*/ uint8_t *buf, uint8_t len);
int32_t BlkDn_On_written_Dcfm(ble_tuds_t *p_tuds,  uint8_t *buf, uint8_t len);

uint32_t BlkUp_timer_init(void);
int32_t BlkUp_On_written_Ucmd(ble_tuds_t *p_tuds,  uint8_t *buf, uint8_t len);
int32_t BlkUp_On_written_Udat(ble_tuds_t *p_tuds,  uint8_t *buf, uint8_t len);
int32_t BlkUp_On_Ucfm( uint8_t *buf, uint8_t len);


// block_dn.c
#define BLK_DN_COUNT (128 + 8)

_EXT uint8_t  m_blkDn_buf[ 16 * BLK_DN_COUNT ]; // 2048 @ BLK_DN_COUNT = 128
_EXT uint8_t  m_blkDn_chk[  1 * BLK_DN_COUNT ]; //  128 @ BLK_DN_COUNT = 128
_EXT uint16_t m_blkDn_len;
_EXT uint16_t m_blkDn_blkCnt;
_EXT uint16_t m_blkDn_rxBlkCnt;


// block_up.c
#define BLK_UP_COUNT (128 + 8)
//_EXT uint8_t  m_blkUp_buf[ 16 * BLK_UP_COUNT ]; // 2048 @ BLK_UP_COUNT = 128
_EXT   uint8_t* m_blkUp_p_buf;//[ 16 * BLK_UP_COUNT ]; // 2048 @ BLK_UP_COUNT = 128
_EXT   uint8_t  m_blkUp_chk[  1 * BLK_UP_COUNT ]; //  128 @ BLK_UP_COUNT = 128
_EXT   uint16_t m_blkUp_len;
_EXT   uint16_t m_blkUp_blkCnt;
_EXT   uint16_t m_blkUp_txBlkCnt;
_EXT   uint8_t  m_blkUp_chkSumLSB;
_EXT   uint8_t  m_blkUp_chkSumMSB;

    
#ifdef __cplusplus
}
#endif

#endif // __DEBUG_ETC_H






