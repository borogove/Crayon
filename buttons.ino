#include "constants.h"
#include "common.h"

action buttonActions[MAX_PINS];
bool buttonState[MAX_PINS];

int debounce[MAX_PINS];

void setup_buttons()
{
    pinMode( BUTTON_A, INPUT );  
    pinMode( BUTTON_B, INPUT );  

    for (int i = 0; i < MAX_PINS; i++)
    {
      buttonActions[i] = nullptr;
    }
}

bool poll_button( int pinId )
{
  if (debounce[pinId] > 0)
  {
    debounce[pinId]--;
    return false;
  }
    // is the button down?
    if (buttonState[pinId])
    {
        if (digitalRead(pinId) == HIGH)
        {
            // button came up
            buttonState[pinId] = false;
            if (buttonActions[pinId] != nullptr)
            {
                (*buttonActions[pinId])();
            }
            debounce[pinId] = 350;
            return true;
        }
    }
    else
    {
        if (digitalRead(pinId) == LOW)
        {
            // button went down
            buttonState[pinId] = true;
        }
    }

    /// everything else is not rising-edge
    return false;
}

void update_buttons()
{
    poll_button(BUTTON_A);
    poll_button(BUTTON_B);
}

bool buttonDown( int buttonId )
{
    return buttonState[buttonId];
}

void set_button_action( int buttonId, action a )
{
    buttonActions[buttonId] = a;
}
