#include "Search.h"
#include "BoardA.h"
#include "UCI.h"


using namespace std;


void boardA(string fen);
void search(string fen);


int main() {
	string startFen = "8/8/2p1k3/p1Kb2r1/8/8/1q6/8 w - - 0 71";


	//boardA(startFen);
	search(startFen);

	//UCI uci;

	//while (true) {
	//	std::cin >> startFen;
	//	if (startFen == "x")
	//		break;
	//}

	return 0;
}

void search(string fen) {
	//Search search; 
	Search search("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

	//    search.perft(4);
	for (int i = 0; i < 3; i++) {
		search.addPly();
		search.preferredVariation();
	}

}

void boardA(string fen) {
	BoardA board(fen);
	//    BoardA board;


	board.displayBoard();
	board.displayFen();
	//    board.getMoveList();   
	board.determineScore();
	std::cout << board.getScore();
}
