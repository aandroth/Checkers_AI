#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include <iostream>
#include <time.h>
#include <math.h>
#include <string>
using std::string;
using std::to_string;
#include <fstream>
using std::ofstream;
#include <vector>
using std::vector;
#include <memory>
//#include <boost\serialization\vector.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>

const float MATH_E = 2.7182818f;
const float sigCo = -8.0f; //The value for 's' in the sigmoid function

const int NUM_LAYERS = 4;
const int LAYER_1_SIZE = 32;
const int LAYER_2_SIZE = 40;
const int LAYER_3_SIZE = 10;
const int LAYER_4_SIZE = 1;

const int NUM_NODES = LAYER_1_SIZE + LAYER_2_SIZE + LAYER_3_SIZE + LAYER_4_SIZE;

const int NUM_WEIGHTS = (LAYER_1_SIZE*LAYER_2_SIZE)
					  + (LAYER_2_SIZE*LAYER_3_SIZE)
					  + (LAYER_3_SIZE*LAYER_4_SIZE);

class NeuralNetwork
{
public:
	NeuralNetwork();	//The default ctor
	~NeuralNetwork(){};
	float computeScoreForBoard(vector<int>); //To compute the final board value
	void fillWeights(float arr1[LAYER_2_SIZE][LAYER_1_SIZE], float arr2[LAYER_3_SIZE][LAYER_2_SIZE], float arr3[LAYER_4_SIZE][LAYER_3_SIZE]); //Filling the weights with random numbers
	void fillWeightsFromFileNumber(int);
	vector<float> returnWeightArr0FromFileNumber(int);
	vector<float> returnWeightArr1FromFileNumber(int);
	vector<float> returnWeightArr2FromFileNumber(int);
	void writeWeightsToFileNumber(int fileNumber);
	void gameWon();
	void gameLost();

	vector<float> weightArr0, weightArr1, weightArr2;
	int m_points, m_class;

private:
	float _weights[NUM_WEIGHTS]; //The total number of weights
	int _nodesInLayer[NUM_LAYERS]; //The number of nodes in each layer
	float * _layersAndNodes[NUM_LAYERS]; //This will hold the total layers and nodes


	//template<class Archive>
	//void save(Archive & ar, string fileName) const
	//{
	//	ar & weightArr0;
	//	ar & weightArr1;
	//	ar & weightArr2;
	//}
	//template<class Archive>
	//void save(Archive & ar)
	//{
	//	ar & weightArr0;
	//	ar & weightArr1;
	//	ar & weightArr2;
	//}
	//BOOST_SERIALIZATION_SPLIT_MEMBER()
};


#endif