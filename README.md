CSC 315 -- Section 2 -- Fall 2018
=================================

Course Information
==================
**Authors:**
	Haley Linnig, Walter Vaughan    

**Date:**
	2018-10-15 (tentative)

**Professor:**
	Dr. Larry Pyeatt

Program Description
===================
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


Algorithm for Histogram
=======================

Our chosen binary format for the histogram is as follows:

```
     ----------------------------------------------
     | Flag | b0 | freq0 | .... | bN | freqN | \0 |
     ----------------------------------------------
Byte: 0      1    2:5      ...   
```

Where `Flag` is a single byte indicating whether or not the zero-byte appears in
the histogram, `b#` is a single byte containing a character, and `freq#` is the
frequency associated with `b#` (4 bytes, or an `int` / `uint32_t`). The pattern
repeats until a 0-byte is found, indicating the end of the histogram. If the
`Flag` is set (i.e. non-zero) then the first 0-byte encountered will actually
be treated as a histogram entry for the 0-byte, and will continue reading until
the actual terminating 0-byte is encountered.

The byte-frequency pairs will be stored in order of frequency (TODO: ascending
or descending!?).

Building
========
`make`

Running
=======
`huffman –e originalfile encodedfile`    (encoder)

`huffman –d encodedfile decodedfile`     (decoder)


Usage
=====
 - TODO

Known Bugs
==========
 - Doesn't exist yet
 - Consequentially, doesn't work

Revision History
================
See [the git
history](https://gitlab.mcs.sdsmt.edu/7385302/csc315_fall2018_project1/commits/master).
