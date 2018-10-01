#include <ADC.h>

analog_action knobActions[MAX_PINS];

int supportedKnobs[] = { KNOB_A, KNOB_B };
ADC *adc = new ADC(); // adc object

void setup_knobs()
{
        for (int i = 0; i < MAX_PINS; i++)
    {
      knobActions[i] = nullptr;
    }
    for (int i = 0; i < NUM_KNOBS; i++)
    {
        pinMode( supportedKnobs[i], INPUT );
    }

    // docs at https://platformio.org/lib/show/355/Teensy_ADC/examples?file=analogContinuousRead.ino
    adc->setAveraging(16,ADC_0);
    adc->setResolution(10,ADC_0);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED,ADC_0); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED,ADC_0); // change the sampling speed
    adc->enableInterrupts(ADC_0);
    adc->startContinuous(supportedKnobs[0], ADC_0);

    adc->setAveraging(16,ADC_1);
    adc->setResolution(10,ADC_1);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_1); // change the sampling speed
    adc->enableInterrupts(ADC_1);
    adc->startContinuous(supportedKnobs[1], ADC_1);
}

//void poll_knob( int knob )
//{
//    int v = analogRead( knob ) >> 2;   
//    if (knobActions[knob] != nullptr)
//    {
//        (*knobActions[knob])(v);
//    }
//}

void update_knobs()
{
    //for (int i = 0; i < NUM_KNOBS; i++)
    //{
    //   poll_knob( supportedKnobs[i] );
    //
    //}
}

void set_knob_action( int pin, analog_action aa )
{
    knobActions[pin] = aa;
}

int dejitter = 2;
int current_value[2] = {-1, -1};

void adc0_isr(void) {
    int v10 = adc->analogReadContinuous(ADC_0);
    int delta = current_value[0] - v10;
    if ((delta < -dejitter) || (delta > dejitter))
    {
        current_value[0] = v10;
        if (knobActions[supportedKnobs[0]] != nullptr)
        {
            (*knobActions[supportedKnobs[0]])(v10>>2);
        }
    }
}

void adc1_isr(void) {
    int v10 = adc->analogReadContinuous(ADC_1);
    int delta = current_value[1] - v10;
    if ((delta < -dejitter) || (delta > dejitter))
    {
        current_value[1] = v10;
        if (knobActions[supportedKnobs[1]] != nullptr)
        {
            (*knobActions[supportedKnobs[1]])(v10>>2);
        }
    }
}

