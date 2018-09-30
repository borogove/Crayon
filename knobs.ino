analog_action knobActions[MAX_PINS];

int supportedKnobs[] = { KNOB_A, KNOB_B };

void setup_knobs()
{
    analogReadResolution(10);
    for (int i = 0; i < NUM_KNOBS; i++)
    {
        pinMode( supportedKnobs[i], INPUT );
    }

    for (int i = 0; i < MAX_PINS; i++)
    {
      knobActions[i] = nullptr;
    }
}

void poll_knob( int knob )
{
    int v = analogRead( knob ) >> 2;   
    if (knobActions[knob] != nullptr)
    {
        (*knobActions[knob])(v);
    }
}
void update_knobs()
{
   for (int i = 0; i < NUM_KNOBS; i++)
    {
        poll_knob( supportedKnobs[i] );
    }
}

void set_knob_action( int pin, analog_action aa )
{
    knobActions[pin] = aa;
}