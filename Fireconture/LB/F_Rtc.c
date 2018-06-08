#include "F_Rtc.h"
#include "MC9S08DZ60.h"
#include "FirM.h"
#include "MCU_CAN.h"
#include  "at24c.h"
#include "MCU_IO.h"


#define LED_ON      LED_ACTIVE
#define LED_OFF     ~LED_ACTIVE
#define LED_BLUE    PTDD_PTDD2
#define LED_GREEN   PTDD_PTDD3
#define LED_RED     PTDD_PTDD4
unsigned char alar=0x88;
unsigned char Emindex=0u;
unsigned char Emstarnext=0u;
static unsigned char scheduler_rx =0u;
static unsigned char scheduler_ax =0u;
static unsigned short scheduler_tick =0u;
unsigned char T_data[7];
unsigned char Life=0;
unsigned char Action_delay=0;
static Fault fault[6];
can_frame Send_f;
void fault_init(void)
{  unsigned char i;
   for(i=1;i<6;i++)
   {
      fault[i].start_index=0;
      fault[i].last_tick=0;
      fault[i].Alarm_lev=0;
   }             
   
}
void scheduler_F_init(void)
{
  scheduler_rx =0u;
  scheduler_ax =0u;
  scheduler_tick =0u;
}

void scheduler_event_updata(void)
{
  scheduler_rx++;
}

void scheduler_main(void)
{

  (void)pin_high(PN_EEPROM_WP);
  Life=0;
  at24c_init();
  scheduler_F_init();
  firdata_init();
  fault_init();
  thresh_init();
  BMU_v_init();
  HMIcommuniction_init();
  at24c_read_in_page(129,&T_data,7);
  for(;;)
  {
     if (scheduler_rx != scheduler_ax)
     {
        scheduler_ax = scheduler_rx  ;
        LED_GREEN=~LED_GREEN;
        scheduler_task_x1();  /*125ms*/
        if      (scheduler_tick & 0x01u) scheduler_task_x2(); /*250ms*/
        else if (scheduler_tick & 0x02u) scheduler_task_x4(); /*500ms*/
        else if (scheduler_tick & 0x04u) scheduler_task_x8(); /*1000ms*/
        else if (scheduler_tick & 0x08u) scheduler_task_x16(); /*2000ms*/
        else __RESET_WATCHDOG();
        scheduler_tick++;
        __RESET_WATCHDOG();
        
     }
  
  
  }

}


void scheduler_task_x1(void)
{
   LED_BLUE=~LED_BLUE;
   fault_pro();
   Lbroke_pro();
   F_action();
}
void scheduler_task_x2(void)
{
 LED_RED=~LED_RED;
}
void scheduler_task_x4(void)
{
 //
}
void scheduler_task_x8(void)
{
 
 Life++;
 if(Life>255) Life=0;
 temp_update();
 cansed11();
 cansed12();
}
void scheduler_task_x16(void)
{
 cansed61();
}
void fault_pro(void)
{ unsigned char i;
  for (i=1;i<3;i++)
  {
    
    if(firdata[i]<700 && firdata[i]>5 ) 
   {
      fault[i-1].start_index=1;
      fault[i-1].last_tick++;
      if   (fault[i-1].last_tick>=   8*T_data[0])  fault[0].Alarm_lev=1;
      if   (fault[i-1].last_tick>=   8*T_data[1])  fault[0].Alarm_lev=2;
      if   (fault[i-1].last_tick>=   8*T_data[2])  fault[0].Alarm_lev=3;
   }else{
      fault[i-1].start_index=0;
      fault[i-1].last_tick=0;
      fault[i-1].Alarm_lev=0;
   }
  }
  if (firdata[3]<=800)
  {
      fault[2].start_index=1;
      fault[2].last_tick++;
      if   (fault[2].last_tick>=   8*T_data[3])  fault[2].Alarm_lev=1;
      if   (fault[2].last_tick>=   8*T_data[4])  fault[2].Alarm_lev=2;
      if   (fault[2].last_tick>=   8*T_data[5])  fault[2].Alarm_lev=3;  
  }else{
      fault[2].start_index=0;
      fault[2].last_tick=0;
      fault[2].Alarm_lev=0; 
  }
   
}

void  threshold_get(unsigned char * d)    //更新外部时间阀值
{
   T_data[0]=*(d+0);//调整火焰报警1
   T_data[1]=*(d+1);//调整火焰报警2
   T_data[2]=*(d+2);//调整火焰报警3
   T_data[3]=*(d+3);//调整烟雾报警1
   T_data[4]=*(d+4);//调整烟雾报警2
   T_data[5]=*(d+5);//调整烟雾报警3
   T_data[6]=*(d+6);//调整延迟喷射时间

} 

void thresh_init(void)
{
   T_data[0]=1;
   T_data[1]=2;
   T_data[2]=3;
   T_data[3]=3;
   T_data[4]=3;
   T_data[5]=3;
   T_data[6]=0;
   
}

void temp_update(void)    //更新温度报警状态
{
   unsigned char i,max,min,temp;
   max=0;
   min=0xff;
   for (i=0;i<6;i++)
   {
    temp=Temp_bmu[i];
    if(max<temp) max=temp;
    if(min>temp) min=temp;  
   }
   if (max>=105 && max <110) fault[3].Alarm_lev=1;    //65~70 40℃的偏移量、
   else fault[3].Alarm_lev=0;
   if (max>=110 && max <115) fault[3].Alarm_lev=2;    //70~75
   if (max>=115 ) fault[3].Alarm_lev=3;               //75
}

void F_action(void)
{  unsigned char sum,i,Alar;
   unsigned char  tindx,eindx,t1indx;
   sum=0;Alar=0;F_index=0;
   S_index=0;tindx=0;eindx=0;
   Emindex=getem();
   for(i=0;i<3;i++)
   {
      if( fault[i].Alarm_lev==3) sum++ ;
      if( fault[i].Alarm_lev>0) Alar++ ;
   }
   if (Alar>0 || Lbroke >0 ) S_index=1;
   if (fault[3].Alarm_lev>0) S_index=1;
   ActionA();
   if (sum>=2) F_index=1;
   Action_delay++;
   if (F_index==0) Action_delay=0;
   if (Action_delay >240 ) 
   {
    Action_delay=240;
   }
   
   if ((Emindex & 0x04u) >0)
   {
      F_index=0;
      ActionF();
   }else{
    
   
   if ((Emindex & 0x01u) >0) 
   {
      F_index=0;
      ActionF();
      if ((Emindex & 0x02u)>0)
      {
         F_index=1;
         ActionF();
         F_index=0;
         Emstarnext=1;
      }
   
   }else {
   
         if (Action_delay>=T_data[6]*8 ) 
         {         
            ActionF();         
         }
   } 
   }
   // duc
   /*if ((Emindex & 0x02u)>0)
   {
      F_index=1;
      ActionF();
      F_index=0;
      Emstarnext=1;
   } else
   {
      if ((Emindex & 0x01u) ==0) 
      {
         if (Action_delay>=T_data[6]*8 ) 
         {
            if ((Emindex & 0x04u) ==0)  
            {
               ActionF();
            }else
            {
               Emstarnext=1;
            }
         }
      }
      if (Emstarnext==1)
      {
         F_index=0;
         ActionF();
         Emstarnext=0;
      }
   } */
}
unsigned char get_Alat(unsigned char em)
{
   unsigned char t=0;
   Emindex=em;
   alar=0x88;
   t=fault[0].Alarm_lev+fault[1].Alarm_lev ;
   if(t>0) alar |=0x02;  //火
   if(fault[2].Alarm_lev>0) alar |=0x01;   //烟
   if(Lbroke>0) alar |=0x04; //系统
   if (F_index==1 && Action_delay<T_data[6]*8) alar |=0x10; //延时
   if (F_feadback==1)
   {
      alar |=0x20;
      alar &=0xEF; //喷射 
   } else
   {
     alar &=0xDF;
   }
   /*if ((Emindex & 0x02) >0)
   {
      alar |=0x20;
      alar &=0xEF; //喷射
   } */
   
   if ((Emindex & 0x01) >0) 
   {
       alar |=0x40;
       alar &=0x7f;   //手动
   }
   if ((Emindex & 0x01) ==0) 
   {
       alar |=0x80;
       alar &=0xBF;     //自动
   }
     
   return alar;  
   
}

unsigned char get_action_delay(void)
{
  unsigned char time;
  if (Action_delay==0)
  {
     time=0;
  }else{
     time=30-Action_delay/8;
     
  }
   return time;
}

unsigned char get_AlarLev(unsigned char i)
{
   unsigned char lever;
   lever=0;
   if (i<=1)
   {
      if(fault[0].Alarm_lev>=fault[1].Alarm_lev)  lever=fault[0].Alarm_lev;
      else  lever=fault[1].Alarm_lev; 
   }
   if(i==2)
   {
      lever=fault[2].Alarm_lev ;
   }
   if (i==3)
   {
      if(Lbroke>0) lever=1;
      else lever=0;
   }
   return lever;
}


void cansed12(void)
{
   //at24c_read_in_page(129,&T_data,7);
 unsigned char en;
 en= getem();
 Send_f.id=0x12;
 Send_f.dlc=8;
 Send_f.data[0]=T_data[0];               //火焰探测器1报警
 Send_f.data[1]=T_data[1];               //火焰探测器2报警
 Send_f.data[2]=T_data[2];               //烟雾报警器报警
 Send_f.data[3]=T_data[3];               //电池温度报警
 Send_f.data[4]=T_data[4];               //断线报警
 Send_f.data[5]=T_data[5];               //灯光执行
 Send_f.data[6]=T_data[6];               //爆破阀执行
 Send_f.data[7]=en;                    //生命周期
 CanTx(& Send_f);
}
                      
void cansed11(void)
{
 Send_f.id=0x11;
 Send_f.dlc=8;
 Send_f.data[0]=fault[0].Alarm_lev;               //火焰探测器1报警
 Send_f.data[1]=fault[1].Alarm_lev;               //火焰探测器2报警
 Send_f.data[2]=fault[2].Alarm_lev;               //烟雾报警器报警
 Send_f.data[3]=fault[3].Alarm_lev;               //电池温度报警
 Send_f.data[4]=Lbroke;                           //断线报警
 Send_f.data[5]=S_index;                          //灯光执行
 Send_f.data[6]=F_index;                          //爆破阀执行
 Send_f.data[7]=F_feadback;                             //生命周期
 CanTx(& Send_f);
}
























