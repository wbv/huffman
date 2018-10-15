#ifndef NODE_H
#define NODE_H

#include <cstdint>
using std::uint32_t;
using std::uint8_t;

struct node
{
	node* left;
	node* right;
	uint32_t weight;
	uint8_t ch;
	bool isLeaf() { return (left == nullptr) and (right == nullptr); }
};

#endif /* NODE_H */
