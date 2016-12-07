

#ifndef __U_PROC0X01_H
#define __U_PROC0X01_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"

//----- Defines ---------------------------------------------------------------


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

#define  CMD_01_0x44   0x44 
#define  REQ_READ_REC_DATA_NEW_COMMAND1_SERIAL       0x44
#define  REQ_READ_REC_DATA_NEW1_COMMAND2_SERIAL      0x00
#define  REQ_READ_REC_DATA_NEW2_COMMAND2_SERIAL      0x01
#define  REQ_READ_REC_DATA_NEW3_COMMAND2_SERIAL      0x02

#define  CMD_01_0x45   0x45 
#define  READ_REC_DATA_NEW_COMMAND1_SERIAL           0x45
#define  READ_REC_DATA_NEW_COMMAND2_SERIAL           0x06
#define  REREAD_REC_DATA_NEW_COMMAND2_SERIAL         0x15

#define  CMD_01_0xF5   0xF5 
#define  CMD_01_0xF8   0xF8
#define  CMD_01_0xF9   0xF9
    
//----- typedefs --------------------------------------------------------------
#define FRAME01_MAXDATALEN 1024

typedef struct tx_frame01_s
{
  uint8_t  soh;      /* SOH 1Byte */
  uint8_t  command;    /* コマンド 1Byte */
  uint8_t  response;    /* 応答コード 1Byte*/
  uint8_t  length1;    /* データ長 2Byte */
  uint8_t  length2;
  uint8_t  data[FRAME01_MAXDATALEN];    /* データ部 1024Byte */
  uint16_t  sum;      /* SUM 2Byte */
} tx_frame01_t;

typedef struct rx_frame01_s
{
  uint8_t  soh;      /* SOH 1Byte */
  uint8_t  command1;    /* コマンド1 1Byte */
  uint8_t  command2;    /* コマンド2 1Byte*/
  uint8_t  length1;    /* データ長 2Byte */
  uint8_t  length2;
  uint8_t  data[FRAME01_MAXDATALEN];    /* データ部 1024Byte */
  uint16_t  sum;      /* SUM 2Byte */
} rx_frame01_t;


typedef union frame01_s
{
  uint8_t byte[7 + FRAME01_MAXDATALEN + 2]; // (5 + 1024 + 2 = 1031)
  tx_frame01_t tx;
  rx_frame01_t rx;
} frame01_t;





int32_t  proc01_checkPacketComplete(uint8_t *buf, uint16_t rp, uint16_t wp );

typedef struct frameUni_s
{
    uint32_t rdPtr;
    uint32_t wrPtr;
    union 
    {
        uint8_t buffer[5000];
        frame01_t frame01;
    } tbd;
} frameUni_t;
    

typedef struct frameUni_s frameUni_t;
int32_t proc01_process_rxframe(frameUni_t* FUin, frameUni_t* FUout);

uint16_t get_checksum(uint8_t *buf, uint16_t rp, uint16_t wp );
    
#ifdef __cplusplus
}
#endif

#endif // __U_PROC0X01_H


