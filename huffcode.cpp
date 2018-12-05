#include <iostream>
using std::cerr;

#include "minheap.h"
#include "huffcode.h"
#include "node.h"

#define BUFSIZE (256/8)


void getHuffMapFromTree(huffcode_t* map, node* root, uint8_t bitcnt, uint128_t bits)
{
	if (root->isLeaf())
	{
		map[root->ch].bitcnt = bitcnt;
		map[root->ch].bits = bits;
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

// recursive function that will clean up the huffman code tree structure after
// you're done with it.
void cleanTree(node* n)
{
	if (n == nullptr)
		return;

	cleanTree(n->left);
	cleanTree(n->right);

	delete n;
}

// given an array which maps bytes to huffman codes, read from fin (start at 0)
// and write out to fout (starting where it was left at)
void writeHuffman(huffcode_t huffmap[256], ifstream& fin, ofstream& fout)
{
	/* stores a byte read in from infile */
	uint8_t inputbyte;
	/* buffers the huffman codes until a byte can be written */
	uint8_t buffer[BUFSIZE] = {0};
	/* keeps track of the number of bits shifted into buffer */
	int bufferedbits = 0;


	/* restart the reading of fin, clearing any flags before doing so */
	fin.clear();
	fin.seekg(0);
	if (not fin)
	{
		cerr << "Error: failed to seek to beginning of infile for Pass 2\n";
		return;
	}

	/* continuously read a character until either fin is unreadable or fout has
	 * problems */
	fin.get((char&)inputbyte);
	while (fin.gcount() > 0 and fout)
	{
		/* shift in the bits for the byte's huffman code, */
		/* one at a time, from left to right */
		uint128_t bits = huffmap[inputbyte].bits;
		uint8_t bitcnt = huffmap[inputbyte].bitcnt;
		for (int i = bitcnt - 1; i >= 0; i--)
		{
			size_t bufpos = bufferedbits / 8;
			size_t bitpos = bufferedbits % 8;
			buffer[bufpos] |= ((bits >> i) & 0x1) << bitpos;
			bufferedbits++;
		}

		/* as long as a byte can be written, */
		/* write it and clear it from the buffer */
		while (bufferedbits >= 8)
		{
			fout.put(buffer[0]);
			for (int i = 0; i < (BUFSIZE-1); i++)
				buffer[i] = buffer[i+1];
			bufferedbits -= 8;
		}

		/* try to fetch the next byte*/
		fin.get((char&)inputbyte);
	}

	/* the last byte in the encoded file says how many trailing zero's are in */
	/* the final encoded byte (second-to-last physical byte in the file) */
	if (fin.gcount() == 0)
	{
		/* empty the buffer */
		if (bufferedbits > 0)
		{
			fout.put(buffer[0]);
			/* only between 1 and 7 bits buffered so this will be correct */
			fout.put(8 - bufferedbits);
		}
		else
		{
			/* buffer empty already? 0 trailing bits */
			fout.put(0);
		}
	}
	else /* fout must be bad */
	{
		cerr << "Error encountered while writing encoded data to outfile.\n";
	}

}


// given the code tree for huffman, read code from fin (starting where it was
// left at) and write out the actual byte to fout (start at 0)
void readHuffman(node* root, ifstream& fin, ofstream& fout)
{
	/* stores a byte read in from infile, nextbyte stores the next one read */
	uint8_t inputbyte, nextbyte;
	/* buffers the huffman codes read in in byte-sized chunks at a time */
	uint8_t buffer[BUFSIZE] = {0};
	/* counts how many bits are in the buffer */
	int bufferedbits;
	/* used for traversing the code tree */
	node* traverse = root;
	/* indicates when terminating byte is found */
	bool lastrun = false;

	/* at this point, fin should be good, and at the byte immediately after */
	/*	the histogram, ready for writing. if not, bail out */
	if (not fin)
	{
		cerr << "Error: failed to read infile after parsing histogram\n";
		return;
	}

	/* make sure we write the outfile from the beginning */
	fout.seekp(0);
	if (not fout)
	{
		cerr << "Error: failed to write to start of outfile\n";
		return;
	}

	/* start buffering data and keeping track of it. Safe to assume there */
	/* will be at least one byte before the terminating one, so this counts */
	/* as buffered data. */
	fin.get((char&)buffer[0]);
	bufferedbits = 8;
	fin.get((char&)inputbyte);
	while (fout and not lastrun)
	{
		fin.get((char&)nextbyte);

		/* check for the terminating byte. if found, empty the buffer */
		if (fin.gcount() == 0)
		{
			/* set our loop to terminate */
			lastrun = true;
			/* subtract terminating byte's # of trailing bits from buffer */
			bufferedbits -= inputbyte;
		}
		/* otherwise inputbyte actually holds data and not the number of */
		/* trailing bits (terminating byte), so add it to the buffer */
		else
		{
			for (size_t i = 0; i < 8; i++)
			{
				size_t bufpos = (bufferedbits) / 8;
				size_t bitpos = (bufferedbits) % 8;
				buffer[bufpos] |= ((inputbyte >> i) & 0x1) << bitpos;
				bufferedbits++;
			}
			inputbyte = nextbyte;
		}

		/* maintain at least 128 bits in buffer so tree traversal can make */
		/* it from the root to the furthest leaf, unless we hit the end,  */
		/* in which case just read 'til the buffer's empty */
		while ((bufferedbits > (lastrun ? 0 : 127)) and fout)
		{
			/* read a bit and traverse the tree in that dir, stopping once */
			/* a leaf is found */
			while (not traverse->isLeaf())
			{
				uint8_t bit = buffer[0] & 1;
				/* shift buffer down by a bit, 1 byte of buffer at a time */
				for (int i = 0; i < BUFSIZE; i++)
				{
					uint8_t carrybit = (i == (BUFSIZE-1) ? 0 : buffer[i+1] & 1);
					buffer[i] >>= 1;
					buffer[i] |= (carrybit << 7);
				}
				bufferedbits--;
				if (bit == 1)
					traverse = traverse->right;
				else // bit == 0
					traverse = traverse->left;
			}

			/* once a leaf is found, write the byte out */
			fout.put((char)traverse->ch);
			traverse = root; /* reset the traversal back to root */
		}
	}
}

// takes a histogram, makes a heap, then turns the heap into a tree for parsing
// into a huffman code table
node* getTreeFromHist(uint32_t hist[256])
{
	minheap heap = minheap();
	node* left;
	node* right;
	node* top;

	/* fill the heap with every histogram entry if it's nonzero */
	for (size_t i = 0; i < 256; i++)
		if (hist[i])
			heap.insert(hist[i], static_cast<uint8_t>(i));

	/* special case -- tree has only one node: default to code 0 */
	if (heap.size() == 1)
	{
		top = new node;
		left = heap.pop_smallest();
		top->weight = left->weight;
		top->left = left;
		return top;
	}

	while (heap.size() > 1)
	{
		left = heap.pop_smallest();
		right = heap.pop_smallest();
		top = new node;
		top->weight = left->weight + right->weight;
		top->left = left;
		top->right = right;
		heap.insert(top);
	}

	top = heap.pop_smallest();

	return top;
}
