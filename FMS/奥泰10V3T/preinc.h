
#ifndef _PREINC_H_
#define _PREINC_H_

#include  "errno.h"

#define OK  1
#define TMO -1
#define NG  -2
#define ERR -3

/* 使用PRINTF的开关 */
#if 1
#include  <stdio.h>
#define PRINTF  (void)printf
#else
#include  "mcu.h"
#define PRINTF  (void)empty_printf
#endif

/* DEBUG调试功能 */

#define DEBUG           1

#if DEBUG
#define DBG_PRINTF  PRINTF
#else
#define DBG_PRINTF  (void)empty_printf
#endif

#define DEBUG_MCU_LED   0
#define DEBUG_MCU_CLOCK 0
#define DEBUG_MCU_TICK  0
#define DEBUG_MCU_UART  0
#define DEBUG_MCU_IIC   0
#define DEBUG_MCU_SPI   0
#define DEBUG_MCU_ADC   0

#define DEBUG_DEV_AT24C 0

#define DEBUG_BMU_CFG   1

/* TEST 测试功能 */
#define TEST            0
#define TEST_EEPROM     0
#define TEST_MSCAN      0
#define TEST_CAN        0
#define TEST_SPI        0
#define TEST_LTC6802    0
#define TEST_ADC        0

#define TUNE_TS         0
#define TUNE_NTC        0
#define TUNE_WX         0

/* 温度输出功能 */
#define TEMP            1

#endif  /*  _PREINC_H_  */
