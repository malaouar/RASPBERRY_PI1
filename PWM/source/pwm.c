
#include "pwm.h"

PWM *pwm = (PWM *)0x2020C000;  // PWM controller base address


void pwm_set_clock(int divisor)
{
	unsigned int ra; 

    divisor &= 0xfff;

    /* Stop PWM clock */
    PUT32(PWMCLK_CNTL, PWM_PASSWRD | 0x01);

     /* wait ??mS: Prevents clock going slow */
	for(ra=0;ra<1100;ra++) dummy(ra);

    /* Wait for the clock to be not busy */
    while (GET32(PWMCLK_CNTL) & 0x80)  // wait if b[7] in CNTL reg is set

	 //wait ??mS
	for(ra=0;ra<10;ra++) dummy(ra);

    /* set the clock divider and enable PWM clock */
    PUT32(PWMCLK_DIV, PWM_PASSWRD | (divisor << 12));
    PUT32(PWMCLK_CNTL, PWM_PASSWRD | 0x11); /* Source=osc and enable */
}


void pwm_set_mode(int channel, int markspace, int enabled)
{
 	//read the current value of control word in CTL reg
  int control = pwm->CTL;

  if (channel == 0)
    {
      if (markspace)
	control |= PWM0_MS_MODE;
      else
	control &= ~PWM0_MS_MODE;
      if (enabled)
	control |= PWM0_ENABLE;
      else
	control &= ~PWM0_ENABLE;
    }
  else if (channel == 1)
    {
      if (markspace)
	control |= PWM1_MS_MODE;
      else
	control &= ~PWM1_MS_MODE;
      if (enabled)
	control |= PWM1_ENABLE;
      else
	control &= ~PWM1_ENABLE;
    }

	//write the new value of control word in CTL reg
	pwm->CTL = control;
}

void pwm_set_range(int channel, int range)
{
 
  if (channel == 0)  pwm->RNG0 = range;
  else if (channel == 1)
      pwm->RNG1 = range;
}

void pwm_set_data(int channel, int data)
{
  if (channel == 0)  pwm->DAT0 = data;
  else if (channel == 1)
      pwm->DAT1 = data;
}
