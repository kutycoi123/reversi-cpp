#include <vector>
#include <iostream>
#include <algorithm>
#include "reversi_utility.h"

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
	Human(const std::string& name) : Player(name) {}
	~Human(){}
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
		update(board, best_move, this->id);
		return best_move;
	}
	AI(const std::string& name) : Player(name) {
		init_board(board);	
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
		for (int i = 0; i < NTIMES; ++i) {
			auto leaf = select(&root);
			auto child = expand(leaf);
			//return Move{0,0};
			auto result = playout(child);
			backprop(child, result);
		}

		auto best_child = root.children[0].get();
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