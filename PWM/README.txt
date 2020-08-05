
 example how to use the PWM. 

PWM controller Page 138 on the datasheet.

You must read:
 channel0 instead of channel1
 channel1 instead of channel2

Note:
RPTL0 (bit2) and  RPTL1 (bit10) in CTL Register is used to enable/disable repeating of the last data available in the FIFO just before it empties. When this bit is 1 and FIFO is used, the last available data in the FIFO is repeatedly sent. This may be useful in PWM mode to avoid duty cycle gaps. 
If the FIFO is not used this bit does not have any effect. Default operation is do-not-repeat.

The FIFO register is the FIFO input for the 2 channels. Data written to this address is stored in channel FIFO and if USEFi is enabled for the channel i it is used as data to be sent. This register is write only, and reading this register will always return bus default return value, pwm0 .
When more than one channel is enabled for FIFO usage, the data written into the FIFO is shared between these channels in turn. For example if the word series A B C D E F G H I .. is written to FIFO and two channels are active and configured to use FIFO then channel 0 will transmit words A C E G I .. and channel 1 will transmit words B D F H .. .
Note that requesting data from the FIFO is in locked-step manner and therefore requires tight coupling of state machines of the channels. If any of the channel range (period) value is different than the others this will cause the channels with small range values to wait between words hence resulting in gaps between words. To avoid that, each channel sharing the FIFO should be configured to use the same range value.
Also note that RPTLi are not meaningful when the FIFO is shared between channels as there is no defined channel to own the last data in the FIFO. Therefore sharing channels must have their RPTLi set to zero.
If the set of channels to share the FIFO has been modified after a configuration change, FIFO should be cleared before writing new data.

