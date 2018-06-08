#ifndef __MCU_CAN_H
#define __MCU_CAN_H

typedef struct{
unsigned char ide;
unsigned char dlc;
unsigned long id; 
unsigned char data[8];
}
can_frame;
extern unsigned short firdata[6];
extern unsigned char Temp_bmu[6];
extern unsigned char canindex;
extern void CanTx(can_frame*);
extern void Can_event_rx(void);
extern void BMU_v_init(void);
void CAN_bmu_getv(can_frame *f);
void HMIcommuniction(can_frame *f);
extern void can_init(void);
extern unsigned char getem(void);
extern void em_init(void);
extern void cansed61(void);
extern void HMIcommuniction_init(void);
#endif