
// Example Shows how to use PWM to control GPIO pins 40/45 (Audio Jack)
// 


#include "pwm.h"

// PWM CHANNEL0  GPIO40
// PWM CHANNEL1  GPIO45
#define PWM_CHANNEL0 0
#define PWM_CHANNEL1 1

// This controls the max range of the PWM signal
#define RANGE 1024


int kmain()
{
	unsigned int ra; 

   // Set GPIO pins 40/45 to Alt Fun 0, to allow PWM channel 0/1 to be output there
    ra=GET32(GPFSEL4);  // read current vlaue of GPFSEL4
    ra&=~(7<<0); //gpio40
    ra|=4<<0;    //alt0
    ra&=~(7<<15); //gpio45
    ra|=4<<15;    //alt0
    PUT32(GPFSEL4,ra);  // save new vlaue of GPFSEL4
 


    // Clock divider is set to 16.
    // With a divider of 16 and a RANGE of 1024, in MARKSPACE mode,
    // the pulse repetition frequency will be
    // 1.2MHz/1024 = 1171.875Hz, suitable for driving a DC motor with PWM
    pwm_set_clock(PWM_CLK_DIV_16);

    pwm_set_mode(PWM_CHANNEL0, 1, 1);
	pwm_set_mode(PWM_CHANNEL1, 1, 1);

    pwm_set_range(PWM_CHANNEL0, RANGE);
	pwm_set_range(PWM_CHANNEL1, RANGE);

    // Vary the PWM m/s ratio between 1/RANGE and (RANGE-1)/RANGE
    int direction = 1;
    int data = 1;
    while (1)
      {
        if (data == 1)
          direction = 1;
        else if (data == RANGE-1)
          direction = -1;
        data += direction;
          pwm_set_data(PWM_CHANNEL0, data);
		  pwm_set_data(PWM_CHANNEL1, data);

     //wait a moment
	for(ra=0;ra<5000;ra++) dummy(ra);
      }
   
    return 0;
}

