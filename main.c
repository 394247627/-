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
#define KeyPort         P1IN                              //独立键盘接在P10~P13
#define KEY_IN    (P1IN & 0xF0)           //键盘口状态输入

uchar KEY_VAL = 0;                        //按键的键值

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
    DisplayCgrom(0x83,"低：");
    DisplayCgrom(0x93,"高：");
    for(i=0;i<4;i++){
    set_dot(i,18);
    }
  }
  else  DisplayCgrom(0x80,"PF");
}
//**********************************************************************
//	键盘扫描子程序，采用逐行(列)扫描的方式
//**********************************************************************
uchar Key_Scan(void) 
{
  uchar key_checkin,key_check;

  KEYPORT=0xf0;                       //在IO口由输出方式变为输入方式时要延迟一个周期
  KEYPORT=0xf0;                       //采取写两次的方法延时
  key_checkin=KEYIN;                  //读取IO口状态，判断是否有键按下
  if(key_checkin!=0xf0)               //IO口值发生变化则表示有键按下
  {
    delay_ms(200);                     //键盘消抖，延时20MS
    key_checkin=KEYIN;
    if(key_checkin!=0xf0)
    {
      KEYPORT=0xfe;                    //逐行扫描开始
      KEYPORT=0xfe;                    //写两次延时，下同
      key_check=KEYIN;
      switch (key_check)               //根据读到的IO口值判断按键的按下情况
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
//	中断服务函数
//*************************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void  PORT1_ISR(void)
{
  if(P1IFG & 0xF0)
  {
    switch(P1IFG&0xF0)                   //进行一个与操作，避免判断值出现其他影响
    {
    case 0x10:
    if(KEY_IN == 0xE0)                   //如果是第一个按键被按下
    {
      delay_ms(20);                      //20ms的消抖时间
      if(KEY_IN == 0xE0)
      {
        while(KEY_IN != 0xF0);           //等待键释放
        KEY_VAL = 1;
        P1IFG = 0;
        return;
      }
    }
    
    case 0x20:
      if(KEY_IN == 0xD0)                 //如果是第二个按键被按下
      {
        delay_ms(20);
        if(KEY_IN == 0xD0)
        {
          while(KEY_IN != 0xF0);         //等待键释放
          KEY_VAL = 2;
          P1IFG = 0;
          return;
        }
      }
      
    case 0x40:
      if(KEY_IN == 0xB0)                //如果是第三个按键被按下
      {
        delay_ms(20);
        if(KEY_IN == 0xB0)
        {
          while(KEY_IN != 0xF0);       //等待键释放
          KEY_VAL = 3;
          P1IFG = 0;
          return;
        }
      }
      
    case 0x80:
    if(KEY_IN == 0x70)                 //如果是第四个按键被按下
    {
      delay_ms(20);
      if(KEY_IN == 0x70)
      {
        while(KEY_IN != 0xF0);        //等待键释放
        KEY_VAL = 4;
        P1IFG = 0;
        return;
      }
    }
    
    default:
      while(KEY_IN != 0xF0);          //等待键释放
      P1IFG = 0;
      return;
        }
    }
} 


//*************************************************************************
//			DAC转换程序
//*************************************************************************
void DAC_Ser(uint data)
{
  uchar i;
  uint DACDATA2;

  DACCS0;                               //片选DAC，置低有效
  SCLK0;                                //时钟信号拉低
  for (i=0;i<16;i++)                    //尽管是10位DAC，但是其寄存器为16位，只有10位数据有效
  {
    DACDATA2=data&0x8000;               //逐位取出
    if(DACDATA2>>15)
      DIN1;
    else
      DIN0;
    SCLK1;                              //上升沿数据锁存
    data<<=1;                           //移位
    SCLK0;
   
  }
  delay_us(10);
  DACCS1;
  SCLK0;

}

void saopin(uint i){
    _DINT();
    DisplayCgrom(0x80,"选择步进：");
    DisplayCgrom(0x90,"1:1kHz");
    DisplayCgrom(0x88,"2:10Hz");
    while(1){
    Key_Scan();                     //键盘扫描，看是否有按键按下
    if(key!=0xff)                   //如果有按键按下，则显示该按键键值0~15
    {        
        if(key == 2)    DisplayCgrom(0x98,"这个太慢了，别点");

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
    DisplayCgrom(0x80,"点频：");
    DisplayCgrom(0x90,"输入频率：");
    DisplayCgrom(0x88,"单位：kHz");
   
    uint i=0;
    ulong tf=0;
    float ad_num;
    while(1)
    {
      Key_Scan();                     //键盘扫描，看是否有按键按下
      if(key!=0xff)                   //如果有按键按下，则显示该按键键值0~15
      {        
          switch(key)
          {
            case 1: tf=tf*10+1;break;   //根据键值显示到数码管上
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
      DisplayCgrom(0x80,"点频：");
      DisplayCgrom(0x90,"输入：");
      LCDfloat(tf,1,0x03);
      DisplayCgrom(0x96,"kHz");
      DisplayCgrom(0x88,"增益:");
      LCDfloat(ad_num,2,0x03);
      DisplayCgrom(0x8e,"-dB");
      ad_init(2);
      delay_ms(5);
      ad_num=ad_get();
      ad_num = (2400-ad_num)/15;
      DisplayCgrom(0x98,"相位差：");
      LCDfloat(ad_num,3,0x03);
      DisplayCgrom(0x9e,"°");
      delay_ms(1000);
      i++;
    }
    ((void(*)())0xFFFE)();
    

}
void Run(){
  if(KEY_VAL == 1){
    LCD_clear();                        //清屏
    saopin(1);
  }
  else if(KEY_VAL == 2){
    LCD_clear();                        //清屏
    saopin(2);
  }
  else if (KEY_VAL ==3){
    LCD_clear();                        //清屏
     dianpin();
    }
    else if (KEY_VAL ==4){
    LCD_clear();                        //清屏
    while(KEY_VAL == 4){
      DisplayCgrom(0x80,"小组成员：");
      DisplayCgrom(0x90,"英俊陈颍锶");
      DisplayCgrom(0x88,"骚气冯瑞瑞");
      DisplayCgrom(0x98,"村花蒋宜霏");
    }
    }
  
}
/***************************主函数***************************/
void main( void )
{

    WDTCTL = WDTPW + WDTHOLD;    //关狗
    Clock_Init();
    Port_Init();
    Ini_Lcd();                  //初始化液晶
    Clear_GDRAM();
    Close_LED();
    uint DACDATA=0x0d1;
    DACDATA=DACDATA<<2;
    DAC_Ser(DACDATA);                           //送入DAC进行转换
    DACDATA=DACDATA>>2;
    delay_ms(100);                      //延时100ms
    DisplayCgrom(0x80,"***SFR扫频仪***");
    DisplayCgrom(0x90,"1.AF特性");
    DisplayCgrom(0x88,"2.PF特性");
    DisplayCgrom(0x98,"3.点频测试");
    _EINT();                            //打开全局中断控制位
    while(1)
    {
          Run();

    }
      
        


}


void ad_line(uint a,uint b,uint flag)
{
   static float ad_num;   //静态局部变量 生存周期为整个源程序
   static int ad1,ad2;
   static int j,i=0;
   static int ad_y1,ad_y2;
   static int ten_av,max1=0,max2,maxf,maxf2;
   ad1=0;
   for(j=0;j<4;j++){
      ad_num=ad_get();     
      ad1+=ad_num;             //读取ad值,3.3v吻合
      delay_us(40);
   }
      ad1/=4;
      ad2=ad1*47.0/3300.0; //读取ad值，与y坐标47吻合
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
        ad_y1=ten_av;              //记下最新的坐标
        set_dot(a+9,ten_av);         //打点
      //  once_position[i]=ad2;   //记录曾今的位置
        
       /* 连接两点 */
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
        
         ad_y2=ad_y1;            //ad_y2为上一次的y坐标
          //原来的程序  全部清点，速度慢
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

