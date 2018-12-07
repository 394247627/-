#define CPU_F ((double)8000000) 
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0)) 
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0)) 

#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long
#define PSB_CLR	        P5OUT &= ~BIT0            //PSB置低，串口方式
#define PSB_SET	        P5OUT |=  BIT0            //PSB置高，并口方式

#define RST_CLR	        P5OUT &= ~BIT1            //RST置低
#define RST_SET	        P5OUT |= BIT1             //RST置高
/*当BRCLK=CPU_F时用下面的公式可以计算，否则要根据设置加入分频系数*/
#define baud           9600                                //设置波特率的大小
#define baud_setting   (uint)((ulong)CPU_F/((ulong)baud))  //波特率计算公式
#define baud_h         (uchar)(baud_setting>>8)            //提取高位
#define baud_l         (uchar)(baud_setting)               //低位

#define LED8PORT        P2OUT                   //P2接8个LED灯
#define LED8SEL         P2SEL                   //P2接8个LED灯
#define LED8DIR         P2DIR                   //P2接8个LED灯

#define DATAPORT	P4OUT                   //数据口所在端口P4
#define DATASEL         P4SEL                   //数据口功能寄存器，控制功能模式
#define DATADIR         P4DIR                   //数据口方向寄存器

#define CTRPORT	        P6OUT                   //控制线所在的端口P6
#define CTRSEL          P6SEL                   //控制口功能寄存器，控制功能模式
#define CTRDIR          P6DIR                   //控制口方向寄存器

#define DCTR0	        P6OUT &= ~BIT4          //数码管段控制位信号置低
#define DCTR1           P6OUT |=  BIT4          //数码管段控制位信号置高
#define WCTR0	        P6OUT &= ~BIT3          //数码管位控制位信号置低
#define WCTR1           P6OUT |=  BIT3          //数码管位控制位信号置高

#define KEYPORT	        P1OUT                   //按键所在的端口P1
#define KEYSEL          P1SEL                   //控制口功能寄存器，控制功能模式
#define KEYDIR          P1DIR                   //控制口方向寄存器
#define KEYIN           P1IN                    //键盘扫描判断需要读取IO口状态值

#define DACCS0          P5OUT &= ~BIT4          //DAC片选信号置低
#define DACCS1          P5OUT |=  BIT4          //DAC片选信号置高
#define DIN0            P3OUT &= ~BIT1          //DAC DIN管脚位数据为0
#define DIN1            P3OUT |=  BIT1          //DAC DIN管脚位数据为1
#define DOUT0           P3OUT &= ~BIT2          //DAC DOUT管脚位数据为0
#define DOUT1           P3OUT |=  BIT2          //DAC DOUT管脚位数据为0
#define SCLK0           P3OUT &= ~BIT3          //DAC SCLK管脚位数据为0
#define SCLK1           P3OUT |=  BIT3          //DAC SCLK管脚位数据为0

#define SOUNDON         P6OUT &= ~BIT2          //蜂鸣器开声音
#define SOUNDOFF        P6OUT |= ~BIT2          //蜂鸣器关声音
uchar key;					//键值变量

//***********************************************************************
//                   系统时钟初始化
//***********************************************************************
void Clock_Init()
{
  uchar i;
  BCSCTL1&=~XT2OFF;                 //打开XT2振荡器
  BCSCTL2|=SELM1+SELS;              //MCLK为8MHZ，SMCLK为8MHZ
  do{
    IFG1&=~OFIFG;                   //清楚振荡器错误标志
    for(i=0;i<100;i++)
       _NOP();
  }
  while((IFG1&OFIFG)!=0);           //如果标志位1，则继续循环等待
  IFG1&=~OFIFG; 
}
//***********************************************************************
//               MSP430内部看门狗初始化
//***********************************************************************
void WDT_Init()
{
   WDTCTL = WDTPW + WDTHOLD;       //关闭看门狗
}

//***********************************************************************
//		关闭数码管
//***********************************************************************
void Close_LED()
{
  DATASEL  = 0x00;                      //设置IO口为普通I/O模式，此句可省
  DATADIR  = 0xFF;                      //设置IO口方向为输出
  DATAPORT = 0xFF;                      //P4口初始设置为FF
  
  CTRSEL  =  0x00;                      //设置IO口为普通I/O模式，此句可省
  CTRDIR |=  BIT3 + BIT4;               //设置IO口方向为输出,控制口在P63,P64
  CTRPORT =  0xFF;                      //P6口初始设置为FF  
  
  DCTR1;                                //关掉数码管，以免显示乱码                   
  WCTR1;                     
  DATAPORT=0xFF;                        //关数码管            
  WCTR0; 
}
//***********************************************************************
//               MSP430IO口初始化
//***********************************************************************
void Port_Init()
{
  P4SEL = 0x00;
  P4DIR = 0xFF;
  P5SEL = 0x00;
  P5DIR|= BIT0 + BIT1 + BIT5 + BIT6 + BIT7;
  PSB_SET;		  //液晶并口方式
  RST_SET;
  
  CTRSEL  =  0x00;                      //设置IO口为普通I/O模式，此句可省
  CTRDIR |=  BIT3 + BIT4;               //设置IO口方向为输出,控制口在P63,P64
  CTRPORT =  0xFF;                      //P6口初始设置为FF 
  
  KEYSEL = 0x00;                        //设置IO口为普通I/O模式，此句可省      
  KEYDIR = 0x0F;                        //高四位输入模式，低四位输出模式，外部上拉电阻
  KEYPORT= 0xF0;                        //初始值0xF0
  
  P1IES = 0xF0;                         //P14~P17选择下降沿中断
  P1IE  = 0xF0;                         //打开中断使能
  P1IFG = 0x00;                         //中断标志位清零
  
  
  P3SEL   = 0x00;                       //设置IO口为普通I/O模式，此句可省      
  P3DIR  |= BIT1 + BIT3;                //DIN管脚、SCLK管脚输出模式，DOUT输入模式
  P3OUT   = 0xFF;                       //初始值0xFF
  
  P5SEL   = 0x00;                       //设置IO口为普通I/O模式，此句可省      
  P5DIR  |= BIT4;                       //CS管脚输出模式
  P5OUT   = 0xFF;                       //初始值0xFF
  
    
}