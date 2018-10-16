#ifndef HUFFMAP_H
#define HUFFMAP_H

#include <cstdint>

#include "node.h"

using std::uint8_t;

struct huffcode_t {
	uint8_t bitcnt;
	uint16_t bits;
};

// fills the supplied huffmap from a tree, returns false on failure
void getHuffMapFromTree(huffcode_t* map, node* root, uint8_t bits = 0, uint16_t bitcnt = 0);


#endif
