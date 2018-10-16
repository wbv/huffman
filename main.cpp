#include <iostream>
#include <iomanip>
#include <fstream>
#include <string> // argument parsing 
#include <vector> // histogram sorting uses vector
#include <algorithm> // sort
#include <numeric> // iota
#include <cstdint> // uint32_t, uint8_t

#include "hufftree.h"
#include "minheap.h"

using namespace std;

void encoderStats(char** argv);
void decoderStats(char** argv);
bool checkOpen (ifstream &fin, ofstream &fout);
bool compareHistEntry(uint32_t* a, uint32_t* b);
bool readHistogram(ifstream& f, uint32_t hist[256]);
bool writeHistogram(ofstream& f, uint32_t hist[256]);
hufftree getTreeFromHist(uint32_t hist[256]);

static void usage()
{
	cerr << "Usage:\n\thuffman -e originalfile encodedfile"
	        "\n\thuffman -d encodedfile decodedfile\n";
}

int main(int argc, char** argv)
{
	ifstream fin;
	ofstream fout;
	bool filesOpen = false;
	uint32_t histogram[256] = {0};


	/* argument checking / usage */
	if (argc != 4)
	{
		cerr << "E: " << argv[0] << " takes 3 args. " << (argc - 1)
		     << " found.\n";
		usage();
		return 3;
	}
	
	/* handle decode */
	if (string("-d") == string(argv[1]))
	{
		//Open encoded file and decoded file in binary and check open
		fin.open(argv[2], ios::in | ios::binary);
		fout.open(argv[3], ios::out | ios::binary | ios::trunc);

		filesOpen = checkOpen(fin, fout);
		if (!filesOpen)
			return 1;

		if (!readHistogram(fin, histogram))
			return 1;

		/* readHistogram will leave fin pointing at the end of the histogram 
		 * so consider working from that point, or make sure you "find" the
		 * end of the histogram section again */
		
		decoderStats(argv);
	}
	
	/* handle encode */
	else if (string("-e") == string(argv[1]))
	{
		
		//Open og file and encoded file in binary and check open
		fin.open(argv[2], ios::in | ios::binary);
		fout.open(argv[3], ios::out | ios::binary | ios::trunc);
	
		filesOpen = checkOpen(fin, fout);
		if (!filesOpen)
			return 1;

		/* read a character at a time, populating histogram */
		uint8_t character; 
		fin.get((char&)character);
		while (fin.gcount() > 0)
		{
			histogram[character]++;
			fin.get((char&)character);
		}

		/* if ending for non-eof reasons, badness occurred :( */
		if (!fin.eof())
			cerr << "Warning: input file read finished prematurely.\n";

		bool writeHistSuccess = writeHistogram(fout, histogram);

		if (!writeHistSuccess)
			cerr << "Warning: output histogram failed.\n";

		hufftree tree = getTreeFromHist(histogram); 

		encoderStats(argv);
	}
	
	else
	{
		usage();
		return 2;
	}

	//Close files
	fin.close();
	fout.close();
	
	return 0;
}


//Check that the input and output files opened correctly
// true = success, false = failure
bool checkOpen (ifstream &fin, ofstream &fout)
{
	//If files opened correctly, return true
	if (fin.is_open() && fout.is_open())
		return true;

	cout << "Could not open file. Exiting program" << endl;

	//Else, close the files and return false
	fin.close();
	fout.close();

	return false;
}


bool compareHistEntry(uint32_t* a, uint32_t* b)
{
	return *a < *b;
}


// Reads the histogram array from an open file, storing it
// inside of the `hist` argument.
// returns true on success, false otherwise
bool readHistogram(ifstream& f, uint32_t hist[256])
{
	bool flag; /* indicates whether 0-byte is in histogram */
	uint8_t character; 
	uint32_t charcount;

	f.seekg(0); /* always read histogram from the beginning */
	if (!f) return false; /* don't bother trying if the file is invalid */

	f.get((char&)character); /* read flag byte */
	flag = static_cast<bool>(character);
	f.get((char&)character); /* read first character histogram entry */
	while (f and (flag or character))
	{
		/* read first byte to determine length 
 		 * of utf-8 encoded frequency count */
		f.get((char&)charcount);

		if (!(0xfc ^ (charcount & 0xfc))) /* 6-byte count */
		{
			charcount &= 1;
			uint8_t tmpbyte;
			for (int i = 4; i >= 0; i--)
			{
				f.get((char&)tmpbyte);
				charcount <<= 6;
				charcount |= tmpbyte & 0x3f;
			}
		}
		else if (!(0xf8 ^ (charcount & 0xf8))) /* 5-byte count */
		{
			charcount &= 3;
			uint8_t tmpbyte;
			for (int i = 3; i >= 0; i--)
			{
				f.get((char&)tmpbyte);
				charcount <<= 6;
				charcount |= tmpbyte & 0x3f;
			}
		}
		else if (!(0xf0 ^ (charcount & 0xf0))) /* 4-byte count */
		{
			charcount &= 7;
			uint8_t tmpbyte;
			for (int i = 2; i >= 0; i--)
			{
				f.get((char&)tmpbyte);
				charcount <<= 6;
				charcount |= tmpbyte & 0x3f;
			}
		}
		else if (!(0xe0 ^ (charcount & 0xe0))) /* 3-byte count */
		{
			charcount &= 0xf;
			uint8_t tmpbyte;
			for (int i = 1; i >= 0; i--)
			{
				f.get((char&)tmpbyte);
				charcount <<= 6;
				charcount |= tmpbyte & 0x3f;
			}
		}
		else if (!(0xc0 ^ (charcount & 0xc0))) /* 2-byte count */
		{
			charcount &= 0x1f;
			uint8_t tmpbyte;
			f.get((char&)tmpbyte);
			charcount <<= 6;
			charcount |= tmpbyte & 0x3f;
		}
		else /* 1-byte count */
		{
			charcount &= 0x7f;
		}

		hist[character] = charcount;
		/* if we save a value for the null-byte, reset the flag so
		 * the next null byte will successfully indicate end of histogram */
		if (character == 0)
			flag = false;

		/* try to get the next character */
		f.get((char&)character);
	}

	/* return false if reading the histogram halted file i/o early */
	return (bool)f;
}


// Writes the histogram array to an open file
// returns true on success, false otherwise
bool writeHistogram(ofstream& f, uint32_t hist[256])
{
		/* pointers to the histogram array */
		vector<uint32_t*> freqs(256);
		/* fill freqs with pointers to each item */
		iota(freqs.begin(), freqs.end(), hist);
		/* sort freqs in ascending order */
		sort(freqs.begin(), freqs.end(), compareHistEntry);
		auto freqIter = freqs.begin();
		while (**freqIter == 0 and ++freqIter != freqs.end())
			continue;

		f.seekp(0); /* always start at the beginning of a file */

		/* if null-byte appears in histogram, set flag byte */
		f.put(hist[0] ? 1 : 0);

		/* write a byte indicating the character, and uing32_t
         * indicating the number of times the character appears */
		for (; freqIter != freqs.end() and f; freqIter++)
		{
			uint8_t character = static_cast<uint8_t>(*freqIter - hist);
			uint32_t charcount = static_cast<uint32_t>(**freqIter);
			if (charcount > 0x7fffffff)
				cerr << "Warning: cannot count more than 2^31 " 
				     << "instances of character '" << (char)character << "'\n"
				     << "         (found " << charcount << ")\n";

			f.write((char*)&character, 1);

			if (charcount > 0x3ffffff) /* 6-byte storage */
			{
				character = 0xfc & ((charcount >> 30) & 1);
				f.write((char*)&character, 1);
				for (int i = 4; i >= 0; i--)
				{
					character = 0x80 & ((charcount >> (i*6)) & 0x3f);
					f.write((char*)&character, 1);
				}
			}
			else if (charcount > 0x1fffff) /* 5-byte storage */
			{
				character = 0xf8 & ((charcount >> 24) & 3);
				f.write((char*)&character, 1);
				for (int i = 3; i >= 0; i--)
				{
					character = 0x80 & ((charcount >> (i*6)) & 0x3f);
					f.write((char*)&character, 1);
				}
			}
			else if (charcount > 0xffff) /* 4-byte storage */
			{
				character = 0xf0 & ((charcount >> 18) & 7);
				f.write((char*)&character, 1);
				for (int i = 2; i >= 0; i--)
				{
					character = 0x80 & ((charcount >> (i*6)) & 0x3f);
					f.write((char*)&character, 1);
				}
			}
			else if (charcount > 0x7ff) /* 3-byte storage */
			{
				character = 0xe0 & ((charcount >> 12) & 0xf);
				f.write((char*)&character, 1);
				for (int i = 1; i >= 0; i--)
				{
					character = 0x80 & ((charcount >> (i*6)) & 0x3f);
					f.write((char*)&character, 1);
				}
			}
			else if (charcount > 0x7f) /* 2-byte storage */
			{
				character = 0xc0 & ((charcount >> 6) & 0x1f);
				f.write((char*)&character, 1);
				character = 0x80 & (charcount & 0x3f);
				f.write((char*)&character, 1);
			}
			else /* 1-byte storage */
			{
				character = charcount & 0x7f;
				f.write((char*)&character, 1);
			}
		}

		/* terminate the histogram section of the file */
		f.put(0);

		/* if the output is successful, f will still evaluate to true */
		return (bool)f;
}

/* takes a histogram, makes a heap, then turns the heap into a hufftree */
hufftree getTreeFromHist(uint32_t hist[256])
{
	minheap heap = minheap();
	node* left;
	node* right;
	node* top;

	for (size_t i = 0; i < 256; i++)
		if (hist[i])
			heap.insert(hist[i], static_cast<uint8_t>(i));

	while (heap.size() > 1)
	{
		left = heap.pop_smallest();
		right = heap.pop_smallest();
		top = new node;
		top->weight = left->weight + right->weight;
		top->left = left;
		top->right = right;
		heap.insert(top);
	}

	hufftree tree = hufftree(heap.pop_smallest());
#ifdef _DEBUG
	tree.printTree();
#endif
	return tree;
}


void encoderStats(char** argv)
{
	//Need to put real value in, maybe pass file?
	int numBytes = 0, numCode = 3;
	//How are we going to access values
	char myChar = 'a';
	double probability = 0.0, compressRatio = 0.0, entropy = 0.0, avgBit = 0.0;
	double codingEff = 0.0;
	int huffCode = 1;

	cout << endl << "Huffman Encoder Pass 1" << endl << setfill ('-') << setw(23);
        cout << "-" << endl << "Read " << numBytes << " from " << argv[2];
        cout <<", found " << numCode << " code words" << endl << endl;
        cout << "Huffman Code Table" << endl << setfill ('-') << setw(18) << "-";
	cout << endl << "ASCII Code " << setfill (' ') << setw(25) << "Probablility (%) ";
	cout << setw(20) << " Huffman Code" << endl;

	//For all code words, print out ASCII code, probability and Huffman Code
	for(int i = 0; i < numCode; i++)
	{
		cout << int(myChar) << " " << myChar << setw(25) << setprecision(2) << fixed;
	        cout << probability << setw(25) <<  huffCode << endl;
	}

	cout << endl << "Huffman Encoder Pass 2" << endl << setfill ('-') << setw(23) << "-";
	cout << endl <<  "Wrote " << numBytes << " encoded bytes to " << argv[3] << " ( ";
	cout << numBytes << " bytes including histogram)" << endl << endl;

	cout << endl << "Huffman Coding Statistics" << endl << setfill ('-') << setw(22);
	cout << "-" << endl << "Compression ratio = " << fixed << setprecision(2);
	cout << compressRatio << "% " << endl << "Entropy = " << entropy << endl; 
	cout << "Average bits per symbol in Huffman coding = " << avgBit << endl;
       	cout << "Coding efficiency = " << codingEff << "%" << endl;

	return;
}


void decoderStats(char** argv)
{
	int numBytes = 0;
	double compressRatio = 0.0;

	cout << endl << "Huffman Coding Statistics" << endl << setfill ('-') << setw(22);
	cout << "-" << endl << "Read " << numBytes << " encoded bytes from " << argv[2];
	cout << "( " << numBytes << " bytes including the histogram" << endl << "Wrote";
	cout << numBytes << " decoded bytes to " << argv[3] << endl << "Compression";
	cout << " ratio: " << fixed << setprecision(2) << compressRatio << endl;
	
	return;
}
