#include <iostream>
#include <fstream>
#include <string> // argument parsing 
#include <vector> // histogram sorting uses vector
#include <algorithm> // sort
#include <numeric> // iota

using namespace std;

bool checkOpen (ifstream &fin, ofstream &fout);
bool compareHistEntry(unsigned* a, unsigned* b);
bool readHistogram(ifstream& f, unsigned hist[256]);
bool writeHistogram(ofstream& f, unsigned hist[256]);

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
	unsigned histogram[256] = {0};


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


bool compareHistEntry(unsigned* a, unsigned* b)
{
	return *a < *b;
}


// Reads the histogram array from an open file, storing it
// inside of the `hist` argument.
// returns true on success, false otherwise
bool readHistogram(ifstream& f, unsigned hist[256])
{
	bool flag; /* indicates whether 0-byte is in histogram */
	uint8_t character; 
	uint32_t charcount;
	f.get((char&)character); /* read flag byte */

	if (!f) return false; /* don't bother trying if the file is invalid */

	f.seekg(0); /* always read histogram from the beginning */
	
	flag = static_cast<bool>(character);
	f.get((char&)character);
	while (f and (flag or character))
	{
		/* for some unholy reason, the second argument is actually
		 * the number of bytes you want to read, minus 1.... */
		f.get((char*)&charcount, 5); 
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
bool writeHistogram(ofstream& f, unsigned hist[256])
{
		/* pointers to the histogram array */
		vector<unsigned*> freqs(256);
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

		/* write a byte indicating the character, and 4-byte unsigned int
         * indicating the number of times the character appears */
		for (; freqIter != freqs.end() and f; freqIter++)
		{
			uint8_t character = static_cast<uint8_t>(*freqIter - hist);
			uint32_t charcount = static_cast<uint32_t>(**freqIter);
			f.write((char*)&character, 1);
			f.write((char*)&charcount, 4);
		}

		/* terminate the histogram section of the file */
		f.put(0);

		/* if the output is successful, f will still evaluate to true */
		return (bool)f;
}
