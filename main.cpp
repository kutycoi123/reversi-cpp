#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>

struct Move{
	int i, j;
};	
inline bool operator==(const Move& lhs, const Move& rhs) {
	return lhs.i == rhs.i && lhs.i == rhs.i;
}
inline bool operator!=(const Move& lhs, const Move& rhs) {
	return !(lhs == rhs);
}
class Player {
public:
	int id;
	virtual Move next_move(const std::vector<Move>&) const = 0;
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
		this->ply1->set_id(1);
		this->ply2->set_id(-1);
		for (int i = 0; i < 8; ++i) {
			std::vector<int> row;
			for (int j = 0; j < 8; ++j) {
				row.push_back(0);
			}
			board.push_back(row);
		}
		board[3][3] = -1;
		board[3][4] = 1;
		board[4][3] = 1;
		board[4][4] = -1;

	}
	int opponent_id(int id) {
		return id * -1;
	}
	bool find_player_move(int id, Move start, int i_delta, int j_delta) {
		for (int start_i = start.i, start_j=start.j;
			start_i >= 0 && start_i < N && start_j >= 0 && start_j < N;
			start_i += i_delta, start_j += j_delta) {
			if (board[start_i][start_j] == id) {
				return true;
			}
		}
		return false;
	}
	std::vector<Move> possible_moves(int id) {
		std::vector<Move> moves;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (board[i][j] == 0) {
					bool legal = (i < N - 1 && board[i+1][j] == opponent_id(id) && find_player_move(id, Move{i+1, j}, 1, 0)) || 
								 (i >= 1 && board[i-1][j] == opponent_id(id) && find_player_move(id, Move{i-1,j}, -1, 0)) || 
								 (j < N - 1 && board[i][j+1] == opponent_id(id) && find_player_move(id, Move{i,j+1}, 0, 1)) || 
								 (j >= 1 && board[i][j-1] == opponent_id(id) && find_player_move(id, Move{i,j-1}, 0, -1)) || 
								 (i < N - 1 && j < N - 1 && board[i+1][j+1] == opponent_id(id) && find_player_move(id, Move{i+1, j+1}, 1, 1)) ||
								 (i >= 1 && j >= 1 && board[i-1][j-1] == opponent_id(id) && find_player_move(id, Move{i-1, j-1}, -1, -1)) || 
								 (i >= 1 && j < N - 1 && board[i-1][j+1] == opponent_id(id) && find_player_move(id, Move{i-1, j+1}, -1, 1)) || 
								 (i < N - 1 && j >= 1 && board[i+1][j-1] == opponent_id(id) && find_player_move(id, Move{i+1, j-1}, 1, -1));
					if (legal) {
						moves.push_back(Move{i, j});
					}
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
				}else if (elem == 1) {
					std::cout << "X";
				}else {
					std::cout << "O";
				}
			}
			std::cout << "|\n";
		}
	}
	Move get_player_move(const Player* ply, std::vector<Move> moves) const {
		Move move;
		while (true) {
			move = ply->next_move(moves);
			if (std::find(moves.begin(), moves.end(), move) == moves.end()) {
				std::cout << "Invalid move. Please choose another move";
				continue;
			}
			return move;
		}

	}
	bool is_move_legal(Move move) {
		return true;
	}
	void run() {
		display_board();
		while (true) {
			auto possible_moves_ply1 = possible_moves(ply1->id);
			if (possible_moves_ply1.size() == 0) {
				break;
			}
			auto ply1_move = get_player_move(ply1.get(), possible_moves_ply1);
			update_board(ply1_move, ply1->id);
			//ply2.acknowledge(ply1_move)
			display_board();
			auto possible_moves_ply2 = possible_moves(ply2->id);
			auto ply2_move = get_player_move(ply2.get(), possible_moves_ply2);
			update_board(ply2_move, ply2->id);
			display_board();
			// ply1.acknowledge(ply1_move)
		}
	}
private:
	void update_board(Move move, int ply_id) {
		board[move.i][move.j] = ply_id;
	}
};

class AI : public Player {
public:
	Move next_move(const std::vector<Move>& possible_moves) const override {
		return Move{0,0};
	}
	AI(){}
	~AI(){}
};

class Human : public Player {
public:
	Move next_move(const std::vector<Move>& possible_moves) const override {
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