#include "BoardA.h"

BoardA::BoardA() {
	initCurrent();
	populateBoard(StartPosition);
	FEN = getFEN();
	getMoves();
	initPrev();
}

BoardA::BoardA(std::string fen) {
	initCurrent();
	populateBoard(fen);
	FEN = getFEN();
	getMoves();
	initPrev();
}

BoardA::~BoardA() {
}

void BoardA::initCurrent() {
	score = 0;
	enPas = 0;
	epFile = -1;
	epRank = -1;
	castleKingW = false;
	castleKingB = false;
	castleQueenW = false;
	castleQueenB = false;
	toMove = white;
	moveCount = 1;
	hundredPlyCount = 0;
	promotePawn = false;
	listPosition = 0;
}

void BoardA::initPrev() {
	prevScore = score;
	prevToMove = toMove;
	prevMoveCount = moveCount;
	prevHundredPlyCount = hundredPlyCount;
	prevEnPas = enPas;
	prevCastleKingW = castleKingW;
	prevCastleKingB = castleKingB;
	prevCastleQueenW = castleQueenW;
	prevCastleQueenB = castleQueenB;
	prevEpRank = epRank;
	prevEpFile = epFile;
	capturedPiece = 'O';
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void BoardA::makeMove(std::string toFrom) {
	int initRank = toFrom[1] - 49;
	int destRank = toFrom[3] - 49;
	int initFile = toFrom[0] - 97;
	int destFile = toFrom[2] - 97;
	char piece = square[initRank][initFile];

	if (toFrom[4] != 'q' && toFrom[4] != 'r' && toFrom[4] != 'b' && toFrom[4] != 'n')
		promotePawn = false;
	else {
		promotePawn = true;
		piece = movePromote(toFrom);
	}

	capturedPiece = square[destRank][destFile];
	square[destRank][destFile] = piece;

	movePawn(initRank, initFile, destRank, destFile, piece);
	moveCastle(initRank, initFile, destRank, destFile, piece);

	if (toMove == white)
		toMove = black;
	else {
		toMove = white;
		moveCount++;
	}

	if (piece == 'P' || piece == 'p' || capturedPiece != 'O')
		hundredPlyCount = 0;
	else
		hundredPlyCount++;


	FEN = getFEN();
	if (hundredPlyCount > 100) {
		score = 0;
		moveList[0] = "*";
		listPosition = 0;
		return;
	}

	getMoves();
	staleCheckMate();
}

char BoardA::movePromote(std::string toFrom) {
	if (toFrom[4] == 'q' && toMove == white) {
		score += 8;
		return 'Q';
	}
	else if (toFrom[4] == 'q' && toMove == black) {
		score -= 8;
		return 'q';
	}
	else if (toFrom[4] == 'r' && toMove == white) {
		score += 4;
		return 'R';
	}
	else if (toFrom[4] == 'r' && toMove == black) {
		score -= 4;
		return 'r';
	}
	else if (toFrom[4] == 'b' && toMove == white) {
		score += 2;
		return 'B';
	}
	else if (toFrom[4] == 'b' && toMove == black) {
		score -= 2;
		return 'b';
	}
	else if (toFrom[4] == 'n' && toMove == white) {
		score += 2;
		return 'N';
	}
	else if (toFrom[4] == 'n' && toMove == black) {
		score -= 2;
		return 'n';
	}
}

void BoardA::moveCastle(int initRank, int initFile, int destRank, int destFile, char piece) {
	if (square[initRank][initFile] == 'K' && abs(destFile - initFile) == 2) {
		square[Rank_1][destFile] = square[Rank_1][initFile];
		square[Rank_1][initFile] = 'O';
		if (destFile == File_C) {
			square[Rank_1][File_D] = square[Rank_1][File_A];
			square[Rank_1][File_A] = 'O';
		}
		else if (destFile == File_G) {
			square[Rank_1][File_F] = square[Rank_1][File_H];
			square[Rank_1][File_H] = 'O';
		}
	}
	else if (square[initRank][initFile] == 'k' && abs(destFile - initFile) == 2) {
		square[Rank_8][destFile] = square[Rank_8][initFile];
		square[Rank_8][initFile] = 'O';
		if (destFile == File_C) {
			square[Rank_8][File_D] = square[Rank_8][File_A];
			square[Rank_8][File_A] = 'O';
		}
		else if (destFile == File_G) {
			square[Rank_8][File_F] = square[Rank_8][File_H];
			square[Rank_8][File_H] = 'O';
		}
	}
	else {
		updateScore(capturedPiece, -1);
		square[destRank][destFile] = piece;
		square[initRank][initFile] = 'O';
	}


	if (initRank == Rank_1 && initFile == File_E &&
		piece == 'K')
	{
		castleKingW = false;
		castleQueenW = false;
	}
	else if ((initRank == Rank_1 && initFile == File_A && piece == 'R') ||
		(destRank == Rank_1 && destFile == File_A))
		castleQueenW = false;
	else if ((initRank == Rank_1 && initFile == File_H && piece == 'R') ||
		(destRank == Rank_1 && destFile == File_H))
		castleKingW = false;
	else if (initRank == Rank_8 && initFile == File_E &&
		piece == 'k')
	{
		castleKingB = false;
		castleQueenB = false;
	}
	else if ((initRank == Rank_8 && initFile == File_A && piece == 'r') ||
		(destRank == Rank_8 && destFile == File_A))
		castleQueenB = false;
	else if ((initRank == Rank_8 && initFile == File_H && piece == 'r') ||
		(destRank == Rank_8 && destFile == File_H))
		castleKingB = false;
}

void BoardA::movePawn(int initRank, int initFile, int destRank, int destFile, char piece) {
	if (square[initRank][initFile] == 'P' &&
		(destRank == epRank && destFile == epFile))
	{
		square[destRank][destFile] = piece;
		square[initRank][initFile] = 'O';
		square[epRank - 1][epFile] = 'O';
		score += 1;
	}
	else if (square[initRank][initFile] == 'p' &&
		(destRank == epRank && destFile == epFile))
	{
		square[destRank][destFile] = piece;
		square[initRank][initFile] = 'O';
		square[epRank + 1][epFile] = 'O';
		score -= 1;
	}
	else
		enPas = 0;

	if (abs(destRank - initRank) == 2 && destFile == initFile &&
		(piece == 'P' || piece == 'p'))
	{
		enPas = moveCount;
		epFile = destFile;
		if (toMove == white)
			epRank = destRank - 1;
		else
			epRank = destRank + 1;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void BoardA::getMoves() {
	int testRank = -1;
	int testFile = -1;

	for (int i = 0; i < SizeOfArray; i++)
		moveList[i] = "*";
	listPosition = 0;

	for (int rank = Rank_1; rank <= Rank_8; rank++) {
		for (int file = File_A; file <= File_H; file++) {
			if (toMove == white) {
				if (square[rank][file] == 'K')
					testKingMoves(rank, file);
				else if (square[rank][file] == 'Q') {
					testDiagonal(rank, file);
					testHorizontal(rank, file);
				}
				else if (square[rank][file] == 'R')
					testHorizontal(rank, file);
				else if (square[rank][file] == 'B')
					testDiagonal(rank, file);
				else if (square[rank][file] == 'N')
					testKnightMoves(rank, file);
				else if (square[rank][file] == 'P')
					testPawnMoves(rank, file);
			}
			else if (toMove == black) {
				if (square[rank][file] == 'k')
					testKingMoves(rank, file);
				else if (square[rank][file] == 'q') {
					testDiagonal(rank, file);
					testHorizontal(rank, file);
				}
				else if (square[rank][file] == 'r')
					testHorizontal(rank, file);
				else if (square[rank][file] == 'b')
					testDiagonal(rank, file);
				else if (square[rank][file] == 'n')
					testKnightMoves(rank, file);
				else if (square[rank][file] == 'p')
					testPawnMoves(rank, file);
			}
		}
	}
}

void BoardA::testKnightMoves(int rank, int file) {
	char charFile = ' ';
	if (rank + 1 <= Rank_8 && file + 2 <= File_H) {
		charFile = file + 2 + 97;
		if (!isAlly(rank + 1, file + 2))
			move(rank, file, rank + 1, file + 2);
	}
	if (rank + 2 <= Rank_8 && file + 1 <= File_H) {
		charFile = file + 1 + 97;
		if (!isAlly(rank + 2, file + 1))
			move(rank, file, rank + 2, file + 1);
	}
	if (rank + 2 <= Rank_8 && file - 1 >= File_A) {
		charFile = file - 1 + 97;
		if (!isAlly(rank + 2, file - 1))
			move(rank, file, rank + 2, file - 1);
	}
	if (rank + 1 <= Rank_8 && file - 2 >= File_A) {
		charFile = file - 2 + 97;
		if (!isAlly(rank + 1, file - 2))
			move(rank, file, rank + 1, file - 2);
	}
	if (rank - 1 >= Rank_1 && file - 2 >= File_A) {
		charFile = file - 2 + 97;
		if (!isAlly(rank - 1, file - 2))
			move(rank, file, rank - 1, file - 2);
	}
	if (rank - 2 >= Rank_1 && file - 1 >= File_A) {
		charFile = file - 1 + 97;
		if (!isAlly(rank - 2, file - 1))
			move(rank, file, rank - 2, file - 1);
	}
	if (rank - 2 >= Rank_1 && file + 1 <= File_H) {
		charFile = file + 1 + 97;
		if (!isAlly(rank - 2, file + 1))
			move(rank, file, rank - 2, file + 1);
	}
	if (rank - 1 >= Rank_1 && file + 2 <= File_H) {
		charFile = file + 2 + 97;
		if (!isAlly(rank - 1, file + 2))
			move(rank, file, rank - 1, file + 2);
	}
}

void BoardA::testDiagonal(int rank, int file) {
	int testRank = rank + 1;
	int testFile = file + 1;
	testRun(rank, file, testRank, testFile);


	testRank = rank + 1;
	testFile = file - 1;
	testRun(rank, file, testRank, testFile);

	testRank = rank - 1;
	testFile = file + 1;
	testRun(rank, file, testRank, testFile);

	testRank = rank - 1;
	testFile = file - 1;
	testRun(rank, file, testRank, testFile);
}

void BoardA::testHorizontal(int rank, int file) {
	int testRank = rank + 1;
	int testFile = file;
	testRun(rank, file, testRank, testFile);

	testRank = rank - 1;
	testFile = file;
	testRun(rank, file, testRank, testFile);

	testRank = rank;
	testFile = file + 1;
	testRun(rank, file, testRank, testFile);

	testRank = rank;
	testFile = file - 1;
	testRun(rank, file, testRank, testFile);
}

void BoardA::testRun(int rank, int file, int testRank, int testFile) {
	int rankInc = testRank - rank;
	int fileInc = testFile - file;

	while (testRank <= Rank_8 && testRank >= Rank_1 &&
		testFile <= File_H && testFile >= File_A)
	{
		if (!isAlly(testRank, testFile)) {
			move(rank, file, testRank, testFile);
			if (isEnemy(testRank, testFile))
				return;
			testRank += rankInc;
			testFile += fileInc;
		}
		else
			return;
	}

}

void BoardA::testKingMoves(int rank, int file) {
	if (rank != Rank_8) {
		if (!isAlly(rank + 1, file))
			move(rank, file, rank + 1, file);
		if (file != File_A) {
			if (!isAlly(rank + 1, file - 1))
				move(rank, file, rank + 1, file - 1);
			if (!isAlly(rank, file - 1))
				move(rank, file, rank, file - 1);
		}
		if (file != File_H) {
			if (!isAlly(rank + 1, file + 1))
				move(rank, file, rank + 1, file + 1);
			if (!isAlly(rank, file + 1))
				move(rank, file, rank, file + 1);
		}
	}
	if (rank != Rank_1) {
		if (!isAlly(rank - 1, file))
			move(rank, file, rank - 1, file);
		if (file != File_A) {
			if (!isAlly(rank - 1, file - 1))
				move(rank, file, rank - 1, file - 1);
			if (rank == Rank_8 && !isAlly(rank, file - 1))
				move(rank, file, rank, file - 1);
		}
		if (file != File_H) {
			if (!isAlly(rank - 1, file + 1))
				move(rank, file, rank - 1, file + 1);
			if (rank == Rank_8 && !isAlly(rank, file + 1))
				move(rank, file, rank, file + 1);
		}
	}
	if (file == File_E) {
		if (rank == Rank_1) {
			if (square[Rank_1][File_F] == 'O' && square[Rank_1][File_G] == 'O' &&
				!isAttacked(Rank_1, File_F) && !isAttacked(Rank_1, File_G) &&
				!isAttacked(Rank_1, File_E) && castleKingW)
				move(Rank_1, File_E, Rank_1, File_G);
			if (square[Rank_1][File_D] == 'O' && square[Rank_1][File_C] == 'O' &&
				square[Rank_1][File_B] == 'O' && !isAttacked(Rank_1, File_E) &&
				!isAttacked(Rank_1, File_D) && !isAttacked(Rank_1, File_C) &&
				castleQueenW)
				move(Rank_1, File_E, Rank_1, File_C);
		}
		else if (rank == Rank_8) {
			if (square[Rank_8][File_F] == 'O' && square[Rank_8][File_G] == 'O' &&
				!isAttacked(Rank_8, File_F) && !isAttacked(Rank_8, File_G) &&
				!isAttacked(Rank_8, File_E) && castleKingB)
				move(Rank_8, File_E, Rank_8, File_G);
			if (square[Rank_8][File_D] == 'O' && square[Rank_8][File_C] == 'O' &&
				square[Rank_8][File_B] == 'O' && !isAttacked(Rank_8, File_E) &&
				!isAttacked(Rank_8, File_D) && !isAttacked(Rank_8, File_C) &&
				castleQueenB)
				move(Rank_8, File_E, Rank_8, File_C);
		}
	}
}

void BoardA::testPawnMoves(int rank, int file) {
	char charFile = file + 97;

	if (toMove == white) {
		if (rank < Rank_8) {
			if (checkPawnMove(rank, file, rank + 1, file))
				move(rank, file, rank + 1, file);
		}
		if (rank == Rank_2) {
			if (checkPawnMove(rank, file, rank + 2, file))
				move(rank, file, rank + 2, file);
		}
		if (file > File_A) {
			charFile = file + 96;
			if (checkPawnMove(rank, file, rank + 1, file - 1))
				move(rank, file, rank + 1, file - 1);
		}
		if (file < File_H) {
			charFile = file + 98;
			if (checkPawnMove(rank, file, rank + 1, file + 1))
				move(rank, file, rank + 1, file + 1);
		}
	}
	else {
		if (rank > Rank_1) {
			if (checkPawnMove(rank, file, rank - 1, file))
				move(rank, file, rank - 1, file);
		}
		if (rank == Rank_7) {
			if (checkPawnMove(rank, file, rank - 2, file))
				move(rank, file, rank - 2, file);
		}
		if (file > File_A) {
			charFile = file + 96;
			if (checkPawnMove(rank, file, rank - 1, file - 1))
				move(rank, file, rank - 1, file - 1);
		}
		if (file < File_H) {
			charFile = file + 98;
			if (checkPawnMove(rank, file, rank - 1, file + 1))
				move(rank, file, rank - 1, file + 1);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool BoardA::checkPawnMove(int startRank, int startFile, int destRank, int destFile) {
	int upOrBack;
	toMove == white ? upOrBack = -1 : upOrBack = 1;

	if (startFile == destFile && square[destRank][destFile] == 'O') {
		if (startRank - destRank == upOrBack)
			return true;
		else if (startRank - destRank == 2 * upOrBack &&
			square[destRank + upOrBack][destFile] == 'O')
			return true;
	}
	else if (startFile != destFile) {
		if (isEnemy(destRank, destFile))
			return true;
		else if (moveCount - enPas <= 1 && startRank == epRank + upOrBack &&
			abs(startFile - epFile) == 1 && destFile == epFile)
			return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool BoardA::isAttacked(int rank, int file) {
	if (diagonalAttack(rank, file) ||
		horizontalAttack(rank, file) ||
		knightAttack(rank, file))
		return true;
	else
		return false;
}

bool BoardA::diagonalAttack(int rank, int file) {
	int testRank = rank + 1;
	int testFile = file + 1;

	while (testRank <= Rank_8 && testFile <= File_H) {
		if (isAlly(testRank, testFile))
			break;
		else if (square[testRank][testFile] == 'O') {
			testRank++;
			testFile++;
			continue;
		}
		else if (isEnemy(testRank, testFile) &&
			(square[testRank][testFile] == 'K' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'Q' ||
			square[testRank][testFile] == 'B' ||
			(square[testRank][testFile] == 'k' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'q' ||
			square[testRank][testFile] == 'b' ||
			(square[testRank][testFile] == 'p' && abs(rank - testRank) == 1))
			return true;

		break;
	}

	testRank = rank + 1;
	testFile = file - 1;
	while (testRank <= Rank_8 && testFile >= File_A) {
		if (isAlly(testRank, testFile))
			break;
		else if (square[testRank][testFile] == 'O') {
			testRank++;
			testFile--;
			continue;
		}
		else if (isEnemy(testRank, testFile) &&
			(square[testRank][testFile] == 'K' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'Q' ||
			square[testRank][testFile] == 'B' ||
			(square[testRank][testFile] == 'k' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'q' ||
			square[testRank][testFile] == 'b' ||
			(square[testRank][testFile] == 'p' && abs(rank - testRank) == 1))
			return true;

		break;
	}

	testRank = rank - 1;
	testFile = file + 1;
	while (testRank >= Rank_1 && testFile <= File_H) {
		if (isAlly(testRank, testFile))
			break;
		else if (square[testRank][testFile] == 'O') {
			testRank--;
			testFile++;
			continue;
		}
		else if (isEnemy(testRank, testFile) &&
			(square[testRank][testFile] == 'K' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'Q' ||
			square[testRank][testFile] == 'B' ||
			(square[testRank][testFile] == 'k' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'q' ||
			square[testRank][testFile] == 'b' ||
			(square[testRank][testFile] == 'P' && abs(rank - testRank) == 1))
			return true;

		break;
	}

	testRank = rank - 1;
	testFile = file - 1;
	while (testRank >= Rank_1 && testFile >= File_A) {
		if (isAlly(testRank, testFile))
			break;
		else if (square[testRank][testFile] == 'O') {
			testRank--;
			testFile--;
			continue;
		}
		else if (isEnemy(testRank, testFile) &&
			(square[testRank][testFile] == 'K' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'Q' ||
			square[testRank][testFile] == 'B' ||
			(square[testRank][testFile] == 'k' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'q' ||
			square[testRank][testFile] == 'b' ||
			(square[testRank][testFile] == 'P' && abs(rank - testRank) == 1))
			return true;

		break;
	}
	return false;
}

bool BoardA::horizontalAttack(int rank, int file) {
	int testRank = rank + 1;
	int testFile = file;

	while (testRank <= Rank_8) {
		if (isAlly(testRank, testFile))
			break;
		else if (square[testRank][testFile] == 'O') {
			testRank++;
			continue;
		}
		else if (isEnemy(testRank, testFile) &&
			(square[testRank][testFile] == 'K' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'Q' ||
			square[testRank][testFile] == 'R' ||
			(square[testRank][testFile] == 'k' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'q' ||
			square[testRank][testFile] == 'r')
			return true;
		else
			break;
		testRank++;
	}

	testRank = rank - 1;
	while (testRank >= Rank_1) {
		if (isAlly(testRank, testFile))
			break;
		else if (square[testRank][testFile] == 'O') {
			testRank--;
			continue;
		}
		else if (isEnemy(testRank, testFile) &&
			(square[testRank][testFile] == 'K' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'Q' ||
			square[testRank][testFile] == 'R' ||
			(square[testRank][testFile] == 'k' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'q' ||
			square[testRank][testFile] == 'r')
			return true;
		else
			break;
		testRank--;
	}

	testRank = rank;
	testFile = file + 1;
	while (testFile <= File_H) {
		if (isAlly(testRank, testFile))
			break;
		else if (square[testRank][testFile] == 'O') {
			testFile++;
			continue;
		}
		else if (isEnemy(testRank, testFile) &&
			(square[testRank][testFile] == 'K' && abs(file - testFile) == 1) ||
			square[testRank][testFile] == 'Q' ||
			square[testRank][testFile] == 'R' ||
			(square[testRank][testFile] == 'k' && abs(file - testFile) == 1) ||
			square[testRank][testFile] == 'q' ||
			square[testRank][testFile] == 'r')
			return true;
		else
			break;
		testFile++;
	}

	testFile = file - 1;
	while (testFile >= File_A) {
		if (isAlly(testRank, testFile))
			break;
		else if (square[testRank][testFile] == 'O') {
			testFile--;
			continue;
		}
		else if (isEnemy(testRank, testFile) &&
			(square[testRank][testFile] == 'K' && abs(file - testFile) == 1) ||
			square[testRank][testFile] == 'Q' ||
			square[testRank][testFile] == 'R' ||
			(square[testRank][testFile] == 'k' && abs(file - testFile) == 1) ||
			square[testRank][testFile] == 'q' ||
			square[testRank][testFile] == 'r')
			return true;
		else
			break;
		testFile--;
	}
	return false;
}

bool BoardA::knightAttack(int rank, int file) {
	int testRank = rank + 2;
	int testFile = file + 1;
	if (testRank <= Rank_8 &&
		testFile <= File_H &&
		isEnemy(testRank, testFile) &&
		(square[testRank][testFile] == 'N' ||
			square[testRank][testFile] == 'n'))
		return true;

	testFile = file - 1;
	if (testRank <= Rank_8 &&
		testFile >= File_A &&
		isEnemy(testRank, testFile) &&
		(square[testRank][testFile] == 'N' ||
			square[testRank][testFile] == 'n'))
		return true;

	testRank = rank + 1;
	testFile = file + 2;
	if (testRank <= Rank_8 &&
		testFile <= File_H &&
		isEnemy(testRank, testFile) &&
		(square[testRank][testFile] == 'N' ||
			square[testRank][testFile] == 'n'))
		return true;

	testFile = file - 2;
	if (testRank <= Rank_8 &&
		testFile >= File_A &&
		isEnemy(testRank, testFile) &&
		(square[testRank][testFile] == 'N' ||
			square[testRank][testFile] == 'n'))
		return true;

	testRank = rank - 2;
	testFile = file + 1;
	if (testRank >= Rank_1 &&
		testFile <= File_H &&
		isEnemy(testRank, testFile) &&
		(square[testRank][testFile] == 'N' ||
			square[testRank][testFile] == 'n'))
		return true;

	testFile = file - 1;
	if (testRank >= Rank_1 &&
		testFile >= File_A &&
		isEnemy(testRank, testFile) &&
		(square[testRank][testFile] == 'N' ||
			square[testRank][testFile] == 'n'))
		return true;

	testRank = rank - 1;
	testFile = file + 2;
	if (testRank >= Rank_1 &&
		testFile <= File_H &&
		isEnemy(testRank, testFile) &&
		(square[testRank][testFile] == 'N' ||
			square[testRank][testFile] == 'n'))
		return true;

	testFile = file - 2;
	if (testRank >= Rank_1 &&
		testFile >= File_A &&
		isEnemy(testRank, testFile) &&
		(square[testRank][testFile] == 'N' ||
			square[testRank][testFile] == 'n'))
		return true;

	return false;
}

bool BoardA::isAlly(int rank, int file) {
	char piece = square[rank][file];

	if (piece == 'O')
		return false;
	else if (toMove == white && piece < 96)
		return true;
	else if (toMove == black && piece > 96)
		return true;
	else
		return false;
}

bool BoardA::isEnemy(int rank, int file) {
	char piece = square[rank][file];

	if (piece == 'O')
		return false;
	else if (toMove == white && piece > 96)
		return true;
	else if (toMove == black && piece < 97)
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void BoardA::findKing(int& aRank, int& aFile) {                                  //store these as class variables
	for (int rank = Rank_1; rank <= Rank_8; rank++)                              //instead of finding them each 
		for (int file = File_A; file <= File_H; file++)                          //time.
			if ((square[rank][file] == 'K' || square[rank][file] == 'k') &&
				isAlly(rank, file)) {
				aRank = rank;
				aFile = file;
				return;
			}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void BoardA::move(int initRank, int initFile, int destRank, int destFile) {
	char capturedPiece = square[destRank][destFile];

	if (square[initRank][initFile] == 'P' &&
		(destRank == epRank && destFile == epFile))
	{
		square[destRank][destFile] = square[initRank][initFile];
		square[initRank][initFile] = 'O';
		square[epRank - 1][epFile] = 'O';
	}
	else if (square[initRank][initFile] == 'p' &&
		(destRank == epRank && destFile == epFile))
	{
		square[destRank][destFile] = square[initRank][initFile];
		square[initRank][initFile] = 'O';
		square[epRank + 1][epFile] = 'O';
	}
	else if (square[initRank][initFile] == 'K' && abs(destFile - initFile) == 2) {
		square[Rank_1][destFile] = square[Rank_1][initFile];
		square[Rank_1][initFile] = 'O';
		if (destFile == File_C) {
			square[Rank_1][File_D] = square[Rank_1][File_A];
			square[Rank_1][File_A] = 'O';
		}
		else if (destFile == File_G) {
			square[Rank_1][File_F] = square[Rank_1][File_H];
			square[Rank_1][File_H] = 'O';
		}
	}
	else if (square[initRank][initFile] == 'k' && abs(destFile - initFile) == 2) {
		square[Rank_8][destFile] = square[Rank_8][initFile];
		square[Rank_8][initFile] = 'O';
		if (destFile == File_C) {
			square[Rank_8][File_D] = square[Rank_8][File_A];
			square[Rank_8][File_A] = 'O';
		}
		else if (destFile == File_G) {
			square[Rank_8][File_F] = square[Rank_8][File_H];
			square[Rank_8][File_H] = 'O';
		}
	}
	else {
		square[destRank][destFile] = square[initRank][initFile];
		square[initRank][initFile] = 'O';
	}

	if (!testMove())
		undoMove(capturedPiece, initRank, initFile, destRank, destFile);
	else {
		if ((destRank == Rank_8 && square[destRank][destFile] == 'P') ||
			(destRank == Rank_1 && square[destRank][destFile] == 'p'))
			promote(initRank, initFile, destRank, destFile);
		else {
			moveList[listPosition] = "";
			moveList[listPosition] += (initFile + 97);
			moveList[listPosition] += (initRank + 49);
			moveList[listPosition] += (destFile + 97);
			moveList[listPosition] += (destRank + 49);
			listPosition++;
		}
		undoMove(capturedPiece, initRank, initFile, destRank, destFile);
	}
}

void BoardA::promote(int initRank, int initFile, int destRank, int destFile) {
	std::string fromTo = "";

	fromTo += (initFile + 97);
	fromTo += (initRank + 49);
	fromTo += (destFile + 97);
	fromTo += (destRank + 49);
	moveList[listPosition] = fromTo;
	moveList[listPosition] += 'q';
	listPosition++;
	moveList[listPosition] = fromTo;
	moveList[listPosition] += 'r';
	listPosition++;
	moveList[listPosition] = fromTo;
	moveList[listPosition] += 'b';
	listPosition++;
	moveList[listPosition] = fromTo;
	moveList[listPosition] += 'n';
	listPosition++;
}

bool BoardA::testMove() {
	int kingRank = -1;
	int kingFile = -1;

	findKing(kingRank, kingFile);

	if (isAttacked(kingRank, kingFile))
		return false;
	else
		return true;
}

void BoardA::undoMakeMove(std::string toFrom, std::string fen) {
	int initRank = toFrom[1] - 49;
	int destRank = toFrom[3] - 49;
	int initFile = toFrom[0] - 97;
	int destFile = toFrom[2] - 97;

	undoMove(capturedPiece, initRank, initFile, destRank, destFile);

	if (promotePawn) {
		if (toMove == black)
			square[initRank][initFile] = 'P';
		else
			square[initRank][initFile] = 'p';
	}
	score = prevScore;
	toMove = prevToMove;
	moveCount = prevMoveCount;
	hundredPlyCount = prevHundredPlyCount;
	enPas = prevEnPas;
	castleKingW = prevCastleKingW;
	castleKingB = prevCastleKingB;
	castleQueenW = prevCastleQueenW;
	castleQueenB = prevCastleQueenB;
	epRank = prevEpRank;
	epFile = prevEpFile;

	FEN = fen;
}

void BoardA::undoMove(char captured, int initRank, int initFile, int destRank, int destFile) {
	if ((square[destRank][destFile] == 'P' || square[destRank][destFile] == 'p') &&
		destFile != initFile && captured == 'O')
	{
		bool whitePawn;
		if (square[destRank][destFile] == 'P')
			whitePawn = true;
		else
			whitePawn = false;

		square[initRank][initFile] = square[destRank][destFile];
		square[destRank][destFile] = 'O';
		if (whitePawn)
			square[epRank - 1][epFile] = 'p';
		else
			square[epRank + 1][epFile] = 'P';
	}
	else if ((square[destRank][destFile] == 'K' || square[destRank][destFile] == 'k') &&
		abs(initFile - destFile) == 2)
	{
		if (destRank == Rank_1) {
			if (destFile == File_G) {
				square[Rank_1][File_E] = 'K';
				square[Rank_1][File_F] = 'O';
				square[Rank_1][File_G] = 'O';
				square[Rank_1][File_H] = 'R';
			}
			else {
				square[Rank_1][File_E] = 'K';
				square[Rank_1][File_C] = 'O';
				square[Rank_1][File_D] = 'O';
				square[Rank_1][File_A] = 'R';
			}
		}
		else if (destRank == Rank_8) {
			if (destFile == File_G) {
				square[Rank_8][File_E] = 'k';
				square[Rank_8][File_F] = 'O';
				square[Rank_8][File_G] = 'O';
				square[Rank_8][File_H] = 'r';
			}
			else {
				square[Rank_8][File_E] = 'k';
				square[Rank_8][File_C] = 'O';
				square[Rank_8][File_D] = 'O';
				square[Rank_8][File_A] = 'r';
			}
		}
	}
	else
	{
		square[initRank][initFile] = square[destRank][destFile];
		square[destRank][destFile] = captured;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool BoardA::populateBoard(std::string fen) {
	int emptyCount = 0;
	int stringPlace = 0;
	char empty = 'O';
	char aPiece = ' ';

	for (int aRank = Rank_8; aRank >= Rank_1; aRank--) {
		for (int aFile = File_A; aFile <= File_H; aFile++) {
			if (emptyCount == 0) {
				aPiece = fen[stringPlace++];
				if (aPiece == '/') {
					aFile--;
					continue;
				}
				else if ((int)aPiece > 57)
					square[aRank][aFile] = aPiece;
				else {
					emptyCount = (int)aPiece - 48;
					square[aRank][aFile] = 'O';
					emptyCount--;
				}
			}
			else {
				square[aRank][aFile] = 'O';
				emptyCount--;
			}
		}
	}
	return populateData(fen, stringPlace);
}

bool BoardA::populateData(std::string fen, int stringPlace) {
	int rank = 0;
	int file = 0;
	bool colorFlag = false;
	bool epFlag = false;
	bool fiftyFlag = false;
	bool moveCountFlag = false;
	char aPiece = ' ';

	while ((stringPlace != fen.length() / sizeof(int)) && (stringPlace < SizeOfArray)) {        //this setup could be buggy.
		aPiece = fen[stringPlace++];
		if (aPiece == ' ' || aPiece == '-')
			continue;
		else if (aPiece == 'K')
			castleKingW = true;
		else if (aPiece == 'Q')
			castleQueenW = true;
		else if (aPiece == 'k')
			castleKingB = true;
		else if (aPiece == 'q')
			castleQueenB = true;

		else if (!colorFlag) {
			if (aPiece == 'w')
				toMove = white;
			else if (aPiece == 'b')
				toMove = black;
			else {
				//                std::cout << "improper fen" << std::endl;
				return false;
			}
			colorFlag = true;
		}
		else if (!epFlag && (int)aPiece > 96) {
			epFile = (int)aPiece - 97;
			aPiece = fen[stringPlace++];
			epRank = (int)aPiece - 49;
			epFlag = true;
		}
		else if (!fiftyFlag && (int)aPiece < 58) {
			if (fen[stringPlace] == ' ') {
				int ones = (int)aPiece - 48;
				hundredPlyCount = ones;
				fiftyFlag = true;
			}
			else {
				int tens = (int)aPiece - 48;
				aPiece = fen[stringPlace++];
				int ones = (int)aPiece - 49;
				hundredPlyCount = tens * 10 + ones;
				fiftyFlag = true;
			}
		}
		else if (!moveCountFlag) {
			int tens = (int)aPiece - 48;
			int ones = 0;

			aPiece = fen[stringPlace];
			if ((int)aPiece > 47 && (int)aPiece < 58) {
				ones = (int)aPiece - 48;
				moveCount = tens * 10 + ones;
			}
			else
				moveCount = tens;

			if (epFlag) {
				if (toMove == white)             //a bit of a shortcut - don't care now
					enPas = moveCount - 1;        //fix later
				else
					enPas = moveCount;
			}

			return true;
		}
		else
			return false;
	}
}

std::string BoardA::getFEN() {
	std::string fen = "";
	char letter = ' ';
	int emptyCount = 0;
	bool castle = false;

	for (int rank = Rank_8; rank >= Rank_1; rank--) {
		for (int file = File_A; file <= File_H; file++) {
			letter = square[rank][file];
			if (letter == 'O')
				emptyCount++;
			else if (emptyCount != 0) {
				fen += (emptyCount + 48);
				fen += letter;
				emptyCount = 0;
			}
			else
				fen += letter;
		}
		if (emptyCount != 0) {
			fen += (emptyCount + 48);
			emptyCount = 0;
		}
		if (rank != 0)
			fen += '/';
	}

	toMove == white ? fen += " w " : fen += " b ";

	if (!castleKingW && !castleQueenW && !castleKingB && !castleQueenB)
		fen += "- ";
	else {
		if (castleKingW)
			fen += "K ";
		if (castleQueenW)
			fen += "Q ";
		if (castleKingB)
			fen += "k ";
		if (castleQueenB)
			fen += "q ";
	}

	if (enPas == moveCount && toMove == black && enPas != 0) {
		fen += epFile + 97;
		fen += epRank + 49;
	}
	else if (enPas == moveCount - 1 && toMove == white && enPas != 0) {
		fen += epFile + 97;
		fen += epRank + 49;
	}
	else
		fen += '-';

	char buffer[4];
	sprintf_s(buffer, sizeof(buffer), "%d", hundredPlyCount);

	fen += ' ';
	fen += buffer;

	char buffer1[4];
	sprintf_s(buffer1, sizeof(buffer), "%d", moveCount);

	fen += ' ';
	fen += buffer1;

	return fen;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void BoardA::determineScore() {
	if (staleCheckMate()) {
		prevScore = score;
		return;
	}

	for (int rank = Rank_1; rank <= Rank_8; rank++)
		for (int file = File_A; file <= File_H; file++)
			updateScore(square[rank][file], 1);

	prevScore = score;
}

void BoardA::updateScore(char piece, int sign) {
	if (piece == 'O')
		return;
	else if (piece == 'Q')
		score += (9 * sign);
	else if (piece == 'R')
		score += (5 * sign);
	else if (piece == 'B')
		score += (3 * sign);
	else if (piece == 'N')
		score += (3 * sign);
	else if (piece == 'P')
		score += (1 * sign);
	else if (piece == 'q')
		score -= (9 * sign);
	else if (piece == 'r')
		score -= (5 * sign);
	else if (piece == 'b')
		score -= (3 * sign);
	else if (piece == 'n')
		score -= (3 * sign);
	else if (piece == 'p')
		score -= (1 * sign);
}

bool BoardA::staleCheckMate() {
	int kingRank = -1;
	int kingFile = -1;

	if (moveList[0] == "*") {
		int kingRank = -1;
		int kingFile = -1;

		findKing(kingRank, kingFile);
		if (isAttacked(kingRank, kingFile)) {
			if (toMove == white)
				score = -1000;
			else
				score = 1000;
		}
		else
			score = 0;
		return true;
	}
	return false;
}

int BoardA::getScore() {
	return score;
}

void BoardA::setScore(int aScore) {
	score = aScore;
	prevScore = aScore;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//void BoardA::displayBoard() {
//	for (int rank = Rank_8; rank >= Rank_1; rank--) {
//		for (int file = File_A; file <= File_H; file++) {
//			std::cout << square[rank][file] << " ";
//		}
//		std::cout << std::endl;
//	}
//}

  void BoardA::displayBoard() {
	std::cout << "+ - - - - - - - - - - - - - - - +" << std::endl;
	for (int rank = Rank_8; rank >= Rank_1; rank--) {
		std::cout << "|";
		for (int file = File_A; file <= File_H; file++) {
			if (square[rank][file] == 'O')
				std::cout << ("   |");
			else
				std:: cout << " " << square[rank][file] << " |";
		}
		std::cout << std::endl;
		if (rank > Rank_1)
			std::cout << "| - + - + - + - + - + - + - + - |" << std::endl;
		else
			std::cout << "+ - - - - - - - - - - - - - - - +" << std::endl;
	}
}

void BoardA::getMoveList() {
	int position = 0;
	while (position < listPosition) {
		std::cout << position << ". " << moveList[position++] << std::endl;
	}
}

int BoardA::getToMove() {
	return toMove;
}

void BoardA::displayFen() {
	std::cout << FEN << std::endl;
}

std::string BoardA::copyFen() {
	return FEN;
}

std::string BoardA::copyMoveList(int position) {
	return moveList[position];
}