#include <msp430x14x.h>
void ad_init(unsigned int t)
{
  int i;

    P6SEL |= 0x03;                            // Enable A/D channel A0
    ADC12CTL0 = ADC12ON+SHT0_5+REFON+REF2_5V; // Turn on and set up ADC12
    ADC12CTL1 = SHP;                          // Use sampling timer
    if(t == 1)  ADC12MCTL0= SREF0 + INCH_0;                     //参考控制位及通道选择
    else ADC12MCTL0= SREF0 + INCH_1;
    for ( i=0; i<1000; i++)                  // Delay for reference start-up
    {
    }

    ADC12CTL0|= ENC;                                //使能转换器

}

float ad_get()
{
    float ad_num;
    ADC12CTL0 |= ADC12SC;                   // Start conversion
    while ((ADC12IFG & BIT0)==0);
     _NOP();                                 // SET BREAKPOINT HERE
    ad_num = 3300.0*ADC12MEM0/4095.0;
    return(ad_num); 
}


