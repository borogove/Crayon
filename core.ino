#include "tables.h"





long frames = 0;

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

    // TODO MODULATE OSC FREQ
    // TODO MODULATE RESO FREQ

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
    
    unsigned long resonated = (((osc * reso) >> 15) * resoDepth) >> 15; // 15 bits

    unsigned long unresonated = (osc * resoComplement) >> 15;   // 15 bits

    unsigned long finalosc = resonated + unresonated; // 16 bits
    
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

}