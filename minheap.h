#ifndef MINHEAP_H
#define MINHEAP_H

/* min-heap of static size, holding a weight and associated character */
/* sorted on the weight of the associated character */
class minheap
{
public:
	struct data
	{
		uint32_t weight;
		uint8_t ch;
	};

	minheap();
	~minheap();

	void insert(data);
	void insert(uint32_t, uint8_t);
	data pop_smallest();
	size_t size();

private:
	data array[257]; /* an empty slot + 256 slots */
	size_t pos;

	void heapify(size_t);

};

#endif /* MINHEAP_H */
