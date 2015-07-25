/***************************内部参考电压2.5v版本**********************************/
#include "msp430g2553.h"
char a[6] ="";
int da;
double c,data;
void delay(int t)
{
	int s=500;
	for(;t>0;t--)
	{
		for(;s>0;s--)
		{
			;;
		}
	}
}

void write_com(char com)
{
	P1OUT&=~(1<<3);//rS=0;
	P1OUT&=~(1<<4);//RW=0;
	P1OUT&=~(1<<5);//en=0;
	P1OUT&=~(1<<1);//
	delay(8);
	P2OUT=com;
	delay(8);
	P1OUT|=(1<<5);//en=1;
	delay(8);
	P1OUT&=~(1<<5);//en=0;
}

void write_data(char dt)
{
	P1OUT|=(1<<3);//rs=1;
	P1OUT&=~(1<<4);//rw=0;
	P1OUT&=~(1<<5);//en=0;
	delay(8);
	P2OUT=dt;
	delay(8);
	P1OUT|=(1<<5);//en=1;
	delay(8);
	P1OUT&=~(1<<5);//en=0;
}

void initial_lcd(void)
{
	delay(20);
	write_com(0x38);
	delay(15);
	write_com(0x0c);
	delay(15);
	write_com(0x06);
	delay(15);
	write_com(0x01)	;
}

void LCD_Disp(char x,char y,char *character)
{
  char xtemp;
    switch(x)
    {
        case 0:xtemp=0x80+y;break;
        case 1:xtemp=0xc0+y;break;
        default:break;
    }
    write_com(xtemp);
    while(*character!=0)
    {
    	write_data(*character);
        character++;
    }
}

void main(void)
{

	char *pa;
	pa=a;

	  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
         //端口初始化
	  P2SEL=0x00;
	  P2DIR |= 0xff;                            // Set P1.0 to output direction
	  P1DIR |= 0x38;
	  initial_lcd();			   //LCD初始化
	  ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE+REF2_5V;//SREF_1+REFON 表示使用内部参考电压2.5v
	  __enable_interrupt();                     // Enable interrupts.
	  TACCR0 = 30;                              // Delay to allow Ref to settle
	  TACCTL0 |= CCIE;                          // Compare-mode interrupt.
	  TACTL = TASSEL_2 | MC_1;                  // TACLK = SMCLK, Up mode.
	  LPM0;                                     // Wait for delay.
	  TACCTL0 &= ~CCIE;                         // Disable timer Interrupt
	  __disable_interrupt();

	  ADC10CTL1 = INCH_1;                       // input A1   选择模拟信号输入端  八通道
	  ADC10AE0 |= 0x02;                         // PA.1 ADC option select

    for (;;)
      {
			ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start  采样和转换开始
			__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exi

			da = ADC10MEM;
			data = ((double)da/1024)*2.5;

			c =data;

			    a[0]=((int)c%10+0x30);
	  	            a[1]=0x2e;
	  	            a[2]=((int)(c*10)%10+0x30);
	  	            a[3]=((int)(c*100)%10+0x30);
	  	            a[4]=((int)(c*1000)%10+0x30);
	  	            a[5]='\0';
	  	            LCD_Disp(0,0,pa);
      }
}

 // ADC10 interrupt service routine
 #pragma vector=ADC10_VECTOR
 __interrupt void ADC10_ISR (void)
 {
   __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
 }

 #pragma vector=TIMER0_A0_VECTOR
 __interrupt void ta0_isr(void)
 {
   TACTL = 0;
   LPM0_EXIT;                                // Exit LPM0 on return
 }



