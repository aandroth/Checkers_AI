#include "BEFNetwork.h"
#include <math.h>
using std::exp;
using std::cout;
using std::cin;
using std::endl;

float L1[LAYER_1];
float L2[LAYER_2];
float L3[LAYER_3]; 
float L4[LAYER_4];

NeuralNetwork::NeuralNetwork(float arr1[40][32], float arr2[10][40], float arr3[1][10])
{
	_layersAndNodes[0] = L1;
	_layersAndNodes[1] = L2;
	_layersAndNodes[2] = L3;
	_layersAndNodes[3] = L4;

	
	//These are for keeping track of the amount of nodes in each layer, since we can't call size() on a C-array
	_nodesInLayer[0] = LAYER_1;
	_nodesInLayer[1] = LAYER_2;
	_nodesInLayer[2] = LAYER_3;
	_nodesInLayer[3] = LAYER_4;
}


//fills the weights with numbers from the NN
void NeuralNetwork::fillWeights(float arr1[LAYER_2][LAYER_1], float arr2[LAYER_3][LAYER_2], float arr3[LAYER_4][LAYER_3])
{		
	int kk = 0;

	for(int ii = 0; ii < LAYER_2; ii++)
	{
		for(int jj = 0; jj < LAYER_1; jj++)
		{
			_weights[kk] = arr1[ii][jj];
			kk++;
		}
	}
	for(int ii = 0; ii < LAYER_3; ii++)
	{
		for(int jj = 0; jj < LAYER_2; jj++)
		{
			_weights[kk] = arr2[ii][jj];
			kk++;
		}
	}
	for(int ii = 0; ii < LAYER_4; ii++)
	{
		for(int jj = 0; jj < LAYER_3; jj++)
		{
			_weights[kk] = arr3[ii][jj];
			kk++;
		}
	}
}


float NeuralNetwork::computeBoardScore(int * inputArray)
{
	//These are all used in the computeBoardScore function
	int currentLayer;
	int previousLayer;
	int currentNode;
	int currentWeight;
	int weightCount;

//	cout<<"computeBoardScore 2: \n";

	//Assign the nodes in the first layer to the inputArray values
	for(int i = 0; i < LAYER_1; ++i)
	{	
		_layersAndNodes[0][i] = inputArray[i];
	}
		//We start at layer 1 so we can start filling in values in the SECOND layer of nodes
		for(currentLayer = 1, previousLayer = 0, weightCount = 0; currentLayer < NUM_LAYERS; ++currentLayer, ++previousLayer)
		{
			//We iterate through the loop however many weights are attached to this node
			for(currentNode = 0; currentNode < _nodesInLayer[currentLayer]; ++currentNode) 
			{
				//Zero out the node before we store a value in it because right now it contains a junk value
				_layersAndNodes[currentLayer][currentNode] = 0;	
	
				//And then start at the first node in the first layer with the attached weight
				//to begin the weight multiplication and addition
				for(currentWeight = 0; currentWeight < _nodesInLayer[previousLayer]; ++currentWeight)	
				{																						
					_layersAndNodes[currentLayer][currentNode] += (_layersAndNodes[previousLayer][currentWeight])*(_weights[weightCount]);
					++weightCount;

					if(currentLayer == 3)
						cout<<"_layersAndNodes[currentLayer][currentNode]: "<<_layersAndNodes[currentLayer][currentNode]<<"\n";
				}

					if(currentLayer != 3)
					{					
						_layersAndNodes[currentLayer][currentNode] = (1/(1+(exp(sigCo*_layersAndNodes[currentLayer][currentNode]))));
					}
			}
		}

	//Return the first node in the last layer, which should be the ONLY node there anyways
	return _layersAndNodes[NUM_LAYERS-1][0];
}