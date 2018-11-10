#include "tables.h"

long frames = 0;


// HMMM either saw or tri should always be 0 so these can go to 512
// but what if not?
volatile short sawMix = 0;	// 0-341 (~8.4 bits)
volatile short sqrMix = 127;	// 0-341
volatile short triMix = 127; 	// 0-341

void setOscMix()
{
    //int envMod = (envelopeShapeDepth * envLevel) >> ENV_BITS;  
    int lfoMod = (lfoShapeDepth * lfoLevel) >> MOD_BITS;
    //int gateMod = (gateShapeDepth * gateLevel) >> MOD_BITS;

    int mx = oscMixSetting + lfoMod; // 0-255
    if (mx < 0)     { mx = 0; }
    if (mx > 255)   { mx = 255; } 

    int x;

    if (mx < 128)
    {
        x = mx*2;
        // first half is saw-to-square
        sawMix = 255-x;    
        sqrMix = x;
        triMix = 0; 
    }
    else
    {
        x = (mx-128) * 2;
        sawMix = 0;
        sqrMix = 255-x;
        triMix = x;
    }
}

long resoRatio = 3543;

void setResoRatio()
{
    int envMod = (envelopeRatioDepth * envLevel) >> ENV_BITS;

    // lfo ratio depth is -128 to +127
    // lfo level is 0-8M
    // -- should the mod depth be unipolar and the mod wave bipolar instead? 
    // This way lines up well with envelope and gate...
    int lfoMod  = ( lfoRatioDepth *  (lfoLevel>>8)) >> (MOD_BITS-8);
    int gateMod = (gateRatioDepth * (gateLevel>>8)) >> (MOD_BITS-8);

    
    int ratioValue = resoRatioSetting + envMod + lfoMod + gateMod;
    
        // oof. reso ratio goes from 0.5 (down one octave, 
    // which may not be useful) to... 32? up 5 octaves?
    // but the resoRatio value is scaled by 1024, so: 
    // 512 - 32768? Ideally it should probably be 
    // roughly exponential as well :/ 

    int scale = 16;
    int start = 512;

    while (ratioValue < 0)
    {
        scale /= 2;
        ratioValue += 32;
        start /= 2;
    }
    while (ratioValue > 32)
    {
        scale *= 2;
        ratioValue -= 32;
        start *= 2;
    }
    resoRatio = start + scale * ratioValue;
    if (resoRatio < 128)
    {
        resoRatio = 128;
    }
    if (resoRatio > 65536)
    {
        resoRatio = 65536;
    }
}

void tick()
{


	frames++;
	if ((frames & CONTROL_RATE_MASK) == 0)
	{
		updateEnvelope();
        updateModulators();

	}

	if ((frames & MIDI_RATE_MASK) == 0)
	{
		update_midi();
	}


	// TODO UPDATE MODULATOR LEVELS
    tickEnvelope();

    lfoLevel += lfoDelta;
    if (lfoLevel > MAX_MOD)
    {
        lfoLevel = MAX_MOD;
    }
    if (lfoLevel < 0)
    {
        lfoLevel = 0;
    }

    gateLevel += gateDelta;
    if (gateLevel > MAX_MOD)
    {
        gateLevel = MAX_MOD;
    }
    if (gateLevel < 0)
    {
        gateLevel = 0;
    }

    // Advance the envelope
    envLevel += envRate;
    if (envLevel > peakLevel)
    {
        envLevel = peakLevel;
    }
    if (envLevel < 0)
    {
        envLevel = 0;
    }

    setOscMix();
    setResoRatio();

    // TODO MODULATE OSC FREQ


    deltaResoPhase = (deltaWavePhase / 1024) * resoRatio;

    // Increment wave phasors
    wavePhase += deltaWavePhase; 
    resoPhase += deltaResoPhase;

    // Did the oscillator phase wrap?
    if (wavePhase < deltaWavePhase)
    {
    	// Sync the resonator
        resoPhase = (wavePhase * deltaResoPhase) / deltaWavePhase;
    }

    // Get table indices
    int waveIx = (wavePhase >> PHASOR_FRACTION_BITS) & TABLE_MASK;
    int resoIx = (resoPhase >> PHASOR_FRACTION_BITS) & TABLE_MASK;
    // Resonator lookup
    unsigned long reso = sine[resoIx] >> 1; // 15 bit unsigned

    // Oscillator lookups
    unsigned long sawOsc = saw[waveIx];	// 16 bit unsigned
    unsigned long triOsc = tri[waveIx];
    unsigned long sqrOsc = sqr[waveIx];

    // Sum and resonate oscillator A
    // 16 bits x 10 bits (mixes sum to < 1024) = 26 bits
    unsigned long osc = (sawOsc * sawMix + sqrOsc * sqrMix + triOsc * triMix) >> 11;	// 15 bits
    
    int resoDepth = resoDepthSetting << 7;
    if (resoDepth < 0) { resoDepth = 0; }
    if (resoDepth > 32767) { resoDepth = 32767; }
    int resoComplement = 32767-resoDepth;
    unsigned long resonated = (((osc * reso) >> 15) * resoDepth) >> 15; // 15 bits

    unsigned long unresonated = (osc * resoComplement) >> 15;   // 15 bits

    unsigned long finalosc = resonated + unresonated; // 16 bits
    
    // TODO gate
    long amp = (envLevel >> (ENV_BITS-15)); // 15 bits


    // 15 x 15 -> 30 >> 14 = 16u
    audioChannel.WriteU16( (amp * (finalosc >> 1)) >> 14 );
    //audioChannel.WriteU16( (amp * triOsc) >> 14 );
}


// K-rate update
void updateModulators()
{
	lfoPhase += lfoDeltaPhase;
	gatePhase += gateDeltaPhase;

    set_led( 0, (lfoPhase  & 0x40000000) > 0 ? 1 : 0 );
    set_led( 2, (gatePhase & 0x40000000) > 0 ? 1 : 0 );

    int lfoIx = (lfoPhase >> PHASOR_FRACTION_BITS) & TABLE_MASK;
    long lfoTarget = ((long)tri[lfoIx]) << 7;  // convert 16 bit to 23 bit
    // get there by the next K-rate update
    lfoDelta = (lfoTarget - lfoLevel) / CONTROL_RATE_RATIO;

    // we just need three bits of the gate phase.
    int gateStep = (gatePhase >> 28) & 0x7; // 0-7 which step we're on
    // gate pattern is an 8 bit mask, if the bit is set the gate 
    // target is high for that step.
    int stepValue = gatePattern & (1 << gateStep);
    unsigned long gateTarget = 0;
    if (stepValue > 0)
    {
        gateTarget = 0x7FFFFF;
        set_led(1,0);
    }
    else
    {
        gateTarget = 0;
        set_led(1,1);
    }
    gateDelta = (gateTarget-gateLevel) / CONTROL_RATE_RATIO;

    // TODO gate contour

    updateTestTone();
}

void inc_program()
{
    currentProgram--;
    if (currentProgram < 0)
    {
        currentProgram = 127;
    }

    select_program(currentProgram);
}

void dec_program()
{
    currentProgram++;
    if (currentProgram > 127)
    {
        currentProgram = 0;
    }

    select_program(currentProgram);
}
