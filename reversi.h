#ifndef REVERSI_H
#define REVERSI_H
#include <vector>
#include <iostream>
#include <algorithm>
#include "reversi_utility.h"
#include "mcts.h"

struct Player {
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

struct Human : public Player {
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




struct AI_MCTS : public Player {
	int board[N][N];
	void acknowledge(const std::vector<Move>& moves, int id) override {
		for (auto& move : moves) {
			board[move.i][move.j] = id;
		}
	}

	Move next_move(const std::vector<Move>& possible_moves) override {
		auto best_move = monte_carlo_tree_search(board, id);
		update(board, best_move, this->id);
		return best_move;
	}

	AI_MCTS(const std::string& name) : Player(name) {
		init_board(board);	
	}

	~AI_MCTS(){}
};

struct AI_HEURISTIC : public AI_MCTS {
	bool is_good_move(const Move& move) {
		// https://guides.net4tv.com/games/how-win-reversi#:~:text=The%20basic%20moves%20of%20Reversi,your%20stone%20in%20that%20square.
		std::vector<Move> good_moves = {
			Move{0,0}, Move{0,2}, Move{0,5}, Move{0,7},
			Move{2,0}, Move{2,2}, Move{2,5}, Move{2,7},
			Move{5,0}, Move{5,2}, Move{5,5}, Move{5,7},
			Move{7,0}, Move{7,2}, Move{7,5}, Move{7,7}
		};
		return std::find(good_moves.begin(), good_moves.end(), move) != good_moves.end();
	}
	Move next_move(const std::vector<Move>& possible_moves) override {
		int NTIMES = 4000;
		TreeNode root(id, board, NULL);
		expand(&root);
		for (int i = 0; i < NTIMES; ++i) {
			int rand_index = rand() % root.children.size();
			auto child = root.children[rand_index].get();
			auto result = playout(child);
			// Adding points for "good" child based on its move
			if (is_good_move(child->move)) {
				child->nWins += 0.75;
			}
			if (result.first == 0) { // Draw
				child->nWins += 0.5;
			} else if (result.first == child->player_id) { // Wins
				child->nWins += 1;
			} else{ // Loses
				child->nLost += 1;
			}
		}
		auto best_child = root.children[0].get();
		for (auto& child : root.children) {
			if (child->nWins / (child->nLost + 1) > best_child->nWins / (best_child->nLost + 1)) {
				best_child = child.get();
			}
		}
		auto best_move = best_child->move;
		update(board, best_move, this->id);
		return best_move;
	}
	AI_HEURISTIC(const std::string& name) : AI_MCTS(name) {}

	~AI_HEURISTIC(){}
};
#endif