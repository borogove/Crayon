#if CONFIG_CRAYON555

#define midiSerial Serial1

#endif

#define BUFFER_LENGTH 16

void setup_midi()
{
	midiSerial.begin(31250);
}


// TODO make midi state a class

byte currentData[3];
byte currentCommand = 0;
byte commandChannel = 0;

byte currentNote = 255;

short length = 0;

void noteOn( byte note, byte velocity )
{
    // TODO portamento
    deltaWavePhase = deltas[note];
    deltaResoPhase = (deltaWavePhase / 1024) * resoRatio;
    long peak15 = peak[velocity];
    peakLevel = peak15 << 8;
    sustainLevel = (peak15 * (long)sustainSetting) << 1;

    envelopeAttack();
}

void noteOff()
{
    envelopeRelease();
}

void controlChange( byte control, byte value )
{


}


void programChange( byte program )
{
    select_program(program);

}

bool midiFlicker = false;


void update_midi()
{
    set_led( 3, midiFlicker ? 1 : 0 );
    while (midiSerial.available() > 0)
    {
        byte midiByte = midiSerial.read();
        // Command byte?
        if (midiByte & 0x80)
        {
            byte cmd = midiByte & 0xF0;
            if (cmd != 0xF0)
            {
                midiFlicker = !midiFlicker;
                commandChannel = midiByte & 0x0F;                  
                currentCommand = cmd; 
                length = 0;
            }
        }
        else
        {
            midiFlicker = !midiFlicker;

            // Data byte
            if (length < 3)
            {
                currentData[length++] = midiByte;
            }

            switch (currentCommand)
            {
            case 0x80:  // note off
                if (length == 2)
                {
                    if (currentNote == currentData[0])
                    {
                        noteOff();
                    }
                    length = 0;
                }
                break;

            case 0x90:
                if (length == 2)
                {
                      //digitalWrite(PIN_LED_GRN, LOW );
                      noteOn(currentData[0],currentData[1]);
                      currentNote = currentData[0];
                      length = 0;
                }
                break;

            case 0xC0:
                if (length == 1)
                {
                    programChange(currentData[0]);
                    length =0;
                }
                break;

            case 0xB0:
            	if (length == 2)
            	{
            		controlChange(currentData[0],currentData[1]);
            		length = 0;
            	}
            	break;

            }
        }
    }
}

// K-rate = 375Hz
const int NUM_TEST_NOTES = 6;

short testOnTime[ NUM_TEST_NOTES ] = {150, 150, 300, 150, 150, 300};  // k rate ticks
short testOffTime[ NUM_TEST_NOTES ] = { 50, 50, 150,  50, 50, 150 }; // k rate ticks

byte testNotes[ NUM_TEST_NOTES ] = { 36, 43, 41, 48, 55, 53 }; // I V IV
byte testVelocity[ NUM_TEST_NOTES ] = { 120, 100, 80, 120, 100, 80 };
bool testTone = false;
long onCounter;
long offCounter;
int testIndex = 0;

void updateTestTone()
{
    if (testTone)
    {
        if (onCounter > 0)
        {
            onCounter--; 
            if (onCounter == 0)
            {
                offCounter = testOffTime[testIndex];
                noteOff();
            }           
        }
        else
        {
            if (offCounter > 0)
            {
                offCounter--;
                if (offCounter == 0)
                {
                    testIndex++;
                    if (testIndex == NUM_TEST_NOTES)
                    {
                        testIndex = 0;
                        //testTone = false;
                        noteOff();
                    }
                }
            }
            else
            {
                noteOn(testNotes[testIndex],testVelocity[testIndex]);
                onCounter = testOnTime[testIndex];
            }
        }
    }
}

void playTestTone()
{
    testTone = true;
}
