#ifndef NODE_H
#define NODE_H

#include "Inits.h"

class Node {
public:
	Node();
	Node(std::string aMove, std::string fen);
	virtual ~Node();

	void setFEN(std::string fen);
	void setFromTo(std::string aMove);
	void setPossibleMoves(std::string fen, std::string aMove, int count);
	void setPossibleMoves(std::string fen, std::string aMove, int count, int aScore);

	std::string getMove();
	std::string getFen();
	Node* getPossible(int count);

	void setPosition();
	int getPosition();
	void resetPosition();

	void setScore(int aScore);
	int getScore();

	void printMoves();

private:
	std::string fromTo;
	std::string FEN;
	int score;
	int arrayPosition;
	Node* possibleMoves[SizeOfArray];
};

#endif 