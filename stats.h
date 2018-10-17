#include main.cpp

struct endodedStats {

	int numBytes = 0, numCodeWords = 0, numEBytes = 0, numOverhead = 0;
	vector <char> inputName, outputFileName;
	double compressRatio = 0.0, entropy = 0.0, avgBit = 0.0, codingEff = 0.0;
	
	//ptr to histogram?
}

struct decodedStats {

	int numEBytes = 0, numOverhead = 0, numDBytes = 0;
	vector <char> inputName, outputFileName;
	double compressRatio = 0.0;
}

int calcCompress();

int calcEntropy();

int calcCodeEff();
