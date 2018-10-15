#ifndef MINHEAP_H
#define MINHEAP_H

#include "node.h"

/* min-heap of static size, holding a weight and associated character */
/* sorted on the weight of the associated character */
class minheap
{
public:
	minheap();
	~minheap();

	void insert(node*);
	void insert(uint32_t, uint8_t);
	node* pop_smallest();
	size_t size();

private:
	node array[257]; /* an empty slot + 256 slots */
	size_t pos;

	void heapify(size_t);

};

#endif /* MINHEAP_H */
