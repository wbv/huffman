#include "utf8.h"

// returns a utf-8 encoded string of bytes as well as a helper value indicating
// the length of the encoded string.
// if val is too large, a 0-byte string is returned indicating failure
utf8_t getUTF8(uint32_t val)
{
	utf8_t code;
	code.nbytes = 0;

	uint32_t upperbound = 1<<31;
	uint8_t firstbytemask = 0xfc;
	
	/* vals less than 128 are just that byte */
	if (val < 0x80)
	{
		code.nbytes = 1;
		code.encoded[0] = (uint8_t)val;
		return code;
	}
		
	/* numbers larger than 2^31-1 can't be encoded, return 0-byte encoding */
	if (val >= upperbound)
		return code;

	/* val needs a 2 to 6-byte encoding */
	for (code.nbytes = 6; val < (upperbound >> 5); code.nbytes--)
	{
		upperbound >>= 5;
		firstbytemask <<= 1;
	}
	
	/* shift out everything that goes into bytes 2 through n */
	/* and apply the appropriate first-byte mask */
	code.encoded[0] = val >> ((code.nbytes - 1) * 6);
	code.encoded[0] |= firstbytemask;

	for (int i = 1; i < code.nbytes; i++)
	{
		/* grab the next 6 bits of val, keep the right 6 bits, */
		/* and prepend 0b10 to first 2 bits */
		code.encoded[i] = (uint8_t)(val >> 6*((code.nbytes - 1) - i));
		code.encoded[i] &= 0x3f;
		code.encoded[i] |= 0x80;
	}

	return code;
}


// perform the inverse of getUTF8: take an encoded byte and return
// the value decoded back to a uint32_t
uint32_t getUInt(utf8_t code)
{
	uint32_t val;
	uint8_t firstbytemask;

	/* single byte encoding means return immediately */
	if (code.encoded[0] < 0x80)
		return code.encoded[0];

	/* mask out the relevant first bits out of the first byte */
	firstbytemask = (2 << (6 - code.nbytes)) - 1;
	val = firstbytemask & code.encoded[0];

	/* grab 6 bits at a time of the remaining bytes */
	for (int i = 1; i < code.nbytes; i++)
	{
		val <<= 6;
		val += code.encoded[i] & 0x3f;
	}

	return val;
}
