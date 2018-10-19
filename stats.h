#include <iostream>
#include <string>
#include <math.h>

using namespace std;

struct stats {
	int numBytes = 0, numCodeWords = 0, numEBytes = 0, numOverhead = 0;
	string inputName, outputName;
	double compressRatio = 0.0, entropy = 0.0, avgBit = 0.0, codingEff = 0.0;
	
} eStats, dStats;

/***************************************************************************//**
 * @author Haley Linnig
 *
 * @par Description: 
 * This function will calculate the compression ratio. The ratio is calculated
 * by dividing the compressed size of the file by divided by the orginal size.
 *
 * @param[in] stats - Structure that holds encoder statistics
 *
 * @returns 0
 *
 ******************************************************************************/

int calcCompress(stats &statInfo)
{
	//Compress ratio will be compressed size / original size
        statInfo.compressRatio = double(statInfo.numEBytes) / statInfo.numBytes;
        statInfo.compressRatio = statInfo.compressRatio * 100;

        return 0;
}

/***************************************************************************//**
 * @author Haley Linnig
 *
 * @par Description: 
 * This function will calculate the entropy value. Entropy is calculated by
 * multiplying the probability of the symbol by log based 2 of the probability.
 * Entropy is the -SUM of this calculated value, this will be done by the for
 * loop.
 *
 * @param[in] prob - Probability for how often symbol shows up in file
 *
 * @returns tempEnt - Return entropy value 
 *
 ******************************************************************************/

double calcEntropy(double prob)
{
        //(-SUM) pi log2 pi     pi = probability of ith symbol
	double tempEnt = 0.0;
	prob = (prob) / 100;
	
	tempEnt = log2 (prob);
	tempEnt = prob * (-tempEnt);
        return tempEnt;
}

/***************************************************************************//**
 * @author Haley Linnig
 *
 * @par Description: 
 * This function will calculate the average bits per symbol
 * by multiplying the probability by the number of bits. The probability has 
 * to be divided by 100 since it is multiplied by 100 in the encoderStats.
 *
 * @param[in] prob - Probability for how often symbol shows up in file
 * @param[in] numBits - Number of bits in the symbol
 *
 * @returns calcAvg - Return value of average bits per symbol
 *
 ******************************************************************************/

double calcAvg(double prob, int numBits)
{
	//Avg = probability * number of bits 
	double calcAvg;
	prob = prob / 100;
	calcAvg = prob * numBits;
	return calcAvg;
}
