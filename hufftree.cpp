#include "hufftree.h"

hufftree::~hufftree()
{
	delete_subtree(root);
}


hufftree::hufftree(node* n)
{
	root = n;
}


void hufftree::delete_subtree(node* n)
{
	if (n == nullptr)
		return;

	delete_subtree(n->left);
	delete_subtree(n->right);

	delete n;
}

void hufftree::printTree()
{
	char txt[16] = {0};
	printTree(root, txt, 1);
}

void hufftree::printTree(node* n, char txt[16], int depth)
{
	if (n->isLeaf())
	{
		std::cout << n->ch << " : ";
		for (int i = 0; i < depth-1; i++)
			std::cout << txt[i];
		std::cout << std::endl;
		return;
	}

	txt[depth-1] = '0';
	printTree(n->left, txt, depth+1);
	txt[depth-1] = '1';
	printTree(n->right, txt, depth+1);
}
