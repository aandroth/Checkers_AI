
#include "MoveResultArray.h"
#include <vector>
using std::vector;

#ifndef PIECE_H
#define PIECE_H

class Piece
{
public:
	int m_value;
	int m_position;
	MoveResultArray * m_moveResultArray;

	void setPosition(int pos){ m_position = pos; }
	virtual vector<int> evaluateJumps() = 0;
	virtual vector<int> evaluateMoves() = 0;
};

class RedChecker : public Piece
{
public:
	RedChecker(MoveResultArray * mRA){ m_value = 1; m_moveResultArray = mRA; }
	~RedChecker(){}
	vector<int> evaluateJumps()
	{
		vector<int> tempVec;
		tempVec.push_back( m_moveResultArray->jumpsDown[m_position].left );
		tempVec.push_back( m_moveResultArray->jumpsDown[m_position].right );
		return tempVec;
	}
	vector<int> evaluateMoves()
	{
		vector<int> tempVec;
		tempVec.push_back(m_moveResultArray->movesDown[m_position].left);
		tempVec.push_back(m_moveResultArray->movesDown[m_position].right);
		return tempVec;
	}
};


class WhiteChecker : public Piece
{
public:
	WhiteChecker(MoveResultArray * mRA){ m_value = 1; m_moveResultArray = mRA; }
	~WhiteChecker(){}
	vector<int> evaluateJumps()
	{
		vector<int> tempVec;
		tempVec.push_back(m_moveResultArray->jumpsUp[m_position].left);
		tempVec.push_back(m_moveResultArray->jumpsUp[m_position].right);
		return tempVec;
	}
	vector<int> evaluateMoves()
	{
		vector<int> tempVec;
		tempVec.push_back(m_moveResultArray->movesUp[m_position].left);
		tempVec.push_back(m_moveResultArray->movesUp[m_position].right);
		return tempVec;
	}
};

class King : public Piece
{
public:
	King(Piece * p)
	{
		m_value = p->m_position;
		m_value = 2;
		m_moveResultArray = p->m_moveResultArray;
	}
	~King(){}
	vector<int> evaluateJumps()
	{
		vector<int> tempVec;
		tempVec.push_back(m_moveResultArray->jumpsDown[m_position].left);
		tempVec.push_back(m_moveResultArray->jumpsDown[m_position].right);
		tempVec.push_back(m_moveResultArray->jumpsUp[m_position].left);
		tempVec.push_back(m_moveResultArray->jumpsUp[m_position].right);
		return tempVec;
	}
	vector<int> evaluateMoves()
	{
		vector<int> tempVec;
		tempVec.push_back(m_moveResultArray->movesDown[m_position].left);
		tempVec.push_back(m_moveResultArray->movesDown[m_position].right);
		tempVec.push_back(m_moveResultArray->movesUp[m_position].left);
		tempVec.push_back(m_moveResultArray->movesUp[m_position].right);
		return tempVec;
	}
};
#endif