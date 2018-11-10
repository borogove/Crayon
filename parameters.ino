// carob-4 parameters


//    PARAM_RESO,         // 1: Reso Freq         2: Reso Depth
//    PARAM_OSCILLATOR,   // 1: Osc Shape         2: LFO -> Pitch (Vibrato)
//    PARAM_ENV_AR,       // 1: Attack Time       2: Release Time
//    PARAM_ENV_DS,       // 1: Decay Time        2: Sustain Level
//    PARAM_ENV_MOD,      // 1: Env -> Reso Freq  2: Env -> Osc Shape
//    PARAM_LFO,          // 1: LFO Shape         2: LFO Rate
//    PARAM_LFO_MOD,      // 1: LFO -> Reso Freq  2: LFO -> Osc Shape
//    PARAM_GATE,         // 1: Gate Contour      2: Gate Rate
//    PARAM_GATE_MOD,     // 1: Gate -> Reso Freq 2: Gate -> Amp  
//    PARAM_GATE_PATTERN, // 1: Pattern 1-8       2: Pattern 9-16
//    PARAM_LAST

enum ParameterId
{
    WaveShape,

    ResoRatio,
    ResoDepth,

    EnvelopeAttack,
    EnvelopeDecay,
    EnvelopeSustain,
    EnvelopeRelease,

    LfoShape,
    LfoRate,

    GatePattern,
    GateContour,
    GateRate,

    ModEnvPitch,
    //ModEnvShape,
    ModEnvRatio,
    //ModEnvReso,     //depth
    ModLfoPitch,
    ModLfoShape,
    ModLfoRatio,
    //ModLfoReso,     //depth
    //ModLfoAmp,
    ModGatePitch,
    //ModGateShape,
    ModGateRatio,
    //ModGateReso,
    ModGateAmp,

    // second envelope?
    // second oscillator?

    NumParams
};

const char* paramLabels[] = 
{
    "WaveShape",

    "ResoRatio",
    "ResoDepth",

    "EnvelopeAttack",
    "EnvelopeDecay",
    "EnvelopeSustain",
    "EnvelopeRelease",

    "LfoShape",
    "LfoRate",

    "GatePattern",
    "GateContour",
    "GateRate",

    "ModEnvPitch",
    "ModEnvShape",
    "ModEnvRatio",
    "ModEnvReso",     //depth
    "ModLfoPitch",
    "ModLfoShape",
    "ModLfoRatio",
    "ModLfoReso",     //depth
    "ModLfoAmp",
    "ModGatePitch",
    "ModGateShape",
    "ModGateRatio",
    "ModGateReso",
    "ModGateAmp" 
};

short paramValue[ NumParams ];

int currentParam;

char buf1[32];
char buf2[32];

volatile int updatePending = -1;

void setParam( int paramIndex, short value )
{
    paramValue[paramIndex] = value;
    updatePending = paramIndex;

    switch (paramIndex)
    {
    case WaveShape: oscMixSetting = value;       break;
    case ResoRatio: resoRatioSetting = value;    break;
    case ResoDepth: resoDepthSetting = value; break;

    case EnvelopeAttack:    attackSetting = value;  break;
    case EnvelopeDecay:     decaySetting = value;   break;
    case EnvelopeSustain:   sustainLevel = (MAX_ENV >> 8) * value; break;
    case EnvelopeRelease:   releaseSetting = value; break;

    case LfoShape:      /* TODO */      break;
    case LfoRate:       lfoDeltaPhase = lfoTable[value];    break;

    case GatePattern:        gatePattern = value;       break;
    case GateContour:        /* TODO */                 break;
    case GateRate:           gateDeltaPhase = lfoTable[value]>>3;    break;

    // ALL MODS TODO
    case ModEnvPitch:       envelopePitchDepth = ((int)value)-128;  break;              
    //case ModEnvShape:       envelopeShapeDepth = ((int)value)-128;  break;   
    case ModEnvRatio:       envelopeRatioDepth = ((int)value)-128;  break;   
    //case ModEnvReso:        envelopeResoDepth = ((int)value)-128;  break;   
    case ModLfoPitch:       lfoPitchDepth = ((int)value)-128;  break;   
    case ModLfoShape:       lfoShapeDepth = ((int)value)-128;  break;   
    case ModLfoRatio:       lfoRatioDepth = ((int)value)-128;  break;
    //case ModLfoReso:        lfoResoDepth = ((int)value)-128;  break;
    //case ModLfoAmp:         lfoAmpDepth = value;    break;   
    case ModGatePitch:      gatePitchDepth = ((int)value)-128;  break;   
    //case ModGateShape:      gateShapeDepth = ((int)value)-128;  break;   
    case ModGateRatio:      gateRatioDepth = ((int)value)-128;  break;   
    //case ModGateReso:       gateResoDepth = ((int)value)-128;  break;  
    case ModGateAmp:        gateAmpDepth = value;   break;   
    }

}

void select_parameter( int v )
{
    int param = ((v * NumParams) / 256);

    currentParam = param;
    setParam( currentParam, paramValue[currentParam]);
}

void change_parameter( int v )
{
    setParam( currentParam, v );
}

void update_param_display()
{
    if (updatePending >= 0)
    {
        if (display_update_wait <= 0)
        {
            int value = paramValue[updatePending];

            serialLcdClear();
            sprintf(buf1,"%16s",paramLabels[updatePending]);
            display( buf1 );

            sprintf(buf2, "%5d          ", value);
            display( buf2);
            updatePending = -1;

            // don't try to update the display too fast
            display_update_wait = SAMPLE_RATE / 20;
        }
    }
}

void select_program( int program )
{
    // todo patch memory


    seedGen(program);

    setParam( WaveShape, genByte() );
    setParam( ResoRatio, genByte() );
    setParam( ResoDepth, genByte() );

    if (genPercent(70))
    {
        // fast envelopes
        setParam( EnvelopeAttack, genByte() >> 1 );
        setParam( EnvelopeDecay, genByte() >> 1 );
        setParam( EnvelopeRelease, genByte() >> 1 );
        setParam( EnvelopeSustain, genByte() >> 1 );
    }
    else
    {
        // slow envelopes
        setParam( EnvelopeAttack,  128 + (genByte() >> 1) );
        setParam( EnvelopeDecay,   128 + (genByte() >> 1) );
        setParam( EnvelopeRelease, 128 + (genByte() >> 1) );
        setParam( EnvelopeSustain, 128 + (genByte() >> 1) );
    }

    setParam( LfoShape, genByte() );
    setParam( LfoRate, genByte() );
    if (genPercent(70))
    {
        setParam( GatePattern, 0 );
    }
    else
    {
        setParam( GatePattern, genByte() );
    }

    setParam( ModEnvPitch, genByte() );
    setParam( ModEnvRatio, genByte() );
    setParam( ModLfoPitch, genByte() );
    setParam( ModLfoShape, genByte() );
    setParam( ModLfoRatio, genByte() );
    setParam( ModGatePitch, genByte() );
    setParam( ModGateRatio, genByte() );
    setParam( ModGateAmp, genByte() );


    playTestTone();
}