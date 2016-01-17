#include <iostream>
#include <time.h>
#include <concurrent_vector.h>
#include <fstream>
#include <utility>
#include <windows.h>   // WinApi header
#include "BEFNetwork.h"
#include "MoveResultArray.h"
#include "Piece.h"
#include <vector>
using std::vector;
using std::pair;
using std::ofstream;
using std::fstream;
using std::cout;
using std::cin;
using std::endl;
using namespace std;//used for the ios:: commands

//Aaron Andrews
//Zach Hetrick

//This program will take a board and test each move that one side can do, 
//and then estimate what the opponent will do in response.
//Each move will be tested recursively, first testing for jumps, 
//and if no jumps can be done, then test for moves.


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
vector<int> redMove(MoveResultArray *, vector<int>, int);


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
vector<int> vectorizeBoard(int[]);
int* unvectorizeBoard(vector<int>);
int decideWinner(float TEMP_VAL_R, float TEMP_VAL_W, int * board);


//variables to hold values we may change (hard code)
//const int LAYER_1 = 32;
//const int LAYER_2 = 40;
//const int LAYER_3 = 10;
//const int LAYER_4 = 1;
struct WeightArrs
{
	//these are the arrays to hold the weights
	float arr1[LAYER_2][LAYER_1];
	float arr2[LAYER_3][LAYER_2];
	float arr3[LAYER_4][LAYER_3];
	int score;
};

void fillArrs(WeightArrs &);

//pointer to save boards
int tempBoard[32];

WeightArrs RED_PLAYER;
WeightArrs WHI_PLAYER;

NeuralNetwork NN_R(RED_PLAYER.arr1, RED_PLAYER.arr2, RED_PLAYER.arr3);
NeuralNetwork NN_W(WHI_PLAYER.arr1, WHI_PLAYER.arr2, WHI_PLAYER.arr3);

int turn;

int main()
{	
	MoveResultArray * _moveResultsArray = new MoveResultArray;
	_moveResultsArray->assignTables();
	srand((unsigned int)time(0)); //Seed the random number generator
	//float TEMP_VAL_R;
	//float TEMP_VAL_W;

	//fillArrs(RED_PLAYER);
	//fillArrs(WHI_PLAYER);

	//NN_R.fillWeights(RED_PLAYER.arr1, RED_PLAYER.arr2, RED_PLAYER.arr3);
	//NN_W.fillWeights(WHI_PLAYER.arr1, WHI_PLAYER.arr2, WHI_PLAYER.arr3);

	int * currBoard;
	
	//take board
	int bestBoardIndex, pieceMovedIndex, movedToPosition;
	vector<int> tempBoard;
	vector<vector<int>> savedBoards;
	vector<int> board;
	for (int ii = 0; ii < 32; ++ii)
	{
		board.push_back(0);
	}
	board[3] = 1;
	board[31] = -2;
	board[21] = -1;

	int currMax = -11;

	Piece ** pieceArray = new Piece*[12];
	pieceArray[0] = new RedChecker(_moveResultsArray);
	pieceArray[0]->setPosition(3);

	vector<int>	movesToEvaluate;

	boardDisplay(board);

	turn = 1;

	for (int ii = 0; ii < 10; ++ii, ++turn)
	{
		cout<<"the turn is: "<<turn<<"\n";
		currMax = -11;
		//Red takes a turn
		/*TEMP_VAL_R = ourMoveDriver(1, 900.0, currBoard);

		cout<<"TEMP_VAL_"<<TEMP_VAL_R<<"\n";

		for(int ii = 0; ii < 32; ii++)
		{board[ii] = tempBoard[ii];}

		turn++;

		cout<<"Red moved\n";
		boardDisplay(currBoard);

		*/

		for (int cc = 0; cc < 32; ++cc)
		{
			if (board[cc] > 0)
			{
				vector<int> pieceMoves;
				pieceMoves = redMove(_moveResultsArray, board, cc);
				for (int ii = 0; ii < pieceMoves.size(); ++ii)
				{
					tempBoard = board;

					tempBoard[pieceMoves[ii]] = tempBoard[cc];
					tempBoard[cc] = 0;

					if (tempBoard[pieceMoves[ii]] == 1 && pieceMoves[ii] > 27)
					{
						cout << pieceMoves[ii] << endl;
						tempBoard[pieceMoves[ii]] = 2;
					}

					// Get the value from the BEF
					int val = std::rand() % 20 - 10;
					cout << "val is " << val << endl;
					// Compare value to current max
					if (currMax < val)
					{
						currMax = val;
						savedBoards.push_back(tempBoard);
						bestBoardIndex = savedBoards.size() - 1;
					}
					cout << "max is " << currMax << endl;
				}
			}
		}

		board = savedBoards[bestBoardIndex];

		/*
		//switch the board so the other NN can make a move
		cout<<"switch 1\n";
		switchBoard(currBoard);

		//White takes a turn
		TEMP_VAL_W = ourMoveDriver(1, 900.0, currBoard);

		cout<<"TEMP_VAL_W"<<TEMP_VAL_W<<"\n";

		for(int ii = 0; ii < 32; ii++)
			{board[ii] = tempBoard[ii];}
			*/

		//switch the board so the other NN can make a move
		//switchBoard(currBoard);

		//cout<<"White moved\n";
		boardDisplay(board);

		//cout<<"switch 3\n";
		//switchBoard(currBoard);

		//boardDisplay(currBoard);


		//cin>>board[0];
	}

	boardDisplay(board);
		cout<<"\n";
		cin>>board[0];
	return 0;
}


//redMove
//
//Function that will take a piece on the board and test for every possible move from its current position
//
//Pre:	The current square, a ready board and the current ply
//Post: Tests each direction that the peice can move,
//		if no moves can be done it returns -900.0 to ourMoveDriver to report no moves,
//		if a move can be made the move function is called
//
vector<int> redMove(MoveResultArray * _moveResultArray, vector<int> board, int index)
{
	vector<int> tempVec;

	int tempInt = _moveResultArray->movesDown[index].left;
	if (tempInt != -1 && board[tempInt] == 0)
	{
		tempVec.push_back(tempInt);
	}
	tempInt = _moveResultArray->movesDown[index].right;
	if (tempInt != -1 && board[tempInt] == 0)
	{
		tempVec.push_back(tempInt);
	}
	if (board[index] > 1)
	{
		tempInt = _moveResultArray->movesUp[index].left;
		if (tempInt != -1 && board[tempInt] == 0)
		{
			tempVec.push_back(tempInt);
		}
		tempInt = _moveResultArray->movesUp[index].right;
		if (tempInt != -1 && board[tempInt] == 0)
		{
			tempVec.push_back(tempInt);
		}
	}

	return tempVec;
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

//switchBoard
//
//Function that will change the board so that the other NN can take its turn
//
//Pre:	The current board
//Post: None
//
void switchBoard(int * board)
{
	int temp[32];

	for(int ii = 0; ii<32; ii++)
	{
		temp[31-ii] = (board[ii]*(-1));	
	}

	for(int ii = 0; ii<32; ii++)
	{
		board[ii] = temp[ii];	
	}
}


//vectorizeBoard
//
//Function that will return a vector with the values of the int array
//
//Pre:	Int array
//Post: Returns a vector with the values of the currBoard
//
vector<int> vectorizeBoard(int * curr_board)
{
	vector<int> tempVec;
	for (int ii = 0; ii < 32; ++ii)
	{
		tempVec.push_back(curr_board[ii]);
	}
	return tempVec;
}

//unvectorizeBoard
//
//Function that will return a pointer to an int array with the values of the vector
//
//Pre:	A vector of ints
//Post: Returns an array with the values of the vector
//
int* unvectorizeBoard(vector<int> intVec)
{
	int tempArr[32];
	for (int ii = 0; ii < 32; ++ii)
	{
		tempArr[ii] = intVec[ii];
	}
	return tempArr;
}

//decideWinner
//
//Function that will decide which NN won the game, or if it's a draw
//
//Pre:	The final return value of both boards
//Post: Returns an int that signifies the outcome
//		1: Red Won
//		2: White Won
//		3: A Draw
//
int decideWinner(float TEMP_VAL_R, float TEMP_VAL_W, int * board)
{
	//if there is a draw
	if(TEMP_VAL_R == -900 || TEMP_VAL_W == -900)
	{
		return 3;
	}

	//if red has lost
	else if(TEMP_VAL_R == -1000)
	{
		return 2;
	}

	//if white has lost
	else if(TEMP_VAL_W == -1000)
	{
		return 1;
	}

	int total = 0;

	//if there are no clear winners we see who has the most (and best) pieces on the board and use that to deermine a winner
	for(int ii = 0; ii < 32; ii++)
		total += board[ii];

	if(total > 0)
		return 3;

	else if(total < 0)
		return 2;

	else
		return 3;
}


//fillArrs
//
//Function that will a WeightArrs with  
//
//Pre:	A WeightArrs object
//Post: None
//
void fillArrs(WeightArrs &player)
{
	for(int ii = 0; ii < 40; ii++)
	{
		for(int jj = 0; jj < 32; jj++)
		{
			player.arr1[ii][jj] = static_cast<float>((rand()%5)-0.4);
		}
	}
	for(int ii = 0; ii < 10; ii++)
	{
		for(int jj = 0; jj < 40; jj++)
		{
			player.arr2[ii][jj] = static_cast<float>((rand()%5)-0.4);
		}
	}
	for(int ii = 0; ii < 1; ii++)
	{
		for(int jj = 0; jj < 10; jj++)
		{
			player.arr3[ii][jj] = static_cast<float>((rand()%5)-0.4);
		}
	}
}

