#pragma once

#include <sstream>
#include <fstream>
#include <ios>

#include "Inits.h"

#include "Search.h"
#include "BoardA.h"

class UCI
{
public:
	UCI();
	~UCI();

	void wait();

	void isReady();
	void setOption();
	void newGame();
	void position(std::string command);
	void go();
	void searchMoves();
	void ponder();
	void quit();

private:
	BoardA* board;
	std::ofstream stream;
	std::string bestMove;
};

