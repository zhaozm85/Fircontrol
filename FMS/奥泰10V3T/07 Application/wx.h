/**
  ******************************************************************************
  * proj    firmware startup project
  * file    WX.h
  * ver     1.0
  * brief   This is a WX file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _WX_H_
#define _WX_H_

/* Includes ------------------------------------------------------------------*/

#include  "can.h"

/* Global Macros & Defines --------------------------------------------------*/
//Warning Level
#define WARL1     0x01  
#define WARL2     0x02
#define WARL3     0x03
#define NORMAL    0x00

//Status 
#define START     0x01
#define STOP      0x00

//Temp extreme
#define TEMPMAX1  40+40   //Temp max threshold level 1    40
#define TEMPMAX2  45+40   //Temp max threshold level 2    45
#define TEMPMAX3  50+40   //Temp max threshold level 3    50
#define TEMPMIN1  -15+40    //Temp min threshold level 1    -15
#define TEMPMIN2  -20+40    //Temp min threshold level 2    -20
#define TEMPMIN3  -25+40   //Temp min threshold level 3    -25

//Voltage extreme
#define VOLTMAX1  0xA8C   //Voltage max threshold level 1   2.700V
#define VOLTMAX2  0xABE   //Voltage max threshold level 2   2.750V
#define VOLTMAX3  0xAF0   //Voltage max threshold level 3   2.800V
#define VOLTMIN1  0x708   //Voltage min threshold level 1   1.800V
#define VOLTMIN2  0x6D6   //Voltage min threshold level 2   1.750V
#define VOLTMIN3  0x6A4   //Voltage min threshold level 3   1.700V

//Temp difference
#define TEMPDIFF1 5+40    //Temp difference level 1     5
#define TEMPDIFF2 8+40    //Temp difference level 2     8
#define TEMPDIFF3 10+40   //Temp difference level 3     10

//Voltage difference
#define VOLTDIFF1 0x0F    //Voltage difference level 1     15mV
#define VOLTDIFF2 0x1E    //Voltage difference level 2     30mV
#define VOLTDIFF3 0x32    //Voltage difference level 3     50mV


/* Global typedefs ----------------------------------------------------------*/

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/
//extern unsigned char fanStatus;

/* Global function prototypes -----------------------------------------------*/

extern void wx_init(void);
extern void wx_crx_save(can_frame_t*);
extern void wx_sync(void);
extern unsigned char wx_crx_proc(void);
extern void wx_ctx(void);
extern void wx_reset(void);
extern unsigned char gf_fanDrive(void);
extern void  Fir_pro(void);

/* Re-include control --------------------------------------------------------*/
#endif  /* _WX_H_ */

