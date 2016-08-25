#include "UCI.h"



UCI::UCI()
{
	std::cout.setf(std::ios::unitbuf);
	stream.open("C:\\Users\\cburd\\Desktop\\program output.txt");

	std::string command = " ";
	while (std::cin >> command) {
		std::string input = " ";

		if (!stream.is_open()) {
			std::cout << "error\n";
			return;
		}
		else {
			stream << "working\n";
		}

		if (command == "uci") {
			std::cout << "id name Slow Chess\n";
			std::cout << "id author Clifton Burdette\n";

			stream << "command\n";
			stream << "id name Slow Chess\n";
			stream << "id author Clifton Burdette\n";

			board = new BoardA(StartPosition);
			std::cout << "uciok\n";
			stream << "uciok\n";
			wait();
			return;
		}
		else if (command == "xboard") {
			std::cout << "unknown command" << command << "\n";
			stream << "unknown command" << command << "\n";
		}
		else {
			stream << command << "\n";
		}
	}
}


UCI::~UCI()
{
}

void UCI::wait() {
	std::string command = " ";
	while (std::cin >> command) {
		stream << command << "\n";
		if (command == "isready")
			isReady();
		else if (command == "setoption")
			setOption();
		else if (command == "register")
			std::cout << "register later" << std::endl;
		else if (command == "ucinewgame")
			newGame();
		else if (command == "position")
			position(command);
		else if (command == "go")
			go();
		else if (command == "quit") {
			quit();
			return;
		}
	}
}

void UCI::quit() {
	stream.close();
}

void UCI::go() {
	std::string command = " ";
	std::string word = " ";

	while (std::cin >> command) {
		stream << command << "\n";
		if (command == "searchmoves")
			searchMoves();
		else if (command == "ponder") {
			ponder();
			return;
		}
		else if (command == "infinite") {
			ponder();
			return;
		}
		else if (command == "isready")
			isReady();
		else if (command == "quit" || command == "stop")
			return;
		else
			board->makeMove(word);
	}
}

void UCI::searchMoves() {
	std::string move = " ";
	while (std::cin >> move) 
		board->makeMove(move);
}

void UCI::ponder() {
	std::string command = " ";

	std::string fen = board->getFEN();
	Search search(fen);
	int searchDepth = 0;
	int peekValue = 0;

	while (searchDepth < 3) {
		search.addPly();
		bestMove = search.getBestMove();
		searchDepth++;
		//std::cout << searchDepth << "\n";
		
		peekValue = std::cin.peek();
		if (peekValue >= (int)' ') {
			std::cin >> command;
			stream << command;
		}
		
		if (command == "stop")
			return;
		else if (command == "isready")
			isReady();
		else
			continue;
	}
	std::cout << "bestmove " << bestMove << "\n";
	stream << "bestmove " << bestMove << "\n";
}

void UCI::isReady() {
	std::cout << "readyok\n";
	stream << "readyok\n";
}

void UCI::setOption() {

}

void UCI::newGame() {
	board = new BoardA(StartPosition);
	std::cout << "readyok\n";
	stream << "readyok\n";
}

void UCI::position(std::string command) {
	std::istringstream ss(command);
	std::string word = " ";
	bool positionSet = false;

	while (std::cin >> word) {
		stream << word << "\n";
		if (word == "position")
			continue;
		else if (word == "startpos") {
			positionSet = true;
			return;
		}
		else if (word == "fen")
			continue;
		else if (!positionSet) {
			board->populateBoard(word);
			positionSet = true;
		}
		else if (command == "isready")
			isReady();
		else if (command == "quit" || command == "stop")
			return;
		else
			board->makeMove(word);
	}
}
