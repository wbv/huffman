#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <string> // argument parsing 
#include <vector> // histogram sorting uses vector
#include <algorithm> // sort
#include <numeric> // iota
#include <cstdint> // uint32_t, uint8_t

#include "huffcode.h"
#include "minheap.h"
#include "node.h"
#include "utf8.h"
#include "stats.h"

using namespace std;

void encoderStats(uint32_t hist[256], huffcode_t huffmap[256]);
void decoderStats();
bool checkOpen (ifstream &fin, ofstream &fout);
bool compareHistEntry(uint32_t* a, uint32_t* b);
bool readHistogram(ifstream& f, uint32_t hist[256]);
bool writeHistogram(ofstream& f, uint32_t hist[256]);
int decode(char* encodedfile, char* outfile);
int encode(char* infile, char* encodedfile);
string huffcodeToString(huffcode_t c);


static void usage()
{
	cerr << "Usage:\n\thuffman -e originalfile encodedfile"
	        "\n\thuffman -d encodedfile decodedfile\n";
}

int main(int argc, char** argv)
{
	/* argument count checking */
	if (argc != 4)
	{
		cerr << "E: " << argv[0] << " takes 3 args. " << (argc - 1)
		     << " found.\n";
	}

	/* handle decode */
	else if (string("-d") == string(argv[1]))
		return decode(argv[2], argv[3]);
	
	/* handle encode */
	else if (string("-e") == string(argv[1]))
		return encode(argv[2], argv[3]);
	
	/* invalid flag or incorrect arg count */
	usage();
	return (int)-1;
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
	uint8_t charcount;

	f.seekg(0); /* always read histogram from the beginning */
	if (!f) return false; /* don't bother trying if the file is invalid */

	f.get((char&)character); /* read flag byte */
	flag = static_cast<bool>(character);

	/* read first character histogram entry */
	f.get((char&)character);
	while (f and (flag or character))
	{
		/* read first byte of utf-8 encoded frequency count */
		f.get((char&)charcount);

		/* 1-byte code point - easy to handle */
		if (charcount < 0x80)
			hist[character] = charcount;

		/* 2 to 6-byte code point */
		else
		{
			uint8_t firstbytemask;
			utf8_t codept;
			codept.nbytes = 6;

			/* first byte indicates number of total bytes, let's see which */
			firstbytemask = 0xfc;
			while (charcount < firstbytemask)
			{
				firstbytemask <<= 1; /* shift in a 0 */
				codept.nbytes--; /* check for smaller number of bytes */
			}

			/* read in the whole encoding from the file to utf8_t object */
			codept.encoded[0] = charcount;
			for (int i = 1; i < codept.nbytes; i++)
				f.get((char&)*(codept.encoded + i));

			hist[character] = getUInt(codept);
		}

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

		/* write a byte indicating the character, and using32_t
         * indicating the number of times the character appears */
		for (; freqIter != freqs.end() and f; freqIter++)
		{
			eStats.numCodeWords++;
			uint8_t character = static_cast<uint8_t>(*freqIter - hist);
			uint32_t charcount = static_cast<uint32_t>(**freqIter);
			utf8_t codept = getUTF8(charcount);

			if (codept.nbytes == 0) /* invalid codept */
			{
				cerr << "Error: cannot count more than 2^31 " 
				     << "instances of character '" << (char)character << "'\n";
				return false;
			}
			
			f.write((char*)&character, 1);
			f.write((char*)codept.encoded, codept.nbytes);
		}

		/* terminate the histogram section of the file */
		f.put(0);

		/* if the output is successful, f will still evaluate to true */
		return (bool)f;
}


void encoderStats(uint32_t hist[256], huffcode_t huffmap[256])
{
	//Need to put real value in, maybe pass file?
	//How are we going to access values
	uint32_t charFreq;
	double probability = 0.0;//, compressRatio = 0.0, entropy = 0.0, avgBit = 0.0;
	//double codingEff = 0.0;
	huffcode_t huffCode;

	cout << endl << "Huffman Encoder Pass 1" << endl << setfill ('-') << setw(22);
        cout << "-" << endl << "Read " << eStats.numBytes << " from " << eStats.inputName;
        cout <<", found " << eStats.numCodeWords << " code words" << endl << endl;
        cout << "Huffman Code Table" << endl << setfill ('-') << setw(18) << "-";
	cout << endl << "ASCII Code " << setfill (' ') << setw(25) << "Probablility (%) ";
	cout << setw(20) << " Huffman Code" << endl;

	//For all code words, print out ASCII code, probability and Huffman Code
	for(int ch = 0; ch < 256; ch++)
	{
		charFreq = hist[ch];
		if (charFreq == 0)
			continue;

		huffCode = huffmap[ch];	

		probability = 100.0 * (double)charFreq / (double)eStats.numBytes;
		cout << ch << " ( " << (char)ch << " )"
		     <<  setw(25) << setprecision(2) << fixed << probability;
	    cout << setw(25) << huffcodeToString(huffCode) << endl;

		eStats.entropy += calcEntropy(probability);
		eStats.avgBit += calcAvg(probability, huffCode.bitcnt);
	}

	//Will calculate the compressed size
	calcCompress(eStats);

	cout << endl << "Huffman Encoder Pass 2" << endl << setfill ('-') << setw(22) << "-";
	cout << endl <<  "Wrote " << eStats.numEBytes << " encoded bytes to " << eStats.outputName << " ( ";
	cout << eStats.numOverhead << " bytes including histogram)" << endl << endl;

	cout << endl << "Huffman Coding Statistics" << endl << setfill ('-') << setw(25);
	cout << "-" << endl << "Compression ratio = " << fixed << setprecision(2);
	cout << eStats.compressRatio << "% " << endl << "Entropy = " << eStats.entropy << endl; 
	cout << "Average bits per symbol in Huffman coding = " << eStats.avgBit << endl;
       	cout << "Coding efficiency = " << eStats.codingEff << "%" << endl;

	return;
}


void decoderStats()
{
	//int numBytes = 0;
	//double compressRatio = 0.0;

	//Will calculate the compressed size
	calcCompress(dStats);
	
	cout << endl << "Huffman Coding Statistics" << endl << setfill ('-') << setw(22);
	cout << "-" << endl << "Read " << dStats.numEBytes << " encoded bytes from " << dStats.inputName;
	cout << "( " << dStats.numOverhead << " bytes including the histogram" << endl << "Wrote";
	cout << dStats.numBytes << " decoded bytes to " << dStats.outputName << endl << "Compression";
	cout << " ratio: " << fixed << setprecision(2) << dStats.compressRatio << endl;
	
	return;
}

int encode(char* infile, char* encodedfile)
{
	ifstream fin;
	ofstream fout;
	bool filesOpen = false;
	uint32_t histogram[256] = {0};
	node* tree;
	huffcode_t map[256] = {0};
	int error = 0;

	//Open og file and encoded file in binary and check open
	fin.open(infile, ios::in | ios::binary);
	fout.open(encodedfile, ios::out | ios::binary | ios::trunc);

	filesOpen = checkOpen(fin, fout);
	if (!filesOpen)
		return 1;

	//Save input and output file names into eStats struct
	eStats.inputName = infile;
	eStats.outputName = encodedfile;

	//Find number of bytes in file
	fin.seekg(0, fin.end);
	eStats.numBytes = fin.tellg();
	fin.seekg(0, fin.beg);

	/** PASS 1 - BUILD HISTOGRAM AND CODE MAP **/
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
	{
		cerr << "Warning: input file read finished prematurely.\n";
		error += 2;
	}

	bool writeHistSuccess = writeHistogram(fout, histogram);

	auto histogramPosition = fout.tellp();

	if (!writeHistSuccess)
	{
		error += 4;
		cerr << "Warning: output histogram failed.\n";
	}

	tree = getTreeFromHist(histogram);

	getHuffMapFromTree(map, tree);

	/** PASS 2: ELECTRIC BOOGALOO **/
	/* with the map made, read fin again and write the rest of the outfile */
	writeHuffman(map, fin, fout);

	//Find number of bytes including histogram written to file
	fout.seekp(0, fout.end);
	eStats.numEBytes = fout.tellp() - histogramPosition;
	eStats.numOverhead = fout.tellp();
	fout.seekp(0, fin.beg);

	encoderStats(histogram, map);

	return error;
}


int decode(char* encodedfile, char* outfile)
{
	ifstream fin;
	ofstream fout;
	bool filesOpen = false;
	uint32_t histogram[256] = {0};
	node* tree;

	//Open encoded file and decoded file in binary and check open
	fin.open(encodedfile, ios::in | ios::binary);
	fout.open(outfile, ios::out | ios::binary | ios::trunc);

	filesOpen = checkOpen(fin, fout);
	if (!filesOpen)
		return false;
	
	//Save input and output file names into dStats struct
	dStats.inputName = encodedfile;
	dStats.outputName = outfile;

	//Find number of bytes in file
	fin.seekg(0, fin.end);
	dStats.numEBytes = fin.tellg();
	fin.seekg(0, fin.beg);

	if (!readHistogram(fin, histogram))
	{
		fin.close();
		fout.close();
		return false;
	}

	tree = getTreeFromHist(histogram);
	/* readHistogram will leave fin pointing at the end of the histogram
	 * so consider working from that point, or make sure you "find" the
	 * end of the histogram section again */
	readHuffman(tree, fin, fout);

	//Find number of bytes including the histogram  in file
	fout.seekp(0, fout.end);
	dStats.numOverhead = fout.tellp();
	fout.seekp(0, fout.beg);

	decoderStats();
	return true;
}


string huffcodeToString(huffcode_t c)
{
	string s;
	for (int i = c.bitcnt - 1; i >= 0; i--)
		s += ((c.bits >> i) & 0x1) ? '1' : '0';
	return s;
}
