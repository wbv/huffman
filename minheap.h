#ifndef MINHEAP_H
#define MINHEAP_H

#include <cstddef>

#include "node.h"

/// min-heap of static size, holding a weight and associated character
/// sorted on the weight of the associated character. max size is 256 (which is
/// all that's needed for this program)
class minheap
{
public:	
	/// constructor initializes empty heap
	minheap();
	/// destructor doesn't have a job but needs a paycheck
	~minheap();

	/// insert a dynamically-allocated node into the statically-stored minheap,
	/// deallocating it after storing it
	void insert(node*);
	/// insert a node into the heap from literal values
	void insert(uint32_t, uint8_t);
	/// default heap-remove.
	node* pop_smallest();
	/// read-only indicator of how many items are in the heap
	size_t size();

private:
	/// primary storage mechanism for the heap. index 0 isn't treated as a spot
	/// in the heap, instead being used for temporary storage. this allows the
	/// children of a node at index `n` to be stored at `2n` and `2n+1`, all the
	/// way up to the max capacity of 256 
	node array[257]; /* an empty slot + 256 slots */

	/// keeps track of how many items are in the heap
	size_t pos;

	/// used to rebuild the heap after a single item is removed from the root
	void heapify(size_t);

};

#endif /* MINHEAP_H */
