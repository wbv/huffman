#ifndef HUFFTREE_H
#define HUFFTREE_H

#include <iostream>

#include "node.h"

class hufftree
{
public:
	hufftree(node*);
	~hufftree();

	void printTree();

private:
	node* root;
	void delete_subtree(node* n);
	void printTree(node* n, char txt[16], int depth);
};

#endif /* HUFFTREE_H */
