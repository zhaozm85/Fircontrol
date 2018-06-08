/**
  ******************************************************************************
  * proj    firmware startup project
  * file    CFG.c
  * ver     1.0
  * brief   This is a CFG file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _CFG_C_ 0x74
#define FILE_No _CFG_C_

/* Includes ------------------------------------------------------------------*/
#include  "cfg.h"  /* mandotary */
#include  "mcu.h"
#include  "at24c.h"
#include  "ntc.h"

/* Private Macros & Defines --------------------------------------------------*/

#define MAGIC_CODE  0x6120
#define CS_SUM      0xCD

/* Private typedefs ----------------------------------------------------------*/

/* Private consts ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

unsigned short  prd_upcnt;
unsigned char prd_we;

/* Private function prototypes -----------------------------------------------*/

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

prd_t prd;
cfg_t cfg;

/* Functions -----------------------------------------------------------------*/

/**
  * bref    CFG function
  * param   none
  * retval  none
  */

static int head_check(void* p, unsigned short n)
{

  head_t * hp = p;

  /* magic code */
  if( hp->magic_code != MAGIC_CODE ){
    DBG_PRINTF("magic code failed: %X\r\n", hp->magic_code);
    return NG;
  }
  
  /* struct length */
  if( hp->struct_length != n ){
    DBG_PRINTF("struct length failed: %d v.s. %d\r\n", hp->struct_length, n );
    return NG;
  }
  
  /* cs */
  {
    unsigned char cse=0, cso=0;
    unsigned short i;
    unsigned char * buf = p;
    for(i=0;i<n;i++){
      if( i&1 ) cso += buf[i];
      else cse += buf[i];
    }    
    if( cso!=CS_SUM || cse!=CS_SUM ){
      DBG_PRINTF("cs failed: %X but %X, %X\r\n", CS_SUM, cso, cse);
      return  NG;
    }
  }
  
  /* pass */
  return  OK;

}

static void head_reset(void* p, unsigned short n)
{
  head_t * hp = p;
  
  /* magic code */
  hp->magic_code = MAGIC_CODE;
  
  /* struct length */
  hp->struct_length = n;

  /* time */
  hp->create_time = rtc_sec;
  
  /* update count */
  hp->update_count = 0;

}

static int head_update(void* p, unsigned short n)
{
  head_t * hp = p;
  unsigned char cse=0, cso=0;
  unsigned short i;
  unsigned char * buf = p;
  
  /* update */
  hp->update_time = rtc_sec;
  if( hp->update_count>=0xffff ){
    DBG_PRINTF("update count reach maximum.\r\n");
    return ERRNO_VALUE_REACH_MAX;
  }
  hp->update_count ++;

  /* cs */
  hp->check_sum_even = 0;
  hp->check_sum_odd  = 0;
  for(i=0;i<n;i++){
    if( i&1 ) cso += buf[i];
    else cse += buf[i];
  }
  hp->check_sum_even = CS_SUM - cse;
  hp->check_sum_odd  = CS_SUM - cso;

}

static void head_print(void* p)
{
  head_t * hp = p;
  PRINTF("---\r\n");
  PRINTF("magic code: %04Xh\r\n", hp->magic_code);
  PRINTF("cs even:    %02Xh\r\n", hp->check_sum_even);
  PRINTF("cs odd:     %02Xh\r\n", hp->check_sum_odd);
  PRINTF("cr time:    %u\r\n", hp->create_time);
  PRINTF("up time:    %u\r\n", hp->update_time);
  PRINTF("up count:   %u\r\n", hp->update_count);
  PRINTF("str len:    %u\r\n", hp->struct_length);
  PRINTF("---\r\n");
}

void prd_reset(void)
{
  (void)memset(&prd,0xff,sizeof(prd));
  head_reset(&prd,sizeof(prd));
}

void prd_restore(void)
{
  prd.hardware_version[0] = 1;
  prd.hardware_version[1] = 6;
  prd.config_address = 0x0100;  
}

int prd_save(void)
{
  if( prd_we==0 ){
    DBG_PRINTF("prd write disabled\r\n");
    return ERRNO_PRD_WRITE_DISABLED;
  }else{
    head_update(&prd,sizeof(prd));
    if( prd.head.update_count==0xFFFF ){
      (void)at24c_init();
      (void)at24c_write(0,(void*)&prd,sizeof(prd));
      (void)at24c_reset();
    }
  }
}

void prd_load(void)
{

  (void)at24c_init();
  (void)at24c_read(0,(void*)&prd,sizeof(prd));
  (void)at24c_reset();

  /* check if the 1st time  */
  if( head_check(&prd,sizeof(prd))==NG ){

    DBG_PRINTF("1st of prd\r\n");
    head_print(&prd);
    prd_reset();
    head_print(&prd);
    prd_restore();
    head_print(&prd);
    prd_save();
    head_print(&prd);

    DBG_PRINTF("reset after init prd...\r\n");
    for(;;);
  }

  prd_we = 0;
  DBG_PRINTF("prd loaded %d\r\n", prd.head.update_count);

}

void cfg_restore(void)
{

  unsigned char i;

  cfg.bmu.can_brp = MSCAN_BRP_250K;
  cfg.bmu.dft_mode = BMU_MODE_NORMAL;

  cfg.vs.ltc6802_count = 1;
  for(i=0;i<LTC6802_MAX;i++){    
    cfg.vs.ltc6802[i].device_address = 0x01;
    cfg.vs.ltc6802[i].pn_ltc6802_ena_n = PN_LTC680X_ENA_N;
    cfg.vs.ltc6802[i].pn_spi_cs_n = PN_SPI_CS_N;
  }
  
  cfg.ts.adc_wait_ms = 5;
  cfg.ts.adc_repeat_count = 512;
  for(i=0;i<24;i++) cfg.ts.sensor_type[i] = NTC_TYPE_CWF4;
  cfg.ts.count1 = 6;
  cfg.ts.list1[0] = 14;
  cfg.ts.list1[1] = 21;
  cfg.ts.list1[2] = 7;
  cfg.ts.list1[3] = 22;
  cfg.ts.list1[4] = 15;
  cfg.ts.list1[5] = 23;
  cfg.ts.count2 = 0;
  cfg.ts.list2[0] = 15;
  cfg.ts.list2[1] = 23;
  cfg.ts.vtemp25  = 1396; //  mV
  cfg.ts.m_hot    = 3638; //  0.001mV/C
  cfg.ts.m_cold   = 3266; //  0.001mV/C
  
  cfg.cal.volt_cnt = 12;
  for(i=0;i<12;i++) cfg.cal.volt_ref[i] = 3333;
  for(i=0;i<12;i++) cfg.cal.volt_map[i] = 0x00+i; //  vs0
  for(i=0;i<12;i++) cfg.cal.volt_map[12+i] = 0x10+i;  //  vs1
  for(i=0;i<12*LTC6802_MAX;i++) cfg.cal.volt_adj[i] = 0;
  cfg.cal.temp_cnt = 6;
  for(i=0;i<24;i++) cfg.cal.temp_ref[i] = 250+400;
  for(i=0;i<24;i++) cfg.cal.temp_map[i] = 0x20+i; //  list1
  
  cfg.wx.grp_num = 1;
  cfg.wx.mod_num = 2;
  cfg.wx.sync_cycle = 20; //  200 ms
  cfg.wx.sync_tmo_mul = 10;
  cfg.wx.volt_count = 12;
  cfg.wx.temp_count = 6;
  cfg.wx.temp_map[0] = 14;
  cfg.wx.temp_map[1] = 21;
  cfg.wx.temp_map[2] = 7;
  cfg.wx.temp_map[3] = 22;
  cfg.wx.temp_map[4] = 15;
  cfg.wx.temp_map[5] = 23;
  cfg.wx.temp_map[6] = 0xff;
  cfg.wx.temp_map[7] = 0xff;

  cfg.bal.low_ena = 1;  
  cfg.bal.low_rnd_cyc = 18;   //  5/18 ~ 50ma
  cfg.bal.low_bal_cyc = 5;    //  
  cfg.bal.low_temp_cool = 500+400; //  50.0C
  cfg.bal.low_temp_hot  = 600+400; //  60.0C
  cfg.bal.ext_ena = 1;
  cfg.bal.int_ena = 1;
  cfg.bal.int_rnd_cyc = 30*5; //  30 sec
  cfg.bal.int_bal_cyc = 20*5; //  20 sec
  cfg.bal.int_vdiff_bad = 50; //  mv

}

void  cfg_reset(void)
{
  (void)memset(&cfg,0xff,sizeof(cfg));
  head_reset(&cfg,sizeof(cfg));
}

void  cfg_save(void)
{
  head_update(&cfg,sizeof(cfg));
  if( cfg.head.update_count<0xffff ){    
    (void)at24c_init();
    (void)at24c_write(prd.config_address,(void*)&cfg,sizeof(cfg));
    (void)at24c_reset();
  }
}

void  cfg_load(void)
{

  (void)at24c_init();
  (void)at24c_read(prd.config_address,(void*)&cfg,sizeof(cfg));
  (void)at24c_reset();

  /* check if the 1st time  */
  if( head_check(&cfg,sizeof(cfg))==NG ){

    DBG_PRINTF("1st of cfg\r\n");
    head_print(&cfg);
    cfg_reset();
    head_print(&cfg);
    cfg_restore();
    head_print(&cfg);
    cfg_save();
    head_print(&cfg);

    DBG_PRINTF("reset after init cfg...\r\n");
    for(;;);
  }
  
  DBG_PRINTF("cfg loaded %d\r\n", cfg.head.update_count);

}

