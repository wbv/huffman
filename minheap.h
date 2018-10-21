/// \file minheap.h
/// \brief defines a customized min-heap
///
/// This file defines a customized min-heap which operates on
/// [nodes](@ref node).


#ifndef MINHEAP_H
#define MINHEAP_H

#include <cstddef>

#include "node.h"

/// \brief a static min-heap for up to 256 nodes
///
/// min-heap of static size, holding a weight and associated character
/// sorted on the weight of the associated character. max size is 256 (which is
/// all that's needed for this program)
class minheap
{
public:
	/// \brief constructor initializes empty heap
	///
	/// constructor initializes empty heap
	minheap();

	/// \brief dynamically-allocated node insert function
	///
	/// insert a dynamically-allocated node into the statically-stored minheap,
	/// deallocating it after storing it
	void insert(node*);
	/// \brief statically-allocated node insert function
	///
	/// insert a dynamically-allocated node into the statically-stored minheap,
	/// deallocating it after storing it
	void insert(uint32_t, uint8_t);
	/// \brief default heap-remove
	///
	/// dynamically allocates a node and pops out the minimum node on the heap.
	/// it's critical that you clean up (dealloc) after getting a node from this
	/// function! (use cleanTree)
	node* pop_smallest();
	/// \brief read-only indicator of how many items are in the heap
	///
	/// read-only indicator of how many items are in the heap
	size_t size();

private:
	/// \brief location of all heap data
	///
	/// primary storage mechanism for the heap. index 0 isn't treated as a spot
	/// in the heap, instead being used for temporary storage. this allows the
	/// children of a node at index `n` to be stored at `2n` and `2n+1`, all the
	/// way up to the max capacity of 256
	node array[257]; /* an empty slot + 256 slots */

	/// \brief keeps track of how many items are in the heap
	///
	/// empty heap = 0, full heap = 256. conveniently, array[pos] points to the
	/// last item on the heap
	size_t pos;

	/// \brief used to rebuild the heap after a single item is removed from the root
	///
	/// takes the index (1-indexed!) of a node in the heap, and if the children
	/// are out of order with that node, swap the smallest child with the parent
	void heapify(size_t);

};

#endif /* MINHEAP_H */
