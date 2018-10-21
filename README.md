CSC 315 -- Section 2 -- Fall 2018
=================================

## Course Information
**Authors:**
	Haley Linnig, Walter Vaughan    

**Date:**
	2018-10-21

**Professor:**
	Dr. Larry Pyeatt

## Program Description
- Data compression programs consist of an encoder and a decoder. The encoder takes
an input file and encodes it to an output file (presumably with lesser storage
requirements). The decoder takes the encoded file as input and outputs a decoded
version; with lossless coding schemes, the decoded file is identical to the
original.

- The Huffman encoder makes two passes through an input data file. In the first
pass, the frequency of occurrence of each character in the file is determined,
and a Huffman coding tree is constructed. The Huffman coding tree (a type of
binary search tree) is used to generate a table of Huffman codes. In the second
pass through the input file, the encoder uses these Huffman codes to compress
the input data. For each character in the input file, the corresponding
variable-length Huffman code is written to the output file. This results in an
optimal encoding of the data (optimal in the sense of entropy, a measure of the
information content).

- For decoding, the identical Huffman codes must be used. Bits are read from the
encoded input file, and decoded into characters using the same Huffman coding
tree. These decoded characters are written to the decoded output file.


## Algorithm for Histogram

Our chosen binary format for the histogram is as follows:

```
     ----------------------------------------------
     | Flag | b0 | freq0 | .... | bN | freqN | \0 |
     ----------------------------------------------
Byte: 0      1    2...      ...   
```

Where `Flag` is a single byte indicating whether or not the zero-byte appears in
the histogram, `b#` is a single byte containing a character, and `freq#` is the
UTF-8 encoded frequency associated with `b#` (variable from 1 to 6 bytes). The
pattern repeats until a 0-byte is found in the expected `b#` location,
indicating the end of the histogram. If the `Flag` is set (i.e. non-zero) then
the first 0-byte encountered will actually be treated as a histogram entry for
the 0-byte, and will continue reading until the actual terminating 0-byte is
encountered.

The UTF-8 encoding stores frequencies of up to `2^31 - 1` appearances of a
character. The encoding scheme is shown here, the way it was presented via Rob
Pike in 2003:
```
   Bits  Hex Min  Hex Max  Byte Sequence in Binary
1    7  00000000 0000007f 0xxxxxxx
2   11  00000080 000007FF 110xxxxx 10xxxxxx
3   16  00000800 0000FFFF 1110xxxx 10xxxxxx 10xxxxxx
4   21  00010000 001FFFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
5   26  00200000 03FFFFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
6   31  04000000 7FFFFFFF 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
```

The leftmost column indicates the number of bytes needed to store the value
between the bounds of Hex Min and Hex Max.

The byte-frequency pairs are stored in ascending order of frequency, to aid
building the code tree.

## Algorithm for Huffman Code Computation

After the histogram is built, characters are added to a statically-allocated
minheap and ordered on their frequency. The minheap is custom-built for this
application, designed around the `node` type which is also used to form the
Huffman code tree.

Once the heap is populated with all nonzero entries, the tree is formed by
repeatedly removing the smallest two nodes, forming an artificial new node with
the combined weights of two removed nodes, setting new node's left child to the
smallest removed node, and the right child to the other removed node. This new
node, which forms a binary search tree, is pushed back into the heap. The
process repeats until two nodes remain in the heap, at which point the final new
node is formed and becomes the root of the Huffman code tree.

All byte combinations now appear in the tree as leaves, and the tree is sorted
on byte frequency. An array (indexed by a byte, which can be thought of as a
lookup-table) which is referred to as a "map" (from a byte to a Huffman code) is
computed by traversing the entirety of the tree. A Huffman code starts empty at
the root, then a left traversal appends a 0 and a right traversal appends a 1 to
that code. Once a leaf is reached, the Huffman code at that node is saved in the
map for quick lookup later.

## Encoded File Format

After the histogram (and its terminating null byte) in the encoded file, the
Huffman code corresponding to each byte in the input file is appended, starting
from bit 0. This differs from the reference implementation wherein codes are
stored starting from bit 7, going down to bit 0, until the next byte:

This implementation, example codes (in order) 0, 11111, 0, 11111:
```
---------------------------------
     | 1011 1110 | .... 1111
-----------------------------------
Byte:  0           1
Bit:   7654 3210   7654 3210
```

Reference implementation, example codes (in order) 0, 111, 0, 111:
```
---------------------------------
     | 0111 1101 | 1111 ....
-----------------------------------
Byte:  0           1
Bit:   7654 3210   7654 3210
```


Once all the input bytes have been translated out, the byte containing that last
Huffman code is filled with zeros until (and if not already) full, and that
number of zeros is stored in a following, final byte. This means that the
decoder doesn't ambiguously interpret 0-valued Huffman codes from any trailing
zero bits.


## Decoding

For the decoding step, the tree made in the encoding step is recreated from the
histogram at the beginning of the encoding file, and then the translation from
encoded file to decoded file is done by traversing the tree in accordance with
each bit of input. Once a leaf is found, that leaf's byte value is recorded in
the output file, and reading of the encoded file continues at the next bit, and
traversal restarts at the root of the Huffman code tree.

Once the final byte is read in, it is interpreted as the number of trailing
bits, and the buffer is truncated by that number.


## Building
`make`

## Running/Usage
`huffman –e originalfile encodedfile`    (encoder)

`huffman –d encodedfile decodedfile`     (decoder)



## Known Bugs
 - Input files which form a Huffman code longer than 2 bytes are not supported

Revision History
================
See [the git
history](https://gitlab.mcs.sdsmt.edu/7385302/csc315_fall2018_project1/commits/master).
