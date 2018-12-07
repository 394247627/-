#include <msp430x14x.h>
typedef unsigned char uchar;
typedef unsigned int  uint;


#define LCD_DataIn    P4DIR =0x00    //���ݿڷ�������Ϊ����
#define LCD_DataOut   P4DIR =0xff    //���ݿڷ�������Ϊ���
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

//12864Ӧ��ָ�
#define CLEAR_SCREEN	0x01		          //����ָ�������ACֵΪ00H
#define AC_INIT		0x02		          //��AC����Ϊ00H�����α��Ƶ�ԭ��λ��
#define CURSE_ADD	0x06		          //�趨�α��Ƶ�����ͼ�������ƶ�����Ĭ���α����ƣ�ͼ�����岻����
#define FUN_MODE	0x30		          //����ģʽ��8λ����ָ�
#define DISPLAY_ON	0x0c		          //��ʾ��,��ʾ�α꣬���α�λ�÷���
#define DISPLAY_OFF	0x08		          //��ʾ��
#define CURSE_DIR	0x14		          //�α������ƶ�:AC=AC+1
#define SET_CG_AC	0x40		          //����AC����ΧΪ��00H~3FH
#define SET_DD_AC	0x80                      //����DDRAM AC
#define FUN_MODEK	0x36		          //����ģʽ��8λ��չָ�

/*******************************************
�������ƣ�Delay_1ms
��    �ܣ���ʱԼ1ms��ʱ��
��    ������
����ֵ  ����
********************************************/
void Delay_1ms(void)
{
	uchar i;
    
	for(i = 150;i > 0;i--)  _NOP();
} 
/*******************************************
�������ƣ�Delay_Nms
��    �ܣ���ʱN��1ms��ʱ��
��    ����n--��ʱ����
����ֵ  ����
********************************************/
void Delay_Nms(uint n)
{
    uint i;
    
    for(i = n;i > 0;i--)    Delay_1ms();
}

/***********��״̬*******************************************************/
unsigned char ReadStatusLCD(void)
{
         unsigned char lcdtemp = 0;   
        
 	 //MCU2LCD_Data_OUT = 0xff;  //ȫ���ø�
 	 //LCD_RS_L;
 	 //LCD_RW_H;
	LCD_DataIn;  //io����
         do                       //��æ
    {   
  
	 LCD_EN_H;
 	 
       			
       	lcdtemp = LCD2MCU_Data_IN; 
      	LCD_EN_L;
        
    }
    while(lcdtemp & 0x80); 
        LCD_DataOut;  //io��� 
 	//LCD_EN_L;
 	return(1);
}

/*******************************************
�������ƣ�Write_Cmd
��    �ܣ���Һ����д��������
��    ����cmd--��������
����ֵ  ����
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
�������ƣ�Write_Data
��    �ܣ���Һ����д��ʾ����
��    ����dat--��ʾ����
����ֵ  ����
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
�������ƣ�Ini_Lcd
��    �ܣ���ʼ��Һ��ģ��
��    ������
����ֵ  ����
********************************************/
void Ini_Lcd(void)
{                  
  Write_Cmd(FUN_MODE);			//��ʾģʽ����
  Delay_Nms(5);
  Write_Cmd(FUN_MODE);			//��ʾģʽ����
  Delay_Nms(5);
  Write_Cmd(CURSE_DIR);			//��ʾģʽ����
  Delay_Nms(5);
  Write_Cmd(DISPLAY_ON);			//��ʾ��
  Delay_Nms(5);
  Write_Cmd(CLEAR_SCREEN);			//����
  Delay_Nms(5);
}
/*******************************************
��������:Clear_GDRAM
��    ��:���Һ��GDRAM�е��������
��    ��:��
����ֵ  :��
********************************************/
void Clear_GDRAM(void)
{
    uchar i,j,k;
    
	Write_Cmd(0x34);        //����չָ�
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
	Write_Cmd(0x30);        //�ص�����ָ�
}

/*********************************************************/
/*                                                       */
/* �趨��ʾλ��                                          */
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
   Write_Cmd(pos);     //��ʾ��ַ
}

/*******************************************
�������ƣ�Disp1Char
��    �ܣ���ĳ��λ����ʾһ���ַ�
��    ����x--λ�õ�������
          y--λ�õ�������
          data--��ʾ���ַ�����
����ֵ  ����
********************************************/
void Disp_Char(uchar x,uchar y,uchar data) //���庯����ʱ��Ҫ�������� ���� Disp1Char 
{
    lcd_pos( x, y );			
    Write_Data( data );		
}

/*******************************************
�������ƣ�DispNchar
��    �ܣ���Һ����ĳ��λ����������ʾN���ַ�
��    ����x--λ�õ�������
          y--λ�õ�������
          n--�ַ�����
          ptr--ָ���ַ����λ�õ�ָ��
����ֵ  ����
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
�������ƣ�DispString
��    �ܣ���Һ����ĳ��λ����������ʾN���ַ�
��    ����x--λ�õ�������
          y--λ�õ�������
          n--�ַ�����
          ptr--ָ���ַ����λ�õ�ָ��
����ֵ  ����
********************************************/

void  DispString(uchar x,uchar y,uchar *ptr)
{
    uchar i = 0;
    lcd_pos( x, y );
    while(ptr[i] != '\0')
     {                         //��ʾ�ַ�
      
       Write_Data(ptr[i]);
       i++;
     }

}

/***********������********************************************************/
unsigned char ReadDataLCD(void)
{
	 unsigned char LCDDA;
         
 	MCU2LCD_Data_OUT = 0xff;  //ȫ���ø�
        LCD_RS_L;
 	LCD_RW_H;
        ReadStatusLCD();
        LCD_RS_H;//LCD_RS_L;�������øߣ���
 	LCD_RW_H;
 	LCD_EN_H;
 	//LCD_EN_H;
       // MCU2LCD_Data_OUT=0;
        LCD_DataIn;  //io����
        LCDDA = LCD2MCU_Data_IN;  //������@��@��@����������������ģ����˰�������������ˡ�
                                  //�� LCD2MCU_Data��P2IN,����MCU2LCD_Data,������岻ֱ��ѽ���ɶ��Բ��У�̫�����ˡ�
                                  //��Ҫ�����ĳ� LCD2MCU_Data_IN��MCU2LCD_Data_OUT�����ɶ��Բź�    
        LCD_DataOut;  //io��� 
       //  MCU2LCD_Data_OUT=0x00;
 	LCD_EN_L;
 	return(LCDDA);
}

/* x-0-127 y-0-63  ������������ϵ  */
void set_dot(unsigned char x,unsigned char y)
{
   unsigned char x_byte, x_bit;        //�ں��������һ���ֽڣ���һ��λ  
   unsigned char y_byte, y_bit;  
   unsigned char tmph, tmpl;           //����������ʱ���������ڴ�Ŷ�����������  
  /*�����д���Ҳ��û�õģ�������ˣ���𣿣���*/
   // x &= 0x7F;  //0111 1111
   //y &= 0x3F;  // 0011 1111
  
   y=63-y; 
   x_byte = x / 16;                    //���������һ���ֽ�(��ַ)  
                                       //ע��һ����ַ��16λ��  
   x_bit = x&0x0F;                     //���������һ��λ  
   y_byte = y /32;                     //y��û���ĸ��ֽ����˵��  
                                       //����ֻ��ȷ�������ϰ��������°���  
                                       //0:�ϰ��� 1:�°���  
   y_bit = y&0x1F;                     //y_bitȷ�������ڵڼ���  ������0x1f  ����0x3f  0001 1111 
  //y_bit = y&0x1F;                     //y_bitȷ�������ڵڼ���  
  // Write_Cmd(0x36);          //��չָ�  
   Write_Cmd(0x34);          //��չָ�  

   // Write_Cmd(0x30);            //��ͼ��ʾ�ر�  
   Write_Cmd(0x80 + y_bit);        //��д��ֱ��ַ(���λ����Ϊ1)  
                                       //������������ֲ�  
   Write_Cmd(0x80 + x_byte + 8 * y_byte);   //ˮƽ����  
                                                //�°�����ˮƽ������ʼ��ַΪ0x88  
                                                //(+8*y_byte)��������ȷ��  
                                                //���ϰ��������°���  
   
   ReadDataLCD();                  //�ȿն�һ��  
   
   tmph = ReadDataLCD();           //����λ  
   tmpl = ReadDataLCD();  
  // Delay_Nms(10);
   Write_Cmd(0x80 + y_bit);       //��������ı�AC��������������һ��  
   Write_Cmd(0x80 + x_byte + 8 * y_byte);  
 //  Delay_Nms(10);
   if (x_bit<8)                             //���x_bitλ��С��8  
   {  
      Write_Data(tmph|(0x01<<(7-x_bit)));  //д���ֽڡ���Ϊ�����Ǵ������ҵ�  
                                                     //��GDRAM��λ���󣬵�λ����  
       Write_Data(tmpl);                         //ԭ�����ͻ�  
   }  
   else 
   {  
       Write_Data(tmph);                         //ԭ�����ͻ�  
       Write_Data(tmpl|(0x01<<(15-x_bit)));  

   }   
   Write_Cmd(0x36);       //�򿪻�ͼ��ʾ  
   Write_Cmd(0x30);     //�ص�����ָ����Ͼ�ST7920�����ַ�Ϊ����  
   
  
  
}

/*������*/
void dot_clear(unsigned char x,unsigned char y)
{
   unsigned char x_byte, x_bit;        //�ں��������һ���ֽڣ���һ��λ  
   unsigned char y_byte, y_bit;  
  // unsigned char tmph, tmpl;           //����������ʱ���������ڴ�Ŷ�����������  
   //x &= 0x7F;  
   //y &= 0x3F;  
   y=63-y; 
   x_byte = x/16;                    //���������һ���ֽ�(��ַ)  
                                       //ע��һ����ַ��16λ��  
   x_bit = x%16;                     //���������һ��λ  
   y_byte = y/32;                     //y��û���ĸ��ֽ����˵��  
                                       //����ֻ��ȷ�������ϰ��������°���  
                                       //0:�ϰ��� 1:�°���  
   y_bit = y%32;                     //y_bitȷ�������ڵڼ���  
   if(x_bit == 0)
   {
   Write_Cmd(0x34);          //��չָ�  
   
   Write_Cmd(0x80 + y_bit);       //��������ı�AC��������������һ��  
   Write_Cmd(0x80 + x_byte + 8*y_byte);  
     
   Write_Data(0x00);   //ֱ�����      
   Write_Data(0x00);  

   Write_Cmd(0x36);
   Write_Cmd(0x30);     //�ص�����ָ����Ͼ�ST7920�����ַ�Ϊ����  
   }
}
/*��ʾ��ֵ*/
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


/* x-0-127 y-0-63  ������������ϵ  */
void clear_the_dot(unsigned char x,unsigned char y)
{
   unsigned char x_byte, x_bit;        //�ں��������һ���ֽڣ���һ��λ  
   unsigned char y_byte, y_bit;  
   unsigned char tmph, tmpl;           //����������ʱ���������ڴ�Ŷ�����������  
  /*�����д���Ҳ��û�õģ�������ˣ���𣿣���*/
   // x &= 0x7F;  //0111 1111
   //y &= 0x3F;  // 0011 1111
  
   y=63-y; 
   x_byte = x / 16;                    //���������һ���ֽ�(��ַ)  
                                       //ע��һ����ַ��16λ��  
   x_bit = x&0x0F;                     //���������һ��λ  
   y_byte = y /32;                     //y��û���ĸ��ֽ����˵��  
                                       //����ֻ��ȷ�������ϰ��������°���  
                                       //0:�ϰ��� 1:�°���  
   y_bit = y&0x1F;                     //y_bitȷ�������ڵڼ���  ������0x1f  ����0x3f  0001 1111 
  //y_bit = y&0x1F;                     //y_bitȷ�������ڵڼ���  
  // Write_Cmd(0x36);          //��չָ�  
   Write_Cmd(0x34);          //��չָ�  

   // Write_Cmd(0x30);            //��ͼ��ʾ�ر�  
   Write_Cmd(0x80 + y_bit);        //��д��ֱ��ַ(���λ����Ϊ1)  
                                       //������������ֲ�  
   Write_Cmd(0x80 + x_byte + 8 * y_byte);   //ˮƽ����  
                                                //�°�����ˮƽ������ʼ��ַΪ0x88  
                                                //(+8*y_byte)��������ȷ��  
                                                //���ϰ��������°���  
   
   ReadDataLCD();                  //�ȿն�һ��  
   
   tmph = ReadDataLCD();           //����λ  
   tmpl = ReadDataLCD();  
  // Delay_Nms(10);
   Write_Cmd(0x80 + y_bit);       //��������ı�AC��������������һ��  
   Write_Cmd(0x80 + x_byte + 8 * y_byte);  
 //  Delay_Nms(10);
   if (x_bit<8)                             //���x_bitλ��С��8  
   {  
      Write_Data(tmph&(~(0x01<<(7-x_bit))));  //д���ֽڡ���Ϊ�����Ǵ������ҵ�  
                                                     //��GDRAM��λ���󣬵�λ����  
       Write_Data(tmpl);                         //ԭ�����ͻ�  
   }  
   else 
   {  
       Write_Data(tmph);                         //ԭ�����ͻ�  
       Write_Data(tmpl&(~(0x01<<(15-x_bit))));      //���� ��1��1   0010 0000 &     

   }   
   Write_Cmd(0x36);       //�򿪻�ͼ��ʾ  
   Write_Cmd(0x30);     //�ص�����ָ����Ͼ�ST7920�����ַ�Ϊ����  
   
  
  
}
//***********************************************************************
//�������ƣ�DisplayCgrom(uchar hz)��ʾCGROM��ĺ���
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
//	��ʾ�������ʾ
//***********************************************************************

void LCD_clear(void) 
{
  Write_Cmd(0x01);
  Delay_Nms(5);
}