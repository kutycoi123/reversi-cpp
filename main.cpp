#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <utility>
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
	int score;
	std::string name;
	virtual Move next_move(const std::vector<Move>&) const = 0;
	void set_id(int id) {
		this->id = id;
	}
	Player() : score(0) {}
	Player(const std::string& name) : score(0), name(name){}
	~Player(){}
};
class AI : public Player {
public:
	Move next_move(const std::vector<Move>& possible_moves) const override {
		return Move{0,0};
	}
	AI() : Player() {}
	AI(const std::string& name) : Player(name) {}
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

	Human() : Player() {}
	Human(const std::string& name) : Player(name) {}
	~Human(){}
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
		this->ply1->score = 2;
		this->ply2->score = 2;

	}
	int get_opponent_id(int id) {
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
		auto opponent_id = get_opponent_id(id);
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (board[i][j] == 0) {
					bool legal = (i < N - 1 && board[i+1][j] == opponent_id && find_player_move(id, Move{i+1, j}, 1, 0)) || 
								 (i >= 1 && board[i-1][j] == opponent_id && find_player_move(id, Move{i-1,j}, -1, 0)) || 
								 (j < N - 1 && board[i][j+1] == opponent_id && find_player_move(id, Move{i,j+1}, 0, 1)) || 
								 (j >= 1 && board[i][j-1] == opponent_id && find_player_move(id, Move{i,j-1}, 0, -1)) || 
								 (i < N - 1 && j < N - 1 && board[i+1][j+1] == opponent_id && find_player_move(id, Move{i+1, j+1}, 1, 1)) ||
								 (i >= 1 && j >= 1 && board[i-1][j-1] == opponent_id && find_player_move(id, Move{i-1, j-1}, -1, -1)) || 
								 (i >= 1 && j < N - 1 && board[i-1][j+1] == opponent_id && find_player_move(id, Move{i-1, j+1}, -1, 1)) || 
								 (i < N - 1 && j >= 1 && board[i+1][j-1] == opponent_id && find_player_move(id, Move{i+1, j-1}, 1, -1));
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
		std::cout << ply1->name << ": " << ply1->score << "\n";
		std::cout << ply2->name << ": " << ply2->score << "\n";
	}
	Move get_player_move(const Player* ply, std::vector<Move> moves) const {
		Move move;
		while (true) {
			move = ply->next_move(moves);
			if (std::find(moves.begin(), moves.end(), move) == moves.end()) {
				std::cout << "Invalid move. Please choose another move\n";
				continue;
			}
			return move;
		}

	}

	void run() {
		display_board();
		while (true) {
			auto possible_moves_ply1 = possible_moves(ply1->id);
			if (possible_moves_ply1.size() == 0) {
				break;
			}
			auto ply1_move = get_player_move(ply1.get(), possible_moves_ply1);
			update(ply1_move, ply1.get());
			//ply2.acknowledge(ply1_move)
			display_board();
			auto possible_moves_ply2 = possible_moves(ply2->id);
			if (possible_moves_ply2.size() == 0) {
				break;
			}
			auto ply2_move = get_player_move(ply2.get(), possible_moves_ply2);
			update(ply2_move, ply2.get());
			display_board();
			// ply1.acknowledge(ply1_move)
		}
		auto ply1_score = ply1->score, ply2_score = ply2->score;
		if (ply1_score > ply2_score) {
			std::cout << "Player " << ply1->name << " has won with " << ply1_score << " score \n";
		} else if (ply1_score < ply2_score) {
			std::cout << "Player " << ply2->name << " has won with " << ply2_score << " score \n";
		} else {
			std::cout << "Draw!!!";
		}
	}
private:
	void update(Move move, Player* ply) {
		int id = ply->id;
		board[move.i][move.j] = id;
		ply->score += 1;
		auto opponent_id = get_opponent_id(id);
		int score = 0;
		std::vector<std::pair<int,int>> deltas;
		deltas.push_back(std::make_pair(0,1));
		deltas.push_back(std::make_pair(0,-1));
		deltas.push_back(std::make_pair(1,0));
		deltas.push_back(std::make_pair(-1,0));
		deltas.push_back(std::make_pair(1,-1));
		deltas.push_back(std::make_pair(-1,1));
		deltas.push_back(std::make_pair(-1,-1));
		deltas.push_back(std::make_pair(1,1));
		//TODO: refactor this ugly code
		for (const auto& delta : deltas) {
			if (move.i <= N - 2 && move.i >= 1 && move.j <= N - 2 && move.j >= 1 && 
				find_player_move(id, Move{move.i+delta.first, move.j+delta.second}, delta.first, delta.second)) {
				for (int i = move.i, j = move.j;
					i <= N - 2 && i >= 1 && j <= N - 2 && j >= 1;
					i += delta.first, j += delta.second) {
					if (board[i+delta.first][j+delta.second] == opponent_id) {
						board[i+delta.first][j+delta.second] = id;
						score++;
					}else {
						break;
					}
				}
			}
		}
		ply->score += score;
		if (id == ply1->id) {
			ply2->score -= score;
		} else {
			ply1->score -= score;
		}
	}
};



int main(int argc, char**argv) {
	Reversi reversi {std::unique_ptr<Player>(new Human("Human 1")), std::unique_ptr<Player>(new Human("Human 2"))};
	reversi.run();
	return 0; 
}