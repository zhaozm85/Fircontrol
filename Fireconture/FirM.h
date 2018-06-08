#ifndef __FIRM_H
#define __FIRM_H 

extern unsigned short F_index,S_index,F_feadback;
extern unsigned char Lbroke;
extern void firdata_init(void);
extern void Lbroke_pro(void);
extern void delay_ms(unsigned int ms);
extern void ActionF(void);
extern void ActionA(void);
#endif