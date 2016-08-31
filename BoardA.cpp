#include "BoardA.h"

BoardA::BoardA() {
	initCurrent();
	populateBoard(StartPosition);
	FEN = getFEN();
	findKing();
	initPrev();
	getMoves();
}

BoardA::BoardA(std::string fen) {
	initCurrent();
	populateBoard(fen);
	FEN = getFEN();
	findKing();
	initPrev();
	getMoves();
}

BoardA::~BoardA() {
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////   Initialization Methods   ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
	whiteKingRank = 0;
	whiteKingFile = 0;
	blackKingRank = 0;
	blackKingFile = 0;
}

/*///////////////////////////////////////////////////////////////////////////////////
	These member variables are used to reset the board after a move is made.
*////////////////////////////////////////////////////////////////////////////////////
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
	prevWhiteKingRank = whiteKingRank;
	prevWhiteKingFile = whiteKingFile;
	prevBlackKingRank = blackKingRank;
	prevBlackKingFile = blackKingFile;
}

/*///////////////////////////////////////////////////////////////////////////////////
	Sets the white and black King position member variables.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::findKing() {
	for (int rank = Rank_1; rank <= Rank_8; rank++)
		for (int file = File_A; file <= File_H; file++) {
			if (square[rank][file] == 'K') {
				whiteKingRank = rank;
				whiteKingFile = file;
			}
			else if (square[rank][file] == 'k') {
				blackKingRank = rank;
				blackKingFile = file;
			}
		}
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////      Make Move      //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*///////////////////////////////////////////////////////////////////////////////////
	These methods take an external string (e.g. e2e4) and implement this move on
	the board (square[][]).  The moves are assumed to be legal, so no effort is
	taken to check for legality.  After puting a move on the board, all legal moves
	and their scores are saved as strings to moveList[].  In addition to updating
	the board the color, move number, castling rights, en passant rights, and the
	50 move rule count are updated.
*////////////////////////////////////////////////////////////////////////////////////

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

	if (piece == 'K') {
		whiteKingRank = destRank;
		whiteKingFile = destFile;
		moveCastle(initRank, initFile, destRank, destFile, piece);
		enPas = 0;
		hundredPlyCount++;
	}
	else if (piece == 'k') {
		blackKingRank = destRank;
		blackKingFile = destFile;
		moveCastle(initRank, initFile, destRank, destFile, piece);
		enPas = 0;
		hundredPlyCount++;
	}
	else if (piece == 'P' || piece == 'p') {
		movePawn(initRank, initFile, destRank, destFile, piece);
		updateScore(capturedPiece, -1);
		square[destRank][destFile] = piece;
		square[initRank][initFile] = 'O';
		hundredPlyCount = 0;
	}
	else {
		enPas = 0;
		updateScore(capturedPiece, -1);
		square[destRank][destFile] = piece;
		square[initRank][initFile] = 'O';
		capturedPiece == 'O' ? hundredPlyCount++ : hundredPlyCount = 0;
	}

	if (toMove == white)
		toMove = black;
	else {
		toMove = white;
		moveCount++;
	}

	FEN = getFEN();
	if (hundredPlyCount > 100) {
		score = 0;
		moveList[0] = "*";
		listPosition = 0;
		return;
	}

	//getMoves();
	staleCheckMate();
}

/*///////////////////////////////////////////////////////////////////////////////////
	This class is called from the makeMove() class if the move calls for a 
	promotion (e.g. e7d8q).  The promotion piece is held in the fifth element
	of the move (toFrom) string.  The score is updated and a char representing
	the promotion piece is returned.
*////////////////////////////////////////////////////////////////////////////////////
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

/*///////////////////////////////////////////////////////////////////////////////////
*	Called by makeMove() when the piece being moved is a king.  Primarily takes 
*	care of castling and castling rights as well as updating the score.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::moveCastle(int initRank, int initFile, int destRank, int destFile, char piece) {
	if (square[initRank][initFile] == 'K' && abs(destFile - initFile) == 2) {
		square[Rank_1][destFile] = square[Rank_1][initFile];
		square[Rank_1][initFile] = 'O';
		if (destFile == File_C) 
		{
			square[Rank_1][File_D] = square[Rank_1][File_A];
			square[Rank_1][File_A] = 'O';
		}
		else if (destFile == File_G) 
		{
			square[Rank_1][File_F] = square[Rank_1][File_H];
			square[Rank_1][File_H] = 'O';
		}
	}
	else if (square[initRank][initFile] == 'k' && abs(destFile - initFile) == 2) 
	{
		square[Rank_8][destFile] = square[Rank_8][initFile];
		square[Rank_8][initFile] = 'O';
		if (destFile == File_C) 
		{
			square[Rank_8][File_D] = square[Rank_8][File_A];
			square[Rank_8][File_A] = 'O';
		}
		else if (destFile == File_G) 
		{
			square[Rank_8][File_F] = square[Rank_8][File_H];
			square[Rank_8][File_H] = 'O';
		}
	}
	else 
	{
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
	{
		castleQueenW = false;
	}
	else if ((initRank == Rank_1 && initFile == File_H && piece == 'R') ||
			 (destRank == Rank_1 && destFile == File_H))
	{
		castleKingW = false;
	}
	else if (initRank == Rank_8 && initFile == File_E &&
		piece == 'k')
	{
		castleKingB = false;
		castleQueenB = false;
	}
	else if ((initRank == Rank_8 && initFile == File_A && piece == 'r') ||
			 (destRank == Rank_8 && destFile == File_A))
	{
		castleQueenB = false;
	}
	else if ((initRank == Rank_8 && initFile == File_H && piece == 'r') ||
			 (destRank == Rank_8 && destFile == File_H))
	{
		castleKingB = false;
	}
}

/*///////////////////////////////////////////////////////////////////////////////////
	Called by makeMove() when the piece being moved is a pawn.  Takes care of
	en passant, en passant rights, and updates the score.
*////////////////////////////////////////////////////////////////////////////////////
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
	{
		updateScore(capturedPiece, -1);
		square[destRank][destFile] = piece;
		square[initRank][initFile] = 'O';
		enPas = 0;
	}


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

/*///////////////////////////////////////////////////////////////////////////////////
	Called by the makeMove() class to return the board to the same state it was 
	before a move was made.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::undoMakeMove(std::string toFrom, std::string fen) {
	int initRank = toFrom[1] - 49;
	int destRank = toFrom[3] - 49;
	int initFile = toFrom[0] - 97;
	int destFile = toFrom[2] - 97;

	undoTestMove(capturedPiece, initRank, initFile, destRank, destFile);

	if (promotePawn) 
	{
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
	whiteKingRank = prevWhiteKingRank;
	whiteKingFile = prevWhiteKingFile;
	blackKingRank = prevBlackKingRank;
	blackKingFile = prevBlackKingFile;

	FEN = fen;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////      Get Moves      //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*///////////////////////////////////////////////////////////////////////////////////
	Called by the initialization classes as well as makeMove().  Loops through
	each square and determines the legal moves for each piece.  The moves are
	stored as strings in the moveList[] array.
*////////////////////////////////////////////////////////////////////////////////////
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

/*///////////////////////////////////////////////////////////////////////////////////
	Called by getMoves() when the piece is a Knight.  Checks each of the eight 
	possible Knight moves and then calls testMove() to make sure the King is
	not in check.  Accepts the rank and file of the Knight as paramaters.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::testKnightMoves(int rank, int file) {
	if (rank + 1 <= Rank_8 && file + 2 <= File_H) 
		if (!isAlly(rank + 1, file + 2))
			testMove(rank, file, rank + 1, file + 2);

	if (rank + 2 <= Rank_8 && file + 1 <= File_H) 
		if (!isAlly(rank + 2, file + 1))
			testMove(rank, file, rank + 2, file + 1);

	if (rank + 2 <= Rank_8 && file - 1 >= File_A) 
		if (!isAlly(rank + 2, file - 1))
			testMove(rank, file, rank + 2, file - 1);

	if (rank + 1 <= Rank_8 && file - 2 >= File_A) 
		if (!isAlly(rank + 1, file - 2))
			testMove(rank, file, rank + 1, file - 2);

	if (rank - 1 >= Rank_1 && file - 2 >= File_A) 
		if (!isAlly(rank - 1, file - 2))
			testMove(rank, file, rank - 1, file - 2);

	if (rank - 2 >= Rank_1 && file - 1 >= File_A) 
		if (!isAlly(rank - 2, file - 1))
			testMove(rank, file, rank - 2, file - 1);

	if (rank - 2 >= Rank_1 && file + 1 <= File_H) 
		if (!isAlly(rank - 2, file + 1))
			testMove(rank, file, rank - 2, file + 1);

	if (rank - 1 >= Rank_1 && file + 2 <= File_H) 
		if (!isAlly(rank - 1, file + 2))
			testMove(rank, file, rank - 1, file + 2);
}

/*///////////////////////////////////////////////////////////////////////////////////
	Called by getMoves() when the piece is a Queen or Bishop.  Calls testRun()
	four times to test each of the diagonals.  Accepts the rank and file of
	the piece as paramaters.
*////////////////////////////////////////////////////////////////////////////////////
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

/*///////////////////////////////////////////////////////////////////////////////////
	Called by getMoves() when the piece is a Queen or Rook.  Calls testRun()
	four times to test each of the horizontal and vertical directions.  Accepts
	the rank and file of the piece as paramaters.
*////////////////////////////////////////////////////////////////////////////////////
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

/*///////////////////////////////////////////////////////////////////////////////////
	Called by testDiagonal() and testHorizontal() classes.  Determines whether
	to test a diagonal or horizonal run and loops through all the legal moves.
	Calls testMove() to make sure the King is not in check.  Accepts the piece's
	rank and file as well as the first diagonal or horizontal test square as 
	paramaters.  Returns if an illegal move is found.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::testRun(int rank, int file, int testRank, int testFile) {
	int rankInc = testRank - rank;
	int fileInc = testFile - file;

	while (testRank <= Rank_8 && testRank >= Rank_1 &&
		   testFile <= File_H && testFile >= File_A)
	{
		if (!isAlly(testRank, testFile)) 
		{
			testMove(rank, file, testRank, testFile);
			if (isEnemy(testRank, testFile))
				return;
			testRank += rankInc;
			testFile += fileInc;
		}
		else
			return;
	}

}

/*///////////////////////////////////////////////////////////////////////////////////
	Called by getMoves() when the piece is a King.  Checks each of the eight
	possible King moves as well as possible castle moves and then calls testMove() 
	to make sure the King is not in check.  Accepts the rank and file of the Knight
	as paramaters.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::testKingMoves(int rank, int file) {
	if (rank != Rank_8) 
	{
		if (!isAlly(rank + 1, file))
			testMove(rank, file, rank + 1, file);
		if (file != File_A) 
		{
			if (!isAlly(rank + 1, file - 1))
				testMove(rank, file, rank + 1, file - 1);
			if (!isAlly(rank, file - 1))
				testMove(rank, file, rank, file - 1);
		}
		if (file != File_H) 
		{
			if (!isAlly(rank + 1, file + 1))
				testMove(rank, file, rank + 1, file + 1);
			if (!isAlly(rank, file + 1))
				testMove(rank, file, rank, file + 1);
		}
	}
	if (rank != Rank_1) 
	{
		if (!isAlly(rank - 1, file))
			testMove(rank, file, rank - 1, file);
		if (file != File_A) 
		{
			if (!isAlly(rank - 1, file - 1))
				testMove(rank, file, rank - 1, file - 1);
			if (rank == Rank_8 && !isAlly(rank, file - 1))
				testMove(rank, file, rank, file - 1);
		}
		if (file != File_H) 
		{
			if (!isAlly(rank - 1, file + 1))
				testMove(rank, file, rank - 1, file + 1);
			if (rank == Rank_8 && !isAlly(rank, file + 1))
				testMove(rank, file, rank, file + 1);
		}
	}
	if (file == File_E) {
		if (rank == Rank_1) 
		{
			if (square[Rank_1][File_F] == 'O' && square[Rank_1][File_G] == 'O' &&
				!isAttacked(Rank_1, File_F)   && !isAttacked(Rank_1, File_G)   &&
				!isAttacked(Rank_1, File_E)   && castleKingW && square[Rank_1][File_H] == 'R')
			{
				testMove(Rank_1, File_E, Rank_1, File_G);
			}
			if (square[Rank_1][File_D] == 'O' && square[Rank_1][File_C] == 'O' &&
				square[Rank_1][File_B] == 'O' && !isAttacked(Rank_1, File_E)   &&
				!isAttacked(Rank_1, File_D)   && !isAttacked(Rank_1, File_C)   &&
				castleQueenW && square[Rank_1][File_A] == 'R')
			{
				testMove(Rank_1, File_E, Rank_1, File_C);
			}
		}
		else if (rank == Rank_8) 
		{
			if (square[Rank_8][File_F] == 'O' && square[Rank_8][File_G] == 'O' &&
				!isAttacked(Rank_8, File_F)   && !isAttacked(Rank_8, File_G)   &&
				!isAttacked(Rank_8, File_E)   && castleKingB && square[Rank_8][File_H] == 'r')
			{
				testMove(Rank_8, File_E, Rank_8, File_G);
			}
			if (square[Rank_8][File_D] == 'O' && square[Rank_8][File_C] == 'O' &&
				square[Rank_8][File_B] == 'O' && !isAttacked(Rank_8, File_E)   &&
				!isAttacked(Rank_8, File_D)   && !isAttacked(Rank_8, File_C)   &&
				castleQueenB && square[Rank_8][File_A] == 'r')
			{
				testMove(Rank_8, File_E, Rank_8, File_C);
			}
		}
	}
}

/*///////////////////////////////////////////////////////////////////////////////////
	Called by getMoves() when the piece is a Pawn.  Checks each of the 
	possible Pawn moves including enPassant and calls checkPawnMove() to confirm the
	move isn't blocked, then testMove() to make sure the King is not in check.  
	Accepts the rank and file of the Pawn as paramaters.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::testPawnMoves(int rank, int file) {
	if (toMove == white) {
		if (rank < Rank_8) 
			if (checkPawnMove(rank, file, rank + 1, file))
				testMove(rank, file, rank + 1, file);
		if (rank == Rank_2) 
			if (checkPawnMove(rank, file, rank + 2, file))
				testMove(rank, file, rank + 2, file);
		if (file > File_A) 
			if (checkPawnMove(rank, file, rank + 1, file - 1))
				testMove(rank, file, rank + 1, file - 1);
		if (file < File_H) 
			if (checkPawnMove(rank, file, rank + 1, file + 1))
				testMove(rank, file, rank + 1, file + 1);
	}
	else {
		if (rank > Rank_1) 
			if (checkPawnMove(rank, file, rank - 1, file))
				testMove(rank, file, rank - 1, file);
		if (rank == Rank_7) 
			if (checkPawnMove(rank, file, rank - 2, file))
				testMove(rank, file, rank - 2, file);
		if (file > File_A) 
			if (checkPawnMove(rank, file, rank - 1, file - 1))
				testMove(rank, file, rank - 1, file - 1);
		if (file < File_H) 
			if (checkPawnMove(rank, file, rank - 1, file + 1))
				testMove(rank, file, rank - 1, file + 1);
	}
}

/*///////////////////////////////////////////////////////////////////////////////////
	Called by testPawnMoves().  Returns true if the pawn move is not blocked.
	Accepts the start rankd and file as well as the destination rank and file
	as paramaters.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::checkPawnMove(int startRank, int startFile, int destRank, int destFile) {
	int upOrBack;
	toMove == white ? upOrBack = -1 : upOrBack = 1;

	if (startFile == destFile && square[destRank][destFile] == 'O') 
	{
		if (startRank - destRank == upOrBack)
			return true;
		else if (startRank - destRank == 2 * upOrBack &&
			square[destRank + upOrBack][destFile] == 'O')
			return true;
	}
	else if (startFile != destFile) 
	{
		if (isEnemy(destRank, destFile))
			return true;
		else if (moveCount - enPas <= 1 && startRank == epRank + upOrBack &&
				 abs(startFile - epFile) == 1 && destFile == epFile)
			return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////      Test Moves      ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*///////////////////////////////////////////////////////////////////////////////////
	Tests a move by moving the pieces on the board and testing whether the King is 
	in check.  When completed, resets the board to its initial state.  Accepts
	the rank and file of the piece to be moved and the rank and file of its 
	destination square as paramaters.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::testMove(int initRank, int initFile, int destRank, int destFile) {
	char capturedPiece = square[destRank][destFile];

	if (square[initRank][initFile] == 'K')
	{
		whiteKingRank = destRank;
		whiteKingFile = destFile;
	}
	else if (square[initRank][initFile] == 'k') 
	{
		blackKingRank = destRank;
		blackKingFile = destFile;
	}

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
	else if (square[initRank][initFile] == 'K' && abs(destFile - initFile) == 2) 
	{
		square[Rank_1][destFile] = square[Rank_1][initFile];
		square[Rank_1][initFile] = 'O';
		if (destFile == File_C) 
		{
			square[Rank_1][File_D] = square[Rank_1][File_A];
			square[Rank_1][File_A] = 'O';
		}
		else if (destFile == File_G)
		{
			square[Rank_1][File_F] = square[Rank_1][File_H];
			square[Rank_1][File_H] = 'O';
		}
	}
	else if (square[initRank][initFile] == 'k' && abs(destFile - initFile) == 2) 
	{
		square[Rank_8][destFile] = square[Rank_8][initFile];
		square[Rank_8][initFile] = 'O';
		if (destFile == File_C) 
		{
			square[Rank_8][File_D] = square[Rank_8][File_A];
			square[Rank_8][File_A] = 'O';
		}
		else if (destFile == File_G)
		{
			square[Rank_8][File_F] = square[Rank_8][File_H];
			square[Rank_8][File_H] = 'O';
		}
	}
	else 
	{
		square[destRank][destFile] = square[initRank][initFile];
		square[initRank][initFile] = 'O';
	}

	if (!legalMove())
		undoTestMove(capturedPiece, initRank, initFile, destRank, destFile);
	else
	{
		if ((destRank == Rank_8 && square[destRank][destFile] == 'P') ||
			(destRank == Rank_1 && square[destRank][destFile] == 'p'))
		{
			testPromote(initRank, initFile, destRank, destFile);
		}
		else 
		{
			moveList[listPosition] = "";
			moveList[listPosition] += (initFile + 97);
			moveList[listPosition] += (initRank + 49);
			moveList[listPosition] += (destFile + 97);
			moveList[listPosition] += (destRank + 49);
			listPosition++;
		}
		undoTestMove(capturedPiece, initRank, initFile, destRank, destFile);
	}
}

/*///////////////////////////////////////////////////////////////////////////////////
	Called by testMove() when the move is a promotion.  Accepts the rank and 
	file of the piece to be moved and the rank and file of its destination square 
	as paramaters.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::testPromote(int initRank, int initFile, int destRank, int destFile) {
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

/*///////////////////////////////////////////////////////////////////////////////////
	Called by testMove().  Returns true if the King is not in check.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::legalMove() {
	int kingRank = -1;
	int kingFile = -1;

	if (toMove == white) {
		if (isAttacked(whiteKingRank, whiteKingFile))
			return false;
		else
			return true;
	}
	else if (toMove == black) {
		if (isAttacked(blackKingRank, blackKingFile))
			return false;
		else
			return true;
	}
}

/*///////////////////////////////////////////////////////////////////////////////////
	Called by testMove() and undoMakeMove().  Resets the board class to the same 
	state as before the last move that was made.
*////////////////////////////////////////////////////////////////////////////////////
void BoardA::undoTestMove(char captured, int initRank, int initFile, int destRank, int destFile) {
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
		if (destRank == Rank_1) 
		{
			if (destFile == File_G)
			{
				square[Rank_1][File_E] = 'K';
				square[Rank_1][File_F] = 'O';
				square[Rank_1][File_G] = 'O';
				square[Rank_1][File_H] = 'R';
			}
			else 
			{
				square[Rank_1][File_E] = 'K';
				square[Rank_1][File_C] = 'O';
				square[Rank_1][File_D] = 'O';
				square[Rank_1][File_A] = 'R';
			}
		}
		else if (destRank == Rank_8) 
		{
			if (destFile == File_G) 
			{
				square[Rank_8][File_E] = 'k';
				square[Rank_8][File_F] = 'O';
				square[Rank_8][File_G] = 'O';
				square[Rank_8][File_H] = 'r';
			}
			else 
			{
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
	whiteKingRank = prevWhiteKingRank;
	whiteKingFile = prevWhiteKingFile;
	blackKingRank = prevBlackKingRank;
	blackKingFile = prevBlackKingFile;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////      Is Attacked      //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*///////////////////////////////////////////////////////////////////////////////////
	Called by several classes.  Determines if a particular square is attacked by
	enemy pieces.  This is useful for castling and determinig whether the King is
	in check.  Accepts the rank and file of the square in question as paramaters.
	Returns true if the square is attacked.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::isAttacked(int rank, int file) {
	if (diagonalAttack(rank, file) ||
		horizontalAttack(rank, file) ||
		knightAttack(rank, file))
	{
		return true;
	}
	else
		return false;
}

/*///////////////////////////////////////////////////////////////////////////////////
	Tests each (four possible) direction diagonally to determine whether an enemy
	piece is attacking a square.  Called by isAttacked() and accepts the rank and
	file of the square in question as paramaters.  Returns true if the square
	is attacked.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::diagonalAttack(int rank, int file) {
	int testRank = rank + 1;
	int testFile = file + 1;

	if (diagonalAttackHelper(rank, file, testRank, testFile))
		return true;

	testRank = rank + 1;
	testFile = file - 1;
	if (diagonalAttackHelper(rank, file, testRank, testFile))
		return true;

	testRank = rank - 1;
	testFile = file + 1;
	if (diagonalAttackHelper(rank, file, testRank, testFile))
		return true;

	testRank = rank - 1;
	testFile = file - 1;
	if (diagonalAttackHelper(rank, file, testRank, testFile))
		return true;

	return false;
}

/*///////////////////////////////////////////////////////////////////////////////////
	Helper method for diagonalAttack().  Accepts the rank and file of the square
	in question as well as the rank and file of the next square in a particular
	direction as paramaters.  Based on the direction, it tests each square on that
	diagonal until it finds an attacker, an ally piece, or the edge of the board.
	Returns true if the square is attacked.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::diagonalAttackHelper(int rank, int file, int testRank, int testFile) {
	int rankDirection = testRank - rank;
	int fileDirection = testFile - file;

	while (testRank <= Rank_8 && testRank >= Rank_1 &&
		   testFile <= File_H && testFile >= File_A)
	{
		if (isAlly(testRank, testFile))
			return false;
		else if (square[testRank][testFile] == 'O')
		{
			testRank += rankDirection;
			testFile += fileDirection;
			continue;
		}
		else if (isEnemy(testRank, testFile)   &&
			(square[testRank][testFile] == 'K' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'Q'  ||
			square[testRank][testFile] == 'B'  ||
			(square[testRank][testFile] == 'k' && abs(rank - testRank) == 1) ||
			square[testRank][testFile] == 'q'  ||
			square[testRank][testFile] == 'b'  ||
			(square[testRank][testFile] == 'P' && abs(rank - testRank) == 1  && rankDirection == -1) ||
			(square[testRank][testFile] == 'p' && abs(rank - testRank) == 1  && rankDirection == 1))
		{
			return true;
		}
		return false;
	}
	return false;
}

/*///////////////////////////////////////////////////////////////////////////////////
	Tests each (four possible) direction horizontally and vertically to determine
	whether an enemy piece is attacking a square.  Called by isAttacked() and accepts
	the rank and file of the square in question as paramaters.  Returns true if the
	square is attacked.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::horizontalAttack(int rank, int file) {
	int testRank = rank + 1;
	int testFile = file;

	if (horizontalAttackHelper(rank, file, testRank, testFile))
		return true;

	testRank = rank - 1;
	if (horizontalAttackHelper(rank, file, testRank, testFile))
		return true;

	testRank = rank;
	testFile = file + 1;
	if (horizontalAttackHelper(rank, file, testRank, testFile))
		return true;

	testFile = file - 1;
	if (horizontalAttackHelper(rank, file, testRank, testFile))
		return true;

	return false;
}

/*///////////////////////////////////////////////////////////////////////////////////
	Helper method for horizontalAttack().  Accepts the rank and file of the square
	in question as well as the rank and file of the next square in a particular
	direction as paramaters.  Based on the direction, it tests each square on that
	rank or file until it finds an attacker, an ally piece, or the edge of the board.
	Returns true if the square is attacked.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::horizontalAttackHelper(int rank, int file, int testRank, int testFile) {
	int rankDirection = testRank - rank;
	int fileDirection = testFile - file;

	while (testRank <= Rank_8 && testRank >= Rank_1 &&
		   testFile <= File_H && testFile >= File_A)
	{
		if (isAlly(testRank, testFile))
			return false;
		else if (square[testRank][testFile] == 'O')
		{
			testRank += rankDirection;
			testFile += fileDirection;
			continue;
		}
		else if (isEnemy(testRank, testFile)   &&
			(square[testRank][testFile] == 'K' && abs(file - testFile) == 1) ||
			square[testRank][testFile] == 'Q'  ||
			square[testRank][testFile] == 'R'  ||
			(square[testRank][testFile] == 'k' && abs(file - testFile) == 1) ||
			square[testRank][testFile] == 'q'  ||
			square[testRank][testFile] == 'r')
		{
			return true;
		}
		return false;
	}
	return false;
}

/*///////////////////////////////////////////////////////////////////////////////////
	Tests each of the eight possible Knight jumps to see whether a Knight is
	attacking a paticular square.  Accepts the rank and file of the square in
	question.  Returns true if the square is attacked.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::knightAttack(int rank, int file) {
	int testRank = rank + 2;
	int testFile = file + 1;

	if (knightAttackHelper(testRank, testFile))
		return true;

	testFile = file - 1;
	if (knightAttackHelper(testRank, testFile))
		return true;

	testRank = rank + 1;
	testFile = file + 2;
	if (knightAttackHelper(testRank, testFile))
		return true;

	testFile = file - 2;
	if (knightAttackHelper(testRank, testFile))
		return true;

	testRank = rank - 2;
	testFile = file + 1;
	if (knightAttackHelper(testRank, testFile))
		return true;

	testFile = file - 1;
	if (knightAttackHelper(testRank, testFile))
		return true;

	testRank = rank - 1;
	testFile = file + 2;
	if (knightAttackHelper(testRank, testFile))
		return true;

	testFile = file - 2;
	if (knightAttackHelper(testRank, testFile))
		return true;

	return false;
}

/*///////////////////////////////////////////////////////////////////////////////////
	Helper method for knightAttack().  Returns true if the square testRank, testFile
	is attacked.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::knightAttackHelper(int testRank, int testFile) {
	if (square[testRank][testFile] == 'N' ||
		square[testRank][testFile] == 'n')
	{
		if (testRank <= Rank_8 && testRank >= Rank_1 &&
			testFile <= File_H && testFile >= File_A &&
			isEnemy(testRank, testFile))
		{
			return true;
		}
	}
	return false;
}

/*///////////////////////////////////////////////////////////////////////////////////
	Determines whether a square contains an ally piece based on the toMove
	variable (white or black).  Accepts the rank and file of the square in
	question and returns true if it is an ally piece.
*////////////////////////////////////////////////////////////////////////////////////
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

/*///////////////////////////////////////////////////////////////////////////////////
	Determines whether a square contains an enemy piece based on the toMove
	variable (white or black).  Accepts the rank and file of the square in
	question and returns true if it is an enemy piece.
*////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////      Set Board      ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*///////////////////////////////////////////////////////////////////////////////////
	Sets the board including all state variables (e.g. toMove) to a particular 
	position.  Accepts a string which contains all information about the board
	and returns true if it is a legal FEN code.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::populateBoard(std::string fen) {
	int emptyCount = 0;
	int stringPlace = 0;
	char empty = 'O';
	char aPiece = ' ';

	for (int aRank = Rank_8; aRank >= Rank_1; aRank--) {
		for (int aFile = File_A; aFile <= File_H; aFile++) {
			if (emptyCount == 0) 
			{
				aPiece = fen[stringPlace++];
				if (aPiece == '/') 
				{
					aFile--;
					continue;
				}
				else if ((int)aPiece > 57)
					square[aRank][aFile] = aPiece;
				else 
				{
					emptyCount = (int)aPiece - 48;
					square[aRank][aFile] = 'O';
					emptyCount--;
				}
			}
			else 
			{
				square[aRank][aFile] = 'O';
				emptyCount--;
			}
		}
	}
	return populateData(fen, stringPlace);
}

/*///////////////////////////////////////////////////////////////////////////////////
	Called by populateBoard().  Sets the state variables of the BoardA class.
	Accepts a string which represents the FEN as well as an int that denotes
	the last character in the string that was read by the populateBoard() class.  
	Returns false if the FEN is improper.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::populateData(std::string fen, int stringPlace) {
	int rank = 0;
	int file = 0;
	bool colorFlag = false;
	bool epFlag = false;
	bool fiftyFlag = false;
	bool moveCountFlag = false;
	char aPiece = ' ';

	while ((stringPlace < fen.length()) && (stringPlace < SizeOfArray)) 
	{        
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

		else if (!colorFlag)
		{
			if (aPiece == 'w')
				toMove = white;
			else if (aPiece == 'b')
				toMove = black;
			else 
			{
				//                std::cout << "improper fen" << std::endl;
				return false;
			}
			colorFlag = true;
		}
		else if (!epFlag && (int)aPiece > 96) 
		{
			epFile = (int)aPiece - 97;
			aPiece = fen[stringPlace++];
			epRank = (int)aPiece - 49;
			epFlag = true;
		}
		else if (!fiftyFlag && (int)aPiece < 58) 
		{
			if (fen[stringPlace] == ' ')
			{
				int ones = (int)aPiece - 48;
				hundredPlyCount = ones;
				fiftyFlag = true;
			}
			else 
			{
				int tens = (int)aPiece - 48;
				aPiece = fen[stringPlace++];
				int ones = (int)aPiece - 49;
				hundredPlyCount = tens * 10 + ones;
				fiftyFlag = true;
			}
		}
		else if (!moveCountFlag) 
		{
			int tens = (int)aPiece - 48;
			int ones = 0;

			aPiece = fen[stringPlace];
			if ((int)aPiece > 47 && (int)aPiece < 58) 
			{
				ones = (int)aPiece - 48;
				moveCount = tens * 10 + ones;
			}
			else
				moveCount = tens;

			if (epFlag) 
			{
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

/*///////////////////////////////////////////////////////////////////////////////////
	Returns a string which reprents the FEN of the current board state.
*////////////////////////////////////////////////////////////////////////////////////
std::string BoardA::getFEN() {
	std::string fen = "";
	char letter = ' ';
	int emptyCount = 0;
	bool castle = false;

	for (int rank = Rank_8; rank >= Rank_1; rank--) 
	{
		for (int file = File_A; file <= File_H; file++) 
		{
			letter = square[rank][file];
			if (letter == 'O')
				emptyCount++;
			else if (emptyCount != 0) 
			{
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
/////////////////////////////       Score       ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void BoardA::determineScore() {
	if (staleCheckMate())
	{
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

/*///////////////////////////////////////////////////////////////////////////////////
	Returns true if there is a checkmate or stalemate on the board.  Sets the score
	of checkmate to +-1000 and stalemate to 0.
*////////////////////////////////////////////////////////////////////////////////////
bool BoardA::staleCheckMate() {
	int kingRank = -1;
	int kingFile = -1;

	if (moveList[0] == "*") 
	{
		int kingRank = -1;
		int kingFile = -1;

		if (toMove == white) 
		{
			kingRank = whiteKingRank;
			kingFile = blackKingFile;
		}
		else 
		{
			kingRank = blackKingRank;
			kingFile = blackKingFile;
		}

		if (isAttacked(kingRank, kingFile)) 
		{
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
/////////////////////////      Utility Methods      ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
