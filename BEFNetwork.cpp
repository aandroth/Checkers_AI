#include "BEFNetwork.h"
#include <math.h>
using std::exp;
using std::cout;
using std::cin;
using std::endl;

float L1[LAYER_1_SIZE];
float L2[LAYER_2_SIZE];
float L3[LAYER_3_SIZE]; 
float L4[LAYER_4_SIZE];

NeuralNetwork::NeuralNetwork()
{
	_layersAndNodes[0] = L1;
	_layersAndNodes[1] = L2;
	_layersAndNodes[2] = L3;
	_layersAndNodes[3] = L4;

	//These are for keeping track of the amount of nodes in each layer, since we can't call size() on a C-array
	_nodesInLayer[0] = LAYER_1_SIZE;
	_nodesInLayer[1] = LAYER_2_SIZE;
	_nodesInLayer[2] = LAYER_3_SIZE;
	_nodesInLayer[3] = LAYER_4_SIZE;
}

void NeuralNetwork::fillWeightsFromFileNumber(int fileNumber)
{
	string fileName = "NNs/NN_" + std::to_string(fileNumber) + ".txt";
	cout << fileName << endl;

	std::ifstream inFile(fileName);
	string tempString, fileString;
	int state = 0, countChar = 0, countLine = 0;
	vector<float> tempVec;
	while (inFile >> tempString)
	{
		switch (state)
		{
		case 0:
			if (countChar == 1)
			{
				m_class = std::stoi(tempString);
				countChar = 0;
				state = 1;
			}
			else
			{
				m_points = std::stoi(tempString);
			}
			break;
		case 1:
			tempVec.push_back(std::stof(tempString));
			if (countChar == LAYER_1_SIZE) // 32 weights
			{
				weightArrs0.push_back(tempVec);
				tempVec.clear();
				countChar = 0;
				++countLine;
			}
			if (countLine == LAYER_2_SIZE) // 40 nodes
			{
				countLine = 0;
				state = 2;
			}
			break;
		case 2:
			tempVec.push_back(std::stof(tempString));

			if (countChar == LAYER_2_SIZE) // 40 weights
			{
				weightArrs1.push_back(tempVec);
				tempVec.clear();
				countChar = 0;
				++countLine;
			}
			if (countLine == LAYER_3_SIZE) // 10 nodes
			{
				countLine = 0;
				state = 3;
			}
			break;
		case 3:
			tempVec.push_back(std::stof(tempString));

			if (countChar == LAYER_3_SIZE) // 10 weights for 1 node
			{
				weightArrs2 = tempVec;
				countChar = 0;
			}
			break;
		}
		++countChar;
	}

	/*
	// For 40 nodes, create 32 weights each
	for (int ii = 0; ii < 40; ++ii)
	{
		for (int jj = 0; jj < 32; ++jj)
		{
			//cout << std::to_string(weightArrs0[ii][jj]) << " ";
		}
	}
	// For 10 nodes, create 40 weights each
	for (int ii = 0; ii < 10; ++ii)
	{
		for (int jj = 0; jj < 40; ++jj)
		{
			//cout << std::to_string(weightArrs1[ii][jj]) << " ";
		}
	}
	// For 1 node, create 10 weights
	for (int ii = 0; ii < 10; ++ii)
	{
		cout << std::to_string(weightArrs2[ii]) << " " << endl;;
	}*/
}

void NeuralNetwork::writeWeightsToFileNumber(int fileNumber)
{
	string fileName = "NNs/NN_" + to_string(fileNumber) + ".txt";
	ofstream outFile;
	outFile.open(fileName);
	outFile << to_string(m_points) << " ";
	outFile << to_string(m_class) << " ";

	// For 40 nodes, create 32 weights each
	for (int ii = 0; ii < 40; ++ii)
	{
		vector<float> tempVec;
		for (int jj = 0; jj < 32; ++jj)
		{
			outFile << to_string(weightArrs0[ii][jj]) << " ";
		}
	}
	// For 10 nodes, create 40 weights each
	for (int ii = 0; ii < 10; ++ii)
	{
		vector<float> tempVec;
		for (int jj = 0; jj < 40; ++jj)
		{
			outFile << to_string(weightArrs1[ii][jj]) << " ";
		}
	}
	// For 1 node, create 10 weights
	for (int ii = 0; ii < 10; ++ii)
	{
		outFile << to_string(weightArrs2[ii]) << " ";
	}
}

float NeuralNetwork::computeScoreForBoard(vector<int> inputArray)
{
	//These are all used in the computeBoardScore function
	int currentLayer;
	int previousLayer;
	int currentNode;
	int currentWeight;
	int weightCount;

	vector<float> resultArrs0, resultArrs1;
	float finalResult;
	// Take the 32 inputs from the Board and multiply them by each node's weights that are in weightArrs0
	// Put the results into a vector that each entry is the averaging of each weight's result
#pragma loop(hint_parallel(8))
	for (int ii = 0; ii < LAYER_2_SIZE; ++ii) // 40 nodes
	{
		resultArrs0.push_back(0);
		vector<float>::iterator weights0ItrInner;
		weights0ItrInner = weightArrs0[ii].begin();
		for (int jj = 0; jj < LAYER_1_SIZE; ++jj) // 32 weights per node
		{
			*(resultArrs0.begin() + ii) += *(weightArrs0[ii].begin() + jj) * *(inputArray.begin() + jj);
			//resultArrs0[ii] += weightArrs0[ii][jj] * inputArray[jj];
		}
		*(resultArrs0.begin() + ii) /= LAYER_1_SIZE; // divide by the 32 weights
		//cout << "resultArrs0[ii]: " << resultArrs0[ii] << endl;
	}

	// Take the 40 inputs from the original results and multiply them by each node's weights that are in weightArrs1
	// Put the results into a vector that each entry is the averaging of each weight's result
#pragma loop(hint_parallel(8))
	for (int ii = 0; ii < LAYER_3_SIZE; ++ii) // 10 nodes
	{
		resultArrs1.push_back(0);
		vector<float>::iterator weights1ItrInner;
		weights1ItrInner = weightArrs1[ii].begin();
		for (int jj = 0; jj < LAYER_2_SIZE; ++jj) // 40 weights per node
		{
			*(resultArrs1.begin() + ii) += *(weights1ItrInner+jj) * *(resultArrs0.begin()+jj);
		}
		*(resultArrs1.begin() + ii) /= LAYER_2_SIZE; // divide by the 40 weights
		//cout << "resultArrs1[ii]: " << resultArrs1[ii] << endl;
	}

	// Take the 10 inputs from the second results and multiply them by each node's weights that are in weightArrs2
	// Put the results into a float that is the averaging of each weight's result
	finalResult = 0;
#pragma loop(hint_parallel(8))
	for (int ii = 0; ii < LAYER_4_SIZE; ++ii)
	{
		finalResult += *(weightArrs2.begin()+ii) * *(resultArrs1.begin()+ii);
	}
	finalResult /= weightArrs2.size();
	//cout << "finalResult: " << finalResult << endl;

	return finalResult;
}

void NeuralNetwork::gameWon()
{
	m_points += 1;
}

void NeuralNetwork::gameLost()
{
	m_points -= 2;
}