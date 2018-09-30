
int ledPins[] =  { LED_A, LED_B, LED_C, LED_D };
int ledState[] = { 0,0,0,0 };

bool do_ripple = true;
int ripple_counter = 0;
int blink_counter = 0;
bool blink_state = false;


void setup_leds()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    pinMode( ledPins[i], OUTPUT );
  }
}

int ledCounter = 0;
void rippleLeds()
{
  ledCounter++;
  if (ledCounter == NUM_LEDS)
  {
    ledCounter = 0;
  }

  for (int i = 0; i < NUM_LEDS; i++)
  {
    digitalWrite( ledPins[i], ledCounter == i ? HIGH : LOW );
  }

}

void refreshLeds()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
  	int pinState = LOW;
  	if (ledState[i] == 1) { pinState = HIGH; }
  	if (ledState[i] == 2) { pinState = blink_state ? HIGH : LOW; }
    digitalWrite( ledPins[i], pinState );
  }	
}


void update_leds()
{
  if (do_ripple)
  {
    ripple_counter++;
    if (ripple_counter > 30000)
    {
      rippleLeds();
      ripple_counter = 0;
    }
  } 
  else
  {
  	blink_counter++;
  	if (blink_counter > 30000)
  	{
  		blink_state = !blink_state;
     blink_counter = 0;
  	}
  	refreshLeds();
  } 
}

void set_led( int i, int state )
{
  if (do_ripple)
  {
    do_ripple = false;
  }
  ledState[i] = state;
}

