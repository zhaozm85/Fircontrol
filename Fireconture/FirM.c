#include "FirM.h"
#include "MC9S08DZ60.h"
#include "MCU_IO.h"
#include "MCU_CAN.h"

unsigned char LbrokeM[7]={0x01,0x02,0x04,0x08,0x10,0x20,0x40};

void firdata_init(void)   //初始化
{
    unsigned char i;
    for(i=0;i<6;i++)
    {
        firdata[i]=0xffff;
        Temp_bmu[i]=0;
    }
    firdata[3]=0;
    F_index=0;
    S_index=0;
    Lbroke=0;
    F_feadback=0;
    em_init();
}

void Lbroke_pro(void)
{ unsigned char i;
  Lbroke=0;
  canindex=canindex+1;
  if (canindex>25) 
  {
     Lbroke |=LbrokeM[6];    //CAN断线
  }
  if (canindex>250) canindex=250;
  for (i=1;i<3;i++)
     {
        if (firdata[i]==0) 
        {
          Lbroke |=LbrokeM[i];  /*火焰探测断线报警*/
        }  
     }
  if (firdata[3]<20) Lbroke |=LbrokeM[3];  /*烟感断线*/
}


void ActionF(void)
{
   if(F_index)  
   {
     pin_high(PN_BLAST_ENA); 
     F_feadback=1;  
   }else
   {
     pin_low(PN_BLAST_ENA);
     F_feadback=0;
   }

}
void ActionA(void)
{

   if(S_index)
   {
      pin_high(PN_BUZZ_ENA);
   }else
   {
      pin_low(PN_BUZZ_ENA);
   }
}

void delay_ms(unsigned int ms)
{
  unsigned int i;
  for(i=0;i<ms;i++){
    unsigned int j;
    for(j=0;j<571;j++){ /* 10ms -> 175ms if j<10000 */
      __RESET_WATCHDOG();
    }
  }
}
