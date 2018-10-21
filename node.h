/// \file node.h
/// \brief defines the node used in the Huffman code tree and heap


#ifndef NODE_H
#define NODE_H

#include <cstdint>
using std::uint32_t;
using std::uint8_t;

/// \brief huffman code tree building block
///
/// Nodes form the data structure for the huffman code tree. They are either
/// leafs (indicated by `isLeaf()`) or they carry pointers to left AND right
/// subchildren. If they are leafs, `ch` corresponds to a unique byte who
/// appears in the source file `weight` number of times.
///
/// Traversing this tree structure yields a huffman code which is a
/// variable-lenth sequence of bits which represent the order and direction of
/// traverses down the tree, where 0 is a left traverse and 1 is a right
/// traverse. Once a leaf is found, that sequence yields the huffman code
/// corresponding to the leaf's `ch`.
struct node
{
	/// \brief pointer to child of the current node
	///
	/// `left` and `right` are pointers to children. If left is valid, then
	/// right should also point to a valid child. If both are nullptr, then this
	/// node is a leaf, and `ch` will be a unique byte among the set of leaves.
	node* left;
	/// \brief pointer to child of the current node
	///
	/// `left` and `right` are pointers to children. If left is valid, then
	/// right should also point to a valid child. If both are nullptr, then this
	/// node is a leaf, and `ch` will be a unique byte among the set of leaves.
	node* right;

	/// \brief number of times `ch` appears in a file OR the combined weight of children
	///
	/// `weight` refers to the frequency of a character if the node is a leaf,
	/// or the combined weights of the subtrees in the left and right subtrees.
	/// In turn, each node's weight corresponds to the total frequency of all of
	/// the characters in the subtrees below.
	uint32_t weight;

	/// \brief a byte which appears in a source file
	///
	/// If the node is a leaf, `ch` is a byte, and its location in the tree
	/// (from the desription of a node) forms its corresponding huffman code.
	/// If this node is not a leaf, then `ch` has no meaning.
	uint8_t ch;

	/// \brief batman-syndrome-identifier
	///
	/// Indicates whether this node is a leaf in its tree or an interior node
	bool isLeaf() { return (left == nullptr) and (right == nullptr); }
};

#endif /* NODE_H */
