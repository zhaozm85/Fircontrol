#include "MCU_IO.h"
#include "MC9S08DZ60.h"

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
#define LED_ACTIVE  0
#define LED_ON      LED_ACTIVE
#define LED_OFF     ~LED_ACTIVE
#define LED_BLUE    PTDD_PTDD2
#define LED_GREEN   PTDD_PTDD3
#define LED_RED     PTDD_PTDD4
//#define EE_WP       PTGD_PTGD4
typedef struct
{
  unsigned char * pD;
  unsigned char Pin;
  unsigned int  Data:1;
  unsigned char * pDD;
}
pin_cfg_t;

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