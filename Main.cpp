#include<string>
using std::to_string;
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
#include <time.h>
#include <concurrent_vector.h>
#include <fstream>
using std::ofstream;
using std::fstream;
#include <utility>
#include <windows.h>   // WinApi header
#include "BEFNetwork.h"
#include "MoveResultArray.h"
#include "PlayerDriver.h"
#include <vector>
using std::vector;
using std::pair;
using namespace std;//used for the ios:: commands
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <chrono>
using namespace std::chrono;
//#include <vld.h>

#include <map>
using std::map;

//Aaron Andrews
//Zach Hetrick

//This program will take a board and test each move that one side can do, 
//and then estimate what the opponent will do in response.
//Each move will be tested recursively, first testing for jumps, 
//and if no jumps can be done, then test for moves.

/*
Notes:
Need to get at least 1,000,000 board evaluations with the Blondie24 node structure
Need to get rid of all [] operations with the vectors, especially inside of the BEF
Need to make sure that the cache isn't exceeded, or it's going to go out to disc
Delete the boards below the top ten when they get children (free up the memory holding their boards, but keep their value)
Because I'm using BFS, I need to keep the board size very small
Look into using bit masks to store the boards (one student is using an unsigned 32 bit int to hold the black, red, and king pieces)
BFS also needs very fast board creation time
need to implement the code to convert the board to and from the rrrrrrrrrrrr________bbbbbbbbbbbb format
Need to do pruning to not follow bad branch paths (alpha-beta pruning)
Need to prioritize which branches to pursue
Need to get more than 10K generations to have a good player
Using the Linux Mint VM, the g++ compiler can optimize for the Intel hardware very well
Track statistics for the ganes: Number of moves, search depth, etc
See game move history
*/

//Because of the way the code is structured, a max ply of n needs to be n+1 
int MAX_PLY = 3;
int CUR_PLY = 0;

struct NN
{
	vector<vector<float>> weightArrays;
};

vector<vector<float>> createNeuralNetworkLayers();
void fillNeuralNetworkWeights(vector<vector<float>>*);
float evaluateBoard(vector<vector<float>>*, vector<float>);

HANDLE  hConsole;	//For displaying colors on the console

//function to change the sides(swap out the current NN and put the other NN in)
void switchBoard(int *);//, WeightArrs);
void boardDisplay(vector<int>);
void squareDisplay(int, vector<int>);
void createNeuralNetworkChildFromFileNumber_ByValue_ToFileNumber(int, float, int);

//pointer to save boards
int tempBoard[32];

NeuralNetwork NN_R;
NeuralNetwork NN_W;

int turn;

int main()
{	
	MoveResultArray * _moveResultsArray = new MoveResultArray;
	_moveResultsArray->assignTables();
	srand((unsigned int)time(0)); //Seed the random number generator
	map<string, TreeNode> boardTree;
	
	//float TEMP_VAL_R;
	//float TEMP_VAL_W;

	//fillArrs(RED_PLAYER);
	//fillArrs(WHI_PLAYER);

	// Fill Weights
	//NN_R.fillWeights(RED_PLAYER.arr1, RED_PLAYER.arr2, RED_PLAYER.arr3);
	//NN_W.fillWeights(WHI_PLAYER.arr1, WHI_PLAYER.arr2, WHI_PLAYER.arr3);
	//
	//vector<vector<float>> weightArrs0, weightArrs1;
	//vector<float> weightArrs2;

	//srand(clock());

	//for (int kk = 9; kk < 12; ++kk)
	//{
	//	// For 40 nodes, create 32 weights each
	//	for (int ii = 0; ii < 40; ++ii)
	//	{
	//		vector<float> tempVec;
	//		for (int jj = 0; jj < 32; ++jj)
	//		{
	//			tempVec.push_back(std::rand() % 100 / 100.0);
	//		}
	//		weightArrs0.push_back(tempVec);
	//	}
	//	// For 10 nodes, create 40 weights each
	//	for (int ii = 0; ii < 10; ++ii)
	//	{
	//		vector<float> tempVec;
	//		for (int jj = 0; jj < 40; ++jj)
	//		{
	//			tempVec.push_back(std::rand() % 100 / 100.0);
	//		}
	//		weightArrs1.push_back(tempVec);
	//	}
	//	// For 1 node, create 10 weights
	//	for (int ii = 0; ii < 10; ++ii)
	//	{
	//		weightArrs2.push_back(std::rand() % 100 / 100.0);
	//	}

	//	ofstream outFile;
	//	string outFileName = "NNs/NN_" + to_string(kk) + ".txt";
	//	outFile.open(outFileName);
	//	outFile << 0 << " ";
	//	outFile << 0 << " ";

	//	// For 40 nodes, send each one's 32 weights to file
	//	for (int ii = 0; ii < 40; ++ii)
	//	{
	//		vector<float> tempVec;
	//		for (int jj = 0; jj < 32; ++jj)
	//		{
	//			outFile << to_string(weightArrs0[ii][jj]) << " ";
	//		}
	//	}
	//	// For 10 nodes, send each one's 40 weights to file
	//	for (int ii = 0; ii < 10; ++ii)
	//	{
	//		vector<float> tempVec;
	//		for (int jj = 0; jj < 40; ++jj)
	//		{
	//			outFile << to_string(weightArrs1[ii][jj]) << " ";
	//		}
	//	}
	//	// For 1 node, send its 10 weights to file
	//	for (int ii = 0; ii < 10; ++ii)
	//	{
	//		outFile << to_string(weightArrs2[ii]) << " ";
	//	}

	//	weightArrs0.clear();
	//	weightArrs1.clear();
	//	weightArrs2.clear();

	//	clock_t goalTime = (clock_t)1000 + clock();
	//	while (goalTime > clock())
	//	{
	//	}

	//}
	
	int * currBoard;
	
	//take board
	string bestBoardKey;
	int pieceMovedIndex, movedToPosition;
	// Starting board
	vector<int> board;
	for (int ii = 0; ii < 12; ++ii)
	{
		board.push_back(1);
	}
	for (int ii = 0; ii < 8; ++ii)
	{
		board.push_back(0);
	}
	for (int ii = 0; ii < 12; ++ii)
	{
		board.push_back(-1);
	}

	//boardDisplay(board);

	std::shared_ptr<MoveResultArray> _moveResArr(new MoveResultArray);
	_moveResArr->assignTables();

	vector<int> boardCopy;

	int battleCount = 0, cacheMisses = 0, bigMisses = 0, cacheHit = 0, runCount = 0;
	vector<vector<float>> v;
	float t = 0.0, oldTimer = 0.0;
	v = createNeuralNetworkLayers();
	fillNeuralNetworkWeights(&v);

	while (runCount < 30)
	{
		while (t < 1.0)
		{
			vector<float> boardIn;
			for (int bb = 0; bb < 32; ++bb)
			{
				boardIn.push_back((std::rand() % 100) / 100.0);
			}
			t += evaluateBoard(&v, boardIn);
			++battleCount;
					/*cout << battleCount << endl;
					cout << t << endl;	*/

			if ((t - oldTimer) < 0.00001)
			{
				++cacheHit;
			}
			else if ((t - oldTimer) < 0.01)
			{
				++cacheMisses;
			}
			else
			{
				++bigMisses;
			}
		}
		t = 0.0;
		++runCount;
		cout << runCount << endl;
	}

	cout << "battleCount: " << battleCount /30 << endl;
	cout << "time: " << t << endl;	
	cout << "cacheHit: " << cacheHit / 30 << endl;
	cout << "cacheMisses: " << cacheMisses / 30 << endl;
	cout << "bigMisses: " << bigMisses / 30 << endl;


	/*while (battleCount < 300)
	{
		
		for (int ii = 0; ii < 100; ++ii)
		{
		boardCopy = board;

		// Pick a NN in the order of the for loop
		// Randomly pick an opponent
		// Create two players, red and white
		RedPlayer redPlayer(ii, _moveResArr);

		for (int bb = 0; bb < 5; ++bb)
		{
		int counterNN = std::rand() % 100;
		WhitePlayer whitePlayer(counterNN, _moveResArr);
		//cout << "Picked counter NN " << counterNN << endl;
		cout << "Battle " << battleCount << ":" << endl;
		cout << "NN_" << ii << " vs NN_" << counterNN << endl;

		// Have each one take a turn, for a total of 100 turns, or until a winner is found
		bool winnerUndetermined = true;
		for (int jj = 0; jj < 100 && winnerUndetermined; ++jj)
		{
		//cout << "turn " << jj << endl;
		redPlayer.beginBoardEvaluation(boardCopy);
		boardCopy = redPlayer.returnBestBoard();

		if (redPlayer.lostGame(boardCopy))
		{
		winnerUndetermined = false;
		redPlayer.NN.gameLost();
		whitePlayer.NN.gameWon();
		cout << "White wins" << endl;
		}
		//boardDisplay(boardCopy);

		whitePlayer.beginBoardEvaluation(boardCopy);
		boardCopy = whitePlayer.returnBestBoard();
		if (whitePlayer.lostGame(boardCopy))
		{
		winnerUndetermined = false;
		redPlayer.NN.gameWon();
		whitePlayer.NN.gameLost();
		cout << "Red wins" << endl;
		}
		//boardDisplay(boardCopy);
		}

		// Save the NNs back to their files
		redPlayer.NN.writeWeightsToFileNumber(ii);
		whitePlayer.NN.writeWeightsToFileNumber(counterNN);
		++battleCount;
		boardDisplay(boardCopy);
		cout << endl << endl;
		}
		}

		// Create an array of pairs: the NNs rank and index
		vector <pair<int, int>> battleRankVec;
		for (int ii = 0; ii < 50; ++ii)
		{
		string fileName = "NNs/NN_" + std::to_string(ii) + ".txt";

		std::ifstream inFile(fileName);
		string tempString;
		// Read in first number of the NN
		inFile >> tempString;
		// put the number into the pair, along with the index
		battleRankVec.push_back(std::make_pair(stoi(tempString), ii));
		//cout << battleRankVec[ii].first << endl;

		}
		// Sort the array by the first value of each pair
		std::sort(battleRankVec.rbegin(), battleRankVec.rend());

		// Spawn children from each of the top 50 NNs, overwriting the bottom 50
		// This function also clears the scores of the parents
		for (int ii = 0; ii < battleRankVec.size()/2; ++ii)
		{
		cout << "Score of " << battleRankVec[ii].second << " is " << battleRankVec[ii].first << ", vs score of " << battleRankVec[ii + 50].second << " is " << battleRankVec[ii + 50].first << endl;
		cout << "Spawn of " << battleRankVec[ii].second << " will overwrite " << battleRankVec[ii + 50].second << endl;

		createNeuralNetworkChildFromFileNumber_ByValue_ToFileNumber(ii, 0.2, ii+50);
		}
	}*/
	
	// TEST BEF FOR TIMING
	battleCount = 0;

	//These are all used in the computeBoardScore function
	int currentLayer;
	int previousLayer;
	int currentNode;
	int currentWeight;
	int weightCount;
	/*
	int run = 0;
	//auto timer = high_resolution_clock::now() - high_resolution_clock::now();
	chrono::duration<float> timer = high_resolution_clock::now() - high_resolution_clock::now();
	chrono::duration<float> oldTimer = timer;
	int jump = 0, avgBattleCount = 0, cacheMisses = 0, bigMisses = 0, cacheHit = 0;
	float jumpAvg = 0.0, finalResult;
	vector<float> weightArr0, weightArr1, weightArr2;
	{
		NeuralNetwork NN;
		NN.fillWeightsFromFileNumber(0);
		weightArr0 = NN.weightArr0;
		weightArr1 = NN.weightArr1;
		weightArr2 = NN.weightArr2;
	}
	//vector<float> timeIncrements;
	int bigRun = 0;
	while (bigRun < 30)
	{
		run = 0;
		timer = high_resolution_clock::now() - high_resolution_clock::now();
		while (run < 10000000)
		{
			
			for (int ii = 0; ii < 100; ++ii)
			{
				// Create two players, red and white
				/*string fileName = "NNs_B/NN_" + std::to_string(ii) + ".txt";
				std::ofstream outFile(fileName);
				boost::archive::text_oarchive ar(outFile);*/
	/*
				// Starting board is randomly generated
				vector<int> board;
				for (int bb = 0; bb < 12; ++bb)
				{
					board.push_back((std::rand() % 100));
				}
				for (int bb = 0; bb < 8; ++bb)
				{
					board.push_back((std::rand() % 100));
				}
				for (int bb = 0; bb < 12; ++bb)
				{
					board.push_back((std::rand() % 100));
				}

				vector<int> inputArray = board;
				float* inputIndexPtr, *weightIndexPtr, *outputIndexPtr;
				int *ptrInputBoard;
				vector<float> resultArrs0(40), resultArrs1(10);
				outputIndexPtr = &resultArrs0[0];
				float finalResult = 0.0;
				float t = 0.0;
				high_resolution_clock::time_point t_begin = high_resolution_clock::now();

				//weightIndexPtr = &weightArr0[0];
				for (int bb = 0; bb < LAYER_2_SIZE; bb += 40) // 40 nodes
				{
					for (int jj = 0; jj < LAYER_1_SIZE; jj+=32) // 32 weights per node
					{
						ptrInputBoard = &inputArray[jj];
						outputIndexPtr = &resultArrs0[0];
						for (int ii = 0; ii < 40; ++ii)
						{
							weightIndexPtr = &weightArr0[ii*LAYER_1_SIZE];
							for (int kk = 0; kk < 32; ++kk) // 32 weights per node
							{
								//t += weightArr0[(ii)*LAYER_1_SIZE + jj] * inputArray[jj];
								*(outputIndexPtr + ii) += *(weightIndexPtr + kk) * *(ptrInputBoard + kk);
								//resultArrs0[ii] += *(ptrii + kk) * *(ptrjj + kk);
								//t += *(ptrii2 + kk) * *(ptrjj + kk);
								//cout << "node " << ii << " with weight " << kk << " * input " << kk << endl;
							}
							//ptrii2 = &resultArrs0[ii];
							//*(ptrii2+ii) = t;
							//resultArrs0[ii] = t;
							//t = 0.0;
						}
					}
				}
				for (int ii = 0; ii < 40; ii+=40)
				{
					//outputIndexPtr = &(resultArrs0[ii]);
					for (int bb = 0; bb < LAYER_2_SIZE; ++bb)
					{
						*(outputIndexPtr + bb) = *(outputIndexPtr + bb) / (1 + abs(*(outputIndexPtr + bb)));
						if (*(outputIndexPtr + bb) < 0.0)
						{
							*(outputIndexPtr + bb) = 0.0;
						}
						else
						{
							*(outputIndexPtr + bb) = 1.0;
						}
					}
				}


				// Take the 40 inputs from the original results and multiply them by each node's weights that are in weightArrs1
				// Put the results into a vector that each entry is the averaging of each weight's result
				
				outputIndexPtr = &resultArrs1[0];
				
				for (int bb = 0; bb < LAYER_3_SIZE; bb+=10) // 10 nodes
				{
					for (int jj = 0; jj < LAYER_2_SIZE; jj+=40) // 40 weights per node
					{
						inputIndexPtr = &resultArrs0[jj];
						for (int ii = 0; ii < 10; ++ii)
						{
							weightIndexPtr = &weightArr1[ii*LAYER_2_SIZE];
							for (int kk = 0; kk < 40; ++kk)
							{
								*(outputIndexPtr + ii) += *(weightIndexPtr + kk) * *(inputIndexPtr + kk);
							}
						}
					}
				}
				for (int ii = 0; ii < 10; ++ii)
				{
					//ptrii3 =
					for (int bb = 0; bb < LAYER_3_SIZE; ++bb) // 10 nodes
					{
						// Sigmoid Function
						*(outputIndexPtr + bb) = *(outputIndexPtr + bb) / (1 + abs(*(outputIndexPtr + bb)));
						if (*(outputIndexPtr + bb) < 0.0)
						{
							*(outputIndexPtr + bb) = 0.0;
						}
						else
						{
							*(outputIndexPtr + bb) = 1.0;
						}
					}
				}

				// Take the 10 inputs from the second results and multiply them by each node's weights that are in weightArrs2
				// Put the results into a float that is the averaging of each weight's result
				//weightsItr = NN.weightArr2.begin();
				//resultsItr0 = resultArrs1.begin();
				//for (int ii = 0; ii < LAYER_3_SIZE; ++ii)
				//{
				//	//finalResult += *(weightArrs2.begin()+ii) * *(resultArrs1.begin()+ii);
				//	//finalResult += *(weightsItr + ii) * *(resultsItr0 + ii);
				//	finalResult += (NN.weightArr2[ii]) * (resultArrs0[ii]);
				//}
				//// Sigmoid Function
				//finalResult = finalResult / (1 + abs(finalResult));
				high_resolution_clock::time_point t_end = high_resolution_clock::now();
				timer += t_end - t_begin;
				battleCount++;
				++avgBattleCount;
				if (timer.count() >= 1.0)
				{
					run = 1000000000;
					break;
				}
				if ((timer - oldTimer).count() < 0.00001)
				{
					++cacheHit;
				}
				else if ((timer - oldTimer).count() < 0.01)
				{
					++cacheMisses;
				}
				else
				{
					++bigMisses;
				}
				if (oldTimer != timer)
				{
					//++jumps;
					//jumpAvg += jump;
					//jump = 0;
					//timeIncrements.push_back((timer - oldTimer).count());
					oldTimer = timer;
				}
			}

			//cout << "battleCount is " << battleCount << endl;
			//cout << "timer is " << timer.count() << endl;
			//cout << "jump is " << jump << endl;
			++run;
			++jump;
		}
		++bigRun;
		cout << "bigRun is " << bigRun << endl;
		cout << "battleCount is " << battleCount << endl;
		battleCount = 0;
		//cout << "battleCount is " << battleCount << endl;
	}
	//jumpAvg /= jumps;
	//cout << "jumps: " << jumps << endl;
	//cout << "jumpAvg: " << jumpAvg << endl;
	
	//for (int ii = 0; ii < timeIncrements.size(); ++ii)
	//{
	//	cout << timeIncrements[ii] << endl;
	//}
	cout << "cacheHits: " << cacheHit/30 << endl;
	cout << "cacheMisses: " << cacheMisses/30 << endl;
	cout << "bigMisses: " << bigMisses/30 << endl;
	cout << "battleCount is " << avgBattleCount / 30 << endl;
	*/
	return 0;
}

vector<vector<float>> createNeuralNetworkLayers()
{
	vector<vector<float>> weightArrays;
	string input, numParse = "";
	int inputNum = 32; // The number of weights is the number of nodes multiplied by the number of inputs
	getline(cin, input);

	for (int ii = 0; ii < input.size(); ++ii)
	{
		if (!isalpha(input[ii]) && !isspace(input[ii]) && input[ii] != ',')
		{
			numParse += input[ii];
		}
		else if (input[ii] == ',')
		{
			weightArrays.push_back(vector<float>(stoi(numParse)*inputNum));
			inputNum = stoi(numParse);
			numParse = "";
		}

		if (ii == input.size() - 1)
		{
			weightArrays.push_back(vector<float>(stoi(numParse)*inputNum));
		}
	}
	return weightArrays;
}

void fillNeuralNetworkWeights(vector<vector<float>> *weightArrs)
{
	float randVal;
	for (int ii = 0; ii < (*weightArrs).size(); ++ii)
	{
		for (int jj = 0; jj < (*weightArrs)[ii].size(); ++jj)
		{
			randVal = (float)(std::rand() % 100 / 100.0);
			if (randVal < -1.0)
			{
				randVal = -1.0;
			}
			else if (randVal  > 1.0)
			{
				randVal = 1.0;
			}
			(*weightArrs)[ii][jj] = randVal;
		}
	}
}

float evaluateBoard(vector<vector<float>>* totalWeightArrsPtr, vector<float> inputBoard)
{
	// The timing object
	chrono::duration<float> timer = high_resolution_clock::now() - high_resolution_clock::now();
	// Pointers to the input, weight, and output array indexes
	float *inputIndexPtr, *weightIndexPtr, *outputIndexPtr, weightsSum = 0.0;
	// Pointers to the input, weight, and output arrays
	//shared_ptr<vector<float>> ;
	vector<float> *inputArrPtr, *weightArrPtr, *outputArrPtr;
	vector<float> arrayHolder0, arrayHolder1 = inputBoard;
	// Sizes for the input, weight block (nodes), and the weight array sizes
	int inputArrSize, weightsArrSize, totalWeightArrSize;

	// Assign the input board to the output pointer so that when the for loop starts it will be put assigned the input pointer
	outputArrPtr = &arrayHolder1;
	inputArrPtr = &arrayHolder0;
	int tile = 4;
	// Loop to go through each layer and multiply its weights by the input
	for (int rr = 0; rr < totalWeightArrsPtr->size(); ++rr)
	{
		// Assign the output array to the input array pointer
		swap(inputArrPtr, outputArrPtr);
		(*outputArrPtr).clear();
		int newSizeForOutput = (*(totalWeightArrsPtr))[rr].size() / (*inputArrPtr).size();
		(*outputArrPtr).resize(newSizeForOutput);
		weightArrPtr = &(*totalWeightArrsPtr)[rr];

		// Assigning the sizes
		inputArrSize = (*inputArrPtr).size();
		totalWeightArrSize = (*(totalWeightArrsPtr))[rr].size();
		weightsArrSize = totalWeightArrSize / inputArrSize;


		//cout << "Layer " << rr << endl;
		// Start timer
		high_resolution_clock::time_point t_begin = high_resolution_clock::now();

		//// Assign the input and output index pointers to the first index of the input and output arrays
		inputIndexPtr = &(*inputArrPtr)[0];
		outputIndexPtr = &(*outputArrPtr)[0];

		// Loop to go through each weight block (node) in the weights array
		for (int ii = 0; ii < weightsArrSize; ++ii)
		{
			//outputIndexPtr = &(*outputArrPtr)[ii];
			weightsSum = 0.0;
			weightIndexPtr = &(*weightArrPtr)[0] + ii*inputArrSize;
			// Loop to go through each index of the input
			for (int jj = 0; jj < inputArrSize; ++jj)
			{
				// The arithmetic
				weightsSum += *(weightIndexPtr + jj) * *(inputIndexPtr + jj);
			}
			*(outputIndexPtr + ii) = weightsSum;
		}

		//outputIndexPtr = &(*outputArrPtr)[0];
		// Loop to go through the output array
		for (int bb = 0; bb < weightsArrSize; ++bb)
		{
			outputIndexPtr = &(*outputArrPtr)[bb];
			// Sigmoid function
			*(outputIndexPtr) = *(outputIndexPtr) / (1 + abs(*(outputIndexPtr)));
			// Make the output 1 or 0
			if (*(outputIndexPtr) < 0.0)
			{
				*(outputIndexPtr) = 0.0;
			}
			else
			{
				*(outputIndexPtr ) = 1.0;
			}
		}
		// End time tracking
		high_resolution_clock::time_point t_end = high_resolution_clock::now();
		timer += t_end - t_begin;

		//cout << endl << endl;
	}

	return timer.count();
}

/*{
	for (unsigned int i = 1; i < layer.size(); ++i)
	{
		for (unsigned int j = 1; j < layer_[i].size(); ++j)
		{
			double temp = 0.0;
			for (unsigned int k = 1; k < layer[i - 1].size(); ++k)
			{
				temp += layer_[i - 1][k].value * (layer_[i][j].weights[k]);
			}
			layer_[i][j].value = sigmoid(temp);
		}
	}
}*/

/////////////////////////////////////////////////
///////////////Begin of helpful functions
/////////////////////////////////////////////////

//boardDisplay
//
//Function that will display the board for testing purposes
//
//Pre:	A ready board
//Post: Displays each square of the board as a '#', a space, a "ww", for a white checker
//		a "WW" for a white king, "rr" for a red checker, or a "RR" for a red king
//
void boardDisplay(vector<int> board)
{
		for(int ii = 0; ii<32; ii+=8)
		{
			cout<<"##";
			squareDisplay(ii, board);
			cout<<"##";
			squareDisplay(ii+1, board);
			cout<<"##";
			squareDisplay(ii+2, board);
			cout<<"##";
			squareDisplay(ii+3, board);
			cout<<"\n";

			squareDisplay(ii+4, board);
			cout<<"##";
			squareDisplay(ii+5, board);
			cout<<"##";
			squareDisplay(ii+6, board);
			cout<<"##";
			squareDisplay(ii+7, board);
			cout<<"##";
			cout<<"\n";
		}
		cout<<"\n";
}

//squareDisplay
//
//Function that will display the squares of the board with colors
//
//Pre:	The current square and a ready board
//Post: Displays a square of the board as a teal '#', a black space, a white "ww",
//		a white "WW", a red "rr", or a red "RR"
//
void squareDisplay(int ii, vector<int> board)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		if(board[ii] > 1)
		{
			SetConsoleTextAttribute(hConsole, 4); 
			cout << "RR";
		}
		else if(board[ii] > 0)
		{
			SetConsoleTextAttribute(hConsole, 4); 
			cout << "rr";
		}
		else if(board[ii] == 0)
		{
			SetConsoleTextAttribute(hConsole, 4); 
			cout << "  ";
		}
		else if(board[ii] < -1)
		{
			SetConsoleTextAttribute(hConsole, 15); 
			cout << "WW";
		}
		else
		{
			SetConsoleTextAttribute(hConsole, 15); 
			cout << "ww";
		}
			SetConsoleTextAttribute(hConsole, 3); 
}


void createNeuralNetworkChildFromFileNumber_ByValue_ToFileNumber(int parentFileNumber, float maxChangeAmount, int fileNumber)
{
	int intMaxChangeAmount = (int)(maxChangeAmount * 200);
	std::shared_ptr<NeuralNetwork> newNeuralNetworkPtr(new NeuralNetwork);
	newNeuralNetworkPtr->fillWeightsFromFileNumber(parentFileNumber);

	// Zero out the parents score for the new generation tournament
	newNeuralNetworkPtr->m_points = 0;
	newNeuralNetworkPtr->writeWeightsToFileNumber(parentFileNumber);

	// Begin modifying the parent's values for the child NN
	for (int ii = 0; ii < newNeuralNetworkPtr->weightArr0.size(); ++ii)
	{
		newNeuralNetworkPtr->weightArr0[ii] += ((float)(std::rand() % intMaxChangeAmount / 100.0) - maxChangeAmount);

		if (newNeuralNetworkPtr->weightArr0[ii] < -1.0)
		{
			newNeuralNetworkPtr->weightArr0[ii] = -1.0;
		}
		else if (newNeuralNetworkPtr->weightArr0[ii] > 1.0)
		{
			newNeuralNetworkPtr->weightArr0[ii] = 1.0;
		}
	}

	for (int ii = 0; ii < newNeuralNetworkPtr->weightArr1.size(); ++ii)
	{
		newNeuralNetworkPtr->weightArr1[ii] += ((float)(std::rand() % intMaxChangeAmount / 100.0) - maxChangeAmount);

		if (newNeuralNetworkPtr->weightArr1[ii] < -1.0)
		{
			newNeuralNetworkPtr->weightArr1[ii] = -1.0;
		}
		else if (newNeuralNetworkPtr->weightArr1[ii] > 1.0)
		{
			newNeuralNetworkPtr->weightArr1[ii] = 1.0;
		}
	}

	for (int ii = 0; ii < newNeuralNetworkPtr->weightArr2.size(); ++ii)
	{
		newNeuralNetworkPtr->weightArr2[ii] += ((float)(std::rand() % intMaxChangeAmount / 100.0) - maxChangeAmount);
		if (newNeuralNetworkPtr->weightArr2[ii] < -1.0)
		{
			newNeuralNetworkPtr->weightArr2[ii] = -1.0;
		}
		else if (newNeuralNetworkPtr->weightArr2[ii] > 1.0)
		{
			newNeuralNetworkPtr->weightArr2[ii] = 1.0;
		}
	}

	string fileName = "NNs/NN_" + to_string(fileNumber) + ".txt";
	ofstream outFile;
	outFile.open(fileName);
	outFile << to_string(0.0) << " ";
	outFile << to_string(0.0) << " ";

	// For 40 nodes, create 32 weights each
	for (int ii = 0; ii < 40; ++ii)
	{
		vector<float> tempVec;
		for (int jj = 0; jj < 32; ++jj)
		{
			outFile << to_string(newNeuralNetworkPtr->weightArr0[ii]) << " ";
		}
	}
	// For 10 nodes, create 40 weights each
	for (int ii = 0; ii < 10; ++ii)
	{
		vector<float> tempVec;
		for (int jj = 0; jj < 40; ++jj)
		{
			outFile << to_string(newNeuralNetworkPtr->weightArr1[ii]) << " ";
		}
	}
	// For 1 node, create 10 weights
	for (int ii = 0; ii < 10; ++ii)
	{
		outFile << to_string(newNeuralNetworkPtr->weightArr2[ii]) << " ";
	}
}
