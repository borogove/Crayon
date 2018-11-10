#ifdef __AVR

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

#else

// https://en.wikipedia.org/wiki/Xorshift#xorwow

/* The state array must be initialized to not be all zero in the first four words */
uint32_t xorwow(uint32_t *state)
{
	/* Algorithm "xorwow" from p. 5 of Marsaglia, "Xorshift RNGs" */
	uint32_t s, t = state[3];
	t ^= t >> 2;
	t ^= t << 1;
	state[3] = state[2]; state[2] = state[1]; state[1] = s = state[0];
	t ^= s;
	t ^= s << 4;	
	state[0] = t;
	return t + (state[4] += 362437);
}

uint32_t state[5] = { 0x3b9acdfd,
                      0x1ef34cdf,
                      0x51e552b9,
                      0x30bca9de,
                      0x3ba38da4 };


void seedGen( int seed )
{
  state[0] = 0x3b9acdfd ^ seed;
  state[1] = 0x1ef34cdf ^ (seed << 3);
  state[2] = 0x51e552b9 ^ (seed << 6);
  state[3] = 0x30bca9de ^ (seed << 9);
  state[4] = 0x3ba38da4 ^ (seed << 12); 
}

uint32_t gen() { return xorwow(state); }
unsigned short genShort() { return xorwow(state) & 0xFFFF; }
int genByte() { return xorwow(state) & 0xFF; }
bool genChoice() { return (xorwow(state) & 0x40) == 0; }
bool genPercent( unsigned x ) { return (xorwow(state) & 0xFFFF) < ((x << 9) + (x << 7) + (x << 4)); }

#endif