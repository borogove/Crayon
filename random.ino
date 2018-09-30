unsigned short stateX = 0x3B9A;
unsigned short stateY = 0xCDFD;

unsigned short rnd_xorshift_32() 
{
  unsigned short t = (stateX ^ ( stateY << 5 )); 
  stateX = stateY;
  stateY = (stateY ^ (stateY >> 1)) ^ (t ^ (t >> 3));
  return stateY;
}

void seedGen( unsigned short seed )
{
    stateX = 0x3B9A ^ seed;
    stateY = 0xCDFD ^ seed;
}

unsigned short genShort() { return rnd_xorshift_32(); }
byte genByte() { return rnd_xorshift_32() & 0xFF; }
bool genChoice() { return (rnd_xorshift_32() & 0x40) == 0; }
bool genPercent( int x ) { return rnd_xorshift_32() < ((x << 9) + (x << 7) + (x << 4)); }