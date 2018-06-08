/**
  *****************************************************************************
  * proj    BMU_HM
  * file    sc.c
  * ver     1.0
  * brief   ϵͳ����
  -----------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id ------------------------------------------------------------------*/
/* Դ�ļ���ʶ ---------------------------------------------------------------*/
#define _SC_C_  0x7A
#define FILE_No _SC_C_

/* Includes -----------------------------------------------------------------*/
/* ��Դ�ļ��漰��ͷ�ļ� -----------------------------------------------------*/

#include  "sc.h"  /* ǿ�������뱾Դ�ļ���Ӧ��ͷ�ļ� */
#include  "at24c.h"
#include  "mcu.h"
#include  "ntc.h"

/* Local Macros & Defines ---------------------------------------------------*/
/* ���غ궨�� ---------------------------------------------------------------*/

#define   SC_MAGIC_CODE         0x5500
#define   SC_ADDRESS_IN_EEPROM  0x0100
#define   SC_UPDATE_COUNT_MAX   50000

/* Local typedefs -----------------------------------------------------------*/
/* �������Ͷ��� -------------------------------------------------------------*/

/* �������ͣ����ڲ������� */
typedef struct
{
  union {
    unsigned char bAll;
    struct{
      unsigned  Loaded:1;
      unsigned  Checked:1;
    }sFlag;
  }uFlag;
}
lt_sSC;

/* Local consts -------------------------------------------------------------*/
/* ���س������� -------------------------------------------------------------*/

/* ��Ʒ��Ϣ��Ĭ�ϳ�ֵ */
const gt_sSC  lc_sSCDefault = {
  //  head
  {
    sizeof(gt_sSC),
    SC_MAGIC_CODE,
    0,
    0,
    0,
    0,
    0,
  },
  //  ��������
  {
    MSCAN_BRP_250K,   //  can baudrate
    BMU_MODE_NORMAL,  //  default mode
  },
  //  ��ѹ�ɼ�
  {
    3,  //  ltc6802 count, if count is 3, it is SY project 
    {
      { 0x01, PN_VS_ENA_A, PN_VS_NCS_A },
      { 0x0C, PN_VS_ENA_B, PN_VS_NCS_B },
    },
  },
  //  �¶Ȳɼ�
  {
    5,  //  adc wait msecs
    512,  //  adc repeat count
    {     //  sensor type
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
      NTC_TYPE_MURATA, NTC_TYPE_MURATA, NTC_TYPE_MURATA,
    },
    6, //  count1
    {   //  list 1
      14, 21, 7, 22, 15, 23,
      1, 10, 2, 5, 7, 11,
      12, 13, 14, 15, 3, 6,
      4, 19, 20, 21, 22, 23,
    },
    24,  // count 2
    {   // list 2
      14, 21, 7, 22, 15, 23,
      1, 10, 2, 5, 7, 11,
      12, 13, 14, 15, 3, 6,
      4, 19, 20, 21, 22, 23,
    },
    1396, //  VTemp25
    3638, //  m hot
    3266, //  m cold
  },
  //  ͳ�Ƽ���
  {
    //  �ο���ѹ
    {
      3330, 3331, 3332, 3333,
      3334, 3335, 3336, 3337,
      3338, 3339, 3340, 3341,
    },
    //24,  //  ��ѹ����
    24,  //  ��ѹ����
    12,   //The cell's number of single Ltc6802 
    //  ��ѹ�б�
    /*{
      0x00, 0x01, 0x02, 0x03,
      0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b,
      0x10, 0x11, 0x12, 0x13,
      0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b,
    },*/
    {
      0x00, 0x01, 0x02, 0x03,
      0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b,
      0x10, 0x11, 0x12, 0x13,
      0x14, 0x15, 0x16, 0x17,
      0x18, 0x19, 0x1a, 0x1b, 
    },
    //  ��ѹУ����
    {
      0,0,0,0,0,0,
      0,0,0,0,0,0,
    },
    //  �ο��¶�
    {
      850, 851, 852, 853, 854, 855,
      860, 861, 862, 863, 864, 865,
      870, 871, 872, 873, 874, 875,
      880, 881, 882, 883, 884, 885,
    },
    24,  //  �¶ȸ���
    12,    //The temp resistor number of single Ltc6802
    //  �¶��б�
    {
      14, 21, 7, 22, 15, 23,
      1, 10, 2, 5, 7, 11,
      12, 13, 14, 15, 3, 6,
      4, 19, 20, 21, 22, 23,
    },
  },
  //  �������
  {
    1,  //  �ײ�����
    20, //  �ײ�����ʱ��
    5,  //  �ײ㿪��ʱ��
    500+400,  //  ���ж��¶�
    600+400,  //  ���ж��¶�
    0,  //  �ⲿ����
    1,  //  �ڲ�����
    30*5, //  �ڲ�����ʱ��
    20*5, //  �ڲ�����ʱ��
    50,   //  �ڲ����⿪�����޵�ѹ
  },
  //  ����Э�����
  {
    1,  //  ���
    1, //  ģ���  bModNum
    20, //  ͬ������ 200ms
    10, //  ͬ����ʱ����
    24,  //  ��ѹ����
    12,   //The cell's number of single Ltc6802
    24,  //  �¶ȸ���
    12,   //The temp resistor number of single Ltc6802
    0,    //fan Status of cell 1 to 12. Start = 1; Stop = 0.
    0,    //fan Status of cell 12 to 24. Start = 1; Stop = 0.
    //  �¶��б�
    {
      16, 8, 17, 0, 18, 9, 0xff, 0xff,
    },    
  },
};

/* Local variables ----------------------------------------------------------*/
/* ���ر������� -------------------------------------------------------------*/

/* ���ڲ������� */
lt_sSC  lv_sSC;

/* Global consts ------------------------------------------------------------*/
/* ȫ�ֳ������� -------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/
/* ȫ�ֱ������� -------------------------------------------------------------*/

gt_sSC  gv_sSC;

/* Local functions ----------------------------------------------------------*/
/* ���غ���ʵ�� -------------------------------------------------------------*/

/**
  * name    lf_nSCLoad
  * bref    ����Ʒ��Ϣ��EEPROM�����뵽RAM��
  * param   none
  * retval  none
  */
int lf_nSCLoad(void)
{
  int res;
  
  /* ��EEPROM */
  res = at24c_init();
  if( res!=OK ) return res;
  
  /* ��ȡEEPROM */
  res = at24c_read(SC_ADDRESS_IN_EEPROM,(void*)&gv_sSC,sizeof(gv_sSC));
  if( res!=OK ) return res;
  
  /* �ر�EEPROM */
  res = at24c_reset();
  if( res!=OK ) return res;
  
  /* ��� */
  lv_sSC.uFlag.sFlag.Loaded = 1;
  return  OK;
}

/**
  * name    lf_nSCCheck
  * bref    ���RAM�еĲ�Ʒ��Ϣ�Ƿ���Ч
  * param   none
  * retval  none
  */
int lf_nSCCheck(void)
{

  /* Length */
  if( gv_sSC.sHead.hLength==0 || gv_sSC.sHead.hLength==0xFFFF ){
    DBG_PRINTF(" ***NG*** Invalid sHead.hLength: %Xh\r\n", gv_sSC.sHead.hLength);
    return  NG;
  }

  /* MAGIC CODE */
  if( gv_sSC.sHead.hMagicCode!=SC_MAGIC_CODE ){
    DBG_PRINTF(" ***NG*** Invalid Magic Code : %Xh\r\n", gv_sSC.sHead.hMagicCode);
    return NG;
  }

  /* CSEven, CSOdd */
  {
    unsigned char cs;
    unsigned short i;
    unsigned char * p;
    p = (unsigned char *)&gv_sSC;
    /* CSEven */
    for(cs=0,i=0;i<gv_sSC.sHead.hLength;i+=2){      
      cs += p[i];
//      DBG_PRINTF("%02X,",p[i]);
    }
    if( cs!=0 ){
      DBG_PRINTF(" ***NG*** CSEven failed: %Xh\r\n", cs);
      return  NG;
    }
    /* CSOdd */
    for(cs=0,i=1;i<gv_sSC.sHead.hLength;i+=2){
      cs += p[i];
//      DBG_PRINTF("%02X,",p[i]);
    }
    if( cs!=0 ){
      DBG_PRINTF(" ***NG*** CSOdd failed: %Xh\r\n", cs);
      return  NG;
    }
  }

  /* ͨ�� */
  lv_sSC.uFlag.sFlag.Checked = 1;
  return  OK;
}

/**
  * name    lf_nSCUpdate
  * bref    ���²�Ʒ��Ϣ��ʱ��ʹ���
  * param   none
  * retval  none
  */
static int lf_nSCUpdate(void)
{
  /* ����ʱ�� */
  DisableInterrupts;
  gv_sSC.sHead.wUpdateTime = rtc_sec;
  EnableInterrupts;
  
  /* ���´��� */
  if( gv_sSC.sHead.hUpdateCount>=SC_UPDATE_COUNT_MAX ){
    DBG_PRINTF(" ***NG*** Update Count Outbreak Limit\r\n");
    return  NG;
  }
  gv_sSC.sHead.hUpdateCount ++;
  
  /* ����ʱ�� */
  if( gv_sSC.sHead.hUpdateCount==1 ){
    gv_sSC.sHead.wCreateTime = gv_sSC.sHead.wUpdateTime;
  }

  /* CS */
  {
    unsigned char cs;
    unsigned short i;
    unsigned char * p;
    p = (unsigned char *)&gv_sSC;
    gv_sSC.sHead.bCSEven = 0;
    gv_sSC.sHead.bCSOdd = 0;
    /* CSEven */
    for(cs=0,i=0;i<gv_sSC.sHead.hLength;i+=2){
      cs -= p[i];
      DBG_PRINTF("%02X,",p[i]);
    }
    gv_sSC.sHead.bCSEven = cs;
    DBG_PRINTF(" - CSEven: %Xh\r\n", cs);
    /* CSOdd */
    for(cs=0,i=1;i<gv_sSC.sHead.hLength;i+=2){
      cs -= p[i];
      DBG_PRINTF("%02X,",p[i]);
    }
    gv_sSC.sHead.bCSOdd = cs;
    DBG_PRINTF(" - CSOdd: %Xh\r\n", cs);
  }

  /* ��� */
  return  OK;
}

/**
  * name    lf_nSCSave
  * bref    ��RAM�еĲ�Ʒ��Ϣ���浽EEPROM��
  * param   none
  * retval  none
  */
static int lf_nSCSave(void)
{
  int res;
  
  /* ��EEPROM */
  res = at24c_init();
  if( res!=OK ) return res;
  
  /* д��EEPROM */
  res = at24c_write(SC_ADDRESS_IN_EEPROM,(void*)&gv_sSC,sizeof(gv_sSC));
  if( res!=OK ) return res;
  
  /* �ر�EEPROM */
  res = at24c_reset();
  if( res!=OK ) return res;
  
  /* ��� */
  return  OK;
}

/* Global functions ---------------------------------------------------------*/
/* ȫ�ֺ���ʵ�� -------------------------------------------------------------*/

/**
  * name    gf_nSCInit
  * bref    ��ʼ��
  * param   none
  * retval  none
  */
int gf_nSCInit(void)
{
  DBG_PRINTF("gf_nSCInit()\r\n");

  /* ��ʼ�� */
  lv_sSC.uFlag.bAll = 0;
  /* ��� */
  return  OK;
}

/**
  * name    gf_nSCLoad
  * bref    ����Ʒ��Ϣ��EEPROM�����뵽RAM��
  * param   none
  * retval  none
  */
int gf_nSCLoad(void)
{
#if 0
  int res;
  DBG_PRINTF("gf_nSCLoad()\r\n");
  
  /* ��EEPROM�������Ʒ��Ϣ */
  res = lf_nSCLoad(); if( res!=OK ) return res;

  /* ���EEPROM��û�в�Ʒ��Ϣ������Ҫ��ʼ�� */
  if( lf_nSCCheck()==NG ){  
    /* ���ó�ʼֵ */
    gv_sSC = lc_sSCDefault;
    /* ���� */
    res = lf_nSCUpdate(); if( res!=OK ) return res;
    /* ���浽EEPROM */
    res = lf_nSCSave(); if( res!=OK ) return res;
    /* ��� */
    DBG_PRINTF(" - OK: SC Set Default\r\n");
    /* ���ÿ��Ź����� */    
    for(;;);
  }
#else
  gv_sSC = lc_sSCDefault;
#endif

  /* ��� */
  DBG_PRINTF(" - OK: SC Loaded: %d\r\n", gv_sSC.sHead.hUpdateCount);
  return  OK;
}

/**
  * name    gf_nSCSave
  * bref    ����Ʒ��Ϣ���浽EEPROM��
  * param   none
  * retval  none
  */
int gf_nSCSave(void) {

  int res;
  DBG_PRINTF("gf_nSCSave()\r\n");

  /* δ����ʱ���ܲ��� */
  if( lv_sSC.uFlag.sFlag.Loaded==0 ){
    DBG_PRINTF(" ***NG*** SC Not Loaded\r\n");
    return NG;
  }
  
  /* ����δͨ�����ܲ��� */
  if( lv_sSC.uFlag.sFlag.Checked==0 ){
    DBG_PRINTF(" ***NG*** SC Not Checked\r\n");
    return  NG;
  }
  
  /* ���²�Ʒ��Ϣ */
  res = lf_nSCUpdate();
  if( res!=OK ) return res;
  
  /* ���浽EEPROM */
  res = lf_nSCSave();
  if( res!=OK ) return res;
  
  /* ��� */
  DBG_PRINTF(" - OK: SC Save\r\n");
  return  OK;
}

