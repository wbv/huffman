#include <iostream>
#include <string>

using namespace std;

struct stats {

	int numBytes = 0, numCodeWords = 0, numEBytes = 0, numOverhead = 0;
	string inputName, outputName;
	double compressRatio = 0.0, entropy = 0.0, avgBit = 0.0, codingEff = 0.0;
	
	//ptr to histogram?
} eStats, dStats;

int calcCompress(stats &statInfo)
{
        //compressed size / original size
        //Do we want to divide without using divide?
        statInfo.compressRatio = double(statInfo.numEBytes) / statInfo.numBytes;
        statInfo.compressRatio = statInfo.compressRatio * 100;

        return 0;
}

int calcEntropy()
{
        //-SUM from i = 0 to M - 1 (pi log2 pi)     pi = probability of ith symbol
        return 0;
}

int calcCodeEff()
{
        return 0;
}


