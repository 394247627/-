#include  "msp430f149.h"

#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

#define FSYNC_1  P2OUT|=BIT3;
#define FSYNC_0  P2OUT&=~BIT3;
#define SDATA_1  P2OUT|=BIT5;
#define SDATA_0  P2OUT&=~BIT5;
#define SCLK_1   P2OUT|=BIT4;
#define SCLK_0   P2OUT&=~BIT4;

void send_con(uint con_word)
{
  uchar i;
  FSYNC_0;
  for(i=0;i<16;i++)
  {
    if(con_word&0x8000) SDATA_1;
    if(!(con_word&0x8000)) SDATA_0;
    con_word<<=1;
    SCLK_0;
    SCLK_1;
  }
  FSYNC_1;
}



void send_freq(uint freq_word)
{
  uchar i;
  FSYNC_0;
  for(i=0;i<16;i++)
  {
    if(freq_word&0x8000) SDATA_1;
    if(!(freq_word&0x8000))SDATA_0;
    freq_word<<=1;
    SCLK_0;
    SCLK_1;
  }
FSYNC_1;
}
       
       
       
       
/////ÉèÖÃÏàÎ»
void send_phase()
{
  send_con(0xc000);
}