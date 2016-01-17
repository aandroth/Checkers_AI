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
float ourMoveRecurse(int square, float CUR_MIN, int CUR_PLY, int board[]);


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

		vector<int> pieceMoves;
		pieceMoves = pieceArray[0]->evaluateMoves();
		for (int ii = 0; ii < pieceMoves.size(); ++ii)
		{
			tempBoard = board;
			if (pieceMoves[ii] != -1 && tempBoard[pieceMoves[ii]] == 0)
			{
				tempBoard[pieceArray[0]->m_position] = 0;
				tempBoard[pieceMoves[ii]] = pieceArray[0]->m_value;
				
				if (pieceArray[0]->m_value == 1 && pieceMoves[ii] > 27)
				{
					tempBoard[pieceMoves[ii]] = 2;
					cout << pieceArray[0]->m_value << endl;
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
					pieceMovedIndex = 0;
					movedToPosition = pieceMoves[ii];
				}
				cout << "max is " << currMax << endl;
			}
		}

		board = savedBoards[bestBoardIndex]; 
		pieceArray[pieceMovedIndex]->setPosition(movedToPosition);

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


//ourMoveDriver
//
//Function looks through the board for our pieces and tests each jump and then move they can do
//
//Pre:	A ready board and the current ply
//Post: Looks through the board for our pieces and sees if any of them can do a jump,
//		if no jumps can be done the driver looks through the board for our pieces and see if they can do a move
//
float ourMoveDriver(int CUR_PLY, float CUR_MIN, int * board)
{
	if(CUR_PLY == MAX_PLY)
	{
		if(turn%2 != 0)
		{	
			return NN_R.computeBoardScore(board);
		}
		else
		{	
			return NN_W.computeBoardScore(board);
		}
	}

	float CUR_MAX = -900.0;
	float TEMP_MAX;

	//search through the board for our pieces
	for(int ii = 0; ii < 32; ++ii)
	{
		//check if the square holds one of our pieces
		if(board[ii] > 0)
		{
			//check if the piece can do a jump
			TEMP_MAX = ourJumpRecurse(ii, CUR_MIN, CUR_PLY, board);

			//check the results for a new max
			if(TEMP_MAX > CUR_MAX)
			{
				CUR_MAX = TEMP_MAX;
			}
		}
	}

	//if no jumps were done
	if(CUR_MAX == -900.0)
	{
		//look through the board for our pieces
		for(int ii = 0; ii < 32; ++ii)
		{

			//check if the sqaure holds one of our pieces
			if(board[ii] > 0)
			{
				//check if the piece can do a move
				//TEMP_MAX = ourMoveRecurse(ii, CUR_MIN, CUR_PLY, board);

				//check the results for a new max
				if(TEMP_MAX > CUR_MAX)
				{
					CUR_MAX = TEMP_MAX;
				}
			}
		}
	}
	//red has no available moves, meaning either a loss or a draw
	if(CUR_MAX == -900)
	{
		for(int ii = 0; ii < 32; ii++)
		{
			if(board[ii] > 0)
			{return CUR_MAX;}
		}
		//if no red pieces remain on the board than white has won
		return -1000;
	}
	return CUR_MAX;
}


//ourJumpRecurse
//
//Function that will take a piece on the board and continue its jumps by testing for every possible jump from its current position
//
//Pre:	The current square, a ready board and the current ply
//Post: Tests each direction that the peice can jump,
//		if no jumps can be done it returns -900.0 to ourMoveDriver to report no jumps,
//		if a jump can be made the jump function is called
//
float ourJumpRecurse(int ii, float CUR_MIN, int CUR_PLY, int * board)
{
	float CUR_MAX = -900.0;
	float TEMP_MAX = -900.0;
	/*
	//check to see if the square has one of our kings
	if(board[ii] > 1) 
	{
		//Jump down to the left
		//check if the left square we can jump to exists and has an enemy piece 
		if(movesDown[ii].left != -1 && board[movesDown[ii].left] < 0)
		{
			//check to make sure that the square beyond the piece exists and that that square is empty
			if(jumpsDown[ii].left != -1 && board[jumpsDown[ii].left] == 0)
			{
				//fill our variables for the jump
				jumpFrom = ii;
				jumpOver = movesDown[ii].left;
				jumpTo = jumpsDown[ii].left;

				//jump down to the left
				TEMP_MAX = jump(jumpFrom, jumpOver, jumpTo, CUR_PLY, CUR_MAX, board);

				//check if this is the best board of the current ply
				if(TEMP_MAX > CUR_MAX)
				{
					CUR_MAX = TEMP_MAX;
				}
			}
		}

		//check for alpha-beta pruning	
		if(CUR_MAX >= CUR_MIN)
		{
			return CUR_MAX;
		}

		//Jump down to the right
		//check if the right square we can jump to exists and has an enemy piece 
		if(movesDown[ii].right != -1 && board[movesDown[ii].right] < 0)
		{
			//check to make sure that the square beyond the piece exists and that that square is empty
			if(jumpsDown[ii].right != -1 && board[jumpsDown[ii].right] == 0)
			{
				//fill our variables for the jump
				jumpFrom = ii;
				jumpOver = movesDown[ii].right;
				jumpTo = jumpsDown[ii].right;

				//jump down to the right
				TEMP_MAX = jump(jumpFrom, jumpOver, jumpTo, CUR_PLY, CUR_MAX, board);

				//check if this is the best board of the current ply
				if(TEMP_MAX > CUR_MAX)
				{
					CUR_MAX = TEMP_MAX;
				}
			}
		}
	}

	//check for alpha-beta pruning	
	if(CUR_MAX >= CUR_MIN)
	{
		return CUR_MAX;
	}

	//Jump up to the left
	//check if the left square we can jump to exists and has an enemy piece 
	if(movesUp[ii].left != -1 && board[movesUp[ii].left] < 0)
	{
		//check to make sure that the square beyond the piece exists and that that square is empty
		if(jumpsUp[ii].left != -1 && board[jumpsUp[ii].left] == 0)
		{

			//fill our variables for the jump
			jumpFrom = ii;
			jumpOver = movesUp[ii].left;
			jumpTo = jumpsUp[ii].left;

			//jump up to the left
			TEMP_MAX = jump(jumpFrom, jumpOver, jumpTo, CUR_PLY, CUR_MAX, board);

			//check if this is the best board of the current ply
			if(TEMP_MAX > CUR_MAX)
			{
				CUR_MAX = TEMP_MAX;
			}
		}
	}

	//check for alpha-beta pruning	
	if(CUR_MAX >= CUR_MIN)
	{
		return CUR_MAX;
	}

	//Jump up to the right
	//check if the right square we can jump to exists and has an enemy piece 
	if(movesUp[ii].right != -1 && board[movesUp[ii].right] < 0)
	{
		//check to make sure that the square beyond the piece exists and that that square is empty
		if(jumpsUp[ii].right != -1 && board[jumpsUp[ii].right] == 0)
		{
			//fill our variables for the jump
			jumpFrom = ii;
			jumpOver = movesUp[ii].right;
			jumpTo = jumpsUp[ii].right;

			//jump up to the left
			TEMP_MAX = jump(jumpFrom, jumpOver, jumpTo, CUR_PLY, CUR_MAX, board);

			//check if this is the best board of the current ply
			if(TEMP_MAX > CUR_MAX)
			{
				CUR_MAX = TEMP_MAX;
			}
		}
	}
	return CUR_MAX;
}


//ourMoveRecurse
//
//Function that will take a piece on the board and test for every possible move from its current position
//
//Pre:	The current square, a ready board and the current ply
//Post: Tests each direction that the peice can move,
//		if no moves can be done it returns -900.0 to ourMoveDriver to report no moves,
//		if a move can be made the move function is called
//
float ourMoveRecurse(int ii, float CUR_MIN, int CUR_PLY, int * board)
{
	float CUR_MAX = -900.0;
	float TEMP_MAX;

	if(board[ii] > 1)
	{

		//Move down to the left
		//Check that the square on the left exists and is empty
		if(movesDown[ii].left != -1 && board[movesDown[ii].left] == 0)
		{
			moveFrom = ii; 
			moveTo = movesDown[ii].left;

			//call the move function to change the board
			TEMP_MAX = move(moveFrom, moveTo, CUR_PLY, CUR_MAX, board);

			if(TEMP_MAX > CUR_MAX)
			{
				CUR_MAX = TEMP_MAX;
			}
		}

		//check for alpha-beta pruning	
		if(CUR_MAX >= CUR_MIN)
		{
			return CUR_MAX;
		}

		//Move down to the right
		//check that the square to the right exists and is empty
		if(movesDown[ii].right != -1 && board[movesDown[ii].right] == 0)
		{
			moveFrom = ii; 
			moveTo = movesDown[ii].right;

			//call the move function to change the board
			TEMP_MAX = move(moveFrom, moveTo, CUR_PLY, CUR_MAX, board);

			if(TEMP_MAX > CUR_MAX)
			{
				CUR_MAX = TEMP_MAX;
			}
		}
	}
	//check for alpha-beta pruning	
	if(CUR_MAX >= CUR_MIN)
	{
		return CUR_MAX;
	}

	//Move up to the left
	//check that the square on the left exists and is empty
	if(movesUp[ii].left != -1 && board[movesUp[ii].left] == 0)
	{
		moveFrom = ii; 
		moveTo = movesUp[ii].left;

		//call the move function to change the board
		TEMP_MAX = move(moveFrom, moveTo, CUR_PLY, CUR_MAX, board);

		if(TEMP_MAX > CUR_MAX)
		{
			CUR_MAX = TEMP_MAX;
		}
	}

	//check for alpha-beta pruning	
	if(CUR_MAX >= CUR_MIN)
	{
		return CUR_MAX;
	}

	//Move up to the right
	//check that the square to the right exists and is empty
	if(movesUp[ii].right != -1 && board[movesUp[ii].right] == 0)
	{
		moveFrom = ii; 
		moveTo = movesUp[ii].right;

		//call the move function to change the board
		TEMP_MAX = move(moveFrom, moveTo, CUR_PLY, CUR_MAX, board);

		if(TEMP_MAX > CUR_MAX)
		{
			CUR_MAX = TEMP_MAX;
		}
	}*/
	return CUR_MAX;
}


//jump
//
//Function changes the board for a piece that does a jump
//
//Pre:	The square where the piece starts, the square the piece is jumping over, the square the piece is moving to,
//		a ready board, and the current ply
//Post: Copies the board and changes the copy, which is then saved to tempBoard if this is the top ply, 
//		sends the board to the other team's move driver
//
float jump(int jumpFrom, int jumpOver, int jumpTo, int CUR_PLY, float CUR_VAL, int board[])
{
	float RET_VAL;
	int arr[32];

	//copy the values from the board to the board copy
	for(int ii = 0; ii<32; ii++)
	{
		arr[ii] = board[ii];
	}

	//move the piece to its new square on the arr
	arr[jumpTo] = arr[jumpFrom];

	//empty the old square and the enemy that was jumped
	arr[jumpFrom] = 0;
	arr[jumpOver] = 0;
//
//////////////////////////////////////testing code
//	cout<<"JUMPING ";
//	squareDisplay(jumpTo, arr);
//	cout<<", which has a value of "<<arr[jumpTo]<<",";
//	cout<<" from " <<jumpFrom<<" to "<<jumpTo<<" on PLY "<<CUR_PLY<<"!\n";
//	boardDisplay(arr);
//////////////////////////////////////testing code

	//call our recursive jump function if a red piece just did a jump
	if(arr[jumpTo] > 0)
	{		
		RET_VAL = ourJumpRecurse(jumpTo, CUR_VAL, CUR_PLY, arr);
	}
	//call the enemy recursive jump function if a white piece just did a jump
	else
	{		
		RET_VAL = enemyJumpRecurse(jumpTo, CUR_VAL, CUR_PLY, arr);
	}

	//if this was our  move and we can't do any more jumps
	if(RET_VAL == -900.0)
	{
		//test the piece that jumped last to see if it will become a king
		if(jumpTo < 4 && arr[jumpTo] < 2)
		{arr[jumpTo] = 2;}

		//save the board if this is the top ply
		if(CUR_PLY == 1)
		{
			for(int ii = 0; ii < 32;ii++)
				tempBoard[ii] = arr[ii];
		}

		RET_VAL = enemyMoveDriver(CUR_PLY+1, CUR_VAL, arr);
	}

	//if this was the enemy move and they can't do any more jumps
	else if(RET_VAL == 900.0)
	{
		//test the piece that jumped last to see if it will become a king
		if(jumpTo > 27 && arr[jumpTo] > -2)
		{arr[jumpTo] = -2;}

		RET_VAL = ourMoveDriver(CUR_PLY+1, CUR_VAL, arr);
	}

	return RET_VAL;
}

//move
//
//Function changes the board for a piece that moves
//
//Pre:	The square where the piece starts, the square the piece is moving to, a ready board and the current ply
//Post: Copies the board and changes the copy, which is then saved to tempBoard if this is the top ply, 
//		sends the board to the other team's move driver
//
float move(int moveFrom, int moveTo, int CUR_PLY, float CUR_VAL, int board[])
{
	float RET_VAL;
	int arr[32];

	//copy the values from the board to the board copy
	for(int ii = 0; ii<32; ii++)
	{
		arr[ii] = board[ii];
	}

	//move the piece to its new square on the board
	arr[moveTo] = arr[moveFrom];

	//empty the old square
	arr[moveFrom] = 0;
	
//////////////////////////////////////testing code
//	cout<<"MOVING ";
//	squareDisplay(0, moveTo, arr);
//	cout<<", which has a value of "<<arr[moveTo]<<",";
//	cout<<" from " <<moveFrom<<" to "<<moveTo<<" on PLY "<<CUR_PLY<<"!\n";
//	boardDisplay(0, arr);
////////////////////////////////////testing code

	//This if and else statement check if the current ply is odd (our move) or even (enemy move)
 	//If it is currently our move, call the enemy move driver
	if(CUR_PLY%2 != 0)
	{
		//test the piece that just moved to see if it will become a king
		if(moveTo < 4 && arr[moveTo] < 2)
		{arr[moveTo] = 2;}

		//call the enemy driver function
		RET_VAL = enemyMoveDriver(CUR_PLY+1, CUR_VAL, arr);
	}
	//If it is currently the enemy's move, call our move driver
	else
	{
		//test the piece that just moved to see if it will become a king
		if(moveTo > 27 && arr[moveTo] > -2)
		{arr[moveTo] = -2;}

		//call our driver function
		RET_VAL = ourMoveDriver(CUR_PLY+1, CUR_VAL, arr);
	}

	//if this is the top ply we may use this baord, so we save it
	if(CUR_PLY == 1)
	{
		for(int ii = 0; ii < 32; ii++)
			tempBoard[ii] = arr[ii];
	}
	return RET_VAL;
}

/////////////////////////////////////////
/////////Enemy Evaluation Move Functions
//////////////////////////////////////////
//////////////////////////////////////////


//enemyMoveDriver
//
//Function looks for through the board for our pieces and tests each jump and then move they can do
//
//Pre:	A ready board and the current ply
//Post: Looks through the board for enemy pieces and sees if any of them can do a jump,
//		if no jumps can be done the driver looks throught the board for enemy pieces and see if they can do a move
//
float enemyMoveDriver(int CUR_PLY, float CUR_MAX, int * board)
{
	float CUR_MIN = 900.0;
	float TEMP_MIN;

	//check the board for enemy pieces
	for(int ii = 0; ii < 32; ++ii)
	{
		//check if the square has an enemy piece
		if(board[ii] < 0)
		{
			//test if the piece can do a jump
			TEMP_MIN = enemyJumpRecurse(ii, CUR_MAX, CUR_PLY, board);
			if(TEMP_MIN < CUR_MIN)
			{
				CUR_MIN = TEMP_MIN;
			}
		}
	}

	//if no jump can be done, test if a piece can do a move
	if(CUR_MIN == 900.0)
	{
		//check the board for enemy pieces
		for(int ii = 0; ii < 32; ++ii)
		{
			//check if the square has an enemy piece
			if(board[ii] < 0)
			{
				//test if the piece can do a move
				TEMP_MIN = enemyMoveRecurse(ii, CUR_MAX, CUR_PLY, board);

				if(TEMP_MIN < CUR_MIN)
				{
					CUR_MIN = TEMP_MIN;
				}
			}
		}
	}

	//white has no available moves, meaning either a loss or a draw
	if(CUR_MIN == 900)
	{
		//check for any remaining white pieces
		for(int ii = 0; ii < 32; ii++)
		{
			if(board[ii] < 0)
			{return CUR_MIN;}
		}
		//if no white pieces remain on the board than red has won
		return 1000;
	}
	return CUR_MIN;
}


//enemyJumpRecurse
//
//Function that will take a piece on the board and continue its jumps by testing for every possible jump from its current position
//
//Pre:	The current square, a ready board and the current ply
//Post: Tests each direction that the peice can jump,
//		if no jumps can be done it returns 900.0 to ourMoveDriver to report no jumps,
//		if a jump can be made the jump function is called
//
float enemyJumpRecurse(int ii, float CUR_MAX, int CUR_PLY, int * board)
{
	float CUR_MIN = 900.0;
	float TEMP_MIN;
	/*
	//check to see if the square has one of enemy kings
	if(board[ii] < -1) 
	{
		//Jump up to the left
		//check if the left square we can jump to exists and has an enemy piece 
		if(movesUp[ii].left != -1 && board[movesUp[ii].left] < 0)
		{
			//check to make sure that the square beyond the piece exists and that that square is empty
			if(jumpsUp[ii].left != -1 && board[jumpsUp[ii].left] == 0)
			{
				//fill enemy variables for the jump
				jumpFrom = ii;
				jumpOver = movesUp[ii].left;
				jumpTo = jumpsUp[ii].left;

				//jump up to the left
				TEMP_MIN = jump(jumpFrom, jumpOver, jumpTo, CUR_PLY, CUR_MIN, board);

				//check if this is the best board of the current ply
				if(TEMP_MIN < CUR_MIN)
				{
					CUR_MIN = TEMP_MIN;
				}
			}
		}

		//check for alpha-beta pruning	
		if(CUR_MAX >= CUR_MIN)
		{
			return CUR_MIN;
		}

		//Jump up to the right
		//check if the right square we can jump to exists and has an enemy piece 
		if(movesUp[ii].right != -1 && board[movesUp[ii].right] < 0)
		{
			//check to make sure that the square beyond the piece exists and that that square is empty
			if(jumpsUp[ii].right != -1 && board[jumpsUp[ii].left] == 0)
			{

				//fill enemy variables for the jump
				jumpFrom = ii;
				jumpOver = movesUp[ii].right;
				jumpTo = jumpsUp[ii].right;

				//jump up to the left
				TEMP_MIN = jump(jumpFrom, jumpOver, jumpTo, CUR_PLY, CUR_MIN, board);

				//check if this is the best board of the current ply
				if(TEMP_MIN < CUR_MIN)
				{
					CUR_MIN = TEMP_MIN;
				}
			}
		}
	}

	//check for alpha-beta pruning	
	if(CUR_MAX >= CUR_MIN)
	{
		return CUR_MIN;
	}

		//Jump down to the left
		//check if the left square we can jump to exists and has an enemy piece 
		if(movesDown[ii].left != -1 && board[movesDown[ii].left] < 0)
		{
			//check to make sure that the square beyond the piece exists and that that square is empty
			if(jumpsDown[ii].left != -1 && board[jumpsDown[ii].left] == 0)
			{

				//fill enemy variables for the jump
				jumpFrom = ii;
				jumpOver = movesDown[ii].left;
				jumpTo = jumpsDown[ii].left;

				//jump down to the left
				TEMP_MIN = jump(jumpFrom, jumpOver, jumpTo, CUR_PLY, CUR_MIN, board);
				//check if this is the best board of the current ply
				if(TEMP_MIN < CUR_MIN)
				{
					CUR_MIN = TEMP_MIN;
				}
			}
		}

		//check for alpha-beta pruning	
		if(CUR_MAX >= CUR_MIN)
		{
			return CUR_MIN;
		}

		//Jump down to the right
		//check if the right square we can jump to exists and has an enemy piece 
		if(movesDown[ii].right != -1 && board[movesDown[ii].right] < 0)
		{
			//check to make sure that the square beyond the piece exists and that that square is empty
			if(jumpsDown[ii].right != -1 && board[jumpsDown[ii].right] == 0)
			{

			//fill enemy variables for the jump
			jumpFrom = ii;
			jumpOver = movesDown[ii].right;
			jumpTo = jumpsDown[ii].right;

			//jump down to the right
			TEMP_MIN = jump(jumpFrom, jumpOver, jumpTo, CUR_PLY, CUR_MIN, board);

			//check if this is the best board of the current ply
			if(TEMP_MIN < CUR_MIN)
			{
				CUR_MIN = TEMP_MIN;
			}
		}
	}*/
	return CUR_MIN;
}

//enemyMoveRecurse
//
//Function that will take a piece on the board and test for every possible move from its current position
//
//Pre:	The current square, a ready board and the current ply
//Post: Tests each direction that the peice can move,
//		if no moves can be done it returns 900.0 to ourMoveDriver to report no moves,
//		if a move can be made the move function is called
//
float enemyMoveRecurse(int ii, float CUR_MAX, int CUR_PLY, int * board)
{
	float CUR_MIN = 900.0;
	float TEMP_MIN;
	/*
	if(board[ii] < -1)
	{
		//Move up to the left
		//check that the square on the left exists and is empty
		if(movesUp[ii].left != -1 && board[movesUp[ii].left] == 0)
		{
			moveFrom = ii; 
			moveTo = movesUp[ii].left;

			//call the move function to change the board
			TEMP_MIN = move(moveFrom, moveTo, CUR_PLY, CUR_MIN, board);

			if(TEMP_MIN < CUR_MIN)
			{
				CUR_MIN = TEMP_MIN;
			}
		}
			
		//check for alpha-beta pruning	
		if(CUR_MAX >= CUR_MIN)
		{
			return CUR_MIN;
		}

		//Move up to the right
		//check that the square to the right exists and is empty
		if(movesUp[ii].right != -1 && board[movesUp[ii].right] == 0)
		{
			moveFrom = ii; 
			moveTo = movesUp[ii].right;

			//call the move function to change the board
			TEMP_MIN = move(moveFrom, moveTo, CUR_PLY, CUR_MIN, board);

			if(TEMP_MIN < CUR_MIN)
			{
				CUR_MIN = TEMP_MIN;
			}
		}
	}

	//check for alpha-beta pruning	
	if(CUR_MAX >= CUR_MIN)
	{
		return CUR_MIN;
	}

	//Move down to the left
	//Check that the square on the left exists and is empty
	if(movesDown[ii].left != -1 && board[movesDown[ii].left] == 0)
	{
		moveFrom = ii; 
		moveTo = movesDown[ii].left;

		//call the move function to change the board
		TEMP_MIN = move(moveFrom, moveTo, CUR_PLY, CUR_MIN, board);

		if(TEMP_MIN < CUR_MIN)
		{
			CUR_MIN = TEMP_MIN;
		}
	}

	//check for alpha-beta pruning	
	if(CUR_MAX >= CUR_MIN)
	{
		return CUR_MIN;
	}

	//Move down to the right
	//check that the square to the right exists and is empty
	if(movesDown[ii].right != -1 && board[movesDown[ii].right] == 0)
	{
		moveFrom = ii; 
		moveTo = movesDown[ii].right;

		//call the move function to change the board
		TEMP_MIN = move(moveFrom, moveTo, CUR_PLY, CUR_MIN, board);

		if(TEMP_MIN < CUR_MIN)
		{
			CUR_MIN = TEMP_MIN;
		}
	}*/
	return CUR_MIN;
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

