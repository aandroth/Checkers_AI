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

#ifndef PLAYER_H
#define PLAYER_H

struct TreeNode
{
	vector<int> _board;
	int _boardValue;
	string _parentKey;
	vector<string> _childKeys;
};

class Player
{
	MoveResultArray * m_moveResultsArray;
	map<string, TreeNode> * m_boardTreeMapPtr;

public:

	Player(){ m_boardTreeMapPtr = new map < string, TreeNode > ; m_moveResultsArray = new MoveResultArray; m_moveResultsArray->assignTables(); }

	int playerSign;

	void beginBoardEvaluation(vector<int>);
	vector<int> returnBestBoard();

	void createRedMoveGeneration(string, map<string, TreeNode> *);
	void createWhiteMoveGeneration(string, map<string, TreeNode> *);

	vector<vector<int>> redMove(MoveResultArray *, vector<int>, int);
	vector<vector<int>> redJump(MoveResultArray *, vector<int>, int);
	vector<vector<int>> whiteMove(MoveResultArray *, vector<int>, int);
	vector<vector<int>> whiteJump(MoveResultArray *, vector<int>, int);

	void sendValueToAncestors(TreeNode *, map<string, TreeNode> *);
	bool parentIsLastChildOfGrandparent(string, map<string, TreeNode> *);
	string keyOfNextValidNode(string, map<string, TreeNode> *);
	string keyOfLeftmostDeepestLeafFromNode(string, map<string, TreeNode> *);
	string keyOfNextSibling(string, map<string, TreeNode> *);
};

void Player::beginBoardEvaluation(vector<int> startingBoard)
{
	TreeNode * topTreeNode;
	topTreeNode = new TreeNode;
	topTreeNode->_board = startingBoard;
	topTreeNode->_boardValue = -11;
	topTreeNode->_parentKey = "";
	(*m_boardTreeMapPtr)["_0"] = *topTreeNode;

	for (int ii = 0; ii < 5; ++ii)
	{
		createRedMoveGeneration(keyOfLeftmostDeepestLeafFromNode("_0", m_boardTreeMapPtr), m_boardTreeMapPtr);
	}

	returnBestBoard();
}

vector<int> Player::returnBestBoard()
{
	int bestBoardValue = -11;
	vector<int> bestBoard;
	TreeNode * topTreeNodePtr = &(*m_boardTreeMapPtr)["_0"];

	for (int ii = 0; ii < topTreeNodePtr->_childKeys.size(); ++ii)
	{
		if (bestBoardValue < (*m_boardTreeMapPtr)[topTreeNodePtr->_childKeys[ii]]._boardValue)
		{
			bestBoard = (*m_boardTreeMapPtr)[topTreeNodePtr->_childKeys[ii]]._board;
			bestBoardValue = (*m_boardTreeMapPtr)[topTreeNodePtr->_childKeys[ii]]._boardValue;
		}
	}

	return bestBoard;
}

void Player::createRedMoveGeneration(string startingNodeKey, map<string, TreeNode> * boardTreeMap)
{
	string nodeKey = startingNodeKey;

	while (true)
	{
		vector<int> board = (*boardTreeMap)[nodeKey]._board;
		int childIndex = 0;
		for (int cc = 0; cc < 32; ++cc)
		{
			// Find a piece in the current node's board
			if (board[cc] > 0)
			{
				// Find all jumps and moves that piece can do
				vector<vector<int>> pieceMoveBoards;
				pieceMoveBoards = redMove(m_moveResultsArray, board, cc);
				vector<vector<int>> pieceJumpBoards = redJump(m_moveResultsArray, board, cc);
				pieceMoveBoards.insert(pieceMoveBoards.end(), pieceJumpBoards.begin(), pieceJumpBoards.end());
				// Push these boards on the child node vector of the parent
				if (pieceMoveBoards.size() == 0)
				{
					(*boardTreeMap)[nodeKey]._boardValue = -11;
				}
				else
				{
					for (int jj = 0; jj < pieceMoveBoards.size(); ++jj)
					{
						TreeNode * newTreeNode = new TreeNode;
						newTreeNode->_board = pieceMoveBoards[jj];
						// Get the value from the BEF
						newTreeNode->_boardValue = std::rand() % 20 - 10;
						newTreeNode->_parentKey = nodeKey;
						(*boardTreeMap)[nodeKey + "_" + to_string(childIndex)] = *newTreeNode;
						(*boardTreeMap)[nodeKey]._childKeys.push_back(nodeKey + "_" + to_string(childIndex));
						//cout << "childIndex: " << childIndex << ", pieceMoveBoards.size(): " << pieceMoveBoards.size() << endl;
						cout << "Created child with key " + nodeKey + "_" + to_string(childIndex) << " for " << nodeKey << endl;
						++childIndex;
						// Give value to ancestors, stopping at the first node that has a higher value already
						sendValueToAncestors(newTreeNode, boardTreeMap);
					}
				}
			}
		}

		// Check if the current non-root node is the last child node of the parent
		if (nodeKey != "_0")
		{
			// If yes, return
			if (parentIsLastChildOfGrandparent(nodeKey, boardTreeMap))
			{
				cout << nodeKey << " is last node of parent" << endl;
				nodeKey = keyOfNextValidNode(nodeKey, boardTreeMap);

				// If the nodeKey was the last possible leaf of this generation
				if (nodeKey == "")
				{
					return;
				}
			}
			else// If no, go to the next sibling node
			{
				cout << "Finding the sibling key for " << nodeKey << endl;
				nodeKey = keyOfNextSibling(nodeKey, boardTreeMap);
				//cout << "Changed to sibling key: " << nodeKey << endl;
			}
		}
		else
		{
			return;
		}


		cout << "Next node is " << nodeKey << endl;
	}
}

void Player::createWhiteMoveGeneration(string startingNodeKey, map<string, TreeNode> * boardTreeMap)
{
	string nodeKey = startingNodeKey;
	vector<int> board = (*boardTreeMap)[nodeKey]._board;
	int childIndex = 0;
	for (int cc = 0; cc < 32; ++cc)
	{
		// Find a piece in the current node's board
		if (board[cc] > 0)
		{
			// Find all jumps and moves that piece can do
			vector<vector<int>> pieceMoveBoards;
			pieceMoveBoards = whiteMove(m_moveResultsArray, board, cc);
			vector<vector<int>> pieceJumpBoards = whiteJump(m_moveResultsArray, board, cc);
			pieceMoveBoards.insert(pieceMoveBoards.end(), pieceJumpBoards.begin(), pieceJumpBoards.end());
			// Push these boards on the child node vector of the parent
			if (pieceMoveBoards.size() == 0)
			{
				(*boardTreeMap)[nodeKey]._boardValue = -11;
			}
			else
			{
				for (int jj = 0; jj < pieceMoveBoards.size(); ++jj)
				{
					TreeNode * newTreeNode = new TreeNode;
					newTreeNode->_board = pieceMoveBoards[jj];
					// Get the value from the BEF
					newTreeNode->_boardValue = std::rand() % 20 - 10;
					newTreeNode->_parentKey = nodeKey;
					(*boardTreeMap)[nodeKey + "_" + to_string(childIndex)] = *newTreeNode;
					(*boardTreeMap)[nodeKey]._childKeys.push_back(nodeKey + "_" + to_string(childIndex));
					//cout << "childIndex: " << childIndex << ", pieceMoveBoards.size(): " << pieceMoveBoards.size() << endl;
					cout << "Created child with key " + nodeKey + "_" + to_string(childIndex) << " for " << nodeKey << endl;
					++childIndex;
					// Give value to ancestors, stopping at the first node that has a higher value already
					sendValueToAncestors(newTreeNode, boardTreeMap);
				}
			}
		}
	}

	// Check if the current node is the last child node of the parent
	// If yes, return
	if (parentIsLastChildOfGrandparent(nodeKey, boardTreeMap))
	{
		return;
	}
	else// If no, go to the next sibling node
	{
		nodeKey = keyOfNextSibling(nodeKey, boardTreeMap);
		//cout << "Changed to sibling key: " << nodeKey << endl;
	}
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
vector<vector<int>> Player::redMove(MoveResultArray * _moveResultArray, vector<int> board, int index)
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
vector<vector<int>> Player::redJump(MoveResultArray * _moveResultArray, vector<int> board, int index)
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
vector<vector<int>> Player::whiteMove(MoveResultArray * _moveResultArray, vector<int> board, int index)
{
	vector<vector<int>> returnBoards;
	vector<int> boardCopy;

	int tempInt = _moveResultArray->movesUp[index].left;
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
	tempInt = _moveResultArray->movesUp[index].right;
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
vector<vector<int>> Player::whiteJump(MoveResultArray * _moveResultArray, vector<int> board, int index)
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
		if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] < 0)
		{
			cout << "jumpsup[currPosition].left" << endl;
			cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << " valued at " << board[enemyPos] << endl;
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
		tempInt = _moveResultArray->jumpsUp[currPosition].right;
		enemyPos = _moveResultArray->movesUp[currPosition].right;
		if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] < 0)
		{
			cout << "jumpsup[currPosition].right" << endl;
			cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << endl;
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
			tempInt = _moveResultArray->jumpsDown[currPosition].left;
			enemyPos = _moveResultArray->movesDown[currPosition].left;
			if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] < 0)
			{
				cout << "jumpsdown[currPosition].left" << endl;
				cout << "Jumping from " << currPosition << " to " << tempInt << " over " << enemyPos << endl;
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
			if (tempInt != -1 && boardCopy[tempInt] == 0 && boardCopy[enemyPos] < 0)
			{
				cout << "jumpsdown[currPosition].right" << endl;
				cout << "Jumping from " << currPosition << " to " << tempInt << endl;
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

void Player::sendValueToAncestors(TreeNode * childNode, map<string, TreeNode> * boardTreeMap)
{
	string parentKey;
	TreeNode * currentNode = childNode;
	int boardValue = childNode->_boardValue;
	parentKey = currentNode->_parentKey;

	while (parentKey != "_0")
	{
		// Send value to node above the current TreeNode
		// Change the value if greater than the current node or if this is the first child node
		if ((*boardTreeMap)[parentKey]._boardValue < boardValue || (*boardTreeMap)[parentKey]._childKeys.size() == 1)
		{
			//cout << "Changed value of " + parentKey + " to " + to_string(boardValue) << endl;
			(*boardTreeMap)[parentKey]._boardValue = boardValue;
		}
		else
		{
			return;
		}

		// Get the parent key from the current TreeNode
		parentKey = (*boardTreeMap)[parentKey]._parentKey;
	}
}

bool Player::parentIsLastChildOfGrandparent(string parentKey, map<string, TreeNode> * boardTreeMap)
{
	TreeNode * parentNode = &(*boardTreeMap)[parentKey];
	TreeNode * grandparentNode = &(*boardTreeMap)[parentNode->_parentKey];
	for (int ii = 0; ii < grandparentNode->_childKeys.size(); ++ii)
	{
		//cout << "ii is " << ii << ", and parentNode->_parentKey is " << parentNode->_parentKey << endl;
		//cout << "Comparing " << grandparentNode->_childKeys[ii] << " to " << parentKey << endl;
		if (grandparentNode->_childKeys[ii] == parentKey)
		{
			//cout << "ii is " << ii << endl;
			if (ii == grandparentNode->_childKeys.size() - 1)
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

string Player::keyOfNextValidNode(string nodeKey, map<string, TreeNode> * boardTreeMap)
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
			cout << "node is last child of parent" << endl;
			nodeKey = "";

			if (nodeKey == "_0") // Encountered root node, there are no more nodes to be parents of new generation
			{
				cout << "node is last child of parent and found root" << endl;
				return "";
			}
		}

		return keyOfLeftmostDeepestLeafFromNode(nodeKey, boardTreeMap);
	}
}

string Player::keyOfLeftmostDeepestLeafFromNode(string nodeKey, map<string, TreeNode> * boardTreeMap)
{
	cout << "keyOfLeftmostDeepestLeafFromNode" << endl;
	cout << (*boardTreeMap)["_0"]._childKeys.size() << endl;
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
		if ((*boardTreeMap)[nodeKey]._boardValue == -11) // don't choose a lost board
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

string Player::keyOfNextSibling(string parentKey, map<string, TreeNode> * boardTreeMap)
{
	cout << "keyOfNextSibling" << endl;
	TreeNode * parentNode = &(*boardTreeMap)[parentKey];
	TreeNode * grandparentNode = &(*boardTreeMap)[parentNode->_parentKey];
	for (int ii = 0; ii < grandparentNode->_childKeys.size(); ++ii)
	{
		if (grandparentNode->_childKeys[ii] == parentKey && ii != grandparentNode->_childKeys.size() - 1)
		{
			return grandparentNode->_childKeys[ii + 1];
		}
	}
	return "";
}

#endif