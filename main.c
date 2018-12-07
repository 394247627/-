#include  "msp430f149.h"
#include  "Cry12864.h"
#include  "ad.h"
#include  "Config.h"
#include  "ad9834.h"

#define CPU_F ((double)8000000) 
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0)) 
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0)) 

#define uchar unsigned char
#define uint  unsigned int
#define ulong  unsigned long
#define FCLK 40
#define FSYNC_1  P2OUT|=BIT3;
#define SCLK_1   P2OUT|=BIT4;
#define KeyPort         P1IN                              //�������̽���P10~P13
#define KEY_IN    (P1IN & 0xF0)           //���̿�״̬����

uchar KEY_VAL = 0;                        //�����ļ�ֵ

void ad_line(uint,uint,uint);


void Dis(void){
  
  uint i;
  for(i=0;i<48;i++){
    set_dot(4,i);
  }
  for(i=0;i<119;i++){
    set_dot(i,4);
    if((i+1)%10 == 0){
      set_dot(i,0);
      set_dot(i,1);
      set_dot(i,2);
      set_dot(i,3);
    }      
  }  
  set_dot(3,47);
  set_dot(2,46);
  set_dot(1,45);
  set_dot(0,44);
  set_dot(119,4);
  set_dot(119,5);
  set_dot(118,6);
  set_dot(117,7);
  set_dot(116,8);
  if(KEY_VAL == 1){
    DisplayCgrom(0x80,"AF");
    DisplayCgrom(0x83,"�ͣ�");
    DisplayCgrom(0x93,"�ߣ�");
    for(i=0;i<4;i++){
    set_dot(i,18);
    }
  }
  else  DisplayCgrom(0x80,"PF");
}
//**********************************************************************
//	����ɨ���ӳ��򣬲�������(��)ɨ��ķ�ʽ
//**********************************************************************
uchar Key_Scan(void) 
{
  uchar key_checkin,key_check;

  KEYPORT=0xf0;                       //��IO���������ʽ��Ϊ���뷽ʽʱҪ�ӳ�һ������
  KEYPORT=0xf0;                       //��ȡд���εķ�����ʱ
  key_checkin=KEYIN;                  //��ȡIO��״̬���ж��Ƿ��м�����
  if(key_checkin!=0xf0)               //IO��ֵ�����仯���ʾ�м�����
  {
    delay_ms(200);                     //������������ʱ20MS
    key_checkin=KEYIN;
    if(key_checkin!=0xf0)
    {
      KEYPORT=0xfe;                    //����ɨ�迪ʼ
      KEYPORT=0xfe;                    //д������ʱ����ͬ
      key_check=KEYIN;
      switch (key_check)               //���ݶ�����IO��ֵ�жϰ����İ������
      {
        case 0xee:key=15;break;
        case 0xde:key=11;break;
        case 0xbe:key=8;break;
        case 0x7e:key=4;break;
     }
     
     KEYPORT=0xfd;
     KEYPORT=0xfd;
     key_check=KEYIN;
     switch (key_check)
     {
        case 0xed:key=14;break;
        case 0xdd:key=10;break;
        case 0xbd:key=7;break;
        case 0x7d:key=3;break;
     }
     
     KEYPORT=0xfb;
     KEYPORT=0xfb;
     key_check=KEYIN;
     switch (key_check)
     {
        case 0xeb:key=13;break;
        case 0xdb:key=0;break;
        case 0xbb:key=6;break;
        case 0x7b:key=2;break;
     }
     
     KEYPORT=0xf7;
     KEYPORT=0xf7;
     key_check=KEYIN;
     switch (key_check)
     {
        case 0xe7:key=12;break;
        case 0xd7:key=9;break;
        case 0xb7:key=5;break;
        case 0x77:key=1;break;
     }
     
    }
    delay_ms(250);
      return(key);
  }
  key = 0xff;
  return(key);
}
//*************************************************************************
//	�жϷ�����
//*************************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void  PORT1_ISR(void)
{
  if(P1IFG & 0xF0)
  {
    switch(P1IFG&0xF0)                   //����һ��������������ж�ֵ��������Ӱ��
    {
    case 0x10:
    if(KEY_IN == 0xE0)                   //����ǵ�һ������������
    {
      delay_ms(20);                      //20ms������ʱ��
      if(KEY_IN == 0xE0)
      {
        while(KEY_IN != 0xF0);           //�ȴ����ͷ�
        KEY_VAL = 1;
        P1IFG = 0;
        return;
      }
    }
    
    case 0x20:
      if(KEY_IN == 0xD0)                 //����ǵڶ�������������
      {
        delay_ms(20);
        if(KEY_IN == 0xD0)
        {
          while(KEY_IN != 0xF0);         //�ȴ����ͷ�
          KEY_VAL = 2;
          P1IFG = 0;
          return;
        }
      }
      
    case 0x40:
      if(KEY_IN == 0xB0)                //����ǵ���������������
      {
        delay_ms(20);
        if(KEY_IN == 0xB0)
        {
          while(KEY_IN != 0xF0);       //�ȴ����ͷ�
          KEY_VAL = 3;
          P1IFG = 0;
          return;
        }
      }
      
    case 0x80:
    if(KEY_IN == 0x70)                 //����ǵ��ĸ�����������
    {
      delay_ms(20);
      if(KEY_IN == 0x70)
      {
        while(KEY_IN != 0xF0);        //�ȴ����ͷ�
        KEY_VAL = 4;
        P1IFG = 0;
        return;
      }
    }
    
    default:
      while(KEY_IN != 0xF0);          //�ȴ����ͷ�
      P1IFG = 0;
      return;
        }
    }
} 


//*************************************************************************
//			DACת������
//*************************************************************************
void DAC_Ser(uint data)
{
  uchar i;
  uint DACDATA2;

  DACCS0;                               //ƬѡDAC���õ���Ч
  SCLK0;                                //ʱ���ź�����
  for (i=0;i<16;i++)                    //������10λDAC��������Ĵ���Ϊ16λ��ֻ��10λ������Ч
  {
    DACDATA2=data&0x8000;               //��λȡ��
    if(DACDATA2>>15)
      DIN1;
    else
      DIN0;
    SCLK1;                              //��������������
    data<<=1;                           //��λ
    SCLK0;
   
  }
  delay_us(10);
  DACCS1;
  SCLK0;

}

void saopin(uint i){
    _DINT();
    DisplayCgrom(0x80,"ѡ�񲽽���");
    DisplayCgrom(0x90,"1:1kHz");
    DisplayCgrom(0x88,"2:10Hz");
    while(1){
    Key_Scan();                     //����ɨ�裬���Ƿ��а�������
    if(key!=0xff)                   //����а������£�����ʾ�ð�����ֵ0~15
    {        
        if(key == 2)    DisplayCgrom(0x98,"���̫���ˣ����");

     }
    if(key==1)  break;
    }
    _EINT();
    LCD_clear();
    Dis();
    ad_init(KEY_VAL);
    ulong freq_value,value1, value2,freq_data;
    uint flag=0;
    uint a,b;
    P2DIR=0XFF;
    SCLK_1;
    FSYNC_1;
    uint LSB_D, MSB_D;
      while(KEY_VAL == i){
        freq_value = 534;
        for(a=0;a<100;a++){
          for(b=0;b<10;b++){
            freq_value += 534;
            freq_data=freq_value*6711/1000;

            value1=freq_data;
            value2=freq_data;
            
            LSB_D=(value1%0x4000)+0x4000;
            MSB_D=(value2/0x4000)+0x4000;
            
            send_con(0x2028);
            send_freq(LSB_D);
            send_freq(MSB_D);
            ad_line(a,b,flag);
          }
        }
        flag=1;
       }
      
}
void dianpin(){
    _DINT();
    DisplayCgrom(0x80,"��Ƶ��");
    DisplayCgrom(0x90,"����Ƶ�ʣ�");
    DisplayCgrom(0x88,"��λ��kHz");
   
    uint i=0;
    ulong tf=0;
    float ad_num;
    while(1)
    {
      Key_Scan();                     //����ɨ�裬���Ƿ��а�������
      if(key!=0xff)                   //����а������£�����ʾ�ð�����ֵ0~15
      {        
          switch(key)
          {
            case 1: tf=tf*10+1;break;   //���ݼ�ֵ��ʾ���������
            case 2: tf=tf*10+2;break;
            case 3: tf=tf*10+3;break;
            case 4: tf=tf*10+4;break;
            case 5: tf=tf*10+5;break;
            case 6: tf=tf*10+6;break;
            case 7: tf=tf*10+7;break;
            case 8: tf=tf*10+8;break;
            case 9: tf=tf*10+9;break;
            case 0: tf=tf*10+0;break;
            case 10: tf/=10;break;
            case 11: break;
            case 12: break;
            case 13: break;
            case 14: break;
            case 15: break;
           }
          LCDfloat(tf,1,0x05);
          
      }
      if(key == 11 || tf>100){
      break;
      }
    }
    ulong freq_value,value1, value2,freq_data;
    P2DIR=0XFF;
    SCLK_1;
    FSYNC_1;
    uint LSB_D, MSB_D;
    freq_value = 534*tf;
    freq_data=freq_value*6711/1000;
    value1=freq_data;
    value2=freq_data;
    
    LSB_D=(value1%0x4000)+0x4000;
    MSB_D=(value2/0x4000)+0x4000;
    
    send_con(0x2028);
    send_freq(LSB_D);
    send_freq(MSB_D);
    LCD_clear();
    _EINT();
    while(i<8){
      ad_init(1);
      delay_ms(5);
      ad_num=ad_get();
      if(ad_num > 1060)
        ad_num = (ad_num-1060)/20;
      else
        ad_num = (1060-ad_num)/20;
      DisplayCgrom(0x80,"��Ƶ��");
      DisplayCgrom(0x90,"���룺");
      LCDfloat(tf,1,0x03);
      DisplayCgrom(0x96,"kHz");
      DisplayCgrom(0x88,"����:");
      LCDfloat(ad_num,2,0x03);
      DisplayCgrom(0x8e,"-dB");
      ad_init(2);
      delay_ms(5);
      ad_num=ad_get();
      ad_num = (2400-ad_num)/15;
      DisplayCgrom(0x98,"��λ�");
      LCDfloat(ad_num,3,0x03);
      DisplayCgrom(0x9e,"��");
      delay_ms(1000);
      i++;
    }
    ((void(*)())0xFFFE)();
    

}
void Run(){
  if(KEY_VAL == 1){
    LCD_clear();                        //����
    saopin(1);
  }
  else if(KEY_VAL == 2){
    LCD_clear();                        //����
    saopin(2);
  }
  else if (KEY_VAL ==3){
    LCD_clear();                        //����
     dianpin();
    }
    else if (KEY_VAL ==4){
    LCD_clear();                        //����
    while(KEY_VAL == 4){
      DisplayCgrom(0x80,"С���Ա��");
      DisplayCgrom(0x90,"Ӣ�������");
      DisplayCgrom(0x88,"ɧ��������");
      DisplayCgrom(0x98,"�廨������");
    }
    }
  
}
/***************************������***************************/
void main( void )
{

    WDTCTL = WDTPW + WDTHOLD;    //�ع�
    Clock_Init();
    Port_Init();
    Ini_Lcd();                  //��ʼ��Һ��
    Clear_GDRAM();
    Close_LED();
    uint DACDATA=0x0d1;
    DACDATA=DACDATA<<2;
    DAC_Ser(DACDATA);                           //����DAC����ת��
    DACDATA=DACDATA>>2;
    delay_ms(100);                      //��ʱ100ms
    DisplayCgrom(0x80,"***SFRɨƵ��***");
    DisplayCgrom(0x90,"1.AF����");
    DisplayCgrom(0x88,"2.PF����");
    DisplayCgrom(0x98,"3.��Ƶ����");
    _EINT();                            //��ȫ���жϿ���λ
    while(1)
    {
          Run();

    }
      
        


}


void ad_line(uint a,uint b,uint flag)
{
   static float ad_num;   //��̬�ֲ����� ��������Ϊ����Դ����
   static int ad1,ad2;
   static int j,i=0;
   static int ad_y1,ad_y2;
   static int ten_av,max1=0,max2,maxf,maxf2;
   ad1=0;
   for(j=0;j<4;j++){
      ad_num=ad_get();     
      ad1+=ad_num;             //��ȡadֵ,3.3v�Ǻ�
      delay_us(40);
   }
      ad1/=4;
      ad2=ad1*47.0/3300.0; //��ȡadֵ����y����47�Ǻ�
      ten_av += ad2;
      //LCDfloat(ad1,0,0x00);
      if (ad1>max1)
      {
        max1=ad1;
        maxf=10*a+b;
      }
      if((KEY_VAL==1) && (flag!=0))
      {
        if(ad1>=max2-62 && i==0 &&((10*a+b)<maxf2)){
          LCDfloat(10*a+b+1,0,0x05);
          i=1;
        }
        else if(ad1<=max2-56 && i==1&&((10*a+b)>maxf2)){
          LCDfloat(10*a+b+1,1,0x05);
          i=0;
        }
      }
      if(a*10+b>=999){
        max2=max1;
        maxf2=maxf;
        maxf=0;
        max1=0;
      }
      if(b == 9)
      {
        ten_av =ten_av/10+4;
        ad_y1=ten_av;              //�������µ�����
        set_dot(a+9,ten_av);         //���
      //  once_position[i]=ad2;   //��¼�����λ��
        
       /* �������� */
        if(ad_y1 >= ad_y2)
        {
          for(j=0;j<(ad_y1-ad_y2);j++)
            set_dot(a+9,ad_y2+j);
        } 
        else 
        {
          for(j=0;j<(ad_y2-ad_y1);j++)
            set_dot(a+9,ad_y1+j);
        }
        
         ad_y2=ad_y1;            //ad_y2Ϊ��һ�ε�y����
          //ԭ���ĳ���  ȫ����㣬�ٶ���
        if(a>88)
         {
         for(j=5;j<48;j++)  clear_the_dot(a-80,j);
         }
         else
         {
         for(j=5;j<48;j++)  dot_clear(a+12,j);
         }
      }
  
}

