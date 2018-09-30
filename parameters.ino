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

int paramSelectRaw;
int currentParam;

char buf1[32];
char buf2[32];

int dejitter = 1;

void setParam( int paramIndex, short value )
{
    paramValue[paramIndex] = value;
    serialLcdClear();
    sprintf(buf1,"%16s",paramLabels[paramIndex]);
    display( buf1 );

    sprintf(buf2, "%5d          ", value);
    display( buf2);
}

void select_parameter( int v )
{
    if ((v > paramSelectRaw+dejitter) || (v < paramSelectRaw-dejitter))
    {
        paramSelectRaw = v;
        int param = ((v * NumParams) / 256);
        if (param != currentParam)
        {
            currentParam = param;
            setParam( currentParam, paramValue[currentParam]);
        }
    }
}

void change_parameter( int v )
{
    int currentValue = paramValue[currentParam];
    if ((v > currentValue+dejitter) || (v < currentValue-dejitter))
    {
        setParam( currentParam, v );
    }
}