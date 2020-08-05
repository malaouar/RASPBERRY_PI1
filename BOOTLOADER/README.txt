Bootloader using XMODEM protocol.

- dispalys messages and go in sleep mode.
- to weak it up send any character, it sends a message that it's ready for xmodem transfer.
- if there is a problem during transfer it resets the system and we are in bootloader mode again !!

--------------------------------------------------------------
This bootloader sits at 0x200000 (there is a jump at 0x8000 to 0x200000 and between this 2 adresses only dummy bytes !!!). You have 0x200000 -0x8000 bytes to develop with.  

it's Fairly easy to change this address ( 0x200000).  bootloader.c and c0.s each have a copy of this value.
You can change the load adress where the bootloader puts yout program from 0X8000 to any other value just change the:
#define ARMBASE 0x8000
 in the bootloader.c file  to the needed value.

this bootloader uses .bin files dont send it .hex files, wont work.

