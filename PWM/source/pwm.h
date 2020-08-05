#ifndef _PWM_H_
#define _PWM_H_
// header file for PWM functions

#define GPFSEL4 0x20200010

#define CLOCK_BASE 		0x20101000 /* Clock controller base address */'
#define PWMCLK_CNTL 	0x201010A0 	// control register offset= (CLOCK_BASE + 0xA0), 40x4=160 =0xA0
#define PWMCLK_DIV  	0x201010A4  // devider register offset= (CLOCK_BASE + 164),  41x4=164= 0xA4
#define PWM_PASSWRD     (0x5A << 24)  /*!< Password to enable setting PWM clock */

#define PWM1_MS_MODE    0x8000  /*!< Run in Mark/Space mode */
#define PWM1_USEFIFO    0x2000  /*!< Data from FIFO */
#define PWM1_REVPOLAR   0x1000  /*!< Reverse polarity */
#define PWM1_OFFSTATE   0x0800  /*!< Ouput Off state */
#define PWM1_REPEATFF   0x0400  /*!< Repeat last value if FIFO empty */
#define PWM1_SERIAL     0x0200  /*!< Run in serial mode */
#define PWM1_ENABLE     0x0100  /*!< Channel Enable */

#define PWM0_MS_MODE    0x0080  /*!< Run in Mark/Space mode */
#define PWM_CLEAR_FIFO  0x0040  /*!< Clear FIFO */
#define PWM0_USEFIFO    0x0020  /*!< Data from FIFO */
#define PWM0_REVPOLAR   0x0010  /*!< Reverse polarity */
#define PWM0_OFFSTATE   0x0008  /*!< Ouput Off state */
#define PWM0_REPEATFF   0x0004  /*!< Repeat last value if FIFO empty */
#define PWM0_SERIAL     0x0002  /*!< Run in serial mode */
#define PWM0_ENABLE     0x0001  /*!< Channel Enable */


/* PWM Clock Divider
  Specifies the divider used to generate the PWM clock from the system clock.
  Figures below give the divider, clock period and clock frequency.
  Clock divided is based on nominal PWM base clock rate of 19.2MHz
  The frequencies shown for each divider have been confirmed by measurement
*/
typedef enum
{
    PWM_CLK_DIV_2048  = 2048,    /*!< 2048 = 9.375kHz */
    PWM_CLK_DIV_1024  = 1024,    /*!< 1024 = 18.75kHz */
    PWM_CLK_DIV_512   = 512,     /*!< 512 = 37.5kHz */
    PWM_CLK_DIV_256   = 256,     /*!< 256 = 75kHz */
    PWM_CLK_DIV_128   = 128,     /*!< 128 = 150kHz */
    PWM_CLK_DIV_64    = 64,      /*!< 64 = 300kHz */
    PWM_CLK_DIV_32    = 32,      /*!< 32 = 600.0kHz */
    PWM_CLK_DIV_16    = 16,      /*!< 16 = 1.2MHz */
    PWM_CLK_DIV_8     = 8,       /*!< 8 = 2.4MHz */
    PWM_CLK_DIV_4     = 4,       /*!< 4 = 4.8MHz */
    PWM_CLK_DIV_2     = 2,       /*!< 2 = 9.6MHz, fastest you can get */
    PWM_CLK_DIV_1     = 1        /*!< 1 = 4.6875kHz, same as divider 4096 */
} PWMClkDiv;

typedef struct {
	int CTL;
	int STA;
	int DMAC;
	int reserved1;
	int RNG0;
	int DAT0;
	int FIFO;
	int reserved2;
	int RNG1;
	int DAT1;
} PWM;

void pwm_set_clock(int divisor);
void pwm_set_mode(int channel, int markspace, int enabled);
void pwm_set_range(int channel, int range);
void pwm_set_data(int channel, int data);


#endif /* _PWM_H_ */
