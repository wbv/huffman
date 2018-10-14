#ifdef _DEBUG
#include <iostream>
#endif

#include "minheap.h"


minheap::~minheap()
{
}

minheap::minheap()
{
	pos = 0;
}

void minheap::insert(data d)
{
	/* i = index of heap to insert new data */
	size_t i;

	if (pos >= 256)
	{
#ifdef _DEBUG
		std::cerr << "Warning: tried to insert into full heap.\n";
#endif
		return;
	}
	
	/* loop while parent node is greater than inserted, and we are't at root */
	for (i = ++(this->pos); i/2 and d.weight < array[i/2].weight; i/=2) 
		/* move parent node down the heap */
		array[i] = array[i/2];

	/* we found the spot for our data */
	array[i] = d;
}

void minheap::insert(uint32_t freq, uint8_t ch)
{
	minheap::data d = {freq, ch};
	insert(d);
}

minheap::data minheap::pop_smallest()
{
	/* initialize an invalid heap node */
	minheap::data small = {(uint32_t)-1, 0}; 

	/* check for empty heap */
	if (pos == 0)
	{
#ifdef _DEBUG
		std::cerr << "Warning: tried to remove from empty heap.\n";
#endif
		return small;
	}

	/* minheap --> smallest is at the root */
	small = array[1];

	/* make sure the heap is restored if it isn't now empty */
	if ((--pos) > 0)
		heapify(1);
	
	return small;
}

void minheap::heapify(size_t start)
{
	size_t left = start*2;
	size_t right = start*2 + 1;

	/* if there are no children, we're at a leaf, so put the */
    /* leaf which was at the end of the array in this hole */
	if (left > pos)
	{
		array[start] = array[pos+1];
		return;
	}

	/* if left child exists but right does not, */
    /* or both exist and the left is smaller than the right */
	if (right > pos or array[left].weight < array[right].weight)
	{
		array[start] = array[left];
		heapify(left);
	}
	/* both children exist and the right is smaller */
	else
	{
		array[start] = array[right];
		heapify(right);
	}
}

size_t minheap::size()
{
	return pos;
}
