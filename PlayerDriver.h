#include "MoveResultArray.h"
#include <vector>
using std::vector;
#include <string>
using std::string;
using std::to_string;
#include <iostream>
using std::cout;
using std::endl;
#include <map>
using std::map;
#include <fstream>
using std::ofstream;

#ifndef PLAYER_H
#define PLAYER_H

struct TreeNode
{
	vector<int> _board;
	float _boardValue;
	string _parentKey;
	vector<string> _childKeys;
};

class Player
{
public:
	std::shared_ptr<MoveResultArray> m_moveResultsArray;
	std::shared_ptr<map<string, TreeNode>>  m_boardTreeMapPtr;
	ofstream m_streamToFile;
	bool foundJumpThisTurn;
	NeuralNetwork NN;
	int whiteSideLoseMultiplier, redSideLoseMultiplier;


	Player()
	{
		/*m_streamToFile.open("Output.txt");
		m_streamToFile << "";
		m_streamToFile.close();
		m_streamToFile.open("Output.txt", std::ios_base::app);
		*/
	}
	~Player()
	{
	}

	int playerSign;

	vector<int> returnBestBoard();
	int returnBestBoardValue();

	void createRedMoveGeneration(string, std::shared_ptr<map<string, TreeNode>>);
	void createRedJumpGeneration(string, std::shared_ptr<map<string, TreeNode>>);
	void createWhiteMoveGeneration(string, std::shared_ptr<map<string, TreeNode>>);
	void createWhiteJumpGeneration(string, std::shared_ptr<map<string, TreeNode>>);
	vector<int> reverseBoard(vector<int>);
	void wonGame();

	vector<vector<int>> redMove(std::shared_ptr<MoveResultArray>, vector<int>, int);
	vector<vector<int>> redJump(std::shared_ptr<MoveResultArray>, vector<int>, int);
	vector<vector<int>> whiteMove(std::shared_ptr<MoveResultArray>, vector<int>, int);
	vector<vector<int>> whiteJump(std::shared_ptr<MoveResultArray>, vector<int>, int);

	void sendValueToAncestors(bool, std::shared_ptr<TreeNode>, std::shared_ptr<map<string, TreeNode>>);
	bool parentIsLastChildOfGrandparent(string, std::shared_ptr<map<string, TreeNode>>);
	string keyOfNextValidNode(string, std::shared_ptr<map<string, TreeNode>>);
	string keyOfLeftmostDeepestLeafFromNode(string, std::shared_ptr<map<string, TreeNode>>);
	string keyOfNextSibling(string, std::shared_ptr<map<string, TreeNode>>);
};

class RedPlayer : public Player
{
public:

	RedPlayer(int NN_FileNumber, std::shared_ptr<MoveResultArray> moveResArrPtr)
	{
		//m_boardTreeMapPtr(new map < string, TreeNode >);
		m_moveResultsArray = moveResArrPtr;
		NN.fillWeightsFromFileNumber(NN_FileNumber);
		whiteSideLoseMultiplier = 1;
		redSideLoseMultiplier = -1;
	}
	~RedPlayer()
	{
		//m_boardTreeMapPtr->clear();
		//delete m_boardTreeMapPtr;
	}

	void beginBoardEvaluation(vector<int> startingBoard)
	{
		//m_boardTreeMapPtr->clear();
		std::shared_ptr<map<string, TreeNode>>  _boardTreeMapPtrTemp(new map<string, TreeNode>);
		m_boardTreeMapPtr = _boardTreeMapPtrTemp;
		
		//TreeNode * topTreeNode;
		std::shared_ptr<TreeNode>  topTreeNode(new TreeNode);
		//topTreeNode = new TreeNode;
		topTreeNode->_board = startingBoard;
		topTreeNode->_boardValue = -11;
		topTreeNode->_parentKey = "";
		(*m_boardTreeMapPtr)["_0"] = *topTreeNode;

		for (int ii = 0; ii < 2; ++ii)
		{
			cout << "Red: figuring out red generation " << ii << " from node " << keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr) << endl;
			foundJumpThisTurn = false;
			//m_streamToFile << "Red's turn" << endl;
			createRedMoveGeneration(keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr), m_boardTreeMapPtr);

			cout << "Red: figuring out white generation " << ii << " from node " << keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr) << endl;
			foundJumpThisTurn = false;
			createWhiteMoveGeneration(keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr), m_boardTreeMapPtr);
		}
	}

	bool lostGame(vector<int> board)
	{
		cout << board.size() << endl;
		for (int ii = 0; ii < board.size(); ++ii)
		{
			if (board[ii] > 0)
			{
				return false;
			}
		}
		return true;
	}
};

class WhitePlayer : public Player
{
public:

	WhitePlayer(int NN_FileNumber, std::shared_ptr<MoveResultArray> moveResArrPtr)
	{
		//m_boardTreeMapPtr = new map < string, TreeNode >;
		m_moveResultsArray = moveResArrPtr;
		NN.fillWeightsFromFileNumber(NN_FileNumber);
		whiteSideLoseMultiplier = -1;
		redSideLoseMultiplier = 1;
	}
	~WhitePlayer()
	{
		//m_boardTreeMapPtr->clear();
		//delete m_boardTreeMapPtr;
	}
	void beginBoardEvaluation(vector<int> startingBoard)
	{		
		//m_boardTreeMapPtr->clear();
		std::shared_ptr<map<string, TreeNode>>  _boardTreeMapPtrTemp(new map<string, TreeNode>);
		m_boardTreeMapPtr = _boardTreeMapPtrTemp;

		//TreeNode * topTreeNode;
		std::shared_ptr<TreeNode>  topTreeNode(new TreeNode);
		//topTreeNode = new TreeNode;
		topTreeNode->_board = startingBoard;
		topTreeNode->_boardValue = -11;
		topTreeNode->_parentKey = "";
		(*m_boardTreeMapPtr)["_0"] = *topTreeNode;

		for (int ii = 0; ii < 2; ++ii)
		{
			//m_streamToFile << "White's turn" << endl;
			cout << "White: figuring out white generation " << ii << " from node " << keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr) << endl;
			foundJumpThisTurn = false;
			createWhiteMoveGeneration(keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr), m_boardTreeMapPtr);
			
			cout << "White: figuring out red generation " << ii << " from node " << keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr) << endl;
			foundJumpThisTurn = false;
			createRedMoveGeneration(keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr), m_boardTreeMapPtr);
		}
	}

	bool lostGame(vector<int> board)
	{
		for (int ii = 0; ii < board.size(); ++ii)
		{
			if (board[ii] < 0)
			{
				return false;
			}
		}
		return true;
	}
};

vector<int> Player::returnBestBoard()
{
	int bestBoardValue = -11;
	vector<int> bestBoard;

	vector<string> childKeysVec = (&(*m_boardTreeMapPtr)["_0"])->_childKeys;
	for (int ii = 0; ii < childKeysVec.size(); ++ii)
	{
		if (bestBoardValue < (*m_boardTreeMapPtr)[childKeysVec[ii]]._boardValue)
		{
			bestBoard = (*m_boardTreeMapPtr)[childKeysVec[ii]]._board;
			bestBoardValue = (*m_boardTreeMapPtr)[childKeysVec[ii]]._boardValue;
		}
	}

	return bestBoard;
}

int Player::returnBestBoardValue()
{
	int bestBoardValue = -11;
	vector<int> bestBoard;
	vector<string> childKeysVec = (&(*m_boardTreeMapPtr)["_0"])->_childKeys;

	for (int ii = 0; ii < childKeysVec.size(); ++ii)
	{
		if (bestBoardValue < (*m_boardTreeMapPtr)[childKeysVec[ii]]._boardValue)
		{
			bestBoard = (*m_boardTreeMapPtr)[childKeysVec[ii]]._board;
			bestBoardValue = (*m_boardTreeMapPtr)[childKeysVec[ii]]._boardValue;
		}
	}

	return bestBoardValue;
}

void Player::createRedMoveGeneration(string startingNodeKey, std::shared_ptr<map<string, TreeNode>> boardTreeMap)
{
	string nodeKey = startingNodeKey;

	while (true)
	{
		vector<int> board = (*boardTreeMap)[nodeKey]._board;
		int childIndex = 0;
		vector<vector<int>> pieceMoveBoards;
		// Find all jumps or moves
		for (int cc = 0; cc < board.size(); ++cc)
		{
			// Find a piece in the current node's board
			if (board[cc] > 0)
			{
				vector<vector<int>> tempVector = redJump(m_moveResultsArray, board, cc);
				pieceMoveBoards.insert(pieceMoveBoards.end(), tempVector.begin(), tempVector.end());
			}
		}
		if (pieceMoveBoards.size() == 0)
		{
			for (int cc = 0; cc < board.size(); ++cc)
			{
				// Find a piece in the current node's board
				if (board[cc] > 0)
				{
					vector<vector<int>> tempVector = redMove(m_moveResultsArray, board, cc);
					pieceMoveBoards.insert(pieceMoveBoards.end(), tempVector.begin(), tempVector.end());
				}
			}
		}		

		for (int jj = 0; jj < pieceMoveBoards.size(); ++jj)
		{
			childIndex = (*boardTreeMap)[nodeKey]._childKeys.size();
			std::shared_ptr<TreeNode> newTreeNode(new TreeNode);
			newTreeNode->_board = pieceMoveBoards[jj];
			// Get the value from the BEF
			newTreeNode->_boardValue = NN.computeScoreForBoard(newTreeNode->_board);
			newTreeNode->_parentKey = nodeKey;
			(*boardTreeMap)[nodeKey + "_r" + to_string(childIndex)] = *newTreeNode;
			(*boardTreeMap)[nodeKey]._childKeys.push_back(nodeKey + "_r" + to_string(childIndex));
			//cout << "childIndex: " << childIndex << ", pieceMoveBoards.size(): " << pieceMoveBoards.size() << endl;
			//m_streamToFile << "Created child with key " + nodeKey + "_r" + to_string(childIndex) << " with value " << newTreeNode->_boardValue << " for " << nodeKey << endl;
			//cout << "Created child with key " + nodeKey + "_r" + to_string(childIndex) << " with value of " << newTreeNode->_boardValue << endl;
			// Give value to ancestors, stopping at the first node that has a higher value already
			sendValueToAncestors(true, newTreeNode, boardTreeMap);
		}

		// Check if the current non-root node is the last child node of the parent
		if (nodeKey != "_0")
		{
			// If yes, return
			if (parentIsLastChildOfGrandparent(nodeKey, boardTreeMap))
			{
				//cout << nodeKey << " is last node of parent" << endl;
				nodeKey = keyOfNextValidNode(nodeKey, boardTreeMap);

				// If the nodeKey was the last possible leaf of this generation
				if (nodeKey == "")
				{
					return;
				}
			}
			else// If no, go to the next sibling node
			{
				//cout << "Finding the sibling key for " << nodeKey << endl;
				nodeKey = keyOfNextSibling(nodeKey, boardTreeMap);
				//cout << "Changed to sibling key: " << nodeKey << endl;
			}
		}
		else
		{
			return;
		}
		//cout << "Next node is " << nodeKey << endl;
	}
}

void Player::createWhiteMoveGeneration(string startingNodeKey, std::shared_ptr<map<string, TreeNode>> boardTreeMap)
{
	string nodeKey = startingNodeKey;

	while (true)
	{
		vector<int> board = (*boardTreeMap)[nodeKey]._board;

		int childIndex = 0;
		vector<vector<int>> pieceMoveBoards;
		// Find all jumps or moves
		for (int cc = 0; cc < board.size(); ++cc)
		{
			// Find a piece in the current node's board
			if (board[cc] < 0)
			{
				vector<vector<int>> tempVector = whiteJump(m_moveResultsArray, board, cc);
				pieceMoveBoards.insert(pieceMoveBoards.end(), tempVector.begin(), tempVector.end());
			}
		}		
		if (pieceMoveBoards.size() == 0)
		{
			for (int cc = 0; cc < board.size(); ++cc)
			{
				// Find a piece in the current node's board
				if (board[cc] < 0)
				{
					vector<vector<int>> tempVector = whiteMove(m_moveResultsArray, board, cc);
					pieceMoveBoards.insert(pieceMoveBoards.end(), tempVector.begin(), tempVector.end());
				}
			}
		}
		// Push these boards on the child node vector of the parent
		if (pieceMoveBoards.size() == 0)
		{
			(*boardTreeMap)[nodeKey]._boardValue = 11 * whiteSideLoseMultiplier;
		}
		else
		{
			for (int jj = 0; jj < pieceMoveBoards.size(); ++jj)
			{
				childIndex = (*boardTreeMap)[nodeKey]._childKeys.size();
				std::shared_ptr<TreeNode> newTreeNode(new TreeNode);
				newTreeNode->_board = pieceMoveBoards[jj];
				// Get the value from the BEF
				newTreeNode->_boardValue = NN.computeScoreForBoard(reverseBoard(newTreeNode->_board)); // BEFORE SENDING THE BOARD, REVERSE IT
				newTreeNode->_parentKey = nodeKey;
				(*boardTreeMap)[nodeKey + "_w" + to_string(childIndex)] = *newTreeNode;
				(*boardTreeMap)[nodeKey]._childKeys.push_back(nodeKey + "_w" + to_string(childIndex));
				//cout << "childIndex: " << childIndex << ", pieceMoveBoards.size(): " << pieceMoveBoards.size() << endl;
				//m_streamToFile << "Created child with key " + nodeKey + "_w" + to_string(childIndex) << " with value " << newTreeNode->_boardValue << " for " << nodeKey << endl;
				//cout << "Created child with key " + nodeKey + "_w" + to_string(childIndex) << " for " << nodeKey << endl;
				// Give value to ancestors, stopping at the first node that has a higher value already
				sendValueToAncestors(true, newTreeNode, boardTreeMap);
			}
		}

		// Check if the current non-root node is the last child node of the parent
		if (nodeKey != "_0")
		{
			//cout << "key is " + nodeKey << ", children amount is " << (*boardTreeMap)[nodeKey]._childKeys.size() << endl;
			// If yes, return
			if (parentIsLastChildOfGrandparent(nodeKey, boardTreeMap))
			{
				//cout << "parentIsLastChildOfGrandparent " + nodeKey << endl;
				//cout << nodeKey << " is last node of parent" << endl;
				nodeKey = keyOfNextValidNode(nodeKey, boardTreeMap);

				// If the nodeKey was the last possible leaf of this generation
				if (nodeKey == "")
				{
					return;
				}
			}
			else// If no, go to the next sibling node
			{
				//cout << "Finding the sibling key for " << nodeKey << endl;
				nodeKey = keyOfNextSibling(nodeKey, boardTreeMap);
				//cout << "Changed to sibling key: " << nodeKey << endl;
			}
		}
		else
		{
			return;
		}

		//cout << "Next node is " << nodeKey << endl;
	}
}

// Reverses the sign of all units on the 
vector<int> Player::reverseBoard(vector<int> board)
{
	for (int ii = 0; ii < board.size(); ++ii)
	{
		board[ii] *= -1;
	}

	return board;
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
vector<vector<int>> Player::redMove(std::shared_ptr<MoveResultArray> _moveResultArray, vector<int> board, int index)
{
	vector<vector<int>> returnBoards;
	vector<int> boardCopy;

	int tempInt = _moveResultArray->movesDown[index].left;
	boardCopy = board;
	if (tempInt != -1 && board[tempInt] == 0)
	{
		boardCopy[tempInt] = boardCopy[index];
		boardCopy[index] = 0;

		if (boardCopy[tempInt] == 1 && tempInt > 27)
		{
			boardCopy[tempInt] = 2;
		}
		returnBoards.push_back(boardCopy);
		boardCopy = board;
	}
	tempInt = _moveResultArray->movesDown[index].right;
	if (tempInt != -1 && board[tempInt] == 0)
	{
		boardCopy[tempInt] = boardCopy[index];
		boardCopy[index] = 0;

		if (boardCopy[tempInt] == 1 && tempInt > 27)
		{
			boardCopy[tempInt] = 2;
		}
		returnBoards.push_back(boardCopy);
		boardCopy = board;
	}
	if (board[index] > 1)
	{
		tempInt = _moveResultArray->movesUp[index].left;
		if (tempInt != -1 && board[tempInt] == 0)
		{
			boardCopy[tempInt] = boardCopy[index];
			boardCopy[index] = 0;

			returnBoards.push_back(boardCopy);
			boardCopy = board;
		}
		tempInt = _moveResultArray->movesUp[index].right;
		if (tempInt != -1 && board[tempInt] == 0)
		{
			boardCopy[tempInt] = boardCopy[index];
			boardCopy[index] = 0;

			returnBoards.push_back(boardCopy);
		}
	}

	return returnBoards;
}

//redJump
//
//Function that will take a piece on the board and test for every possible jump from its current position
//
//Pre:	The current square, a ready board and the current ply
//Post: Tests each direction that the peice can jump,
//		if a move can be made the move is put in the vector of jumps to be evaluated
//
vector<vector<int>> Player::redJump(std::shared_ptr<MoveResultArray> _moveResultArray, vector<int> board, int index)
{

	vector<vector<int>> evaluationBoards, returnBoards;
	vector<int> boardCopy, piecePositions;
	int currPosition;

	evaluationBoards.push_back(board);
	piecePositions.push_back(index);
	int foundJump = 0;


	for (int ii = 0; ii < evaluationBoards.size(); ++ii)
	{
		boardCopy = evaluationBoards[ii];
		currPosition = piecePositions[ii];
		foundJump = 0;

		int tempInt = _moveResultArray->jumpsDown[currPosition].left;
		int enemyPos = _moveResultArray->movesDown[currPosition].left;
		if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] < 0)
		{
			//cout << "jumpsDown[currPosition].left" << endl;
			//cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << " valued at " << board[enemyPos] << endl;
			boardCopy[tempInt] = boardCopy[currPosition];
			boardCopy[enemyPos] = 0;
			boardCopy[currPosition] = 0;

			if (boardCopy[tempInt] == 1 && tempInt > 27)
			{
				boardCopy[tempInt] = 2;
			}
			//boardDisplay(boardCopy);

			foundJump = 1;
			evaluationBoards.push_back(boardCopy);
			piecePositions.push_back(tempInt);
			boardCopy = evaluationBoards[ii];

		}
		tempInt = _moveResultArray->jumpsDown[currPosition].right;
		enemyPos = _moveResultArray->movesDown[currPosition].right;
		if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] < 0)
		{
			//cout << "jumpsDown[currPosition].right" << endl;
			//cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << endl;
			boardCopy[tempInt] = boardCopy[currPosition];
			boardCopy[enemyPos] = 0;
			boardCopy[currPosition] = 0;

			if (boardCopy[tempInt] == 1 && tempInt > 27)
			{
				boardCopy[tempInt] = 2;
			}
			//boardDisplay(boardCopy);

			foundJump = 1;
			evaluationBoards.push_back(boardCopy);
			piecePositions.push_back(tempInt);
			boardCopy = evaluationBoards[ii];
		}
		if (board[index] > 1)
		{
			tempInt = _moveResultArray->jumpsUp[currPosition].left;
			enemyPos = _moveResultArray->movesUp[currPosition].left;
			if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] < 0)
			{
				//cout << "jumpsUp[currPosition].left" << endl;
				//cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << endl;
				boardCopy[tempInt] = boardCopy[currPosition];
				boardCopy[enemyPos] = 0;
				boardCopy[currPosition] = 0;
				//boardDisplay(boardCopy);

				evaluationBoards.push_back(boardCopy);
				piecePositions.push_back(tempInt);
				foundJump = 1;
				boardCopy = evaluationBoards[ii];
			}
			tempInt = _moveResultArray->jumpsUp[currPosition].right;
			enemyPos = _moveResultArray->movesUp[currPosition].right;
			if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] < 0)
			{
				//cout << "jumpsUp[currPosition].right" << endl;
				//cout << "Jumping from " << currPosition << " to " << tempInt << endl;
				boardCopy[tempInt] = boardCopy[currPosition];
				boardCopy[enemyPos] = 0;
				boardCopy[currPosition] = 0;
				//boardDisplay(boardCopy);

				evaluationBoards.push_back(boardCopy);
				piecePositions.push_back(tempInt);
				foundJump = 1;
			}
		}

		if (!foundJump && ii != 0)
		{
			returnBoards.push_back(evaluationBoards[ii]);
		}
		//cout << evaluationBoards.size() << endl;
		//cout << "piecePositions[ii]: " << piecePositions[ii] << endl;
		//boardDisplay(evaluationBoards[ii]);
	}

	return returnBoards;
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
vector<vector<int>> Player::whiteMove(std::shared_ptr<MoveResultArray> _moveResultArray, vector<int> board, int index)
{
	vector<vector<int>> returnBoards;
	vector<int> boardCopy;

	int tempInt = _moveResultArray->movesUp[index].left;
	boardCopy = board;
	if (tempInt != -1 && board[tempInt] == 0)
	{
		boardCopy[tempInt] = boardCopy[index];
		boardCopy[index] = 0;

		if (boardCopy[tempInt] == -1 && tempInt < 4)
		{
			boardCopy[tempInt] = -2;
		}
		returnBoards.push_back(boardCopy);
		boardCopy = board;
	}
	tempInt = _moveResultArray->movesUp[index].right;
	if (tempInt != -1 && board[tempInt] == 0)
	{
		boardCopy[tempInt] = boardCopy[index];
		boardCopy[index] = 0;

		if (boardCopy[tempInt] == -1 && tempInt < 4)
		{
			boardCopy[tempInt] = -2;
		}
		returnBoards.push_back(boardCopy);
		boardCopy = board;
	}
	if (board[index] < -1)
	{
		tempInt = _moveResultArray->movesDown[index].left;
		if (tempInt != -1 && board[tempInt] == 0)
		{
			boardCopy[tempInt] = boardCopy[index];
			boardCopy[index] = 0;

			returnBoards.push_back(boardCopy);
			boardCopy = board;
		}
		tempInt = _moveResultArray->movesDown[index].right;
		if (tempInt != -1 && board[tempInt] == 0)
		{
			boardCopy[tempInt] = boardCopy[index];
			boardCopy[index] = 0;

			returnBoards.push_back(boardCopy);
		}
	}

	return returnBoards;
}

//redJump
//
//Function that will take a piece on the board and test for every possible jump from its current position
//
//Pre:	The current square, a ready board and the current ply
//Post: Tests each direction that the peice can jump,
//		if a move can be made the move is put in the vector of jumps to be evaluated
//
vector<vector<int>> Player::whiteJump(std::shared_ptr<MoveResultArray> _moveResultArray, vector<int> board, int index)
{

	vector<vector<int>> evaluationBoards, returnBoards;
	vector<int> boardCopy, piecePositions;
	int currPosition;

	evaluationBoards.push_back(board);
	piecePositions.push_back(index);
	int foundJump = 0;


	for (int ii = 0; ii < evaluationBoards.size(); ++ii)
	{
		boardCopy = evaluationBoards[ii];
		currPosition = piecePositions[ii];
		foundJump = 0;

		int tempInt = _moveResultArray->jumpsUp[currPosition].left;
		int enemyPos = _moveResultArray->movesUp[currPosition].left;
		if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] > 0)
		{
			//cout << "jumpsup[currPosition].left" << endl;
			//cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << " valued at " << board[enemyPos] << endl;
			boardCopy[tempInt] = boardCopy[currPosition];
			boardCopy[enemyPos] = 0;
			boardCopy[currPosition] = 0;

			if (boardCopy[tempInt] == -1 && tempInt < 4)
			{
				boardCopy[tempInt] = -2;
			}
			//boardDisplay(boardCopy);

			foundJump = 1;
			evaluationBoards.push_back(boardCopy);
			piecePositions.push_back(tempInt);
			boardCopy = evaluationBoards[ii];

		}
		tempInt = _moveResultArray->jumpsUp[currPosition].right;
		enemyPos = _moveResultArray->movesUp[currPosition].right;
		if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] > 0)
		{
			//cout << "jumpsup[currPosition].right" << endl;
			//cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << endl;
			boardCopy[tempInt] = boardCopy[currPosition];
			boardCopy[enemyPos] = 0;
			boardCopy[currPosition] = 0;

			if (boardCopy[tempInt] == -1 && tempInt < 4)
			{
				boardCopy[tempInt] = -2;
			}
			//boardDisplay(boardCopy);

			foundJump = 1;
			evaluationBoards.push_back(boardCopy);
			piecePositions.push_back(tempInt);
			boardCopy = evaluationBoards[ii];
		}
		if (board[index] < -1)
		{
			tempInt = _moveResultArray->jumpsDown[currPosition].left;
			enemyPos = _moveResultArray->movesDown[currPosition].left;
			if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] > 0)
			{
				//cout << "jumpsdown[currPosition].left" << endl;
				//cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << endl;
				boardCopy[tempInt] = boardCopy[currPosition];
				boardCopy[enemyPos] = 0;
				boardCopy[currPosition] = 0;
				//boardDisplay(boardCopy);

				evaluationBoards.push_back(boardCopy);
				piecePositions.push_back(tempInt);
				foundJump = 1;
				boardCopy = evaluationBoards[ii];
			}
			tempInt = _moveResultArray->jumpsDown[currPosition].right;
			enemyPos = _moveResultArray->movesDown[currPosition].right;
			if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] > 0)
			{
				//cout << "jumpsdown[currPosition].right" << endl;
				//cout << "Jumping from " << currPosition << " to " << tempInt << endl;
				boardCopy[tempInt] = boardCopy[currPosition];
				boardCopy[enemyPos] = 0;
				boardCopy[currPosition] = 0;
				//boardDisplay(boardCopy);

				evaluationBoards.push_back(boardCopy);
				piecePositions.push_back(tempInt);
				foundJump = 1;
			}
		}

		if (!foundJump && ii != 0)
		{
			returnBoards.push_back(evaluationBoards[ii]);
		}
		//cout << evaluationBoards.size() << endl;
		//cout << "piecePositions[ii]: " << piecePositions[ii] << endl;
		//boardDisplay(evaluationBoards[ii]);
	}

	return returnBoards;
}

void Player::sendValueToAncestors(bool sendMaxBack, std::shared_ptr<TreeNode> childNode, std::shared_ptr<map<string, TreeNode>> boardTreeMap)
{
	string parentKey;
	std::shared_ptr<TreeNode> currentNode = childNode;
	float boardValue = childNode->_boardValue;
	parentKey = currentNode->_parentKey;

	while (parentKey != "_0")
	{
		// Send value to node above the current TreeNode
		// Change the value if greater than the current node or if this is the first child node
		if ((*boardTreeMap)[parentKey]._childKeys.size() == 1)
		{
			(*boardTreeMap)[parentKey]._boardValue = boardValue;
			sendMaxBack = !sendMaxBack;
		}
		else
		{
			if (sendMaxBack)
			{
				if ((*boardTreeMap)[parentKey]._boardValue < boardValue)
				{
					//cout << "Changed value of " + parentKey + " to " + to_string(boardValue) << endl;
					(*boardTreeMap)[parentKey]._boardValue = boardValue;
					sendMaxBack = !sendMaxBack;
				}
				else
				{
					return;
				}
			}
			else // ((*boardTreeMap)[parentKey]._boardValue < boardValue && !sendMaxBack)
			{
				if ((*boardTreeMap)[parentKey]._boardValue > boardValue)
				{
					//cout << "Changed value of " + parentKey + " to " + to_string(boardValue) << endl;
					(*boardTreeMap)[parentKey]._boardValue = boardValue;
					sendMaxBack = !sendMaxBack;
				}
				else
				{
					return;
				}
			}
		}

		// Get the parent key from the current TreeNode
		parentKey = (*boardTreeMap)[parentKey]._parentKey;
	}
}

bool Player::parentIsLastChildOfGrandparent(string parentKey, std::shared_ptr<map<string, TreeNode>> boardTreeMap)
{
	vector<string> grandParentKeys = (&(*boardTreeMap)[(&(*boardTreeMap)[parentKey])->_parentKey])->_childKeys;
	for (int ii = 0; ii < grandParentKeys.size(); ++ii)
	{
		//cout << "ii is " << ii << ", and parentNode->_parentKey is " << parentNode->_parentKey << endl;
		//cout << "Comparing " << grandparentNode->_childKeys[ii] << " to " << parentKey << endl;
		if (grandParentKeys[ii] == parentKey)
		{
			//cout << "ii is " << ii << endl;
			if (ii == grandParentKeys.size() - 1)
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

string Player::keyOfNextValidNode(string nodeKey, std::shared_ptr<map<string, TreeNode>> boardTreeMap)
{
	if (nodeKey == "_0")
	{
		if (nodeKey == "_0" && (*boardTreeMap)[nodeKey]._childKeys.size() > 0)
		{
			nodeKey = (*boardTreeMap)[nodeKey]._childKeys[0];
		}
		else
		{
			return "_0";
		}
	}

	while (true)
	{
		while (parentIsLastChildOfGrandparent(nodeKey, boardTreeMap))
		{
			//cout << "node is last child of parent" << endl;
			nodeKey = (*boardTreeMap)[nodeKey]._parentKey;

			if (nodeKey == "_0") // Encountered root node, there are no more nodes to be parents of new generation
			{
				//cout << "node is last child of parent and found root" << endl;
				return "";
			}
		}

		nodeKey = keyOfNextSibling(nodeKey, boardTreeMap);
		return keyOfLeftmostDeepestLeafFromNode(nodeKey, boardTreeMap);
	}
}

string Player::keyOfLeftmostDeepestLeafFromNode(string nodeKey, std::shared_ptr<map<string, TreeNode>> boardTreeMap)
{
	//m_streamToFile << "keyOfLeftmostDeepestLeafFromNode from " << nodeKey << endl;
	//cout << "keyOfLeftmostDeepestLeafFromNode from "  << nodeKey << endl;
	//cout << (*boardTreeMap)["_0"]._childKeys.size() << endl;
	if (nodeKey == "_0")
	{
		if ((*boardTreeMap)[nodeKey]._childKeys.size() > 0)
		{
			nodeKey = (*boardTreeMap)[nodeKey]._childKeys[0];
		}
		else
		{
			return "_0";
		}
	}

	while (true)
	{
		if ((*boardTreeMap)[nodeKey]._boardValue == -11 && !parentIsLastChildOfGrandparent(nodeKey, boardTreeMap)) // don't choose a lost board
		{
			//cout << "Found losing board. Going to next sibling. Parent is " << (*boardTreeMap)[nodeKey]._nodeKey << endl;
			nodeKey = keyOfNextSibling(nodeKey, boardTreeMap);
			//cout << "Next sibling is " << nodeKey << endl;
		}
		else if ((*boardTreeMap)[nodeKey]._childKeys.size() == 0) // Return a child node with no children and a value that isn't -11
		{
			//cout << "found viable node" << endl;
			return nodeKey;
		}
		else
		{
			//cout << "Moving down the tree" << endl;
			nodeKey = (*boardTreeMap)[nodeKey]._childKeys[0];
		}
	}
}

string Player::keyOfNextSibling(string parentKey, std::shared_ptr<map<string, TreeNode>> boardTreeMap)
{
	//m_streamToFile << "keyOfNextSibling " << parentKey << endl;
	//cout << "keyOfNextSibling " << parentKey << endl;
	vector<string> grandParentKeys = (&(*boardTreeMap)[(&(*boardTreeMap)[parentKey])->_parentKey])->_childKeys;
	for (int ii = 0; ii < grandParentKeys.size(); ++ii)
	{
		if (grandParentKeys[ii] == parentKey && ii != grandParentKeys.size() - 1)
		{
			return grandParentKeys[ii + 1];
		}
	}
	cout << "Making a blank return from keyOfNextSibling" << endl;
	return "";
}

#endif