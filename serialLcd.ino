//SoftwareSerial lcd = SoftwareSerial(26,10);
#define lcd Serial2

void serialLcdClear()
{
  // clear sparkfun LCD
  lcd.write(254);
  lcd.write(1);  
}

void serialLcdSetCursor( int pos )
{
    // clear sparkfun LCD
  lcd.write(254);
  lcd.write(128);
  lcd.write(pos);
}
void serialLcdCursorLeft()
{
    // clear sparkfun LCD
  lcd.write(254);
  lcd.write(0x10);
}
void serialLcdCursorRight()
{
    // clear sparkfun LCD
  lcd.write(254);
  lcd.write(0x14);
}
void serialLcdScrollLeft()
{
    // clear sparkfun LCD
  lcd.write(254);
  lcd.write(0x18);
}
void serialLcdScrollRight()
{
    // clear sparkfun LCD
  lcd.write(254);
  lcd.write(0x1c);
}

void displayAt(int x, int y, char* text)
{
  serialLcdSetCursor(x + (y*16));
  lcd.print(text);
}
void display(char* text)
{
  lcd.print(text);
}

void setup_serialLcd()
{
  delay(100);


  lcd.setRX(26);
  lcd.begin(9600);

  serialLcdClear();

  delay(100);
  
  serialLcdSetCursor(0);
  
  lcd.print("hullo this is   ");
  lcd.print("rage bear       ");
}


