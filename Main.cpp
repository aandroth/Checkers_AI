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
Need to get rid of all [] operations with the vectors, especially inside of the BEF
Need to make sure that the cache isn't exceeded, or it's going to go out to disc
Need to change the move evaluator to not allow jumps after a pawn is kinged
*/

//Because of the way the code is structured, a max ply of n needs to be n+1 
int MAX_PLY = 3;
int CUR_PLY = 0;

HANDLE  hConsole;	//For displaying colors on the console

//jump variables to be used by both sides
int jumpFrom, jumpOver, jumpTo;

//move variables to used by both sides
int moveFrom, moveTo;

//function to fill the tables with the correct values
void assignTables();

//functions to drive the moves
float ourMoveDriver(int CUR_PLY, float CUR_MIN, int board[]);
float ourJumpRecurse(int square, float CUR_MIN, int CUR_PLY, int board[]);
vector<vector<int>> redMove(MoveResultArray *, vector<int>, int);
vector<vector<int>> redJump(MoveResultArray *, vector<int>, int);


//functions to change the board for each jump
float jump(int jumpFrom, int jumpOver, int jumpTo, int CUR_PLY, float CUR_VAL, int board[]);
//functions to change the board for each move
float move(int moveFrom, int moveTo, int CUR_PLY, float CUR_VAL, int board[]);

//functions estimate the enemy moves
float enemyMoveDriver(int CUR_PLY, float CUR_MAX, int *);
float enemyJumpRecurse(int square, float CUR_MAX, int CUR_PLY, int *);
float enemyMoveRecurse(int square, float CUR_MAX, int CUR_PLY, int *);

//function to change the sides(swap out the current NN and put the other NN in)
void switchBoard(int *);//, WeightArrs);
void boardDisplay(vector<int>);
void squareDisplay(int, vector<int>);
void createNeuralNetworkChildFromFileNumber_ByValue_ToFileNumber(int, float, int);

// Functions to modify the tree
void sendValueToAncestors(TreeNode *, map<string, TreeNode> *);
bool parentIsLastChildOfGrandparent(string, map<string, TreeNode> *);
string keyOfNextValidNode(string, map<string, TreeNode> *);
string keyOfNextSibling(string, map<string, TreeNode> *);

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
	int battleCount = 0;
	while (true)
	{
		for (int ii = 0; ii < 100; ++ii)
		{
			boardCopy = board;
			cout << "Battle " << battleCount << ":" << endl;

			// Pick a NN in the order of the for loop
			// Randomly pick an opponent
			// Create two players, red and white
			RedPlayer redPlayer(ii, _moveResArr);
			int counterNN = std::rand() % 100;
			WhitePlayer whitePlayer(counterNN, _moveResArr);
			//cout << "Picked counter NN " << counterNN << endl;

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
		
		// Create an array of pairs: the NNs rank and index
		vector <pair<int, int>> battleRankVec;
		for (int ii = 0; ii < 100; ++ii)
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
		for (int ii = 0; ii < battleRankVec.size()/2; ++ii)
		{
			cout << "Score of " << battleRankVec[ii].second << " is " << battleRankVec[ii].first << ", vs score of " << battleRankVec[ii + 50].second << " is " << battleRankVec[ii + 50].first << endl;
			cout << "Spawn of " << battleRankVec[ii].second << " will overwrite " << battleRankVec[ii + 50].second << endl;

			createNeuralNetworkChildFromFileNumber_ByValue_ToFileNumber(ii, 0.2, ii+50);
		}
	}

	/*
	for (int ii = 0; ii < 1; ++ii)
	{
		redPlayer.beginBoardEvaluation(board);
		board = redPlayer.returnBestBoard();
		boardDisplay(board);
		whitePlayer.beginBoardEvaluation(board);
		board = whitePlayer.returnBestBoard();
		boardDisplay(board);
	}
	*/
	// Base the rank on the outcome and the NNs rank
	// Write the new rank back to the NN's file
	 
	// 

	/*
	// TEST
	//create a tree with three layers
	// Original Board
	// Parent boards
	// Child boards
	map<string, TreeNode> testTree;
	map<string, TreeNode> * testTreePtr = &testTree;
	TreeNode * topTreeNode = new TreeNode;
	// Get the value from the BEF
	topTreeNode->_boardValue = -11;
	testTree["_0"] = *topTreeNode;

	for (int ii = 0; ii < 3; ++ii)
	{
		TreeNode * parentTreeNode = new TreeNode;
		// Get the value from the BEF
		parentTreeNode->_boardValue = std::rand() % 20 - 10;
		string parentKey = "_0_" + to_string(ii);
		parentTreeNode->_parentKey = "_0";
		testTree[parentKey] = *parentTreeNode;
		testTree["_0"]._childKeys.push_back(parentKey);
		cout << "Created parent with value of " + to_string(testTree[parentKey]._boardValue) << " and key " << parentKey << endl;

		for (int jj = 0; jj < 5; ++jj)
		{
			TreeNode * childTreeNode = new TreeNode;
			// Get the value from the BEF
			childTreeNode->_boardValue = std::rand() % 20 - 10;
			string childKey = parentKey + "_" + to_string(jj);
			childTreeNode->_parentKey = parentKey;
			testTree[childKey] = *childTreeNode;
			testTree[parentKey]._childKeys.push_back(childKey);
			cout << "Pushed back " << childKey << " to " << parentKey << endl;
			cout << "Created child with value of " + to_string(testTree[childKey]._boardValue) << " and key " << childKey << endl;
			sendValueToAncestors(childTreeNode, testTreePtr);
		}
	}

	cout << "topTreeNode->_childKeys.size(): " << testTree["_0"]._childKeys.size() << endl;

	for (int ii = 0; ii < testTree["_0"]._childKeys.size(); ++ii)
	{
		if (parentIsLastChildOfGrandparent(testTree["_0"]._childKeys[ii], testTreePtr))
		{
			cout << "Node with key " << testTree["_0"]._childKeys[ii] << " is the last child node" << endl;
		}
		else
		{
			cout << "Node with key " << testTree["_0"]._childKeys[ii] << " is not the last child node" << endl;
		}
	}

	for (int ii = 0; ii < testTree["_0"]._childKeys.size(); ++ii)
	{
		cout << "Sibling of " << testTree["_0"]._childKeys[ii] << " is " << keyOfNextSibling(testTree["_0"]._childKeys[ii], testTreePtr) << endl;
	}

	boardDisplay(board);
		cout<<"\n";
		cin>>board[0];
		*/
	return 0;
}


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

	for (int ii = 0; ii < newNeuralNetworkPtr->weightArrs0.size(); ++ii)
	{
		for (int jj = 0; jj < newNeuralNetworkPtr->weightArrs0[ii].size(); ++jj)
		{
			newNeuralNetworkPtr->weightArrs0[ii][jj] += ((float)(std::rand() % intMaxChangeAmount / 100.0) - maxChangeAmount);

			if (newNeuralNetworkPtr->weightArrs0[ii][jj] < 0.0)
			{
				newNeuralNetworkPtr->weightArrs0[ii][jj] = 0.0;
			}
			else if (newNeuralNetworkPtr->weightArrs0[ii][jj] > 1.0)
			{
				newNeuralNetworkPtr->weightArrs0[ii][jj] = 1.0;
			}
		}
	}

	for (int ii = 0; ii < newNeuralNetworkPtr->weightArrs1.size(); ++ii)
	{
		for (int jj = 0; jj < newNeuralNetworkPtr->weightArrs1[ii].size(); ++jj)
		{
			newNeuralNetworkPtr->weightArrs1[ii][jj] += ((float)(std::rand() % intMaxChangeAmount / 100.0) - maxChangeAmount);

			if (newNeuralNetworkPtr->weightArrs1[ii][jj] < 0.0)
			{
				newNeuralNetworkPtr->weightArrs1[ii][jj] = 0.0;
			}
			else if (newNeuralNetworkPtr->weightArrs1[ii][jj] > 1.0)
			{
				newNeuralNetworkPtr->weightArrs1[ii][jj] = 1.0;
			}
		}
	}

	for (int ii = 0; ii < newNeuralNetworkPtr->weightArrs2.size(); ++ii)
	{
		newNeuralNetworkPtr->weightArrs2[ii] += ((float)(std::rand() % intMaxChangeAmount / 100.0) - maxChangeAmount);
		if (newNeuralNetworkPtr->weightArrs2[ii] < 0.0)
		{
			newNeuralNetworkPtr->weightArrs2[ii] = 0.0;
		}
		else if (newNeuralNetworkPtr->weightArrs2[ii] > 1.0)
		{
			newNeuralNetworkPtr->weightArrs2[ii] = 1.0;
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
			outFile << to_string(newNeuralNetworkPtr->weightArrs0[ii][jj]) << " ";
		}
	}
	// For 10 nodes, create 40 weights each
	for (int ii = 0; ii < 10; ++ii)
	{
		vector<float> tempVec;
		for (int jj = 0; jj < 40; ++jj)
		{
			outFile << to_string(newNeuralNetworkPtr->weightArrs1[ii][jj]) << " ";
		}
	}
	// For 1 node, create 10 weights
	for (int ii = 0; ii < 10; ++ii)
	{
		outFile << to_string(newNeuralNetworkPtr->weightArrs2[ii]) << " ";
	}
}
