#ifndef UTF8_H
#define UTF8_H

#include <cstdint>

using std::uint8_t;
using std::uint32_t;

// utf-8 encoding (circa '93) allows for a 1-6 byte variable encoding
// of up to 31 bits of data.
// this struct neatly stores the number of bytes encoded as well as the actual
// string of bytes, and fits into an 8-byte chunk of memory.
struct utf8_t {
	uint8_t nbytes;
	uint8_t encoded[6];
};


// returns a utf-8 encoded string of bytes as well as a helper value indicating
// the length of the encoded string.
// if val is too large, a 0-byte string is returned indicating failure
utf8_t getUTF8(uint32_t val);

// perform the inverse of getUTF8: take an encoded byte and return
// the value decoded back to a uint32_t
uint32_t getUInt(utf8_t   val);

#endif

