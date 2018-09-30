

long getRate( int setting )
{
    long a = envRateTable[setting];     // 19 bits of 23
    return ((a >> 4) * (peakLevel >> 8)) >> 11; // 15 x 15 = 30 
}

void envelopeAttack()
{
    envState = STATE_ATTACK;

}

void envelopeRelease()
{
    if (envState != STATE_IDLE)
    {
        envState = STATE_RELEASE;
    }
}

void tickEnvelope()
{
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

}

void updateEnvelope()
{
    switch (envState)
    {
        case STATE_IDLE: 
            envLevel = 0; 
            envRate = 0; 
            break;
        case STATE_ATTACK:
            // allow rate changes during the envelope
            envRate = getRate(attackSetting);
            if (envLevel >= peakLevel)
            {
                if (decayEnabled)
                {
                    envState = STATE_DECAY;
                    envRate = -getRate(decaySetting);
                }
                else
                {
                    envState = STATE_SUSTAIN;
                    envRate = 0;
                }
            }
            break;

        case STATE_DECAY:
            // allow rate changes during the envelope
            if (envLevel <= sustainLevel)
            {
                envState = STATE_SUSTAIN;
                envRate = 0;
            }
            else
            {
                envRate = -getRate(decaySetting);
            }
            break;            

        case STATE_SUSTAIN:
            // nothing happens!
            envRate = 0;
            break;

        case STATE_RELEASE:
            // allow rate changes during the envelope
            envRate = -getRate(releaseSetting);
            if (envLevel < 0)
            {
                envState = STATE_IDLE;
                envRate = 0;
                envLevel = 0;
            }
            break;            
    }
}
