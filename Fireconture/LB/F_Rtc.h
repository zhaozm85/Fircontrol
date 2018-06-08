#ifndef __F_RTC_H
#define __F_RTC_H

typedef struct
{
   unsigned char  start_index;
   unsigned short last_tick;
   unsigned char  Alarm_lev;
}
Fault;
extern void scheduler_F_init(void);
extern void scheduler_event_updata(void);
extern void scheduler_main(void);
extern void  threshold_get(unsigned short * d);
void scheduler_task_x1(void);
void scheduler_task_x2(void);
void scheduler_task_x4(void);
void scheduler_task_x8(void);
void scheduler_task_x16(void);
void fault_init(void);
void fault_pro(void);
void temp_update(void);
extern void threshold_get(unsigned char * d);
extern unsigned char get_action_delay(void);
extern unsigned char get_Alat(unsigned char em);
void thresh_init(void);
void F_action(void);
void cansed12(void);
void cansed11(void);
extern unsigned char get_AlarLev(unsigned char i);
#endif