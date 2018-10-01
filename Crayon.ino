#include <SoftwareSerial.h>
#include "common.h"
#include "constants.h"
#include "tables.h"

// ENVELOPE

const int STATE_IDLE = 0;
const int STATE_ATTACK = 1;
const int STATE_DECAY = 2;
const int STATE_SUSTAIN = 3;
const int STATE_RELEASE = 4;

// TODO make envelope a class

volatile long envLevel = 0; 
volatile long envRate = 0;  

byte attackSetting = 12;
byte decaySetting = 18;
byte releaseSetting = 18;
byte sustainSetting = 64;
long sustainLevel = MAX_ENV >> 1;
bool decayEnabled = false;
long peakLevel = MAX_ENV;

byte envState = STATE_IDLE;

short gatePattern;

// CORE

// wave table phasors are 32 bits unsigned
// wave tables are TABLE_SIZE = 1024 elements = 10 bits
// so phasors are 10.22 fixed point

volatile unsigned long wavePhase;	
volatile unsigned long resoPhase;

volatile unsigned long deltaWavePhase;
volatile unsigned long deltaResoPhase;

volatile long lfoPhase = 0;
volatile long lfoDeltaPhase = 100;
volatile long gatePhase = 0;
volatile long gateDeltaPhase = 100;


// HMMM either saw or tri should always be 0 so these can go to 512
// but what if not?
volatile short sawMix = 0;	// 0-341 (~8.4 bits)
volatile short sqrMix = 127;	// 0-341
volatile short triMix = 127; 	// 0-341

volatile long resoDepth = 6000; // 15 bits
volatile long resoComplement = 32768-resoDepth;
volatile long resoRatio = 3453;


int oscillatorShape = 0;
int vibratoDepth = 16;


void setup() {
  // put your setup code here, to run once:
  setup_buttons();
  setup_audioDac();
  setup_serialLcd();
  setup_leds();
  setup_midi();
  setup_knobs();

  set_button_action( BUTTON_A, page_up );
  set_button_action( BUTTON_B, page_down );

  set_knob_action( KNOB_A, select_parameter );
  set_knob_action( KNOB_B, change_parameter );
  

}

void loop() {
  // put your main code here, to run repeatedly:

  update_buttons();
  update_serialLcd();
  update_leds();
  update_midi();
  update_knobs();
  update_param_display();
 
  //tick();

}


