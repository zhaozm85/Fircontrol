/**
  *****************************************************************************
  * proj    BMU_HM
  * file    pi.c
  * ver     1.0
  * brief   ��Ʒ��Ϣ
  -----------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id ------------------------------------------------------------------*/
/* Դ�ļ���ʶ ---------------------------------------------------------------*/
#define _PI_C_  0x79
#define FILE_No _PI_C_

/* Includes -----------------------------------------------------------------*/
/* ��Դ�ļ��漰��ͷ�ļ� -----------------------------------------------------*/

#include  "pi.h"  /* ǿ�������뱾Դ�ļ���Ӧ��ͷ�ļ� */
#include  "at24c.h"
#include  "mcu.h"

/* Local Macros & Defines ---------------------------------------------------*/
/* ���غ궨�� ---------------------------------------------------------------*/

#define   PI_MAGIC_CODE         0x55AA
#define   PI_ADDRESS_IN_EEPROM  0x0000
#define   PI_UPDATE_COUNT_MAX   50000

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
      unsigned  Unlocked:1;
    }sFlag;
  }uFlag;
}
lt_sPI;

/* Local consts -------------------------------------------------------------*/
/* ���س������� -------------------------------------------------------------*/

/* ��Ʒ��Ϣ��Ĭ�ϳ�ֵ */
const gt_sPI  lc_sPIDefault = {
  {
    sizeof(gt_sPI),
    PI_MAGIC_CODE,
    0,
    0,
    0,
    0,
    0,
  },
  { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF  } // ��Ʒ���к�
};

/* Local variables ----------------------------------------------------------*/
/* ���ر������� -------------------------------------------------------------*/

/* ���ڲ������� */
lt_sPI  lv_sPI;

/* Global consts ------------------------------------------------------------*/
/* ȫ�ֳ������� -------------------------------------------------------------*/

/* Global variables ---------------------------------------------------------*/
/* ȫ�ֱ������� -------------------------------------------------------------*/

gt_sPI  gv_sPI;

/* Local functions ----------------------------------------------------------*/
/* ���غ���ʵ�� -------------------------------------------------------------*/

/**
  * name    lf_nPILoad
  * bref    ����Ʒ��Ϣ��EEPROM�����뵽RAM��
  * param   none
  * retval  none
  */
static int lf_nPILoad(void)
{
#if 0  
  int res;

  /* ��EEPROM */
  res = at24c_init();
  if( res!=OK ) return res;
  
  /* ��ȡEEPROM */
  res = at24c_read(PI_ADDRESS_IN_EEPROM,(void*)&gv_sPI,sizeof(gv_sPI));
  if( res!=OK ) return res;
  
  /* �ر�EEPROM */
  res = at24c_reset();
  if( res!=OK ) return res;
#else
  gv_sPI = lc_sPIDefault;
#endif
  
  /* ��� */
  lv_sPI.uFlag.sFlag.Loaded = 1;
  return  OK;
}

/**
  * name    lf_nPICheck
  * bref    ���RAM�еĲ�Ʒ��Ϣ�Ƿ���Ч
  * param   none
  * retval  none
  */
int lf_nPICheck(void)
{

  /* Length */
  if( gv_sPI.sHead.hLength==0 || gv_sPI.sHead.hLength==0xFFFF ){
    DBG_PRINTF(" ***NG*** Invalid sHead.hLength: %Xh\r\n", gv_sPI.sHead.hLength);
    return  NG;
  }

  /* MAGIC CODE */
  if( gv_sPI.sHead.hMagicCode!=PI_MAGIC_CODE ){
    DBG_PRINTF(" ***NG*** Invalid Magic Code : %Xh\r\n", gv_sPI.sHead.hMagicCode);
    return NG;
  }

  /* CSEven, CSOdd */
  {
    unsigned char cs;
    unsigned short i;
    unsigned char * p;
    p = (unsigned char *)&gv_sPI;
    /* CSEven */
    for(cs=0,i=0;i<gv_sPI.sHead.hLength;i+=2){
      cs += p[i];
//      DBG_PRINTF("%02X,",p[i]);
    }
    if( cs!=0 ){
      DBG_PRINTF(" ***NG*** CSEven failed: %Xh\r\n", cs);
      return  NG;
    }
    /* CSOdd */
    for(cs=0,i=1;i<gv_sPI.sHead.hLength;i+=2){
      cs += p[i];
//      DBG_PRINTF("%02X,",p[i]);
    }
    if( cs!=0 ){
      DBG_PRINTF(" ***NG*** CSOdd failed: %Xh\r\n", cs);
      return  NG;
    }
  }

  /* ͨ�� */
  lv_sPI.uFlag.sFlag.Checked = 1;
  return  OK;
}

/**
  * name    lf_nPIUnlock
  * bref    ƥ������������޸�
  * param   none
  * retval  none
  */
static int lf_nPIUnlock(unsigned char key)
{
  int i;

  /* ƥ�� */
  for(i=0;i<8;i++) key += gv_sPI.baSN[i];
  if( key!=0 ){
    DBG_PRINTF(" ***NG*** Unmatched Key: %Xh\r\n", key);
    return NG;
  }

  /* ͨ�� */
  lv_sPI.uFlag.sFlag.Unlocked = 1;
  return  OK;
}

/**
  * name    lf_nPIUpdate
  * bref    ���²�Ʒ��Ϣ��ʱ��ʹ���
  * param   none
  * retval  none
  */
static int lf_nPIUpdate(void)
{
  /* ����ʱ�� */
  DisableInterrupts;
  gv_sPI.sHead.wUpdateTime = rtc_sec;
  EnableInterrupts;
  
  /* ���´��� */
  if( gv_sPI.sHead.hUpdateCount>=PI_UPDATE_COUNT_MAX ){
    DBG_PRINTF(" ***NG*** Update Count Outbreak Limit\r\n");
    return  NG;
  }
  gv_sPI.sHead.hUpdateCount ++;
  
  /* ����ʱ�� */
  if( gv_sPI.sHead.hUpdateCount==1 ){
    gv_sPI.sHead.wCreateTime = gv_sPI.sHead.wUpdateTime;
  }

  /* CS */
  {
    unsigned char cs;
    unsigned short i;
    unsigned char * p;
    p = (unsigned char *)&gv_sPI;
    gv_sPI.sHead.bCSEven = 0;
    gv_sPI.sHead.bCSOdd = 0;
    /* CSEven */
    for(cs=0,i=0;i<gv_sPI.sHead.hLength;i+=2){
      cs -= p[i];
      DBG_PRINTF("%02X,",p[i]);
    }
    gv_sPI.sHead.bCSEven = cs;
    DBG_PRINTF(" - CSEven: %Xh\r\n", cs);
    /* CSOdd */
    for(cs=0,i=1;i<gv_sPI.sHead.hLength;i+=2){
      cs -= p[i];
      DBG_PRINTF("%02X,",p[i]);
    }
    gv_sPI.sHead.bCSOdd = cs;
    DBG_PRINTF(" - CSOdd: %Xh\r\n", cs);
  }

  /* ��� */
  return  OK;
}

/**
  * name    lf_nPISave
  * bref    ��RAM�еĲ�Ʒ��Ϣ���浽EEPROM��
  * param   none
  * retval  none
  */
static int lf_nPISave(void)
{
  int res;
  
  /* ��EEPROM */
  res = at24c_init();
  if( res!=OK ) return res;
  
  /* д��EEPROM */
  res = at24c_write(PI_ADDRESS_IN_EEPROM,(void*)&gv_sPI,sizeof(gv_sPI));
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
  * name    gf_nPIInit
  * bref    ��ʼ��
  * param   none
  * retval  none
  */
int gf_nPIInit(void)
{
  DBG_PRINTF("gf_nPIInit()\r\n");

  /* ��ʼ�� */
  lv_sPI.uFlag.bAll = 0;
  /* ��� */
  return  OK;
}

/**
  * name    gf_nPILoad
  * bref    ����Ʒ��Ϣ��EEPROM�����뵽RAM��
  * param   none
  * retval  none
  */
int gf_nPILoad(void)
{
  int res;
  DBG_PRINTF("gf_nPILoad()\r\n");
  
  /* ��EEPROM�������Ʒ��Ϣ */
  res = lf_nPILoad(); if( res!=OK ) return res;
#if 0
  /* ���EEPROM��û�в�Ʒ��Ϣ������Ҫ��ʼ�� */
  if( lf_nPICheck()==NG ){  
    /* ���ó�ʼֵ */
    gv_sPI = lc_sPIDefault;
    /* ���� */
    res = lf_nPIUpdate(); if( res!=OK ) return res;
    /* ���浽EEPROM */
    res = lf_nPISave(); if( res!=OK ) return res;
    /* ��� */
    DBG_PRINTF(" - OK: PI Set Default\r\n");
    /* ���ÿ��Ź����� */    
    for(;;);
  }

  /* ���SN��ȫ0xFF�����Զ����� */
  {
    unsigned char i;
    for(i=0;i<8;i++){
      if( gv_sPI.baSN[i]==0xFF ) continue;
      break;
    }
    if(i==8 ){
      lv_sPI.uFlag.sFlag.Unlocked = 1;
      DBG_PRINTF(" - PI Auto Unlocked\r\n");
    }
  }
#endif
  /* ��� */
  DBG_PRINTF(" - OK: PI Loaded: %d\r\n", gv_sPI.sHead.hUpdateCount);
  return  OK;
}

/**
  * name    gf_nPIUnlock
  * bref    ����Ʒ��Ϣ����
  * param   none
  * retval  none
  */
int gf_nPIUnlock(unsigned char key)
{
  int res;
  DBG_PRINTF("gf_nPIUnlock(%Xh)\r\n", key);

  /* δ����ʱ���ܲ��� */
  if( lv_sPI.uFlag.sFlag.Loaded==0 ){
    DBG_PRINTF(" ***NG*** PI Not Loaded\r\n");
    return NG;
  }
  
  /* ����δͨ�����ܲ��� */
  if( lv_sPI.uFlag.sFlag.Checked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Checked\r\n");
    return  NG;
  }
  
  /* ���� */
  res = lf_nPIUnlock(key);
  if( res!=OK ) return res;
  
  /* ��� */
  DBG_PRINTF(" - OK: PI Unlocked\r\n");
  return  OK;
}

/**
  * name    gf_nPISetSN
  * bref    ���ò�Ʒ��Ϣ�е����к�
  * param   none
  * retval  none
  */
int gf_nPISetSN(unsigned char * sn)
{ 
  unsigned char i;
  DBG_PRINTF("gf_nPISetSN(sn)\r\n");

  /* δ����ʱ���ܲ��� */
  if( lv_sPI.uFlag.sFlag.Loaded==0 ){
    DBG_PRINTF(" ***NG*** PI Not Loaded\r\n");
    return NG;
  }
  
  /* ����δͨ�����ܲ��� */
  if( lv_sPI.uFlag.sFlag.Checked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Checked\r\n");
    return  NG;
  }

  /* δ�������ܲ��� */
  if( lv_sPI.uFlag.sFlag.Unlocked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Unlocked\r\n");
    return  NG;
  }
  
  /* ���� */
  for(i=0;i<8;i++) gv_sPI.baSN[i] = sn[i];
  
  /* ��� */
  DBG_PRINTF(" - OK: PI Set SN\r\n");
  return  OK;
}

/**
  * name    gf_nPISave
  * bref    ����Ʒ��Ϣ���浽EEPROM��
  * param   none
  * retval  none
  */
int gf_nPISave(void) {

  int res;
  DBG_PRINTF("gf_nPISave()\r\n");

  /* δ����ʱ���ܲ��� */
  if( lv_sPI.uFlag.sFlag.Loaded==0 ){
    DBG_PRINTF(" ***NG*** PI Not Loaded\r\n");
    return NG;
  }
  
  /* ����δͨ�����ܲ��� */
  if( lv_sPI.uFlag.sFlag.Checked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Checked\r\n");
    return  NG;
  }

  /* δ�������ܲ��� */
  if( lv_sPI.uFlag.sFlag.Unlocked==0 ){
    DBG_PRINTF(" ***NG*** PI Not Unlocked\r\n");
    return  NG;
  }
  
  /* ���²�Ʒ��Ϣ */
  res = lf_nPIUpdate();
  if( res!=OK ) return res;
  
  /* ���浽EEPROM */
  res = lf_nPISave();
  if( res!=OK ) return res;
  
  /* ��� */
  DBG_PRINTF(" - OK: PI Save\r\n");
  return  OK;
}

