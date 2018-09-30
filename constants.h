#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define CONFIG_CRAYON555 1


// Crayon555 based on Teensy 3.2
#ifdef CONFIG_CRAYON555

#define DAC_A       A14
#define SERIALOUT_A 10
#define SERIALIN_A  26
#define BUTTON_A    7
#define BUTTON_B    8
#define KNOB_A      A4
#define KNOB_B      A3
#define MIDIOUT_A   5
#define MIDIIN_A    0
#define LED_A       9
#define LED_B       1
#define LED_C       2
#define LED_D       23
#define MAX_PINS    34

const int DAC_BUFFER_SIZE = 8;
const int DAC_BITS = 12;



/*

short                >>4        +bias
32767       0x7FFF   0x07FF     0xFFF
16384       0x4000   0x0400     0xC00
0           0x0000   0x0000     0x800
-1          0xFFFF   0xFFFF     0x7FF
-16384      0xC000   0xFC00     0x400
-32768      0x8000   0xF800     0x000

*/



#define OUTPUT_CHANNELS 1

#endif

const int DAC_SHIFT = 16-DAC_BITS;
const int DAC_MAX = (1<<DAC_BITS)-1;
const int DAC_BIAS = 1<<(DAC_BITS-1);   // 0x800

#endif // _CONSTANTS_H
