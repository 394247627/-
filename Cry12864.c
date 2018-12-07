#include <msp430x14x.h>
typedef unsigned char uchar;
typedef unsigned int  uint;


#define LCD_DataIn    P4DIR =0x00    //数据口方向设置为输入
#define LCD_DataOut   P4DIR =0xff    //数据口方向设置为输出
#define LCD2MCU_Data_IN   P4IN
#define MCU2LCD_Data_OUT  P4OUT
#define LCD_CMDOut    P5DIR|=0xff     
#define LCD_RS_H      P5OUT|=BIT5      
#define LCD_RS_L      P5OUT&=~BIT5    
#define LCD_RW_H      P5OUT|=BIT6    
#define LCD_RW_L      P5OUT&=~BIT6    
#define LCD_EN_H      P5OUT|=BIT7      
#define LCD_EN_L      P5OUT&=~BIT7    
uchar table_num[]={"0123456789 "};

//12864应用指令集
#define CLEAR_SCREEN	0x01		          //清屏指令：清屏且AC值为00H
#define AC_INIT		0x02		          //将AC设置为00H。且游标移到原点位置
#define CURSE_ADD	0x06		          //设定游标移到方向及图像整体移动方向（默认游标右移，图像整体不动）
#define FUN_MODE	0x30		          //工作模式：8位基本指令集
#define DISPLAY_ON	0x0c		          //显示开,显示游标，且游标位置反白
#define DISPLAY_OFF	0x08		          //显示关
#define CURSE_DIR	0x14		          //游标向右移动:AC=AC+1
#define SET_CG_AC	0x40		          //设置AC，范围为：00H~3FH
#define SET_DD_AC	0x80                      //设置DDRAM AC
#define FUN_MODEK	0x36		          //工作模式：8位扩展指令集

/*******************************************
函数名称：Delay_1ms
功    能：延时约1ms的时间
参    数：无
返回值  ：无
********************************************/
void Delay_1ms(void)
{
	uchar i;
    
	for(i = 150;i > 0;i--)  _NOP();
} 
/*******************************************
函数名称：Delay_Nms
功    能：延时N个1ms的时间
参    数：n--延时长度
返回值  ：无
********************************************/
void Delay_Nms(uint n)
{
    uint i;
    
    for(i = n;i > 0;i--)    Delay_1ms();
}

/***********读状态*******************************************************/
unsigned char ReadStatusLCD(void)
{
         unsigned char lcdtemp = 0;   
        
 	 //MCU2LCD_Data_OUT = 0xff;  //全部置高
 	 //LCD_RS_L;
 	 //LCD_RW_H;
	LCD_DataIn;  //io输入
         do                       //判忙
    {   
  
	 LCD_EN_H;
 	 
       			
       	lcdtemp = LCD2MCU_Data_IN; 
      	LCD_EN_L;
        
    }
    while(lcdtemp & 0x80); 
        LCD_DataOut;  //io输出 
 	//LCD_EN_L;
 	return(1);
}

/*******************************************
函数名称：Write_Cmd
功    能：向液晶中写控制命令
参    数：cmd--控制命令
返回值  ：无
********************************************/
void Write_Cmd(uchar cmd)
{
   //uchar lcdtemp = 0;
			
    LCD_RS_L;
    LCD_RW_H;
    ReadStatusLCD();
    LCD_RW_L;  		
    MCU2LCD_Data_OUT = cmd; 
    LCD_EN_H;
    _NOP();						  			
    LCD_EN_L;
}
/*******************************************
函数名称：Write_Data
功    能：向液晶中写显示数据
参    数：dat--显示数据
返回值  ：无
********************************************/
void  Write_Data(uchar dat)
{
   // uchar lcdtemp = 0;   
        
    LCD_RS_L;
    LCD_RW_H;  
    ReadStatusLCD();
    LCD_RS_H;
    LCD_RW_L;  
        
    MCU2LCD_Data_OUT = dat;
    LCD_EN_H;
    _NOP();
    LCD_EN_L;
}  
/*******************************************
函数名称：Ini_Lcd
功    能：初始化液晶模块
参    数：无
返回值  ：无
********************************************/
void Ini_Lcd(void)
{                  
  Write_Cmd(FUN_MODE);			//显示模式设置
  Delay_Nms(5);
  Write_Cmd(FUN_MODE);			//显示模式设置
  Delay_Nms(5);
  Write_Cmd(CURSE_DIR);			//显示模式设置
  Delay_Nms(5);
  Write_Cmd(DISPLAY_ON);			//显示开
  Delay_Nms(5);
  Write_Cmd(CLEAR_SCREEN);			//清屏
  Delay_Nms(5);
}
/*******************************************
函数名称:Clear_GDRAM
功    能:清除液晶GDRAM中的随机数据
参    数:无
返回值  :无
********************************************/
void Clear_GDRAM(void)
{
    uchar i,j,k;
    
	Write_Cmd(0x34);        //打开扩展指令集
	i = 0x80;            
	for(j = 0;j < 32;j++)
	{
        Write_Cmd(i++);
        Write_Cmd(0x80);
  		for(k = 0;k < 16;k++)
  		{
  		    Write_Data(0x00);
  		}
	}
	i = 0x80;
 	for(j = 0;j < 32;j++)
	{
 	    Write_Cmd(i++);
        Write_Cmd(0x88);	   
  		for(k = 0;k < 16;k++)
  		{
   		    Write_Data(0x00);
   		} 
	}   
	Write_Cmd(0x30);        //回到基本指令集
}

/*********************************************************/
/*                                                       */
/* 设定显示位置                                          */
/*                                                       */
/*********************************************************/
void lcd_pos(uchar X,uchar Y)
{                          
   uchar  pos;
   if (X==0)
     {X=0x80;}
   else if (X==1)
     {X=0x90;}
   else if (X==2)
     {X=0x88;}
   else if (X==3)
     {X=0x98;}
   pos = X+Y ;  
   Write_Cmd(pos);     //显示地址
}

/*******************************************
函数名称：Disp1Char
功    能：在某个位置显示一个字符
参    数：x--位置的列坐标
          y--位置的行坐标
          data--显示的字符数据
返回值  ：无
********************************************/
void Disp_Char(uchar x,uchar y,uchar data) //定义函数名时不要夹杂数字 比如 Disp1Char 
{
    lcd_pos( x, y );			
    Write_Data( data );		
}

/*******************************************
函数名称：DispNchar
功    能：让液晶从某个位置起连续显示N个字符
参    数：x--位置的列坐标
          y--位置的行坐标
          n--字符个数
          ptr--指向字符存放位置的指针
返回值  ：无
********************************************/
void DispNChar(uchar x,uchar y, uchar n,uchar *ptr) 
{
    uchar i;
    
    for (i=0;i<n;i++)
    {
        Disp_Char(x++,y,ptr[i]);
        if (x == 0x0f)
        {
           x = 0; 
            y ^= 1;
        }
    }
}

/*******************************************
函数名称：DispString
功    能：让液晶从某个位置起连续显示N个字符
参    数：x--位置的列坐标
          y--位置的行坐标
          n--字符个数
          ptr--指向字符存放位置的指针
返回值  ：无
********************************************/

void  DispString(uchar x,uchar y,uchar *ptr)
{
    uchar i = 0;
    lcd_pos( x, y );
    while(ptr[i] != '\0')
     {                         //显示字符
      
       Write_Data(ptr[i]);
       i++;
     }

}

/***********读数据********************************************************/
unsigned char ReadDataLCD(void)
{
	 unsigned char LCDDA;
         
 	MCU2LCD_Data_OUT = 0xff;  //全部置高
        LCD_RS_L;
 	LCD_RW_H;
        ReadStatusLCD();
        LCD_RS_H;//LCD_RS_L;这里是置高！！
 	LCD_RW_H;
 	LCD_EN_H;
 	//LCD_EN_H;
       // MCU2LCD_Data_OUT=0;
        LCD_DataIn;  //io输入
        LCDDA = LCD2MCU_Data_IN;  //！！！@！@！@！！！！！！他妈的，搞了半天是这里出错了。
                                  //是 LCD2MCU_Data，P2IN,不是MCU2LCD_Data,这个定义不直观呀，可读性不行，太不好了。
                                  //我要把它改成 LCD2MCU_Data_IN，MCU2LCD_Data_OUT这样可读性才好    
        LCD_DataOut;  //io输出 
       //  MCU2LCD_Data_OUT=0x00;
 	LCD_EN_L;
 	return(LCDDA);
}

/* x-0-127 y-0-63  正常右手坐标系  */
void set_dot(unsigned char x,unsigned char y)
{
   unsigned char x_byte, x_bit;        //在横坐标的哪一个字节，哪一个位  
   unsigned char y_byte, y_bit;  
   unsigned char tmph, tmpl;           //定义两个临时变量，用于存放读出来的数据  
  /*这两行代码也是没用的，这是在耍我吗？？？*/
   // x &= 0x7F;  //0111 1111
   //y &= 0x3F;  // 0011 1111
  
   y=63-y; 
   x_byte = x / 16;                    //算出它在哪一个字节(地址)  
                                       //注意一个地址是16位的  
   x_bit = x&0x0F;                     //算出它在哪一个位  
   y_byte = y /32;                     //y是没在哪个字节这个说法  
                                       //这里只是确定它在上半屏还是下半屏  
                                       //0:上半屏 1:下半屏  
   y_bit = y&0x1F;                     //y_bit确定它是在第几行  这里是0x1f  不是0x3f  0001 1111 
  //y_bit = y&0x1F;                     //y_bit确定它是在第几行  
  // Write_Cmd(0x36);          //扩展指令集  
   Write_Cmd(0x34);          //扩展指令集  

   // Write_Cmd(0x30);            //绘图显示关闭  
   Write_Cmd(0x80 + y_bit);        //先写垂直地址(最高位必须为1)  
                                       //具体参照数据手册  
   Write_Cmd(0x80 + x_byte + 8 * y_byte);   //水平坐标  
                                                //下半屏的水平坐标起始地址为0x88  
                                                //(+8*y_byte)就是用来确定  
                                                //在上半屏还是下半屏  
   
   ReadDataLCD();                  //先空读一次  
   
   tmph = ReadDataLCD();           //读高位  
   tmpl = ReadDataLCD();  
  // Delay_Nms(10);
   Write_Cmd(0x80 + y_bit);       //读操作会改变AC，所以重新设置一次  
   Write_Cmd(0x80 + x_byte + 8 * y_byte);  
 //  Delay_Nms(10);
   if (x_bit<8)                             //如果x_bit位数小于8  
   {  
      Write_Data(tmph|(0x01<<(7-x_bit)));  //写高字节。因为坐标是从左向右的  
                                                     //而GDRAM高位在左，底位在右  
       Write_Data(tmpl);                         //原数据送回  
   }  
   else 
   {  
       Write_Data(tmph);                         //原数据送回  
       Write_Data(tmpl|(0x01<<(15-x_bit)));  

   }   
   Write_Cmd(0x36);       //打开绘图显示  
   Write_Cmd(0x30);     //回到基本指令集，毕竟ST7920是以字符为主的  
   
  
  
}

/*清点程序*/
void dot_clear(unsigned char x,unsigned char y)
{
   unsigned char x_byte, x_bit;        //在横坐标的哪一个字节，哪一个位  
   unsigned char y_byte, y_bit;  
  // unsigned char tmph, tmpl;           //定义两个临时变量，用于存放读出来的数据  
   //x &= 0x7F;  
   //y &= 0x3F;  
   y=63-y; 
   x_byte = x/16;                    //算出它在哪一个字节(地址)  
                                       //注意一个地址是16位的  
   x_bit = x%16;                     //算出它在哪一个位  
   y_byte = y/32;                     //y是没在哪个字节这个说法  
                                       //这里只是确定它在上半屏还是下半屏  
                                       //0:上半屏 1:下半屏  
   y_bit = y%32;                     //y_bit确定它是在第几行  
   if(x_bit == 0)
   {
   Write_Cmd(0x34);          //扩展指令集  
   
   Write_Cmd(0x80 + y_bit);       //读操作会改变AC，所以重新设置一次  
   Write_Cmd(0x80 + x_byte + 8*y_byte);  
     
   Write_Data(0x00);   //直接清点      
   Write_Data(0x00);  

   Write_Cmd(0x36);
   Write_Cmd(0x30);     //回到基本指令集，毕竟ST7920是以字符为主的  
   }
}
/*显示数值*/
void LCDfloat(int f_data,unsigned char x,uchar y)        
{           
            unsigned char lcd_table[5];
            int aa;
            aa=f_data;                   
      	lcd_table[0]=aa%10000000%1000000%100000/10000; if(lcd_table[0]==0) lcd_table[0]=10;
	lcd_table[1]=aa%10000000%1000000%100000%10000/1000; if(lcd_table[0]==10) {if(lcd_table[1]==0) lcd_table[1]=10;}
	lcd_table[2]=aa%10000000%1000000%100000%10000%1000/100;if(lcd_table[1]==10) {if(lcd_table[2]==0) lcd_table[2]=10;}
	lcd_table[3]=aa%10000000%1000000%100000%10000%1000%100/10;if(lcd_table[2]==10) {if(lcd_table[3]==0) lcd_table[3]=10;}
	lcd_table[4]=aa%10;
        
        lcd_pos(x,y);
        Write_Data(table_num[lcd_table[0]]);
        Write_Data(table_num[lcd_table[1]]);
        Write_Data(table_num[lcd_table[2]]);
        Write_Data(table_num[lcd_table[3]]);
        Write_Data(table_num[lcd_table[4]]);
              
}


/* x-0-127 y-0-63  正常右手坐标系  */
void clear_the_dot(unsigned char x,unsigned char y)
{
   unsigned char x_byte, x_bit;        //在横坐标的哪一个字节，哪一个位  
   unsigned char y_byte, y_bit;  
   unsigned char tmph, tmpl;           //定义两个临时变量，用于存放读出来的数据  
  /*这两行代码也是没用的，这是在耍我吗？？？*/
   // x &= 0x7F;  //0111 1111
   //y &= 0x3F;  // 0011 1111
  
   y=63-y; 
   x_byte = x / 16;                    //算出它在哪一个字节(地址)  
                                       //注意一个地址是16位的  
   x_bit = x&0x0F;                     //算出它在哪一个位  
   y_byte = y /32;                     //y是没在哪个字节这个说法  
                                       //这里只是确定它在上半屏还是下半屏  
                                       //0:上半屏 1:下半屏  
   y_bit = y&0x1F;                     //y_bit确定它是在第几行  这里是0x1f  不是0x3f  0001 1111 
  //y_bit = y&0x1F;                     //y_bit确定它是在第几行  
  // Write_Cmd(0x36);          //扩展指令集  
   Write_Cmd(0x34);          //扩展指令集  

   // Write_Cmd(0x30);            //绘图显示关闭  
   Write_Cmd(0x80 + y_bit);        //先写垂直地址(最高位必须为1)  
                                       //具体参照数据手册  
   Write_Cmd(0x80 + x_byte + 8 * y_byte);   //水平坐标  
                                                //下半屏的水平坐标起始地址为0x88  
                                                //(+8*y_byte)就是用来确定  
                                                //在上半屏还是下半屏  
   
   ReadDataLCD();                  //先空读一次  
   
   tmph = ReadDataLCD();           //读高位  
   tmpl = ReadDataLCD();  
  // Delay_Nms(10);
   Write_Cmd(0x80 + y_bit);       //读操作会改变AC，所以重新设置一次  
   Write_Cmd(0x80 + x_byte + 8 * y_byte);  
 //  Delay_Nms(10);
   if (x_bit<8)                             //如果x_bit位数小于8  
   {  
      Write_Data(tmph&(~(0x01<<(7-x_bit))));  //写高字节。因为坐标是从左向右的  
                                                     //而GDRAM高位在左，底位在右  
       Write_Data(tmpl);                         //原数据送回  
   }  
   else 
   {  
       Write_Data(tmph);                         //原数据送回  
       Write_Data(tmpl&(~(0x01<<(15-x_bit))));      //或门 有1则1   0010 0000 &     

   }   
   Write_Cmd(0x36);       //打开绘图显示  
   Write_Cmd(0x30);     //回到基本指令集，毕竟ST7920是以字符为主的  
   
  
  
}
//***********************************************************************
//函数名称：DisplayCgrom(uchar hz)显示CGROM里的汉字
//***********************************************************************
void DisplayCgrom(uchar addr,uchar *hz)
{
  Write_Cmd(addr);
  Delay_Nms(1);
  while(*hz != '\0')  
  {
    Write_Data(*hz);
    hz++;
    Delay_Nms(1);
  }

} 
//***********************************************************************
//	显示屏清空显示
//***********************************************************************

void LCD_clear(void) 
{
  Write_Cmd(0x01);
  Delay_Nms(5);
}