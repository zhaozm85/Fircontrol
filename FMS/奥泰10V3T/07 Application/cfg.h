/**
  ******************************************************************************
  * proj    firmware startup project
  * file    CFG.h
  * ver     1.0
  * brief   This is a CFG file for all H files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id & Re-include control ----------------------------------------------*/
#ifndef _CFG_H_
#define _CFG_H_

/* Includes ------------------------------------------------------------------*/

#include  "bmu.h"
#include  "mng.h"
#include  "mcu.h"

/* Global Macros & Defines --------------------------------------------------*/

/* Global typedefs ----------------------------------------------------------*/

typedef struct
{
  volatile unsigned short magic_code;   //  0x55AA
  volatile unsigned char  check_sum_even;
  volatile unsigned char  check_sum_odd;
  volatile unsigned long  create_time;  //  seconds since 2000.1.1 0:00:00
  volatile unsigned long  update_time;  //  seconds since 2000.1.1 0:00:00
  volatile unsigned short update_count; //  eeprom update count
  volatile unsigned short struct_length;
}
head_t;

typedef struct
{
  volatile head_t head;

  volatile unsigned char  hardware_version[2];
  volatile unsigned char  serial_number[8];
  volatile unsigned short config_address;
}
prd_t;

typedef struct
{
  volatile head_t head;

  struct{
    volatile mscan_brp_t  can_brp;
    volatile gt_eBmuMode   dft_mode;
  }bmu;

  struct{
    volatile unsigned char  ltc6802_count;
    struct{
      volatile unsigned char  device_address;
      volatile pin_number_t   pn_ltc6802_ena_n;
      volatile pin_number_t   pn_spi_cs_n;
    }ltc6802[LTC6802_MAX];
  }vs;
  
  struct{
    volatile unsigned int   adc_wait_ms;
    volatile unsigned int   adc_repeat_count;
    volatile unsigned char  sensor_type[24];
    volatile unsigned char  count1;
    volatile unsigned char  list1[24];
    volatile unsigned char  count2;
    volatile unsigned char  list2[24];
    volatile unsigned short vtemp25;
    volatile unsigned short m_hot;
    volatile unsigned short m_cold;
  }ts;
  
  struct{
    volatile unsigned short volt_ref[12];
    volatile unsigned char  volt_cnt;
    volatile unsigned char  volt_map[12*LTC6802_MAX];
    volatile signed char    volt_adj[12*LTC6802_MAX];
    volatile unsigned short temp_ref[24];
    volatile unsigned char  temp_cnt;
    volatile unsigned char  temp_map[24];
  }cal;
  
  struct{
    volatile unsigned short grp_num;
    volatile unsigned char  mod_num;
    volatile unsigned char  sync_cycle;
    volatile unsigned char  sync_tmo_mul;
    volatile unsigned char  volt_count;
    volatile unsigned char  temp_count;
    volatile unsigned char  temp_map[8];  //  0x00~, 0x20~, 0x40~, 0xE0~
  }wx;

  struct{
    volatile unsigned char  low_ena;
    volatile unsigned char  low_rnd_cyc;  //  round cycles
    volatile unsigned char  low_bal_cyc;  //  balance cycles
    volatile unsigned short low_temp_cool;
    volatile unsigned short low_temp_hot;
    volatile unsigned char  ext_ena;
    volatile unsigned char  int_ena;
    volatile unsigned short int_rnd_cyc;  //  internal balance cycles
    volatile unsigned short int_bal_cyc;
    volatile unsigned short int_vdiff_bad;
  }bal;

}
cfg_t;

/* Global consts ------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/

extern prd_t prd;
extern cfg_t cfg;

/* Global function prototypes -----------------------------------------------*/

extern void prd_load(void);
extern int  prd_save(void);
extern void prd_restore(void);
extern void prd_reset(void);

extern void cfg_load(void);     //  from eeprom
extern void cfg_save(void);     //  to eeprom
extern void cfg_restore(void);  //  to factory settings
extern void cfg_reset(void);    //  to 0xFFs

/* Re-include control --------------------------------------------------------*/
#endif  /* _CFG_H_ */

