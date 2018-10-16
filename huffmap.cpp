#ifdef _DEBUG
#include <iostream>
#endif

#include "huffmap.h"
#include "node.h"

void getHuffMapFromTree(huffcode_t* map, node* root, uint8_t bitcnt, uint16_t bits)
{
	if (root->isLeaf())
	{
		map[root->ch].bitcnt = bitcnt;
		map[root->ch].bits = bits;
#ifdef _DEBUG
		std::cout << root->ch << " : ";
		for (int i = 0; i < bitcnt; i++)
			std::cout << (((bits >> (bitcnt - i - 1)) & 1) ? '1' : '0');
		std::cout << std::endl;
#endif
		return;
	}

	/* recursively calculate code for left and right subtrees */
	bitcnt++;   /* add another bit */
	bits <<= 1; /* that bit is 0   */
	getHuffMapFromTree(map, root->left, bitcnt, bits);
	bits |= 1;  /* now it's 1      */
	getHuffMapFromTree(map, root->right, bitcnt, bits);
	bits >>= 1; /* remove that bit */
	bitcnt--;

	return;
}
