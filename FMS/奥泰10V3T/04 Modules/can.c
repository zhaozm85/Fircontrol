/**
  ******************************************************************************
  * proj    firmware startup project
  * file    template.c
  * ver     1.0
  * brief   This is a template file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _CAN_C_ 0x44
#define FILE_No _CAN_C_

/* Includes ------------------------------------------------------------------*/
#include  "can.h"  /* mandotary */
#include  "mcu.h"
#include  "sc.h"

/* Private Macros & Defines --------------------------------------------------*/

#ifndef TEST_MSCAN
#define TEST_MSCAN  0
#endif

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  mscan_txframe_t txframe;
  can_frame_t frame;
}
can_t;

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static can_t can;

/* Private function prototypes -----------------------------------------------*/

static void can_down_frame(can_frame_t* f, mscan_txframe_t* m);
static void can_up_frame(mscan_rxframe_t* m, can_frame_t* f);

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
  * bref    template function
  * param   none
  * retval  none
  */

int can_init(void) {

  mscan_config_t  f;
  
  /* init can */
  f.brp = gv_sSC.sBMU.eCanBaudrate;
  f.idam = MSCAN_IDAM_32x2;

  /* filter 1 : STD, 10X, 30X, 50X, 70X only */
  f.idar[0] = 0x20; //  ID10 ID9  1   0   0   0  0  ID3
  f.idmr[0] = 0xC1; //    -   -   M   M   M   M  M   -
  f.idar[1] = 0x00; //  ID2  ID1 ID0 RTR IDE  -  -   -
  f.idmr[1] = 0xE7; //   -    -   -   M   M   -  -   -  
  f.idmr[2] = 0xFF;
  f.idmr[3] = 0xFF;

  /* filter 2 : no pass */
  f.idar[4] = 0x00;
  f.idar[5] = 0x00;
  f.idar[6] = 0x00;
  f.idar[7] = 0x00;
  f.idmr[4] = 0x00;
  f.idmr[5] = 0x00;
  f.idmr[6] = 0x00;
  f.idmr[7] = 0x00;

  (void)mscan_init(&f);
  (void)pin_low(PN_CAN_STB);

  return 1;
}

int can_reset(void){

  (void)pin_high(PN_CAN_STB);
  (void)mscan_reset();

  return 1;
}

int can_xmit(can_frame_t * f){

  /* down frame */
  can_down_frame(f,&can.txframe);
  
  /* send out */
  return mscan_xmit(&can.txframe);

}

#if !TEST_MSCAN
void mscan_rxcb(mscan_rxframe_t * f){

  /* up frame */
 // can_up_frame(f,&can.frame);

  /* rx callback */
  gf_vCanRxcb(&can.frame);

}
#endif

static void can_down_frame(can_frame_t* f, mscan_txframe_t* m)
{

  /* standard id : 0x000-0x7FF */
  if( f->id < 0x800 ){
    /* ID[10:3] */
  	m->idr.Overlap_STR.CANTIDR0STR.Byte = (unsigned char)(f->id>>3);
  	/* ID[2:0] RTR=0 IDE=0 X X X */
  	m->idr.Overlap_STR.CANTIDR1STR.Byte = (unsigned char)(((f->id<<5)&0xE0)|0x00);
  	/* X[7:0] */
  	m->idr.Overlap_STR.CANTIDR2STR.Byte = (unsigned char)(0x00);
  	/* X[7:0] */
  	m->idr.Overlap_STR.CANTIDR3STR.Byte = (unsigned char)(0x00);
  }
  /* extended id : 0x20000000-0x3FFFFFFF */
  else{
    /* ID[28:21] */
  	m->idr.Overlap_STR.CANTIDR0STR.Byte = (unsigned char)(f->id>>21);
  	/* ID[20:18] SRR=1 IDE=1 ID[17:15] */
  	m->idr.Overlap_STR.CANTIDR1STR.Byte = (unsigned char)(((f->id>>13)&0xE0)|0x18|((f->id>>15)&0x07));
  	/* ID[14:7] */
  	m->idr.Overlap_STR.CANTIDR2STR.Byte = (unsigned char)(f->id>>7);
  	/* ID[6:0] RTR=0 */
  	m->idr.Overlap_STR.CANTIDR3STR.Byte = (unsigned char)((f->id<<1)|0);
  }
  /* data length */
  m->dlr.Byte = f->dlc;
  /* data */
  m->dsr[0] = f->data[0];
  m->dsr[1] = f->data[1];
  m->dsr[2] = f->data[2];
  m->dsr[3] = f->data[3];
  m->dsr[4] = f->data[4];
  m->dsr[5] = f->data[5];
  m->dsr[6] = f->data[6];
  m->dsr[7] = f->data[7];
  /* priority */
  m->bpr.Byte = 0;

}

static void can_up_frame(mscan_rxframe_t* m, can_frame_t* f)
{

  /* standard id : 0x000-0x7FF */
  if( m->idr.Overlap_STR.CANRIDR1STR.Bits.IDE==0 ){
    unsigned long id;
    /* ID[10:3] */
    id = m->idr.Overlap_STR.CANRIDR0STR.Byte;
  	/* ID[2:0] RTR=0 IDE=0 X X X */
  	id <<= 3; id += m->idr.Overlap_STR.CANRIDR1STR.Byte >> 5;
  	/* X[7:0] */
  	/* X[7:0] */
  	/* ok */
  	f->id = id;
  }
  /* extended id : 0x20000000-0x3FFFFFFF */
  else{
    unsigned long id;
    /* ID[28:21] */
  	id = m->idr.Overlap_STR.CANRIDR0STR.Byte;
  	/* ID[20:18] SRR=1 IDE=1 ID[17:15] */
  	id <<= 3; id += m->idr.Overlap_STR.CANRIDR1STR.Byte >> 5;
  	id <<= 3; id += m->idr.Overlap_STR.CANRIDR1STR.Byte & 0x07;
  	/* ID[14:7] */
  	id <<= 8; id += m->idr.Overlap_STR.CANRIDR2STR.Byte;
  	/* ID[6:0] RTR=0 */
  	id <<= 7; id += m->idr.Overlap_STR.CANRIDR3STR.Byte >> 1;
  	/* ok */
  	f->id = id + 0x20000000;
  }
  /* data length */
  f->dlc = m->dlr.Byte & 0x0F;
  /* data */
  f->data[0] = m->dsr[0];
  f->data[1] = m->dsr[1];
  f->data[2] = m->dsr[2];
  f->data[3] = m->dsr[3];
  f->data[4] = m->dsr[4];
  f->data[5] = m->dsr[5];
  f->data[6] = m->dsr[6];
  f->data[7] = m->dsr[7];
  /* time stamp */
  f->ts = m->tsr.Word;

}
