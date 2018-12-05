/// \file huffcode.h
/// \brief defines operations on the Huffman code tree, map, & translation
///
/// This file defines operations on the Huffman code tree, the mapping between
/// bytes and their corresponding Huffman code, and the translations between
/// bytes and codes in each direction. 


#ifndef HUFFCODE_H
#define HUFFCODE_H

#include <cstdint>
#include <fstream>

#include "node.h"

using std::uint8_t;
using std::uint32_t;
using std::uint64_t;
typedef unsigned __int128 uint128_t;
using std::ifstream;
using std::ofstream;

/// \brief represents a single Huffman code point, up to 128 bits long
///
/// represents a single Huffman code point, up to 128 bits long
struct huffcode_t
{
	/// \brief indicates how many bits are in the code
	///
	/// indicates how many bits are in the code
	uint8_t bitcnt;
	/// \brief contains the code itself
	///
	/// contains the bits actually in the Huffman code point.
	/// The first bit of the huffman code is stored most significant, and the
	/// last bit in the least significant bit of `bits`.
	uint128_t bits;
};

/// \brief turns a histogram into its corresponding huffman code tree
///
/// takes a histogram, makes a heap, then turns the heap into a tree for parsing
/// into a huffman code table
node* getTreeFromHist(uint32_t hist[256]);

/// \brief anti-memory-leak weapon. aim at root of the huffman code tree
///
/// recursive function that will clean up the huffman code tree structure after
/// you're done with it.
void cleanTree(node* n);

/// \brief fills `map` based on the tree given by `root`
///
/// fills the supplied huffmap from a tree,
/// returns false on failure. the map is an array that gives O(1) lookup to 
void getHuffMapFromTree(huffcode_t* map, node* root, uint8_t bits = 0, uint128_t bitcnt = 0);

/// \brief use `huffmap` to translates bytes of `fin` to codes in `fout`
///
/// given an array which maps bytes to huffman codes, read from fin (start at 0)
/// and write out to fout (starting where it was left at)
void writeHuffman(huffcode_t huffmap[256], ifstream& fin, ofstream& fout);

/// \brief use `huffmap` to translates huffman codes of `fin` to bytes in `fout`
///
/// given the code tree for huffman, read code from fin (starting where it was
/// left at) and write out the actual byte to fout (start at 0)
void readHuffman(node* root, ifstream& fin, ofstream& fout);

#endif
