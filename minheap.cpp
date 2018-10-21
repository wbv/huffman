#ifdef _DEBUG
#include <iostream>
#endif

#include "minheap.h"


minheap::minheap()
{
	pos = 0;
}

void minheap::insert(node* n)
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
	for (i = ++(this->pos); i/2 and n->weight < array[i/2].weight; i/=2)
		/* move parent node down the heap */
		array[i] = array[i/2];

	/* we found the spot for our data */
	array[i].left = n->left;
	array[i].right = n->right;
	array[i].weight = n->weight;
	array[i].ch = n->ch;

	/* if the node passed in wasn't from the "empty spot" on the array stack */
	/* then clean up its memory footprint from the heap */
	if (n != array)
		delete n;
}

void minheap::insert(uint32_t freq, uint8_t ch)
{
	array[0].weight = freq;
	array[0].ch = ch;
	array[0].left = nullptr;
	array[0].right = nullptr;
	insert(array);
}

node* minheap::pop_smallest()
{
	/* initialize an invalid heap node */
	node* small = new node;
	small->weight = (uint32_t)-1;
	small->ch = 0;

	/* check for empty heap */
	if (pos == 0)
	{
#ifdef _DEBUG
		std::cerr << "Warning: tried to remove from empty heap.\n";
#endif
		return small;
	}

	/* minheap --> smallest is at the root */
	small->left = array[1].left;
	small->right = array[1].right;
	small->weight = array[1].weight;
	small->ch = array[1].ch;

	/* make sure the heap is restored if it isn't now empty */
	if ((--pos) > 0)
	{
		array[1] = array[pos+1];
		heapify(1);
	}

	return small;
}

/* starting from some node, percolate nodes up to correct ordering, */
/* traversing down until the ordering is correct */
void minheap::heapify(size_t start)
{
	size_t left = start*2;
	size_t right = start*2 + 1;

	/* if there are no children, we're at a leaf, so the ordering is correct */
	if (left > pos)
		return;

	/* if left child exists but right does not, */
	/*  and it's out of order, fix its order and then recurse */
    /* if both exist and the left is smaller than the right */
	/*  recursively fix order if it's incorrect */
	else if (right > pos or array[left].weight < array[right].weight)
	{
		if (array[left].weight < array[start].weight)
		{
			node tmp = array[left];
			array[left] = array[start];
			array[start] = tmp;
			heapify(left);
		}
		return;
	}

	else // array[left].weight >= array[right].weight
	{
		if(array[right].weight < array[start].weight)
		{
			node tmp = array[right];
			array[right] = array[start];
			array[start] = tmp;
			heapify(right);
		}
		return;
	}
}

size_t minheap::size()
{
	return pos;
}
