#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <math.h>
#define INF 2147483647.0

struct Move{
	int i, j;
};	

inline bool operator==(const Move& lhs, const Move& rhs) {
	return lhs.i == rhs.i && lhs.j == rhs.j;
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
	int curr_player_id;
	const Player* winner = NULL;
	int board[8][8];
	std::unique_ptr<Player> ply1;
	std::unique_ptr<Player> ply2;
	Reversi(std::unique_ptr<Player> ply1, std::unique_ptr<Player> ply2) :
			ply1(std::move(ply1)), ply2(std::move(ply2)) {
		this->ply1->set_id(1);
		this->ply2->set_id(-1);
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				board[i][j] = 0;				
			}
		}
		board[3][3] = -1;
		board[3][4] = 1;
		board[4][3] = 1;
		board[4][4] = -1;
		this->ply1->score = 2;
		this->ply2->score = 2;
		curr_player_id = 1; // first player

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
	void display_board(const std::vector<Move>& moves) {
		std::cout << " ";
		for (int i = 0; i < N; ++i) {
			std::cout << " " << i;
		}
		std::cout << "\n";
		for (int i = 0; i < N; ++i) {
			std::cout << i;
			for (int j = 0; j < N; ++j) {
				auto elem = board[i][j];
				std::cout << "|";
				Move m{i, j};
				if (std::find(moves.begin(), moves.end(), m) != moves.end()) {
					std::cout << "#";
				}else if (elem == 0) {
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

		while (true) {
			auto possible_moves_ply1 = possible_moves(ply1->id);
			display_board(possible_moves_ply1);
			if (possible_moves_ply1.size() == 0) {
				break;
			}
			auto ply1_move = get_player_move(ply1.get(), possible_moves_ply1);
			update(ply1_move, ply1.get());
			//ply2.acknowledge(ply1_move)

			auto possible_moves_ply2 = possible_moves(ply2->id);
			display_board(possible_moves_ply2);
			if (possible_moves_ply2.size() == 0) {
				break;
			}
			auto ply2_move = get_player_move(ply2.get(), possible_moves_ply2);
			update(ply2_move, ply2.get());
			// display_board();
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

std::vector<Move> possible_moves(int board[8][8], int player) {
	std::vector<Move> moves;
	return moves;
}
bool terminal_test(int board[8][8]) {
	return true;
}

struct TreeNode {
	Move move;
	const TreeNode* parent = NULL;
	int board[8][8];
	int nVisited = 0;
	int nWins = 0;
	int player_id; //player about to take move
	std::vector<std::unique_ptr<TreeNode>> children;
	TreeNode() {}
	TreeNode(int player_id, int b[8][8], const TreeNode* par) : parent(par), player_id(player_id) {
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				board[i][j] = b[i][j];
			}
		}
	}
	TreeNode* best_child() {
		auto heu = [](const TreeNode& node){
			if (node.nVisited == 0 || node.parent->nVisited == 0) {
				return INF;
			}
			return (double)node.nWins / node.nVisited + 1.4*(sqrt(log((double)node.parent->nVisited)) / node.nVisited);
		};
		double max = -1;
		TreeNode* best_child = NULL;
		for (auto& child : children) {
			double heu_score = heu(*child);
			if (heu_score > max) {
				max = heu_score;
				best_child = child.get();
			}
		}
		return best_child;
	}

};



class AI : public Player {
public:
	Move next_move(const std::vector<Move>& possible_moves) const override {
		return Move{0,0};
	}
	AI() : Player() {}
	AI(const std::string& name) : Player(name) {}

	TreeNode* select(TreeNode* node) {
		if (node != NULL && node->children.size() != 0) {
			return select(node->best_child());
		}
		return node;
	}
	TreeNode* expand(TreeNode* node) {
		if (node->children.size() == 0 && !terminal_test(node->board)) {
			auto moves = possible_moves(node->board, node->player_id);
			for (auto& move : moves) {
				auto child = std::make_unique<TreeNode>(node->player_id*-1, node->board, node);
				child->board[move.i][move.j] = node->player_id;
				node->children.push_back(std::move(child));
			}
		}
		return select(node);
	}
	Move monte_carlo_tree_search(int board[8][8], int N = 1000) {

		TreeNode root(id, board, NULL);
		for (int i = 0; i < N; ++i) {
			auto leaf = select(&root);
			auto child = expand(leaf);
		}
		return Move{0,0};
	}
	~AI(){}
};

int main(int argc, char**argv) {
	Reversi reversi {std::unique_ptr<Player>(new Human("Human 1")), std::unique_ptr<Player>(new Human("Human 2"))};
	reversi.run();
	return 0; 
}