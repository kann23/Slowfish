#ifndef BOARDA_H
#define BOARDA_H

//#include <cmath>
#include <cstdio>
#include "Inits.h"



class BoardA {
public:
	BoardA();
	BoardA(std::string fen);
	~BoardA();

	void initCurrent();
	void initPrev();

	bool populateBoard(std::string fen);
	bool populateData(std::string fen, int stringPlace);

	std::string getFEN();

	void getMoves();
	void testDiagonal(int rank, int file);
	void testHorizontal(int rank, int file);
	void testKingMoves(int rank, int file);
	void testPawnMoves(int rank, int file);
	void testKnightMoves(int rank, int file);
	void testRun(int rank, int file, int testRank, int testFile);

	bool checkPawnMove(int startRank, int startFile, int destRank, int destFile);

	bool isAttacked(int rank, int file);
	bool diagonalAttack(int rank, int file);
	bool horizontalAttack(int rank, int file);
	bool knightAttack(int rank, int file);

	void makeMove(std::string toFrom);
	void undoMakeMove(std::string toFrom, std::string fen);

	void move(int initRank, int initFile, int destRank, int destFile);
	char movePromote(std::string toFrom);
	void moveCastle(int initRank, int initFile, int destRank, int destFile, char piece);
	void movePawn(int initRank, int initFile, int destRank, int destFile, char piece);

	void promote(int initRank, int initFile, int destRank, int destFile);
	bool testMove();
	void undoMove(char captured, int initRank, int initFile, int destRank, int destFile);
	void findKing(int &rank, int &file);

	bool isAlly(int rank, int file);
	bool isEnemy(int rank, int file);

	void determineScore();
	bool staleCheckMate();
	void updateScore(char piece, int sign);
	void setScore(int aScore);
	int getScore();

	void displayBoard();
	void getMoveList();
	int getToMove();
	void displayFen();
	std::string copyFen();
	std::string copyMoveList(int position);

private:
	char square[8][8];
	int score;
	int toMove;
	int moveCount;
	int hundredPlyCount;
	int enPas;
	bool castleKingW;
	bool castleQueenW;
	bool castleKingB;
	bool castleQueenB;
	int epRank;
	int epFile;
	std::string FEN;
	std::string moveList[SizeOfArray];
	int listPosition;

	int prevScore;
	int prevToMove;
	int prevMoveCount;
	int prevHundredPlyCount;
	int prevEnPas;
	bool prevCastleKingW;
	bool prevCastleKingB;
	bool prevCastleQueenW;
	bool prevCastleQueenB;
	int prevEpRank;
	int prevEpFile;
	char capturedPiece;

	bool promotePawn;
};

#endif