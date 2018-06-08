#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "FirM.h"
#include "F_Rtc.h"

#ifdef __cplusplus
 extern "C"
#endif
void MCU_init(void); /* Device initialization function declaration */

void main(void) {
  MCU_init(); /* call Device Initialization */
  
  /* include your code here */
  EnableInterrupts;
  scheduler_main();

  for(;;) {
    /* __RESET_WATCHDOG(); by default, COP is disabled with device init. When enabling, also reset the watchdog. */
  } /* loop forever */
  /* please make sure that you never leave main */
}
