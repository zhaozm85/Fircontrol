#include  "mcu.h"
#include  "can.h"
#include  "at24c.h"
#include  "ltc6802.h"

#ifndef TEST
#define TEST  0
#endif

#ifndef TEST_EEPROM
#define TEST_EEPROM 0
#endif

#if TEST
unsigned char buffer[512];
unsigned char txbuf[32];
unsigned char rxbuf[32];
#endif

void  test_eeprom(void)
{

#if TEST_EEPROM

  int page;
  unsigned short addr, txcnt, i;
  unsigned char b;

  PRINTF("test_eeprom()\r\n");

  (void)at24c_init();

  /* clear leds */
  (void)pin_high(PN_LED_RED_N);
  (void)pin_high(PN_LED_GREEN_N);
  (void)pin_high(PN_LED_BLUE_N);

  /* write test data */
  for(addr=0,txcnt=100;addr<0x10000-txcnt;addr+=txcnt,txcnt+=10){

    if( txcnt>400 ) txcnt = 100;

    /* prepare data */
    b = (unsigned char)(addr+txcnt);
    for(i=0;i<txcnt;i++){
      buffer[i] = b-i;
    }
    
    /* write */
    (void)at24c_write(addr,buffer,txcnt);

  }
  
  /* read test data */
  for(addr=0,txcnt=100;addr<0x10000-txcnt;addr+=txcnt,txcnt+=10){

    if( txcnt>400 ) txcnt = 100;

    /* read */
    (void)at24c_read(addr,buffer,txcnt);

    /* check data */
    b = (unsigned char)(addr+txcnt);
    for(i=0;i<txcnt;i++){
      if( buffer[i] != (unsigned char)(b-i) ){
        PRINTF("[E] @ %u:%u - %02X(%02X)\r\n", addr, i, buffer[i], (unsigned char)(b-i) );
      }
    }

  }
  
  /* erase pages */
  for(i=0;i<128;i++) buffer[i] = 0xff;
  for(page=0;page<512;page++){
    PRINTF("e %d\r\n", page);
    (void)pin_toggle(PN_LED_GREEN_N);
//    (void)at24c_write_page(page,buffer);
//    (void)at24c_write(page*128,buffer,128);
  }

  (void)at24c_reset();

  for(;;)feed_watch_dog();

#endif

}

#if TEST_MSCAN
int mscan_rxcb(mscan_rxframe_t* f){
  (void)pin_low(PN_LED_BLUE_N);
  PRINTF("mscan rx: %08X %d\r\n", f->idr.Dword, f->dlr.MergedBits.grpDLC );
  (void)pin_high(PN_LED_BLUE_N);
  return 1;
}
#endif

void  test_mscan(void)
{
#if TEST_MSCAN

  int i;
  mscan_config_t  cfg;
  mscan_txframe_t txf;
  
  txf.idr.Dword = 0;
  txf.dlr.MergedBits.grpDLC = 8;

  /* init can */
  cfg.brp = MSCAN_BRP_250K;
  cfg.idam = MSCAN_IDAM_32x2;
  for(i=0;i<8;i++){
    cfg.idar[i] = 0x00;
    cfg.idmr[i] = 0xFF; //  ignore
  }
  (void)mscan_init(&cfg);
  (void)pin_low(PN_CAN_STB);
  
  /* send can */
  dec_tick = 100;
  for(;;){
    
    /* wait cycle */
    while(dec_tick>0) feed_watch_dog();
    dec_tick = 100;
    
    /* send data */
    (void)pin_low(PN_LED_GREEN_N);
    txf.idr.Dword++;
    txf.idr.Dword |= 0x00101000;
    (void)mscan_xmit(&txf);
    (void)pin_high(PN_LED_GREEN_N);

  }
  
#endif
}

#if TEST_CAN
int can_rxcb(can_frame_t* f){
  char i;
  LED_BLUE = LED_ON;
  PRINTF("can rx: %08LX %d - ", f->id, (int)f->dlc);
  for(i=0;i<8;i++) PRINTF("%02X ", f->data[i]);
  PRINTF("\r\n");
  LED_BLUE = LED_OFF;
  return 1;
}
#endif

void test_can(void){
#if TEST_CAN

  can_frame_t f;
  f.id = 1;
  f.dlc = 8;
  f.data[0] = 2;
  f.data[1] = 3;
  f.data[2] = 4;
  f.data[3] = 5;
  f.data[4] = 6;
  f.data[5] = 7;
  f.data[6] = 8;
  f.data[7] = 9;
  
  /* init can */
  (void)can_init();
  
  dec_tick = 100;
  /* send can */
  for(;;){
  
    //  wait cycle
    while(dec_tick>0) feed_watch_dog();
    dec_tick = 100;
    
    //  send
    LED_GREEN = LED_ON;
    PRINTF("send can %08LX\r\n", f.id);
    (void)can_xmit(&f);
    f.id *= 3;
    f.data[0] *= 3;
    f.data[1] *= 3;
    f.data[2] *= 3;
    f.data[3] *= 3;
    f.data[4] *= 3;
    f.data[5] *= 3;
    f.data[6] *= 3;
    f.data[7] *= 3;
    LED_GREEN = LED_OFF;
    
  }

#endif
}

void test_spi(void){
#if TEST_SPI

  int i;
  xfer_t  x;
  x.txbuf = txbuf;
  txbuf[0] = 0x80|0x01;   //  address
  x.rxbuf = rxbuf;

  /* init spi */
  (void)spi_init();
  
  /* enable ltc6802 */
  (void)pin_low(PN_LTC680X_ENA_N);
  
  dec_tick = 100;
  /* read spi */
  for(;;){

    //  wait cycle
    while(dec_tick>0) feed_watch_dog();
    dec_tick = 100;
    
    //  read
    LED_GREEN = LED_ON;
    (void)pin_low(PN_SPI_CS_N);
    delay_ms(1);
    x.txcnt = 2;
    x.rxmax = 6+1;
    x.txbuf[1] = 0x02;        //  rdcfg
    (void)spi_xfer(&x);
    delay_ms(1);
    (void)pin_high(PN_SPI_CS_N);
    PRINTF("spi rx: ");
    for(i=0;i<x.rxmax;i++) PRINTF("%02X ", x.rxbuf[i] );
    PRINTF("\r\n");
    LED_GREEN = LED_OFF;
    
    //  write
    LED_BLUE = LED_ON;
    (void)pin_low(PN_SPI_CS_N);
    delay_ms(1);
    x.txcnt = 2+6;
    x.rxmax = 0;
    x.rxbuf[0] = 0xE1;
    x.rxbuf[1] = 0x00;
    x.rxbuf[2] = 0xF0;
    x.rxbuf[3] = 0xFF;
    x.rxbuf[4] = 0x00;
    x.rxbuf[5] = 0x00;
    x.txbuf[1] = 0x01;        //  wrcfg
    for(i=0;i<6;i++) x.txbuf[2+i] = x.rxbuf[i];
    (void)spi_xfer(&x);
    delay_ms(1);
    (void)pin_high(PN_SPI_CS_N);
    PRINTF("spi tx: ");
    for(i=0;i<x.txcnt;i++) PRINTF("%02X ", x.txbuf[i] );
    PRINTF("\r\n");
    LED_BLUE = LED_OFF;

  }
  
#endif
}

void test_ltc6802(void){
#if TEST_LTC6802

  static ltc6802_cfgreg_t cfg;
  static ltc6802_cvreg_t cv;
  static ltc6802_tmpreg_t tmp;
  
  (void)ltc6802_init();
  
  dec_tick = 100;
  while(dec_tick>0);
  dec_tick = 100;

  cfg.cdc = 0x01;
  cfg.cell10 = 0;
  cfg.lvlpl = 0;
  cfg.gpio1 = 1;
  cfg.gpio2 = 1;
  cfg.wdt = 1;
  cfg.dcc8_1 = 0x00;
  cfg.dcc12_9 = 0x00;
  cfg.mci4_1 = 0xF;
  cfg.mci12_5 = 0xFF;
  cfg.vuv = 0x00;
  cfg.vov = 0x00;
  
  LED_BLUE = LED_ON;
  (void)pin_low(PN_SPI_CS_N);
  (void)ltc6802_wrcfg(0x01,&cfg);
  (void)pin_high(PN_SPI_CS_N);
  LED_BLUE = LED_OFF;

  for(;;){
  
    int i;
  
    while(dec_tick>0);
    dec_tick = 100;

    PRINTF("\r\nCV: ");

    LED_GREEN = LED_ON;
    (void)pin_low(PN_SPI_CS_N);
    (void)ltc6802_stad(0x01,LTC6802_CMD_STCVAD);
    (void)pin_high(PN_SPI_CS_N);
    LED_GREEN = LED_OFF;
    
    delay_ms(20);

    LED_GREEN = LED_ON;
    (void)pin_low(PN_SPI_CS_N);
    (void)ltc6802_rdcv(0x01,&cv);
    (void)pin_high(PN_SPI_CS_N);
    LED_GREEN = LED_OFF;
    for(i=0;i<6;i++){
      int volt;
      volt = (cv.group[i].c0v11_8<<8) + cv.group[i].c0v7_0;
      volt = volt + (volt+1)/2;
      PRINTF("%04d ", volt);
      volt = (cv.group[i].c1v11_4<<4) + cv.group[i].c1v3_0;
      volt = volt + (volt+1)/2;
      PRINTF("%04d ", volt);
    }

    PRINTF("\r\nTMP: ");

    LED_GREEN = LED_ON;
    (void)pin_low(PN_SPI_CS_N);
    (void)ltc6802_stad(0x01,LTC6802_CMD_STTMPAD);
    (void)pin_high(PN_SPI_CS_N);
    LED_GREEN = LED_OFF;
    
    delay_ms(20);

    LED_GREEN = LED_ON;
    (void)pin_low(PN_SPI_CS_N);
    (void)ltc6802_rdtmp(0x01,&tmp);
    (void)pin_high(PN_SPI_CS_N);
    LED_GREEN = LED_OFF;

    {
      int volt, temp;

      volt = (tmp.etmp1_11_8<<8)+tmp.etmp1_7_0;
      volt = volt + (volt+1)/2;
      PRINTF("%04d ", volt);

      volt = (tmp.etmp2_11_4<<4)+tmp.etmp2_3_0;
      volt = volt + (volt+1)/2;
      PRINTF("%04d ", volt);

      volt = (tmp.itmp_11_8<<8)+tmp.itmp_7_0;
      volt = volt + (volt+1)/2;
      PRINTF("%04d ", volt);
      
      temp = (volt+4)/8;      
      temp -= 273;
      PRINTF("%d", temp);

    }


  }

#endif
}

void test_adc(void)
{
#if TEST_ADC

  unsigned char ch = 0;

  (void)adc_init();

  dec_tick = 10;

  for(;;){

    while(dec_tick>0);
    dec_tick = 100;

//    PRINTF("---- begin adcs\r\n");    
//    (void)adc_conv(26,5,512);
//    (void)adc_conv(29,5,512);
//    (void)adc_conv(14,5,512);
//    (void)adc_conv(21,5,512);
//    (void)adc_conv(7,5,512);
//    (void)adc_conv(22,5,512);
//    (void)adc_conv(15,5,512);
//    (void)adc_conv(23,5,512);

    (void)adc_conv(22,5,1);
    (void)adc_conv(7,5,1);
    (void)adc_conv(22,5,1024);
    (void)adc_conv(7,5,1024);
    (void)adc_conv(22,5,10);
    (void)adc_conv(7,5,10);
    (void)adc_conv(22,5,1024);
    (void)adc_conv(7,5,1024);
    (void)adc_conv(22,5,100);
    (void)adc_conv(7,5,100);
    (void)adc_conv(22,5,1024);
    (void)adc_conv(7,5,1024);

  }

#endif
}

void test(void){
#if TEST
  test_eeprom();
  test_mscan();
  test_can();
  test_spi();
  test_ltc6802();
  test_adc();
#endif
}
