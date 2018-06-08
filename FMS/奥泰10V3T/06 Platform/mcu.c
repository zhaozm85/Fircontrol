/**
  ******************************************************************************
  * proj    firmware startup project - mc9s08dz60
  * file    mcu.c
  * ver     1.0
  * brief   This is a template file for all C files.
  ------------------------------------------------------------------------------
  * 2012.02 created by taoyu@bakai.com
  */ 

/* File Id -------------------------------------------------------------------*/
#define _MCU_C_ 0x62
#define FILE_No _MCU_C_

/* Includes ------------------------------------------------------------------*/
#include  "mcu.h"  /* mandotary */
#include  <hidef.h> /* for EnableInterrupts macro */
#include  "derivative.h" /* include peripheral declarations */
#include  "log.h"

/* Private Macros & Defines --------------------------------------------------*/

#ifndef DEBUG_MCU_LED
#define DEBUG_MCU_LED   0
#endif

#ifndef DEBUG_MCU_CLOCK
#define DEBUG_MCU_CLOCK 0 /* 3 leds : 1/2/4/8 */
#endif

#ifndef DEBUG_MCU_TICK
#define DEBUG_MCU_TICK  0 /* 3 leds : 5/A */
#endif

#ifndef DEBUG_MCU_UART
#define DEBUG_MCU_UART  0
#endif

#ifndef DEBUG_MCU_IIC
#define DEBUG_MCU_IIC   0
#endif

#ifndef DEBUG_MCU_SPI
#define DEBUG_MCU_SPI   0
#endif

#ifndef DEBUG_MCU_ADC
#define DEBUG_MCU_ADC   0
#endif

#if DEBUG_MCU_IIC
#define IIC_PRINTF  PRINTF
#else
#define IIC_PRINTF  empty_printf
#endif

#if DEBUG_MCU_SPI
#define SPI_PRINTF  PRINTF
#else
#define SPI_PRINTF  empty_printf
#endif

#if DEBUG_MCU_ADC
#define ADC_PRINTF  PRINTF
#else
#define ADC_PRINTF  empty_printf
#endif

#define PIN_0 0x01
#define PIN_1 0x02
#define PIN_2 0x04
#define PIN_3 0x08
#define PIN_4 0x10
#define PIN_5 0x20
#define PIN_6 0x40
#define PIN_7 0x80

#define HIGH  1
#define LOW   0

#define SPID_DUMMY 0xCD

/* Private typedefs ----------------------------------------------------------*/

typedef struct
{
  unsigned char * pD;
  unsigned char Pin;
  unsigned int  Data:1;
  unsigned char * pDD;
}
pin_cfg_t;

typedef void (*FUNC)(void);

/* Private consts ------------------------------------------------------------*/

const pin_cfg_t pin_cfg[PN_MAX] = 
{
 { &PTDD, PIN_2, HIGH, &PTDDD },  //  blue led, D2
 { &PTDD, PIN_3, HIGH, &PTDDD },  //  green led, D3
 { &PTDD, PIN_4, HIGH, &PTDDD },  //  red led, D4
 { &PTED, PIN_2, HIGH, &PTEDD },  //  spi ss, E2
 { &PTGD, PIN_2, HIGH, &PTGDD },  //  ltc680x ena, G2
 { &PTGD, PIN_4, HIGH, &PTGDD },  //  eeprom wp, G4
 { &PTGD, PIN_5, HIGH, &PTGDD },  //  can stb, G5
 { &PTDD, PIN_0, LOW , &PTDDD },  //  fan ena, D0
 { &PTDD, PIN_1, LOW , &PTDDD },  //  heat ena, D1
 // ENA-A, NCS-A
 { &PTFD, PIN_0, HIGH, &PTFDD },  //  ENA-A F0
 { &PTED, PIN_2, HIGH, &PTEDD },  //  NCS-A E2
 // ENA-B, NCS-B
 { &PTFD, PIN_1, HIGH, &PTFDD },  //  ENA-B F1
 { &PTGD, PIN_2, HIGH, &PTGDD },  //  NCS-B G2
};

const char illegal_instructions[] = { 0x8D, 0xAC };
const FUNC reset_func = (FUNC)(void*)&illegal_instructions;

/* Private variables ---------------------------------------------------------*/

unsigned char lvw_dis = 200;  //  0~255

//mscan_rxframe_t mscan_rxframe  @ 0x000018A0;
mscan_txframe_t mscan_txframe  @ 0x000018B0;

/* Private function prototypes -----------------------------------------------*/

static void init_write_once(void);
static void init_xtal(void);
static void init_watchdog(void);
static void init_tick(void);
static void init_pins(void);

/* Global consts -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

unsigned long tick;
unsigned long dec_tick;
unsigned long rtc_sec;

/* Functions -----------------------------------------------------------------*/

void  empty_printf(char* fmt, ...)
{
  (void)fmt;
}

interrupt void fault_isr(void){
  LED_ISR_FAULT = LED_ON;
  PRINTF("fault isr\r\n");
}

/**
  * bref    template function
  * param   none
  * retval  none
  */

static void init_write_once(void)
{
  /* Common initialization of the write once registers */
  /* SOPT1: COPT=3,STOPE=0,SCI2PS=0,IICPS=0 */
  SOPT1 = 0xC0;                                      
  /* SOPT2: COPCLKS=0,COPW=0,ADHTS=0,MCSEL=0 */
  SOPT2 = 0x00;                                      
  /* SPMSC1: LVWF=0,LVWACK=0,LVWIE=1,LVDRE=0,LVDSE=1,LVDE=1,BGBE=0 */
  SPMSC1 = 0x2C;                                      
  /* SPMSC2: LVDV=1,LVWV=1,PPDF=0,PPDACK=0,PPDC=0 */
  SPMSC2 = 0x30;                                      
  /*  result
   *    - watch dog time out : 1024ms
   *    - lvd disabled, lvw enabled
   */
}

interrupt void lvw_isr(void)
{
  //  clear flag
  SPMSC1_LVWACK = 1;
  //  disabled
  if( lvw_dis ){
    DBG_PRINTF("lvw_dis %d\r\n", lvw_dis);
    return;
  }
  //  all led off
  LED_RED = LED_OFF;
  LED_GREEN = LED_OFF;
  LED_BLUE = LED_OFF;
  //  all devices off
  (void)pin_high(PN_LTC680X_ENA_N);
  (void)pin_high(PN_CAN_STB);
  (void)pin_low(PN_FAN_ENA);
  (void)pin_low(PN_HEAT_ENA);
  //  log exit
  feed_watch_dog();
//  (void)log_exit();
  LED_RED = LED_ON;
  DBG_PRINTF("lvw %u\r\n", tick);
  for(;;);
}

static void init_xtal(void)
{

  /* ### lit led before init xtal */
  /* PTDD: PTDD4=1,PTDD3=1,PTDD2=1 */
  PTDD &= (unsigned char)~0x1C;
  /* PTDPE: PTDPE4=0,PTDPE3=0,PTDPE2=0 */
  PTDPE &= (unsigned char)~0x1C;                     
  /* PTDDD: PTDDD4=1,PTDDD3=1,PTDDD2=1 */
  PTDDD |= (unsigned char)0x1C;

#if DEBUG_MCU_LED
  for(;;){    

    LED_RED = LED_ON;
    LED_RED = LED_OFF;

    LED_GREEN = LED_ON;
    LED_GREEN = LED_OFF;

    LED_BLUE = LED_ON;
    LED_BLUE = LED_OFF;

  }
#endif

  /*  System clock initialization */
  if (*(unsigned char*far)0xFFAF != 0xFF) { /* Test if the device trim value is stored on the specified address */
    MCGTRM = *(unsigned char*far)0xFFAF; /* Initialize MCGTRM register from a non volatile memory */
    MCGSC = *(unsigned char*far)0xFFAE; /* Initialize MCGSC register from a non volatile memory */
  }
  /* MCGC2: BDIV=0,RANGE=1,HGO=1,LP=0,EREFS=1,ERCLKEN=1,EREFSTEN=0 */
  MCGC2 = 0x36;                        /* Set MCGC2 register */
  /* MCGC1: CLKS=2,RDIV=7,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
  MCGC1 = 0xB8;                        /* Set MCGC1 register */
  while(!MCGSC_OSCINIT) {              /* Wait until external reference is stable */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }
  while(MCGSC_IREFST) {                /* Wait until external reference is selected */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }
  while((MCGSC & 0x0C) != 0x08) {      /* Wait until external clock is selected as a bus clock reference */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }
  /* MCGC2: BDIV=0,RANGE=1,HGO=1,LP=1,EREFS=1,ERCLKEN=1,EREFSTEN=0 */
  MCGC2 = 0x3E;                        /* Set MCGC2 register */
  /* MCGC1: CLKS=2,RDIV=2,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
  MCGC1 = 0x90;                        /* Set MCGC1 register */
  /* MCGC3: LOLIE=0,PLLS=1,CME=0,VDIV=4 */
  MCGC3 = 0x44;                        /* Set MCGC3 register */
  /* MCGC2: LP=0 */
  MCGC2 &= (unsigned char)~0x08;                     
  while(!MCGSC_PLLST) {                /* Wait until PLL is selected */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }
  while(!MCGSC_LOCK) {                 /* Wait until PLL is locked */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }
  /* MCGC1: CLKS=0,RDIV=2,IREFS=0,IRCLKEN=0,IREFSTEN=0 */
  MCGC1 = 0x10;                        /* Set MCGC1 register */
  while((MCGSC & 0x0C) != 0x0C) {      /* Wait until PLL clock is selected as a bus clock reference */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }
  /*  result:
   *  - xtal      8MHz
   *  - bus       16MHz
   *  - internal  enabled
   */

  /* test clock */
#if DEBUG_MCU_CLOCK

  for(;;){

    #define TEST_TICKS  65535
    volatile unsigned short tick;
    
    feed_watch_dog();

    tick = TEST_TICKS;
    while(--tick>0);
    LED_RED = LED_ON;
    LED_GREEN = LED_OFF;
    LED_BLUE = LED_OFF;

    tick = TEST_TICKS;
    while(--tick>0);
    LED_RED = LED_OFF;
    LED_GREEN = LED_ON;
    LED_BLUE = LED_OFF;

    tick = TEST_TICKS;
    while(--tick>0);
    LED_RED = LED_OFF;
    LED_GREEN = LED_OFF;
    LED_BLUE = LED_ON;

  }


#endif

}

static void init_watchdog(void)
{
  /* init watch dog (COP) is done in init_write_once() */
  /* ### Init_COP init code */
  SRS = 0x55;                          /* Clear WatchDog counter - first part */
  SRS = 0xAA;                          /* Clear WatchDog counter - second part */
}

interrupt void rtc_isr(void)
{
  /* begin */
  LED_ISR_RTC = LED_ON;
  /* clear interrupt flag */
  RTCSC_RTIF = 1;
  /* tick up */
  tick++;
  /* tick down */
  if( dec_tick>0 ) dec_tick--;
  /* lvw disable */
  if( lvw_dis>0 ) lvw_dis--;
  /* rtc sec */
  if( rtc_sec>0 ){
    static unsigned char cnt = 0;
    cnt ++;
    if( cnt==100 ){
      cnt = 0;
      rtc_sec ++;
    }
  }
  /* end */
  LED_ISR_RTC = LED_OFF;
}

static void init_tick(void)
{
  tick = 0;
  dec_tick = 0;
  rtc_sec = 0;
  /* ### Init_RTC init code */
  /* RTCMOD: RTCMOD=0x4F */
  RTCMOD = 0x4F;                       /* Set modulo register */
  /* RTCSC: RTIF=1,RTCLKS=1,RTIE=1,RTCPS=8 */
  RTCSC = 0xB8;                        /* Configure RTC */
  /*  result
   *    interrupt every 10 ms
   */

#if DEBUG_MCU_TICK

  /* enable interrupts */
  EnableInterrupts;
  for(;;){

    dec_tick = 50;
    while(dec_tick>0) feed_watch_dog();
    LED_RED = LED_ON;

    dec_tick = 50;
    while(dec_tick>0) feed_watch_dog();
    LED_RED = LED_OFF;

  }
#endif

}

static void bit_clr(unsigned char * pReg, unsigned char mask)
{
  unsigned char tmp;
  tmp = *pReg;
  tmp &= ~mask;
  *pReg = tmp;
}

static void bit_set(unsigned char * pReg, unsigned char mask)
{
  unsigned char tmp;
  tmp = *pReg;
  tmp |= mask;
  *pReg = tmp;
}

static int bit_get(unsigned char * pReg, unsigned char mask)
{
  unsigned char tmp;
  tmp = *pReg;
  if( tmp&mask ) return 1;
  return 0;
}

static void init_pins(void)
{
  int i;
  
  /* io pins all default are input */
  PTADD = 0x00;
  PTBDD = 0x00;
  PTCDD = 0x00;
  PTDDD = 0x00;
  PTEDD = 0x00;
  PTFDD = 0x00;
  PTGDD = 0x00;

  /* io pins pull up, default on, to make io pins at stable status */
  PTAPE = 0xFF;
  PTBPE = 0xFF;
  PTCPE = 0xFF;
  PTDPE = 0xFF;
  PTEPE = 0xFF;
  PTFPE = 0xFF;
  PTGPE = 0xFF;
  
  /* io pins slew rate, default off */
  PTASE = 0x00;                                      
  PTBSE = 0x00;                                      
  PTCSE = 0x00;                                      
  PTDSE = 0x00;                                      
  PTESE = 0x00;
  PTFSE = 0x00;                                      
  PTGSE = 0x00;

  /* io pins drive strength, default off */
  PTADS = 0x00;                                      
  PTBDS = 0x00;                                      
  PTCDS = 0x00;                                      
  PTDDS = 0x00;                                      
  PTEDS = 0x00;                                      
  PTFDS = 0x00;                                      
  PTGDS = 0x00;
  
  /* detailed setting for each output io pin in use */
  for(i=0;i<PN_MAX;i++){
    /*  data  */
    if( pin_cfg[i].Data==HIGH ) bit_set(pin_cfg[i].pD, pin_cfg[i].Pin);
    else bit_clr(pin_cfg[i].pD, pin_cfg[i].Pin);
    /*  set direction to output */
    bit_set(pin_cfg[i].pDD, pin_cfg[i].Pin);
  }

}

static void init_sci(void)
{

  // BD: 38400
  /* ### Init_SCI init code */
  /* SCI1C2: TIE=0,TCIE=0,RIE=0,ILIE=0,TE=0,RE=0,RWU=0,SBK=0 */
  SCI1C2 = 0x00;                       /* Disable the SCI1 module */
  (void)(SCI1S1 == 0);                 /* Dummy read of the SCI1S1 register to clear flags */
  (void)(SCI1D == 0);                  /* Dummy read of the SCI1D register to clear flags */
  /* SCI1S2: LBKDIF=1,RXEDGIF=1,RXINV=0,RWUID=0,BRK13=0,LBKDE=0,RAF=0 */
  SCI1S2 = 0xC0;                                      
  /* SCI1BDH: LBKDIE=0,RXEDGIE=0,SBR12=0,SBR11=0,SBR10=0,SBR9=0,SBR8=0 */
  SCI1BDH = 0x00;                                      
  /* SCI1BDL: SBR7=0,SBR6=0,SBR5=0,SBR4=1,SBR3=1,SBR2=0,SBR1=1,SBR0=0 */
  SCI1BDL = 0x1A;                                      
  /* SCI1C1: LOOPS=0,SCISWAI=0,RSRC=0,M=0,WAKE=0,ILT=0,PE=0,PT=0 */
  SCI1C1 = 0x00;                                      
  /* SCI1C3: R8=0,T8=0,TXDIR=1,TXINV=0,ORIE=0,NEIE=0,FEIE=0,PEIE=0 */
  SCI1C3 = 0x20;                                      
  /* SCI1C2: TIE=0,TCIE=0,RIE=0,ILIE=0,TE=1,RE=0,RWU=0,SBK=0 */
  SCI1C2 = 0x08;                                      
  
#if DEBUG_MCU_UART
  for(;;){
    (void)printf("U");
    feed_watch_dog();
  }
#endif
  DBG_PRINTF("uart init done\r\n");

}

void TERMIO_PutChar(char ch)
{
//  feed_watch_dog();
  while( SCI1S1_TDRE==0 );
  SCI1D = ch;
//  (void)pin_toggle(PN_LED_BLUE_N);
}

void reset(void)
{
  DBG_PRINTF("reset()\r\n...");
  reset_func();
}

void init(void)
{

  /* disable interrupts */
  DisableInterrupts;

  /* init write once registers */
  init_write_once();

  /* init xtal to 24MHz */
  init_xtal();

  /* init io pins, turn off leds */
  init_pins();
  
  /* init uart for printf */
  init_sci();

  /* init watch dog timer */
  init_watchdog();

  /* init tick */
  init_tick();

  /* enable interrupts */
  EnableInterrupts;

}

void feed_watch_dog(void)
{  
 /* feeds the dog */
  __RESET_WATCHDOG();
}

void delay_ms(unsigned int ms)
{
  unsigned int i;
//  LED_RED = LED_ON;
  for(i=0;i<ms;i++){
    unsigned int j;
    for(j=0;j<571;j++){ /* 10ms -> 175ms if j<10000 */
      feed_watch_dog();
    }
  }
//  LED_RED = LED_OFF;
}

void halt(void)
{
  for(;;) feed_watch_dog();
}

int pin_high(gt_ePinNumber pn)
{
  if( pn<PN_MAX ){
    bit_set(pin_cfg[pn].pD, pin_cfg[pn].Pin);
    return 1;
  }
  return -1;
}

int pin_low(gt_ePinNumber pn)
{
  if( pn<PN_MAX ){
    bit_clr(pin_cfg[pn].pD, pin_cfg[pn].Pin);
    return 1;
  }
  return -1;
}

int pin_toggle(gt_ePinNumber pn)
{
  if( pn<PN_MAX ){
    if( bit_get(pin_cfg[pn].pD, pin_cfg[pn].Pin) ){
      bit_clr(pin_cfg[pn].pD, pin_cfg[pn].Pin);
    }else{
      bit_set(pin_cfg[pn].pD, pin_cfg[pn].Pin);
    }
  }
  return -1;
}

int iic_init(void)
{
  // 100Kbps
  /* ### Init_IIC init code */
  /* IICC2: GCAEN=0,ADEXT=0,AD10=0,AD9=0,AD8=0 */
  IICC2 = 0x00;                                      
  /* IICA: AD7=0,AD6=0,AD5=0,AD4=0,AD3=0,AD2=0,AD1=0 */
  IICA = 0x00;                                      
  /* IICF: MULT1=1,MULT0=0,ICR5=0,ICR4=0,ICR3=0,ICR2=1,ICR1=1,ICR0=1 */
  IICF = 0x87;                                      
  /* IICS: TCF=1,IAAS=0,BUSY=0,ARBL=1,SRW=0,IICIF=1,RXAK=0 */
  IICS = 0x92;                         /* Clear the interrupt flags */
  /* IICC: IICEN=1 */
  IICC |= (unsigned char)0x80;                      
  /* IICC: IICEN=1,IICIE=0,MST=0,TX=1,TXAK=0,RSTA=0 */
  IICC = 0x90;                                      

  /* done */
  IIC_PRINTF("iic init done\r\n");
  return  OK;
}

int iic_reset(void)
{
  IICC = 0x00;
  IICC2 = 0x00;
  return  OK;
}

int iic_xfer(xfer_t * x)
{

  unsigned short i;
  unsigned char iics;

  /* check parameters */
  if( x==0 ){
    PERRNO(ERRNO_INVALID_PARAMETERS,0);
    return NG;
  }
  if( x->txcnt==0 && x->rxmax==0 ){
    PERRNO(ERRNO_INVALID_PARAMETERS,0);
    return NG;
  }

  /* feed watch dog incase timeout */
  feed_watch_dog();

  /* check iic bus status */
  iics = IICS;
  IIC_PRINTF("%02X\t", iics);
  if( iics==0x20) {
    (void)iic_reset();
    (void)iic_init();
    IIC_PRINTF("iic bus error !!!\r\n");
    return -1;
  }

  /* before tx start */
  IICC_TXAK = 0;

  /* generate a start */
  IIC_PRINTF("S");
  IICC |= 0x30;   // And generate START condition;
  
  /* tx */
  if( x->txcnt>0 ){

    /* send address, with WRITE direction */
    IIC_PRINTF("T");
    IICD = (x->devadd<<1);    // Address the slave and set up for master transmit;
    while (!IICS_IICIF);  // wait until IBIF;
    IICS_IICIF=1; // clear the interrupt event flag;
    while(IICS_RXAK); // check for RXAK;

    /* send data buffer */  
    for(i=0;i<x->txcnt;i++){
      IIC_PRINTF(">");
      IICD = x->txbuf[i];    // Address the slave and set up for master transmit;
      IIC_PRINTF("%02X",x->txbuf[i]);
      while (!IICS_IICIF);  // wait until IBIF;
      IICS_IICIF=1; // clear the interrupt event flag;
      while(IICS_RXAK); // check for RXAK;
    }

    /* stop of write */
    if( x->rxmax==0 ){
      IIC_PRINTF("P");
      IICC_MST = 0;
    }
    /* restart for read */
    else{
      IIC_PRINTF("s");
      IICC_RSTA = 1;
    }

  }

  /* rx */
  if( x->rxmax>0 ){
  
    /* send address, with READ direction */
    IIC_PRINTF("T");
    IICD = (x->devadd<<1)+1;    // Address the slave and set up for master transmit;
    while (!IICS_IICIF);  // wait until IBIF;
    IICS_IICIF=1; // clear the interrupt event flag;
    while(IICS_RXAK); // check for RXAK;

    /* start first byte */
    IICC_TX = 0;
    IICC_TXAK = 0;

    IIC_PRINTF("R");
    x->rxbuf[0] = IICD; //  dummy read to start receive

    /* receive data bytes */
    for(i=0;i<x->rxmax-1;i++){

      /* wait done */
      while( !IICS_IICIF );
      IICS_IICIF = 1; //  clear

      /* set next ack */
      if( i==x->rxmax-2 ) IICC_TXAK = 1;
      /* read and save data and start the next */
      IIC_PRINTF("<");
      x->rxbuf[i] = IICD;
      IIC_PRINTF("%02X",x->rxbuf[i]);

    }

    /* receive last byte */
    while( !IICS_IICIF );
    IICS_IICIF = 1;
    /* stop */
    IIC_PRINTF("<");
    IICC_MST = 0;
    /* save last data */
    x->rxbuf[i] = IICD;
    IIC_PRINTF("%02X",x->rxbuf[i]);
    IIC_PRINTF("P");

  }
  
  IIC_PRINTF("\r\n");

  /* done */
  return  OK;
}

int  mscan_init(mscan_config_t* f){
  (void)f;

  /* ### Init_MSCAN init code */
  /* CANCTL0: INITRQ=1 */
  CANCTL0_INITRQ = 1;
  while(!CANCTL1_INITAK) {             /* Wait for init acknowledge */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }

  /* CANCTL1: CANE=1,CLKSRC=0,LOOPB=0,LISTEN=0,BORM=0,WUPM=0,SLPAK=0,INITAK=1 */
       // can enable, clock source is 8MHz xtal, no loop back
       // no listen mode, auto bus off recovery, wake up on any dominant
  CANCTL1 = 0x81;
  /* CANBTR1: SAMP=1,TSEG22=0,TSEG21=1,TSEG20=1,TSEG13=1,TSEG12=0,TSEG11=1,TSEG10=0 */
      //  3 sample per bit, tseg2 is 3(4Tq), tseg 1 is 10(11Tq)
  CANBTR1 = 0xBA;                                      
  /* CANBTR0: SJW1=0,SJW0=1,BRP5=0,BRP4=0,BRP3=0,BRP2=0,BRP1=0,BRP0=1 */
      //  sjw is 1(2Tq), BRP is 1(2)
  CANBTR0 = 0x41;                                      
      //  default can bitrate is 8MHz/16/2 = 250K
  CANBTR0_BRP = f->brp;
//  PRINTF("CANBTR0: %02X\r\n", CANBTR0);


  /* CANIDAC: IDAM1=0,IDAM0=0,IDHIT2=0,IDHIT1=0,IDHIT0=0 */
      //  2 32-bit filters
  CANIDAC = 0x00;
                                        
  CANIDAC_IDAM = f->idam;
//  PRINTF("CANIDAC: %02X\r\n", CANIDAC);

  CANIDAR0 = f->idar[0];
  CANIDAR1 = f->idar[1];
  CANIDAR2 = f->idar[2];
  CANIDAR3 = f->idar[3];
  CANIDAR4 = f->idar[4];
  CANIDAR5 = f->idar[5];
  CANIDAR6 = f->idar[6];
  CANIDAR7 = f->idar[7];
  
  CANIDMR0 = f->idmr[0];
  CANIDMR1 = f->idmr[1];
  CANIDMR2 = f->idmr[2];
  CANIDMR3 = f->idmr[3];
  CANIDMR4 = f->idmr[4];
  CANIDMR5 = f->idmr[5];
  CANIDMR6 = f->idmr[6];
  CANIDMR7 = f->idmr[7];

  /* CANCTL0: INITRQ=0 */
  CANCTL0_INITRQ = 0;
  while(CANCTL1_INITAK) {              /* Wait for init exit */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }

  /* CANCTL0: RXFRM=0,RXACT=0,CSWAI=0,SYNCH=0,TIME=0,WUPE=0,SLPRQ=0,INITRQ=0 */
  CANCTL0 = 0x00;                                      
  /* CANRIER: WUPIE=0,CSCIE=0,RSTATE1=0,RSTATE0=0,TSTATE1=0,TSTATE0=0,OVRIE=0,RXFIE=0 */
  CANRIER = 0x00;                                      
  /* CANTIER: TXEIE2=0,TXEIE1=0,TXEIE0=0 */
  CANTIER = 0x00;                                      

  /* enable can rx interrupt */
  CANRIER_RXFIE = 1;

  return 1;
}

int  mscan_reset(void){

  /* disable interrupts */
  /* CANRIER: WUPIE=0,CSCIE=0,RSTATE1=0,RSTATE0=0,TSTATE1=0,TSTATE0=0,OVRIE=0,RXFIE=0 */
  CANRIER = 0x00;                                      
  /* CANTIER: TXEIE2=0,TXEIE1=0,TXEIE0=0 */
  CANTIER = 0x00;                                      
  
  CANCTL1_CANE = 0; /* write once in normal */

  return 1;
}

int  mscan_xmit(mscan_txframe_t* f){

  unsigned char tbsel;

  /* select transmit buffer */
  CANTBSEL = CANTFLG; /* LOAD CANTFLG, WRITE TO CANTBSEL */
  tbsel = CANTBSEL;   /* read CANTBSEL */

  /* no xmit buffer */
  if( tbsel==0 ) return -1;

  /* copy buffer */
  mscan_txframe = *f;

  /* start transmit */
  CANTFLG = tbsel;

  /* ok */
  return tbsel;

}

interrupt void mscan_rx_isr(void){
  /* begin */
  LED_ISR_MSCAN_RX = LED_ON;

  /* mscan rx call back */
  (void)mscan_rxcb(&mscan_rxframe);

  /* reset rx flag for next frame */
  CANRFLG_RXF = 1;

  /* end */
  LED_ISR_MSCAN_RX = LED_OFF;
}

int  spi_init(void){
  // 250kbps
   /* ### Init_SPI init code */
  /* SPIC1: SPIE=0,SPE=0,SPTIE=0,MSTR=0,CPOL=0,CPHA=0,SSOE=0,LSBFE=0 */
  SPIC1 = 0x00;                        /* The SPRF interrupt flag is cleared when the SPI module is disabled. */
  /* SPIC2: MODFEN=0,BIDIROE=0,SPISWAI=0,SPC0=0 */
  SPIC2 = 0x00;                                      
  /* SPIBR: SPPR2=0,SPPR1=1,SPPR0=1,SPR2=0,SPR1=1,SPR0=1 */
  SPIBR = 0x33;                                      
  (void)(SPIS == 0);                   /* Dummy read of the SPIS registr to clear the MODF flag */
  /* SPIC1: SPIE=0,SPE=1,SPTIE=0,MSTR=1,CPOL=1,CPHA=1,SSOE=0,LSBFE=0 */
  SPIC1 = 0x5C;                                        

  return 1;
}

int  spi_xfer(xfer_t* x){
  unsigned short i;
  unsigned char spid;

  /* feed watch dog */
  feed_watch_dog();

  SPI_PRINTF("SPI TX: ");

  /* tx */
  for(i=0;i<x->txcnt;i++){
    /* send data */
    SPI_PRINTF("%02X ", x->txbuf[i]);
    SPID = x->txbuf[i];
    /* wait done */
    while( SPIS_SPRF==0 );    
    spid = SPID;  /* read spid to clear sprf */
  }

  SPI_PRINTF(", RX: ");

  /* rx */
  for(i=0;i<x->rxmax;i++){
    /* send data */
    SPID = SPID_DUMMY;
    /* wait done */
    while( SPIS_SPRF==0 );    
    spid = SPID;  /* read spid to clear sprf */
    /* save data */
    x->rxbuf[i] = spid;
    SPI_PRINTF("%02X ", x->rxbuf[i]);
  }
  
  SPI_PRINTF("\r\n");

  return 1;
}

int  spi_reset(void){
  SPIC1 = 0x00;
  SPIC2 = 0x00;
  SPIBR = 0x00;
  return 1;
}

int  adc_init(void){
  /* ### Init_ADC init code */
  /* SOPT2: ADHTS=0 */
  SOPT2 &= (unsigned char)~0x10;       /* Select the RTC as the source of internal HW trigger */
  /* ADCCFG: ADLPC=0,ADIV1=1,ADIV0=1,ADLSMP=1,MODE1=0,MODE0=1,ADICLK1=0,ADICLK0=0 */
  ADCCFG = 0x74;                                      
  /* ADCCV: ADCV11=0,ADCV10=0,ADCV9=0,ADCV8=0,ADCV7=0,ADCV6=0,ADCV5=0,ADCV4=0,ADCV3=0,ADCV2=0,ADCV1=0,ADCV0=0 */
  ADCCV = 0x00U;                            
  /* ADCSC2: ADACT=0,ADTRG=0,ACFE=0,ACFGT=0 */
  ADCSC2 = 0x00;                                      
  /* ADCSC1: COCO=0,AIEN=0,ADCO=1,ADCH4=1,ADCH3=1,ADCH2=1,ADCH1=1,ADCH0=1 */
  ADCSC1 = 0x3F;                                      

  /*  An ADC channel is configured only before start convertion   */
  APCTL1  = 0x00;
  APCTL2  = 0x00;
  APCTL3  = 0x00;

  return 1;
}

int  adc_conv(unsigned char ch, int delay, int count){

  unsigned short diff, ave;

  /* check ch */
  if( ch==24 || ch==25 || ch==28 || ch>30 ){
    PERRNO(ERRNO_INVALID_PARAMETERS,ch);
    return ERR;
  }

  /* check delay */
  if( delay<0 || delay>1024 ){
    PERRNO(ERRNO_INVALID_PARAMETERS,delay);
    return ERR;
  }

  /* check count */
  if( count<=0 ) count = 1;

  /* close all io pin channels */
  APCTL1  = 0x00;
  APCTL2  = 0x00;
  APCTL3  = 0x00;

  /* open adc channel */
  {
    unsigned char mask;
    mask = 0x01;
    mask <<= (ch & 0x07);
    /* PTA,B,C */
    if( ch<8 ) APCTL1 |= mask;
    else if( ch<16 ) APCTL2 |= mask;
    else if( ch<24 ) APCTL3 |= mask;
  }

  /* delay until stable */
  delay_ms(delay);

  /* do adc */
  {
    int i;
    unsigned short max, min;
    unsigned long sum;
    max = 0;
    min = 0xffff;
    sum = 0;
    /* incase reset */
    feed_watch_dog();
    /* start adc */
    ADCSC1 = 0x20|ch;
    /* adc many times */
    for(i=0;i<count;i++){
      unsigned short adc;

      /* wait done */
      while( ADCSC1_COCO==0 ) feed_watch_dog();

      /* save result and start next */
      adc = ADCR;      

      /* statistics */
      sum += adc;
      if( max < adc ) max = adc;
      if( min > adc ) min = adc;

    }
    /* stop adc */
    ADCSC1 = 0x00|0x1F;
    /* final calc */           
    sum += (count+1)/2;
    ave = (unsigned short)(sum/count);
    diff = max - min;
  }
  
  /* close adc channel */
  APCTL1  = 0x00;
  APCTL2  = 0x00;
  APCTL3  = 0x00;
  
  /* out */
  {
    ADC_PRINTF("adc ch%d %dms %d : ", ch, delay, count);
    ADC_PRINTF("%u,%d\r\n",ave,(int)(diff));
  }

  return ave;

}

int  adc_reset(void){
  return 1;
}

