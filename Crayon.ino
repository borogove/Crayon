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

byte attackSetting = 128;
byte decaySetting = 128;
byte releaseSetting = 128;
byte sustainSetting = 128;
long sustainLevel = MAX_ENV >> 1;
bool decayEnabled = true;
long peakLevel = MAX_ENV;

byte envState = STATE_IDLE;

volatile short gatePattern;


// CORE

// wave table phasors are 32 bits unsigned
// wave tables are TABLE_SIZE = 1024 elements = 10 bits
// so phasors are 10.22 fixed point

volatile unsigned long wavePhase;	
volatile unsigned long resoPhase;

volatile unsigned long deltaWavePhase;
volatile unsigned long deltaWaveBase;
volatile unsigned long deltaResoPhase;
volatile unsigned long deltaResoBase;

volatile long lfoPhase = 0;
volatile long lfoDeltaPhase = 100;
volatile long gatePhase = 0;
volatile long gateDeltaPhase = 100;

const int MOD_BITS = ENV_BITS;
const int MAX_MOD = MAX_ENV;
volatile long lfoLevel;  // unsigned 23 bits
volatile long gateLevel; // unsigned 23 bits
long lfoDelta = 0;
long gateDelta = 0;

volatile int envelopePitchDepth = 0;              
//volatile int envelopeShapeDepth = 0;   
volatile int envelopeRatioDepth = 0;   
//volatile int envelopeResoDepth = 0;   
volatile int lfoPitchDepth = 0;   
volatile int lfoShapeDepth = 0;   
volatile int lfoRatioDepth = 0;
//volatile int lfoResoDepth = 0;
//volatile int lfoAmpDepth = 0;   
volatile int gatePitchDepth = 0;   
//volatile int gateShapeDepth = 0;   
volatile int gateRatioDepth = 0;   
//volatile int gateResoDepth = 0;  
volatile int gateAmpDepth = 0;

volatile int resoDepthSetting = 127;
volatile long waveResoScale = 6000; // 15 bits
volatile long waveDryScale = 32768-waveResoScale;
volatile long resoRatioSetting = 127;
volatile int oscMixSetting = 127;


int oscillatorShape = 0;
int vibratoDepth = 16;

int currentProgram = 0;

volatile long display_update_wait = 0;

void setup() {
  // stall before setting things up
  while (millis() < 1000) ;

  setup_buttons();
  setup_audioDac();
  setup_serialLcd();
  setup_leds();
  setup_midi();
  setup_knobs();

  set_button_action( BUTTON_A, dec_program );
  set_button_action( BUTTON_B, inc_program );

  select_program(0);

  set_knob_action( KNOB_A, select_parameter );
  set_knob_action( KNOB_B, change_parameter );
}

void loop() {
  // put your main code here, to run repeatedly:

  update_buttons();
  update_leds();
  update_midi();
  update_knobs();
  update_param_display();
 
  tick();

}


