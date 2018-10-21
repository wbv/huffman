#ifndef UTF8_H
#define UTF8_H

#include <cstdint>

using std::uint8_t;
using std::uint32_t;

/// a utf-8 encoded number
///
/// utf-8 encoding (circa '93) allows for a 1-6 byte variable encoding
/// of up to 31 bits of data.
/// this struct neatly stores the number of bytes encoded as well as the actual
/// string of bytes, and fits into an 8-byte chunk of memory.
struct utf8_t {
	/// number of bytes needed to store an instances value
	///
	/// indicates how many bytes are needed to store the encoded value
	uint8_t nbytes;
	/// the bytes representing the character
	///
	/// physical bytes representing a utf-8 character. Only indices 0 through
	/// nbytes-1 are valid.
	uint8_t encoded[6];
};


/// uint32_t -> utf8_t
///
/// returns a utf-8 encoded string of bytes as well as a helper value indicating
/// the length of the encoded string.
/// if val is too large, a 0-byte string is returned indicating failure
utf8_t getUTF8(uint32_t val);

/// utf8_t -> uint32_t 
///
/// perform the inverse of getUTF8: take an encoded byte and return
/// the value decoded back to a uint32_t
uint32_t getUInt(utf8_t   val);

#endif

