#ifndef __MCU_IO_H
#define __MCU_IO_H

typedef enum
{
  PN_LED_BLUE_N,
  PN_LED_GREEN_N,
  PN_LED_RED_N,
  PN_SPI_CS_N,
  PN_LTC680X_ENA_N,
  PN_EEPROM_WP,
  PN_CAN_STB,
  PN_BLAST_ENA,
  PN_BUZZ_ENA,
  PN_VS_ENA_A,
  PN_VS_NCS_A,
  PN_VS_ENA_B,
  PN_VS_NCS_B,
  PN_MAX,
}
gt_ePinNumber;

extern void bit_clr(unsigned char * pReg, unsigned char mask);
extern void bit_set(unsigned char * pReg, unsigned char mask);
extern int pin_low(gt_ePinNumber pn);
extern int pin_high(gt_ePinNumber pn);

#endif