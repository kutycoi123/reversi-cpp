#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#define INF 2147483647.0
#define N 8
struct Move{
	int i, j;
};	
void display_board(int board[N][N], const std::vector<Move>& moves = {}) {
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
}
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
	virtual Move next_move(const std::vector<Move>&) = 0;
	virtual void acknowledge(const std::vector<Move>&, int) = 0;
	void set_id(int id) {
		this->id = id;
	}
	Player() : score(0) {}
	Player(const std::string& name) : score(0), name(name){}
	~Player(){}
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
	void acknowledge(const std::vector<Move>& moves, int id) override {

	}
	Human() : Player() {}
	Human(const std::string& name) : Player(name) {}
	~Human(){}
};
int get_opponent_id(int id) {
	return id * -1;
}
class Reversi {
public:
	int curr_player_id;
	const Player* winner = NULL;
	int board[N][N];
	std::unique_ptr<Player> ply1;
	std::unique_ptr<Player> ply2;
	Reversi(std::unique_ptr<Player> ply1, std::unique_ptr<Player> ply2) :
			ply1(std::move(ply1)), ply2(std::move(ply2)) {
		this->ply1->set_id(1);
		this->ply2->set_id(-1);
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				board[i][j] = 0;				
			}
		}
		// board[N/2-1][N/2-1] = -1;
		// board[N/2-1][N/2] = 1;
		// board[N/2][N/2-1] = 1;
		// board[N/2][N/2] = -1;

		board[N/2-1][N/2-1] = 1;
		board[N/2-1][N/2] = -1;
		board[N/2][N/2-1] = -1;
		board[N/2][N/2] = 1;
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
			if (board[start_i][start_j] == 0) {
				return false;
			}
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
	void display_board(const std::vector<Move>& moves = {}) {
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
	Move get_player_move(Player* ply, std::vector<Move> moves) const {
		Move move;
		while (true) {
			move = ply->next_move(moves);
			std::cout << move.i << " " << move.j << "\n";
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
			//ply2->acknowledge(ply1_move, ply1->id);

			auto possible_moves_ply2 = possible_moves(ply2->id);
			display_board(possible_moves_ply2);
			if (possible_moves_ply2.size() == 0) {
				break;
			}
			auto ply2_move = get_player_move(ply2.get(), possible_moves_ply2);
			update(ply2_move, ply2.get());
			// display_board();
			//ply1->acknowledge(ply2_move, ply2->id);
		}
		auto ply1_score = ply1->score, ply2_score = ply2->score;
		if (ply1_score > ply2_score) {
			std::cout << "Player " << ply1->name << " has won with " << ply1_score << " scores \n";
		} else if (ply1_score < ply2_score) {
			std::cout << "Player " << ply2->name << " has won with " << ply2_score << " scores \n";
		} else {
			std::cout << "Draw!!!";
		}
	}
private:
	void update(Move move, Player* ply) {
		int id = ply->id;
		std::vector<Move> new_moves;
		board[move.i][move.j] = id;
		new_moves.push_back(move);
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
			if (move.i < N && move.i >= 0 && move.j < N && move.j >= 0 && 
				find_player_move(id, Move{move.i+delta.first, move.j+delta.second}, delta.first, delta.second)) {
				// if (move.i + delta.first >= 0 && move.i+delta.first < N && move.j + delta.second >= 0
				// 	&& move.j + delta.second < N
				// 	&& )
				for (int i = move.i, j = move.j;
					i + delta.first < N && i + delta.first >= 0 && j + delta.second < N && j + delta.second >= 0;
					i += delta.first, j += delta.second) {
					if (board[i+delta.first][j+delta.second] == opponent_id) {
						board[i+delta.first][j+delta.second] = id;
						new_moves.push_back(Move{i+delta.first, j+delta.second});
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
			ply2->acknowledge(new_moves, id);
		} else {
			ply1->score -= score;
			ply1->acknowledge(new_moves, id);
		}
	}
};



struct TreeNode {
	int board[N][N];
	int nVisited = 0;
	int nWins = 0;
	int player_id; //player about to take move
	Move move;
	TreeNode* parent = NULL;	
	std::vector<std::unique_ptr<TreeNode>> children;
	TreeNode() {}
	TreeNode(int player_id, int b[N][N], TreeNode* par) : player_id(player_id), parent(par) {
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
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
bool find_player_move(int board[N][N], int id, Move start, int i_delta, int j_delta) {
	for (int start_i = start.i, start_j=start.j;
		start_i >= 0 && start_i < N && start_j >= 0 && start_j < N;
		start_i += i_delta, start_j += j_delta) {
		if (board[start_i][start_j] == 0) {
			return false;
		}
		else if (board[start_i][start_j] == id) {
			return true;
		}
	}
	return false;
}
std::vector<Move> possible_moves(int board[N][N], int id) {
	std::vector<Move> moves;
	auto opponent_id = get_opponent_id(id);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (board[i][j] == 0) {
				bool legal = (i < N - 1 && board[i+1][j] == opponent_id && find_player_move(board, id, Move{i+1, j}, 1, 0)) || 
							 (i >= 1 && board[i-1][j] == opponent_id && find_player_move(board, id, Move{i-1,j}, -1, 0)) || 
							 (j < N - 1 && board[i][j+1] == opponent_id && find_player_move(board, id, Move{i,j+1}, 0, 1)) || 
							 (j >= 1 && board[i][j-1] == opponent_id && find_player_move(board, id, Move{i,j-1}, 0, -1)) || 
							 (i < N - 1 && j < N - 1 && board[i+1][j+1] == opponent_id && find_player_move(board, id, Move{i+1, j+1}, 1, 1)) ||
							 (i >= 1 && j >= 1 && board[i-1][j-1] == opponent_id && find_player_move(board, id, Move{i-1, j-1}, -1, -1)) || 
							 (i >= 1 && j < N - 1 && board[i-1][j+1] == opponent_id && find_player_move(board, id, Move{i-1, j+1}, -1, 1)) || 
							 (i < N - 1 && j >= 1 && board[i+1][j-1] == opponent_id && find_player_move(board, id, Move{i+1, j-1}, 1, -1));
				if (legal) {
					moves.push_back(Move{i, j});
				}
			}
		}
	}

	return moves;
}
int get_winner(int board[N][N]) {
	int ply1_count = 0, ply2_count = 0;
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			if (board[i][j] == -1) 
				ply1_count++;
			else if (board[i][j] == 1) 
				ply2_count++;
		}
	}
	if (ply1_count > ply2_count) 
		return -1;
	else if (ply1_count < ply2_count)
		return 1;
	else
		return 0;
}
void update(int board[N][N], Move move, int id) {
	board[move.i][move.j] = id;
	auto opponent_id = get_opponent_id(id);
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
		if (move.i < N && move.i >= 0 && move.j < N && move.j >= 0 && 
			find_player_move(board, id, Move{move.i+delta.first, move.j+delta.second}, delta.first, delta.second)) {
			for (int i = move.i, j = move.j;
				i + delta.first < N && i + delta.first >= 0 && j + delta.second < N && j + delta.second >= 0;
				i += delta.first, j += delta.second) {
				if (board[i+delta.first][j+delta.second] == opponent_id) {
					board[i+delta.first][j+delta.second] = id;
				}else {
					break;
				}
			}
		}
	}
}

class AI : public Player {
public:
	int board[N][N];
	void acknowledge(const std::vector<Move>& moves, int id) override {
		for (auto& move : moves) {
			board[move.i][move.j] = id;
		}
	}
	Move next_move(const std::vector<Move>& possible_moves) override {
		std::cout << "moves size: " << possible_moves.size() << "\n";
		auto best_move = monte_carlo_tree_search(board);
		// board[best_move.i][best_move.j] = this->id;
		update(board, best_move, this->id);
		return best_move;
	}
	AI() : Player() {
		// board[N/2-1][N/2-1] = -1;
		// board[N/2-1][N/2] = 1;
		// board[N/2][N/2-1] = 1;
		// board[N/2][N/2] = -1;
		board[N/2-1][N/2-1] = 1;
		board[N/2-1][N/2] = -1;
		board[N/2][N/2-1] = -1;
		board[N/2][N/2] = 1;		
	}
	AI(const std::string& name) : Player(name) {
		// board[N/2-1][N/2-1] = -1;
		// board[N/2-1][N/2] = 1;
		// board[N/2][N/2-1] = 1;
		// board[N/2][N/2] = -1;
		board[N/2-1][N/2-1] = 1;
		board[N/2-1][N/2] = -1;
		board[N/2][N/2-1] = -1;
		board[N/2][N/2] = 1;		
	}

	TreeNode* select(TreeNode* node) {
		if (node != NULL && node->children.size() != 0) {
			return select(node->best_child());
		}
		return node;
	}
	TreeNode* expand(TreeNode* node) {
		if (node->children.size() == 0 ) {
			auto moves = possible_moves(node->board, node->player_id);
			if (moves.size() > 0) {
				for (auto& move : moves) {
					auto child = std::make_unique<TreeNode>(node->player_id*-1, node->board, node);
					child->move = Move{move.i, move.j};
					// child->board[move.i][move.j] = node->player_id;
					update(child->board, child->move, node->player_id);
					node->children.push_back(std::move(child));
				}
			}
		}
		return select(node);
	}
	int playout(TreeNode* node) {
		auto cur_player = node->player_id;
		int copied_board[N][N];
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				copied_board[i][j] = node->board[i][j];
			}
		}
		while (true) {
			// Get all possible moves
			auto moves = possible_moves(copied_board, cur_player);
			if (moves.size() == 0) {
				break;
			}
			// Randomly choose a move among possible moves
			int rand_index = rand() % moves.size();
			Move rand_move = moves[rand_index];
			// Update the copied_board
			//copied_board[rand_move.i][rand_move.j] = cur_player;
			update(copied_board, rand_move, cur_player);
			cur_player *= -1;
		}
		int winner = get_winner(copied_board);
		if (winner == 0) 
			return 0.5;
		if (winner == node->player_id)
			return -1;
		return 1;
	}
	void backprop(TreeNode* node, int result){
		if (result > 0) {
			node->nWins += result;
		}
		node->nVisited++;
		if (node->parent) {
			backprop(node->parent, -result);
		}
	}
	Move monte_carlo_tree_search(int board[N][N], int NTIMES = 5000) {

		TreeNode root(id, board, NULL);
		// expand(&root);
		// return Move{0,0};
		for (int i = 0; i < NTIMES; ++i) {
			auto leaf = select(&root);
			auto child = expand(leaf);
			//return Move{0,0};
			auto result = playout(child);
			backprop(child, result);
		}
		auto best_child = root.children[0].get();
		std::cout << "possible moves size: " << possible_moves(root.board, root.player_id).size() << "\n";
		// display_board(root.board, possible_moves(root.board, root.player_id));
		for (auto& child : root.children) {
			std::cout << child->move.i << " " << child->move.j << "\n";
			if (child->nVisited > best_child->nVisited) {
				best_child = child.get();
			}
		}
		return best_child->move;
	}
	~AI(){}
};

int main(int argc, char**argv) {
	srand (time(NULL));
	//Reversi reversi {std::unique_ptr<Player>(new Human("Human 1")), std::unique_ptr<Player>(new Human("Human 2"))};
	// Reversi reversi {std::unique_ptr<Player>(new Human("Human")),
	// 				std::unique_ptr<Player>(new AI("AI"))};
	Reversi reversi {std::unique_ptr<Player>(new AI("AI")),
				std::unique_ptr<Player>(new Human("Human"))};
	reversi.run();

	return 0; 
}