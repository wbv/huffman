#ifndef HUFFCODE_H
#define HUFFCODE_H

#include <cstdint>
#include <fstream>

#include "node.h"

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::ifstream;
using std::ofstream;

/// represents a single Huffman code point, up to 16 bits long
struct huffcode_t {
	/// indicates how many bits are in the code
	uint8_t bitcnt;
	/// contains the bits actually in the Huffman code point.
	/// The first bit of the huffman code is stored most significant, and the
	/// last bit in the least significant bit of `bits`.
	uint16_t bits; };

/// takes a histogram, makes a heap, then turns the heap into a tree for parsing
/// into a huffman code table
node* getTreeFromHist(uint32_t hist[256]);

/// recursive function that will clean up the huffman code tree structure after
/// you're done with it.
void cleanTree(node* n);

/// fills the supplied huffmap from a tree, returns false on failure
void getHuffMapFromTree(huffcode_t* map, node* root, uint8_t bits = 0, uint16_t bitcnt = 0);

/// given an array which maps bytes to huffman codes, read from fin (start at 0)
/// and write out to fout (starting where it was left at)
void writeHuffman(huffcode_t huffmap[256], ifstream& fin, ofstream& fout);

/// given the code tree for huffman, read code from fin (starting where it was
/// left at) and write out the actual byte to fout (start at 0)
void readHuffman(node* root, ifstream& fin, ofstream& fout);

#endif
