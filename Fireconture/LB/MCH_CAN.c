#include "MCU_CAN.h"
#include "MC9S08DZ60.h"
#include "F_Rtc.h"
#include "CRC16.h"
#include "FirM.h"

unsigned char threshold[7];
unsigned char buff[7];
unsigned short BMU_v[16];
unsigned char Emindexrx;
can_frame f_rx,ftx;
void  CanTx(can_frame *f)
{
  unsigned char tbsel;
  unsigned short id=0x11;

  if( !CANCTL0_SYNCH ) return ;
  CANTBSEL=CANTFLG;
  tbsel = CANTBSEL;
  if(!tbsel) return ;
  CANTIDR0 = f->id>>3;
  CANTIDR1 = f->id<<5;
  CANTDSR0 = f->data[0];
  CANTDSR1 = f->data[1];
  CANTDSR2 = f->data[2];
  CANTDSR3 = f->data[3];
  CANTDSR4 = f->data[4];
  CANTDSR5 = f->data[5];
  CANTDSR6 = f->data[6];
  CANTDSR7 = f->data[7];
  CANTDLR = f->dlc;
  CANTTBPR =0;
  CANTFLG=tbsel;    
}

void Can_event_rx(void)
{
   static unsigned short temp  ;
   static unsigned short id  ;
   unsigned char bmunum,firmmun;
   if(CANRFLG_RXF==0) return;
   if (CANRIDR1_IDE){ } 
   else{
         id = CANRIDR0<<8;
         id |=CANRIDR1;
         id = id>>5;
         bmunum=id>>4;
         firmmun=id & 0x0f;
         f_rx.id=id;
         f_rx.data[0]=CANRDSR0;
         f_rx.data[1]=CANRDSR1;
         f_rx.data[2]=CANRDSR2;
         f_rx.data[3]=CANRDSR3;
         f_rx.data[4]=CANRDSR4;
         f_rx.data[5]=CANRDSR5;
         f_rx.data[6]=CANRDSR6;
         f_rx.data[7]=CANRDSR7;
         if (id==0x13)
         {
           temp = CANRDSR0<<8;
           temp |=CANRDSR1;
           firdata[0]=temp;
           temp = CANRDSR2<<8;
           temp |=CANRDSR3;
           firdata[1]=temp;
           temp = CANRDSR4<<8;
           temp |=CANRDSR5;
           firdata[2]=temp;
           temp = CANRDSR6<<8;
           temp |=CANRDSR7;
           firdata[3]=temp; 
           canindex=0;
         }
         if (id==0x14)
         {
           temp = CANRDSR0<<8;
           temp |=CANRDSR1;
           firdata[4]=temp;
           temp = CANRDSR2<<8;
           temp |=CANRDSR3;
           firdata[5]=temp;
           canindex=0;   
         }  
         if (id==0x710)
         {
           HMIcommuniction(&f_rx) ;
         }
         if (bmunum<=2 && firmmun>=6 && firmmun<=8)
         {
          CAN_bmu_getv(& f_rx);
         }
         if (id==0x7EF)
         {
           
           threshold[0]=CANRDSR0;       
           threshold[1]=CANRDSR1;
           threshold[2]=CANRDSR2;
           threshold[3]=CANRDSR3;       
           threshold[4]=CANRDSR4;
           threshold[5]=CANRDSR5;
           threshold[6]=CANRDSR6;
           threshold_get(&threshold); 
           //(void)at24c_write(0,threshold,7);
          (void) at24c_write_in_page(129,&threshold,7); 
          //at24c_write_page()
         }
    }
}

void CAN_bmu_getv(can_frame *f) 
{
  unsigned char BMUnum,Firmnum,i;
  BMUnum=(f->id)>>4;
  Firmnum=(f->id) & 0x0F;
  if(BMUnum>=3 || BMUnum==0) return;
  if(Firmnum<6 || Firmnum>8) return;
  if (Firmnum==6)
  {
     Temp_bmu[(BMUnum-1)*3]  =f->data[0]; 
     Temp_bmu[(BMUnum-1)*3+1]=f->data[1]; 
     Temp_bmu[(BMUnum-1)*3+2]=f->data[2]; 
  }
  if(Firmnum==7 || Firmnum==8)
  {
     for(i=0;i<4;i++)
     {
        BMU_v[(BMUnum-1)*8+i+(Firmnum-7)*4]  = f->data[2*i] <<8 ;
        BMU_v[(BMUnum-1)*8+i+(Firmnum-7)*4] |= f->data[2*i+1] ;
     }
  }
  
}

void HMIcommuniction(can_frame *f)
{
   unsigned char ServetAddre,FunctionCode,DelayT;
   unsigned short FunctionAdd,crc,etype;
   ServetAddre =f->data[0];
   FunctionCode=f->data[1];
   FunctionAdd  = f->data[2] <<8;
   FunctionAdd |= f->data[3] ;
   if(ServetAddre==1 && FunctionCode==3 )
   {   
     if(FunctionAdd<=16 && FunctionAdd>=1)
     {  
        ftx.data[3]=BMU_v[FunctionAdd-1]>>8 ;
        ftx.data[4]=BMU_v[FunctionAdd-1] & 0xFF;

     }
     if(FunctionAdd>=17 && FunctionAdd<=22)
     {
        ftx.data[3]=Temp_bmu[FunctionAdd-17]>>8 ;
        ftx.data[4]=Temp_bmu[FunctionAdd-17] & 0xFF ;
     }
     if(FunctionAdd==30)
     {
        DelayT=get_Alat(Emindexrx);
        ftx.data[3]=DelayT>>8 ;
        ftx.data[4]=DelayT & 0xFF ;
     }  
     if(FunctionAdd==31)
     {
        DelayT=get_action_delay();
        ftx.data[3]=DelayT>>8 ;
        ftx.data[4]=DelayT & 0xFF ;
     } 
     if(FunctionAdd==35)
     {
        DelayT= get_AlarLev(2);
        ftx.data[3]=DelayT>>8 ;
        ftx.data[4]=DelayT & 0xFF ;
     }
     if(FunctionAdd==36)
     {
        DelayT= get_AlarLev(1);
        ftx.data[3]=DelayT>>8 ;
        ftx.data[4]=DelayT & 0xFF ;
     }
     if(FunctionAdd==37)
     {
        DelayT= get_AlarLev(3);
        ftx.data[3]=DelayT>>8 ;
        ftx.data[4]=DelayT & 0xFF ;
     }
     if(FunctionAdd==38)
     {
        if (F_feadback>=1) DelayT=1;
        else DelayT=0;
        ftx.data[3]=DelayT>>8 ;
        ftx.data[4]=DelayT & 0xFF ;
     }
     
     crc=CRCget(5,&ftx.data);
     ftx.data[5]= crc>>8;
     ftx.data[6]= crc & 0xff;
     CanTx(& ftx);
     return;
   }
   if( FunctionCode==6 && FunctionAdd==32)
   {
     etype  = f->data[4] <<8;
     etype |= f->data[5] ;
     if (etype>=1) Emindexrx |= 0x01 ;
     if (etype==0) Emindexrx &= 0xFE ;
     return;
   }
   if( FunctionCode==6 && FunctionAdd==33)
   {
     etype  = f->data[4] <<8;
     etype |= f->data[5] ;
     if (etype>=1) Emindexrx |= 0x02 ;
     if (etype==0) Emindexrx &= 0xFD ;
     return;
   }
   if( FunctionCode==6 && FunctionAdd==34)
   {
     etype  = f->data[4] <<8;
     etype |= f->data[5] ;
     if (etype>=1) Emindexrx |= 0x04 ;
     if (etype==0) Emindexrx &= 0xFB ;
     return;
   }
}

void BMU_v_init(void)
{
   unsigned char i;
   for(i=0;i<16;i++)
   {
      BMU_v[i]=0; 
   }
}

void can_init(void)
{
   CANCTL0 |= (unsigned char)0x01;                      
  while(!CANCTL1_INITAK) {             /* Wait for init acknowledge */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }
  /* CANCTL1: CANE=1,CLKSRC=1,LOOPB=0,LISTEN=0,BORM=0,WUPM=0,SLPAK=0,INITAK=1 */
  CANCTL1 = 0xC1;                                      
  /* CANBTR1: SAMP=1,TSEG22=0,TSEG21=1,TSEG20=1,TSEG13=1,TSEG12=0,TSEG11=1,TSEG10=0 */
  CANBTR1 = 0xBA;                                      
  /* CANBTR0: SJW1=0,SJW0=1,BRP5=0,BRP4=0,BRP3=0,BRP2=1,BRP1=0,BRP0=1 */
  CANBTR0 = 0x45;                                      
  /* CANIDAC: IDAM1=0,IDAM0=0,IDHIT2=0,IDHIT1=0,IDHIT0=0 */
  CANIDAC = 0x00;                                      
  /* CANIDAR0: AC7=0,AC6=0,AC5=0,AC4=0,AC3=0,AC2=0,AC1=0,AC0=0 */
  CANIDAR0 = 0x00;                                      
  /* CANIDAR1: AC7=0,AC6=0,AC5=0,AC4=0,AC3=0,AC2=0,AC1=0,AC0=0 */
  CANIDAR1 = 0x00;                                      
  /* CANIDAR2: AC7=0,AC6=0,AC5=0,AC4=0,AC3=0,AC2=0,AC1=0,AC0=0 */
  CANIDAR2 = 0x00;                                      
  /* CANIDAR3: AC7=0,AC6=0,AC5=0,AC4=0,AC3=0,AC2=0,AC1=0,AC0=0 */
  CANIDAR3 = 0x00;                                      
  /* CANIDAR4: AC7=0,AC6=0,AC5=0,AC4=0,AC3=0,AC2=0,AC1=0,AC0=0 */
  CANIDAR4 = 0x00;                                      
  /* CANIDAR5: AC7=0,AC6=0,AC5=0,AC4=0,AC3=0,AC2=0,AC1=0,AC0=0 */
  CANIDAR5 = 0x00;                                      
  /* CANIDAR6: AC7=0,AC6=0,AC5=0,AC4=0,AC3=0,AC2=0,AC1=0,AC0=0 */
  CANIDAR6 = 0x00;                                      
  /* CANIDAR7: AC7=0,AC6=0,AC5=0,AC4=0,AC3=0,AC2=0,AC1=0,AC0=0 */
  CANIDAR7 = 0x00;                                      
  /* CANIDMR0: AM7=1,AM6=1,AM5=1,AM4=1,AM3=1,AM2=1,AM1=1,AM0=1 */
  CANIDMR0 = 0xFF;                                      
  /* CANIDMR1: AM7=1,AM6=1,AM5=1,AM4=1,AM3=1,AM2=1,AM1=1,AM0=1 */
  CANIDMR1 = 0xFF;                                      
  /* CANIDMR2: AM7=1,AM6=1,AM5=1,AM4=1,AM3=1,AM2=1,AM1=1,AM0=1 */
  CANIDMR2 = 0xFF;                                      
  /* CANIDMR3: AM7=1,AM6=1,AM5=1,AM4=1,AM3=1,AM2=1,AM1=1,AM0=1 */
  CANIDMR3 = 0xFF;                                      
  /* CANIDMR4: AM7=1,AM6=1,AM5=1,AM4=1,AM3=1,AM2=1,AM1=1,AM0=1 */
  CANIDMR4 = 0xFF;                                      
  /* CANIDMR5: AM7=1,AM6=1,AM5=1,AM4=1,AM3=1,AM2=1,AM1=1,AM0=1 */
  CANIDMR5 = 0xFF;                                      
  /* CANIDMR6: AM7=1,AM6=1,AM5=1,AM4=1,AM3=1,AM2=1,AM1=1,AM0=1 */
  CANIDMR6 = 0xFF;                                      
  /* CANIDMR7: AM7=1,AM6=1,AM5=1,AM4=1,AM3=1,AM2=1,AM1=1,AM0=1 */
  CANIDMR7 = 0xFF;                                      
  /* CANCTL0: INITRQ=0 */
  CANCTL0 &= (unsigned char)~0x01;                     
  while(CANCTL1_INITAK) {              /* Wait for init exit */
   SRS = 0x55;                         /* Reset watchdog counter write 55, AA */
   SRS = 0xAA;
  }
  /* CANCTL0: RXFRM=0,RXACT=0,CSWAI=0,SYNCH=0,TIME=0,WUPE=0,SLPRQ=0,INITRQ=0 */
  CANCTL0 = 0x00;                                      
  /* CANRIER: WUPIE=0,CSCIE=0,RSTATE1=0,RSTATE0=0,TSTATE1=0,TSTATE0=0,OVRIE=1,RXFIE=1 */
  CANRIER = 0x03;                                      
  /* CANTIER: TXEIE2=0,TXEIE1=0,TXEIE0=0 */
  CANTIER = 0x00; 
}

unsigned char getem(void)
{
  return Emindexrx;
}

void em_init(void)
{
  Emindexrx=0x00;
}

void cansed61(void)
{
 can_frame Send_f2;
 Send_f2.id=0x61;
 Send_f2.dlc=8;
 Send_f2.data[0]=BMU_v[0]>>8;               //»ðÑæÌ½²âÆ÷1±¨¾¯
 Send_f2.data[1]=BMU_v[0] & 0xff;               //»ðÑæÌ½²âÆ÷2±¨¾¯
 Send_f2.data[2]=BMU_v[1]>>8;               //ÑÌÎí±¨¾¯Æ÷±¨¾¯
 Send_f2.data[3]=BMU_v[1] & 0xff;               //µç³ØÎÂ¶È±¨¾¯
 Send_f2.data[4]=BMU_v[2]>>8;                           //¶ÏÏß±¨¾¯
 Send_f2.data[5]=BMU_v[2] & 0xff;                          //µÆ¹âÖ´ÐÐ
 Send_f2.data[6]=BMU_v[3]>>8;                          //±¬ÆÆ·§Ö´ÐÐ
 Send_f2.data[7]=BMU_v[3] & 0xff;                             //ÉúÃüÖÜÆÚ
 CanTx(& Send_f2);
  Send_f2.id=0x62;
 Send_f2.dlc=8;
 Send_f2.data[0]=BMU_v[4]>>8;               //»ðÑæÌ½²âÆ÷1±¨¾¯
 Send_f2.data[1]=BMU_v[4] & 0xff;               //»ðÑæÌ½²âÆ÷2±¨¾¯
 Send_f2.data[2]=BMU_v[5]>>8;               //ÑÌÎí±¨¾¯Æ÷±¨¾¯
 Send_f2.data[3]=BMU_v[5] & 0xff;               //µç³ØÎÂ¶È±¨¾¯
 Send_f2.data[4]=BMU_v[6]>>8;                           //¶ÏÏß±¨¾¯
 Send_f2.data[5]=BMU_v[6] & 0xff;                          //µÆ¹âÖ´ÐÐ
 Send_f2.data[6]=BMU_v[7]>>8;                          //±¬ÆÆ·§Ö´ÐÐ
 Send_f2.data[7]=BMU_v[7] & 0xff;                             //ÉúÃüÖÜÆÚ
 CanTx(& Send_f2);
  Send_f2.id=0x63;
 Send_f2.dlc=8;
 Send_f2.data[0]=BMU_v[8]>>8;               //»ðÑæÌ½²âÆ÷1±¨¾¯
 Send_f2.data[1]=BMU_v[8] & 0xff;               //»ðÑæÌ½²âÆ÷2±¨¾¯
 Send_f2.data[2]=BMU_v[9]>>8;               //ÑÌÎí±¨¾¯Æ÷±¨¾¯
 Send_f2.data[3]=BMU_v[9] & 0xff;               //µç³ØÎÂ¶È±¨¾¯
 Send_f2.data[4]=BMU_v[10]>>8;                           //¶ÏÏß±¨¾¯
 Send_f2.data[5]=BMU_v[10] & 0xff;                          //µÆ¹âÖ´ÐÐ
 Send_f2.data[6]=BMU_v[11]>>8;                          //±¬ÆÆ·§Ö´ÐÐ
 Send_f2.data[7]=BMU_v[11] & 0xff;                             //ÉúÃüÖÜÆÚ
 CanTx(& Send_f2);
}

void HMIcommuniction_init(void)
{
     ftx.id=0x711;
     ftx.dlc=7;
     ftx.data[0]=1;
     ftx.data[1]=3;
     ftx.data[2]=2 ; 
     ftx.data[3]=0;
     ftx.data[4]=0;
     ftx.data[5]=0 ;
     ftx.data[6]=0;
     ftx.data[7]=0;
}
