#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool checkOpen (ifstream &fin, ofstream &fout);

static void usage()
{
	cerr << "Usage:\n\thuffman -e originalfile encodedfile"
	        "\n\thuffman -d encodedfile decodedfile\n";
}

int main(int argc, char** argv)
{
	ifstream fin;
	ofstream fout;
	bool fileOpen = 1;

	/* argument checking / usage */
	if (argc != 4)
	{
		cerr << "E: " << argv[0] << " takes 3 args. " << (argc - 1)
		     << " found.\n";
		usage();
		return 1;
	}
	
	/* handle decode */
	if (string("-d") == string(argv[1]))
	{
		//Open encoded file and decoded file in binary and check open
		fin.open(argv[2], ios::in | ios::binary);
		fout.open(argv[3], ios::out | ios::binary);
	
		fileOpen = checkOpen (fin, fout);
		if (!fileOpen)
			return 1;
	}
	
	/* handle encode */
	else if (string("-e") == string(argv[1]))
	{
		
		//Open og file and encoded file in binary and check open
		fin.open(argv[2], ios::in | ios::binary);
		fout.open(argv[3], ios::out | ios::binary);
	
		fileOpen = checkOpen (fin, fout);
		if(!fileOpen)
			return 1;
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
bool checkOpen (ifstream &fin, ofstream &fout)
{
	//If files opened correctly, return true
	if (fin.is_open() && fout.is_open())
		return 1;

	cout << "Could not open file. Exiting program" << endl;

	//Else, close the files and return false
	fin.close();
	fout.close();

	return 0;
}
