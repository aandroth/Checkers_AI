#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include <iostream>
#include <time.h>
#include <math.h>

const float MATH_E = 2.7182818f;
const float sigCo = -8.0f; //The value for 's' in the sigmoid function

const int NUM_LAYERS = 4;
const int LAYER_1 = 32;
const int LAYER_2 = 40;
const int LAYER_3 = 10;
const int LAYER_4 = 1;

const int NUM_NODES = LAYER_1 + LAYER_2 + LAYER_3 + LAYER_4;

const int NUM_WEIGHTS = (LAYER_1*LAYER_2)
					  + (LAYER_2*LAYER_3)
					  + (LAYER_3*LAYER_4);

class NeuralNetwork
{
public:
	NeuralNetwork(float arr1[40][32], float arr2[10][40], float arr3[1][10]);	//The default ctor
	float computeBoardScore(int *); //To compute the final board value
	void fillWeights(float arr1[LAYER_2][LAYER_1], float arr2[LAYER_3][LAYER_2], float arr3[LAYER_4][LAYER_3]); //Filling the weights with random numbers

private:
	float _weights[NUM_WEIGHTS]; //The total number of weights
	int _nodesInLayer[NUM_LAYERS]; //The number of nodes in each layer
	float * _layersAndNodes[NUM_LAYERS]; //This will hold the total layers and nodes


};


#endif