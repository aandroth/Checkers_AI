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
	//cout << fileName << endl;

	std::ifstream inFile(fileName);
	string tempString, fileString;
	int state = 0, countChar = 0, stateLimit1 = LAYER_1_SIZE*LAYER_2_SIZE, stateLimit2 = LAYER_2_SIZE*LAYER_3_SIZE, stateLimit3 = LAYER_3_SIZE;
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
			if (countChar <= stateLimit1) // 32 weights * 40 nodes
			{
				weightArr0.push_back(std::stof(tempString));
			}
			else
			{
				countChar = 1;
				state = 2;
				weightArr1.push_back(std::stof(tempString));
			}
			break;
		case 2:
			if (countChar <= stateLimit2) // 40 weights * 10 nodes
			{
				weightArr1.push_back(std::stof(tempString));
			}
			else
			{
				countChar = 1;
				state = 3;
				weightArr2.push_back(std::stof(tempString));
			}
			break;
		case 3:
			if (countChar <= stateLimit3) // 10 weights for 1 node
			{
				weightArr2.push_back(std::stof(tempString));
			}
			break;
		}
		++countChar;
	}

	// For 40 nodes, create 32 weights each
	/*for (int ii = 0; ii < 40*32; ++ii)
	{
			cout << std::to_string(weightArr0[ii]) << " ";
	}
	// For 10 nodes, create 40 weights each
	for (int ii = 0; ii < 10*40; ++ii)
	{
			cout << std::to_string(weightArr1[ii]) << " ";
	}
	// For 1 node, create 10 weights
	for (int ii = 0; ii < 10; ++ii)
	{
		cout << std::to_string(weightArr2[ii]) << " " << endl;;
	}*/
}

void NeuralNetwork::writeWeightsToFileNumber(int fileNumber)
{
	/*string fileName = "NNs/NN_" + to_string(fileNumber) + ".txt";
	ofstream outFile;
	outFile.open(fileName);
	outFile << to_string(m_points) << " ";
	outFile << to_string(m_class) << " ";

	//vector<float>::iterator weightsItr;
	//weightsItr = weightArrs0.begin();
	// For 40 nodes, create 32 weights each
	for (int ii = 0; ii < 40; ++ii)
	{
		vector<float> tempVec;
		for (int jj = 0; jj < 32; ++jj)
		{
			outFile << to_string(weightArrs0[ii][jj]) << " ";
			//outFile << to_string(*(weightsItr +ii)) << " ";
		}
	}
	//weightsItr = weightArrs1.begin();
	// For 10 nodes, create 40 weights each
	for (int ii = 0; ii < 10; ++ii)
	{
		vector<float> tempVec;
		for (int jj = 0; jj < 40; ++jj)
		{
			outFile << to_string(weightArrs1[ii][jj]) << " ";
			//outFile << to_string(*(weightsItr + ii)) << " ";
		}
	}
	//weightsItr = weightArrs2.begin();
	// For 1 node, create 10 weights
	for (int ii = 0; ii < 10; ++ii)
	{
		outFile << to_string(weightArrs2[ii]) << " ";
		//outFile << to_string(*(weightsItr + ii)) << " " << " ";
	}*/
}

float NeuralNetwork::computeScoreForBoard(vector<int> inputArray)
{/*
	//These are all used in the computeBoardScore function
	int currentLayer;
	int previousLayer;
	int currentNode;
	int currentWeight;
	int weightCount;

	vector<float> resultArrs0(40), resultArrs1(10);
	float finalResult = 0.0;
	// Take the 32 inputs from the Board and multiply them by each node's weights that are in weightArrs0
	// Put the results into a vector that each entry is the averaging of each weight's result
	vector<float>::iterator weightsItr, resultsItr0, resultsItr1, inputItr;
	vector<int>::iterator inputBoardItr;
	weightsItr = weightArr0.begin();
	inputBoardItr = inputArray.begin();
	resultsItr0 = resultArrs0.begin();

	for (int ii = 0; ii < LAYER_2_SIZE; ++ii) // 40 nodes
	{
		for (int jj = 0; jj < LAYER_1_SIZE; ++jj) // 32 weights per node
		{
			*(resultsItr0 + ii) += *(weightsItr + ii*LAYER_1_SIZE + jj) * *(inputBoardItr + jj);
		}
		// Sigmoid Function
		*(resultsItr0 + ii) = *(resultsItr0 + ii) / (1 + abs(*(resultsItr0 + ii)));
		//*(resultsItr0 + ii) = 1 / (1 + exp(-(*(resultsItr0 + ii))));
		if (!(*(resultsItr0 + ii) > 0.0))
		{
			*(resultsItr0 + ii) = 0.0;
		}
		else
		{
			*(resultsItr0 + ii) = 1.0;
		}
		//*(resultsItr0 + ii) = (*(resultsItr0 + ii) + (abs(*(resultsItr0 + ii)))) / (2 * *(resultsItr0 + ii));
		//*(resultsItr0 + ii)  = ((*(resultsItr0 + ii) AND -1) OR 1) * 0.5
		cout << "resultsItr0[" << ii << "]: " << *(resultsItr0 + ii) << endl;
	}
	
	// Take the 40 inputs from the original results and multiply them by each node's weights that are in weightArrs1
	// Put the results into a vector that each entry is the averaging of each weight's result
	weightsItr = weightArr1.begin();
	inputItr = resultArrs0.begin();
	resultsItr1 = resultArrs1.begin();
	for (int ii = 0; ii < LAYER_3_SIZE; ++ii) // 10 nodes
	{
		for (int jj = 0; jj < LAYER_2_SIZE; ++jj) // 40 weights per node
		{
			*(resultsItr1 + ii) += *(weightsItr + ii*LAYER_2_SIZE + jj) * *(resultsItr0 + jj);
		}
		// Sigmoid Function
		//*(resultArrs1.begin() + ii) /= LAYER_2_SIZE; // divide by the 40 weights
		//cout << "resultArrs1[ii]: " << resultArrs1[ii] << endl;
		*(resultsItr1 + ii) = *(resultsItr1 + ii) / (1 + abs(*(resultsItr1 + ii)));
		if (!(*(resultsItr1 + ii) > 0.0))
		{
			*(resultsItr1 + ii) = 0.0;
		}
		else
		{
			*(resultsItr1 + ii) = 1.0;
		}
	}

	// Take the 10 inputs from the second results and multiply them by each node's weights that are in weightArrs2
	// Put the results into a float that is the averaging of each weight's result
	weightsItr = weightArr2.begin();
	resultsItr0 = resultArrs1.begin();
	for (int ii = 0; ii < LAYER_3_SIZE; ++ii)
	{
		//finalResult += *(weightArrs2.begin()+ii) * *(resultArrs1.begin()+ii);
		finalResult += *(weightsItr + ii) * *(resultsItr0 + ii);
		cout << "finalReult: " << finalResult;
		cout << ", weightsItr[" << ii << "]: " << *(weightsItr + ii);
		cout << ", resultsItr0[" << ii << "]: " << *(resultsItr0 + ii) << endl;
	}
	// Sigmoid Function
	finalResult = finalResult / (1 + abs(finalResult));
	*/
	return 0;// finalResult;
}

void NeuralNetwork::gameWon()
{
	m_points += 1;
}

void NeuralNetwork::gameLost()
{
	m_points -= 2;
}