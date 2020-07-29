#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>

struct Move{
	int i, j;
};	
class Player {
public:
	int id;
	virtual Move next_move(const std::vector<Move>&) = 0;
	void set_id(int id) {
		this->id = id;
	}
	Player(){}
	~Player(){}
};

class Reversi {
public:
	int N = 8;
	const Player* winner = NULL;
	std::vector<std::vector<int>> board;
	std::unique_ptr<Player> ply1;
	std::unique_ptr<Player> ply2;
	Reversi(std::unique_ptr<Player> ply1, std::unique_ptr<Player> ply2) :
			ply1(std::move(ply1)), ply2(std::move(ply2)) {
		this->ply1->set_id(-1);
		this->ply2->set_id(1);
		for (int i = 0; i < 8; ++i) {
			std::vector<int> row;
			for (int j = 0; j < 8; ++j) {
				row.push_back(0);
			}
			board.push_back(row);
		}

	}
	std::vector<Move> possible_moves() {
		std::vector<Move> moves;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (board[i][j] == 0) {
					moves.push_back(Move{i, j});
				}
			}
		}
		return moves;
	}
	void display_board() {
		std::cout << " ";
		for (int i = 0; i < N; ++i) {
			std::cout << " " << i;
		}
		std::cout << "\n";
		for (int i = 0; i < N; ++i) {
			auto row = board[i];
			std::cout << i;
			for (auto& elem : row) {
				std::cout << "|";
				if (elem == 0) {
					std::cout << " ";
				}else if (elem == -1) {
					std::cout << "X";
				}else {
					std::cout << "O";
				}
			}
			std::cout << "|\n";
		}
	}
	bool is_move_legal(Move move) {
		return true;
	}
	void run() {
		display_board();
		while (true) {
			auto ply1_move = ply1->next_move(possible_moves());
			if (!is_move_legal(ply1_move)) {
				continue;
			}
			update_board(ply1_move, ply1->id);
			//ply2.acknowledge(ply1_move)
			display_board();
			auto ply2_move = ply2->next_move(possible_moves());
			if (!is_move_legal(ply2_move)) {
				continue;
			}
			update_board(ply2_move, ply2->id);
			display_board();
			// ply1.acknowledge(ply1_move)
		}
	}
private:
	void update_board(Move move, int ply) {
		board[move.i][move.j] = ply;
	}

};

class AI : public Player {
public:
	Move next_move(const std::vector<Move>& possible_moves) override {
		return Move{0,0};
	}
	AI(){}
	~AI(){}
};

class Human : public Player {
public:
	Move next_move(const std::vector<Move>& possible_moves) override {
		int i, j;
		std::cout << "Choose row: ";
		std::cin >> i;
		std::cout << "Choose column: ";
		std::cin >> j;
		return Move{i,j};
	}

	Human(){}
	~Human(){}
};
int main(int argc, char**argv) {
	Reversi reversi {std::unique_ptr<Player>(new Human()), std::unique_ptr<Player>(new Human())};
	reversi.run();
	return 0; 
}