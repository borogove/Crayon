#include <TimerThree.h>
#include "constants.h"
#include "tables.h"

const int usSamplePeriod = 1000000 / SAMPLE_RATE;
volatile bool dacBufferAvailable = false;

void setup_audioDac( void )
{
    Timer3.initialize( usSamplePeriod );
    Timer3.attachInterrupt( audioDac_interrupt );
}

class SampleChannel
{
public:

    short buffer[DAC_BUFFER_SIZE];
    volatile short readIndex;
    volatile short writeIndex;
    volatile long dropCount;
    volatile short prevSample;
    volatile long writeCount;
    volatile long readCount;
    
    SampleChannel()
    {
        readIndex = 0;
        writeIndex = DAC_BUFFER_SIZE/2;

        for (int i = 0; i < DAC_BUFFER_SIZE; i++)
        {
            buffer[i] = 0;
        }
        prevSample = 0;
        writeCount = 0;
        readCount = 0;
        dropCount = 0;
    }

    ~SampleChannel() {}

    short ReadNative()
    {
        if (readIndex == writeIndex)  
        {
            dropCount++;
        }
        else
        {
            prevSample = buffer[readIndex];
            readIndex++;
            if (readIndex == DAC_BUFFER_SIZE)
            {
                readIndex = 0;
            }
            readCount++;

        }

        return prevSample;
    }

    void WriteS16( short sample )
    {
        int nextIndex = writeIndex + 1;
        if (nextIndex == DAC_BUFFER_SIZE)
        {
            nextIndex = 0;
        }
        while (nextIndex == readIndex)
        {
            // wait here til read index advances
        }
            // convert to DAC format
        buffer[writeIndex] = (sample >> DAC_SHIFT) + DAC_BIAS;
        writeIndex = nextIndex;
        writeCount++;
    }

    void WriteU16( unsigned short sample )
    {
        int nextIndex = writeIndex + 1;
        if (nextIndex == DAC_BUFFER_SIZE)
        {
            nextIndex = 0;
        }
        while (nextIndex == readIndex)
        {
            // wait here til read index advances
        }
            // convert to DAC format
        buffer[writeIndex] = (sample >> DAC_SHIFT);
        writeIndex = nextIndex;
        writeCount++;
    }
};

// TODO stereo for Crayon655 series
SampleChannel audioChannel;

void audioDac_interrupt( void )
{
    analogWrite( DAC_A, audioChannel.ReadNative() );
    dacBufferAvailable = true;
    //tick();

    // clock the LCD parameter updates
    if (display_update_wait > 0)
    {
        display_update_wait--;
    }
}
