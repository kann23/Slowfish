#ifndef SEARCH_H
#define SEARCH_H

#include <iostream>
#include "Inits.h"
#include "BoardA.h"
#include "Node.h"

class Search {
public:
	Search();
	Search(std::string fen);
	~Search();

	void perft(int depth);
	void perftHelper(Node* &node, int &total, bool &nodeComplete, int &count1, int &total1);
	void preferredVariation();
	int switchMove();

	void addPly();
	void findNode(Node* &node, int &total, bool &nodeComplete, int move);
	void minMax(Node* &node);

	int fillNodeArray(Node* &node);
	int fillArray(Node* &node);

	void printNode(Node* node);

private:
	Node *head;
	std::string FEN;
	int toMove;
};

#endif