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
    ModEnvShape,
    ModEnvRatio,
    ModEnvReso,     //depth
    ModLfoPitch,
    ModLfoShape,
    ModLfoRatio,
    ModLfoReso,     //depth
    ModLfoAmp,
    ModGatePitch,
    ModGateShape,
    ModGateRatio,
    ModGateReso,
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

void setOscMix( short mx )
{
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

void setResoRatio( short r )
{
    // oof. reso ratio goes from 0.5 (down one octave, 
    // which may not be useful) to... 32? up 5 octaves?
    // but the resoRatio value is scaled by 1024, so: 
    // 512 - 32768? Ideally it should probably be 
    // roughly exponential as well :/ 

    int scale = 16;
    int start = 512;

    while (r > 32)
    {
        scale *= 2;
        r -= 32;
        start *= 2;
    }
    resoRatio = start + scale * r;
}

void setParam( int paramIndex, short value )
{
    paramValue[paramIndex] = value;
    updatePending = paramIndex;

    switch (paramIndex)
    {
    case WaveShape: setOscMix(value);       break;
    case ResoRatio: setResoRatio(value);    break;
    case ResoDepth: resoDepth = value*128; resoComplement = 32768-resoDepth;    break;

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
    case ModEnvPitch:       break;              
    case ModEnvShape:       break;   
    case ModEnvRatio:       break;   
    case ModEnvReso:        break;   
    case ModLfoPitch:       break;   
    case ModLfoShape:       break;   
    case ModLfoRatio:       break;   
    case ModLfoReso:        break;   
    case ModLfoAmp:         break;   
    case ModGatePitch:      break;   
    case ModGateShape:      break;   
    case ModGateRatio:      break;   
    case ModGateReso:       break;   
    case ModGateAmp:        break;   
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