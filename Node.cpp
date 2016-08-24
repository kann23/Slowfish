#include "Node.h"

Node::Node() {
	fromTo = " ";
	FEN = " ";
	score = 0;
	arrayPosition = 0;
	for (int i = 0; i < SizeOfArray; i++)
		possibleMoves[i] = NULL;
}

Node::Node(std::string aMove, std::string fen) {
	fromTo = aMove;
	FEN = fen;
	score = 0;
	arrayPosition = 0;
	for (int i = 0; i < SizeOfArray; i++)
		possibleMoves[i] = NULL;
}

Node::~Node() {
}

void Node::setScore(int aScore) {
	score = aScore;
}

int Node::getScore() {
	return score;
}

void Node::resetPosition() {
	arrayPosition = 0;
}

void Node::setPosition() {
	arrayPosition++;
}

int Node::getPosition() {
	return arrayPosition;
}

void Node::setFEN(std::string fen) {
	FEN = fen;
}

void Node::setFromTo(std::string aMove) {
	fromTo = aMove;
}

void Node::setPossibleMoves(std::string fen, std::string aMove, int count) {
	possibleMoves[count] = new Node(aMove, fen);
}

void Node::setPossibleMoves(std::string fen, std::string aMove, int count, int aScore) {
	possibleMoves[count] = new Node(aMove, fen);
	possibleMoves[count]->setScore(aScore);
}

std::string Node::getMove() {
	return fromTo;
}

std::string Node::getFen() {
	return FEN;
}

Node* Node::getPossible(int count) {
	return possibleMoves[count];
}

void Node::printMoves() {
	int count = 0;

	while (count < SizeOfArray) {
		if (possibleMoves[count] != NULL)
			std::cout << count + 1 << ". " << possibleMoves[count]->getMove() << std::endl;
		else
			break;
		count++;
	}
}