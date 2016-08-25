#include "Search.h"

Search::Search() {
	head = NULL;
	FEN = StartPosition;
	toMove = -1;
}

Search::Search(std::string fen) {
	head = NULL;
	FEN = fen;
	toMove = -1;
}

Search::~Search() {
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Search::addPly() {
	int total = 0;
	bool nodeComplete = false;

	findNode(head, total, nodeComplete, toMove);
	//std::cout << total << " " << head->getScore() << std::endl;
}

void Search::findNode(Node* &node, int &total, bool &nodeComplete, int move) {
	if (head == NULL) {
		int score = 0;
		head = new Node("head", FEN);
		BoardA *board = new BoardA(FEN);
		toMove = board->getToMove();
		board->determineScore();
		score = board->getScore();
		head->setScore(score);
		return findNode(node, total, nodeComplete, toMove);
	}
	else if (node == NULL)
		return;
	else
		toMove = move;


	Node* temp = node->getPossible(node->getPosition());
	findNode(temp, total, nodeComplete, switchMove());
	toMove = move;

	if (nodeComplete) {
		if (node->getPossible(node->getPosition())->getMove() == "end")
			return;
		node->setPosition();
		if (node->getPossible(node->getPosition())->getMove() == "end") {
			node->resetPosition();
			minMax(node);
			return;
		}
		nodeComplete = false;
	}

	if (node->getPossible(0) == NULL) {
		total += fillNodeArray(node);
		nodeComplete = true;
	}
	else {
		findNode(node, total, nodeComplete, toMove);
	}
}

int Search::switchMove() {
	return toMove == white ? black : white;
}

void Search::minMax(Node* &node) {
	int count = 1;
	int score = node->getPossible(0)->getScore();

	if (node->getPossible(0)->getMove() == "end") {
		node->setScore(score);
		return;
	}

	if (toMove == white) {
		while (count < SizeOfArray) {
			if (node->getPossible(count)->getMove() == "end")
				break;
			if (node->getPossible(count)->getScore() > score)
				score = node->getPossible(count)->getScore();
			if (node->getPossible(count)->getMove() == "end")
				break;
			count++;
		}
	}
	else {
		while (count < SizeOfArray) {
			if (node->getPossible(count)->getMove() == "end")
				break;
			if (node->getPossible(count)->getScore() < score)
				score = node->getPossible(count)->getScore();
			if (node->getPossible(count)->getMove() == "end")
				break;
			count++;
		}
	}
	node->setScore(score);
}

int Search::fillNodeArray(Node*& node) {
	std::string fen = node->getFen();
	std::string newFen = " ";
	std::string aMove = node->getMove();
	std::string newMove = " ";
	int count = 0;
	int nodeScore = 0;

	std::string moves[SizeOfArray];
	BoardA *aBoard = new BoardA(fen);
	while (count < SizeOfArray) {
		moves[count] = aBoard->copyMoveList(count);
		if (moves[count] == "*")
			break;
		count++;
	}
	count = 0;
	nodeScore = node->getScore();
	aBoard->setScore(nodeScore);

	while (count < SizeOfArray) {
		newMove = moves[count];
		if (newMove == "*") {
			node->setPossibleMoves("end", "end", count);
			return count;
		}
		aBoard->makeMove(newMove);
		nodeScore = aBoard->getScore();
		newFen = aBoard->copyFen();

		node->setPossibleMoves(newFen, newMove, count, nodeScore);

		aBoard->undoMakeMove(newMove, fen);
		count++;
	}
	minMax(node);
	return count;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

std::string Search::getBestMove() {
	std::string bestMove = " ";
	int bestScore = head->getScore();
	int count = 0;

	while (count < SizeOfArray) {
		if (head->getPossible(count)->getScore() == bestScore)
			return head->getPossible(count)->getMove();
		count++;
	}
}

void Search::preferredVariation() {
	Node* node = head;
	int count = 0;
	int score = node->getScore();

	while (true) {
		if (node->getPossible(count)->getMove() == "end") {
			std::cout << node->getMove() << std::endl;
			return;
		}
		if (node->getPossible(count)->getScore() == score) {
			if (node != head)
				std::cout << node->getMove() << " ";
			node = node->getPossible(count);
			if (node->getPossible(0) == NULL) {
				std::cout << node->getMove() << std::endl;
				return;
			}
			count = 0;
			continue;
		}
		count++;
	}
	std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Search::perft(int depth) {
	int total = 0;
	int count1 = 0;
	int total1 = 0;
	bool nodeComplete = false;
	for (int i = depth; depth > 0; depth--) {
		total = 0;
		count1 = 0;
		total1 = 0;
		nodeComplete = false;

		perftHelper(head, total, nodeComplete, count1, total1);
		std::cout << total << std::endl;
	}
}

void Search::perftHelper(Node* &node, int &total, bool &nodeComplete, int &count1, int &total1) {

	if (node == NULL && node != head)
		return;
	else if (head == NULL) {
		head = new Node("head", FEN);
		return perftHelper(node, total, nodeComplete, count1, total1);
	}

	int count = node->getPosition();
	Node* temp = node->getPossible(count);

	perftHelper(temp, total, nodeComplete, count1, total1);
	if (total == 477)
		int a = 1;

	if (nodeComplete) {
		if (node->getPossible(node->getPosition())->getMove() == "end") {
			count1 = total;
			return;
		}
		node->setPosition();
		if (node->getPossible(node->getPosition())->getMove() == "end") {
			node->resetPosition();
			//            total1 += (total-count1);
			//            if(total-count1 == 0)
			//                std::cout << node->getFen() << "\t" << node->getMove() << " " <<  total-count1 << " " << total1 << std::endl;
			//            std::cout << node->getFen() << "\t" << node->getMove() << " " <<  total-count1 << std::endl;
			//            std::cout << node->getMove() << " " << total-count1 << std::endl;
			count1 = total;
			return;
		}
		nodeComplete = false;
	}

	if (node->getPossible(0) == NULL) {
		total += fillArray(node);
		nodeComplete = true;
	}
	else
		perftHelper(node, total, nodeComplete, count1, total1);
}

int Search::fillArray(Node* &node) {
	std::string fen = node->getFen();
	std::string newFen = " ";
	std::string aMove = node->getMove();
	std::string newMove = " ";
	int count = 0;

	if (fen == "end")
		return 0;

	std::string moves[SizeOfArray];
	BoardA *aBoard = new BoardA(fen);
	while (count < SizeOfArray) {
		moves[count] = aBoard->copyMoveList(count);
		if (moves[count] == "*")
			break;
		count++;
	}
	count = 0;


	while (count < SizeOfArray) {
		newMove = moves[count];
		if (newMove == "*") {
			node->setPossibleMoves("end", "end", count);
			//            std::cout << aMove << " " << count << std::endl;
			return count;
		}
		aBoard->makeMove(newMove);
		newFen = aBoard->copyFen();
		node->setPossibleMoves(newFen, newMove, count);

		aBoard->undoMakeMove(newMove, fen);
		count++;
	}
	return count;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Search::printNode(Node* node) {
	node->printMoves();
}
