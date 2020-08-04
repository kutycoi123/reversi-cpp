#ifndef REVERSI_H
#define REVERSI_H
#include <vector>
#include <iostream>
#include <algorithm>
#include "reversi_utility.h"
#include "mcts.h"

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




class AI : public Player {
public:
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

	AI(const std::string& name) : Player(name) {
		init_board(board);	
	}

	~AI(){}
};

class AI_noob : public AI {
public:	
	Move next_move(const std::vector<Move>& possible_moves) override {
		int NTIMES = 4000;
		TreeNode root(id, board, NULL);
		expand(&root);
		for (int i = 0; i < NTIMES; ++i) {
			int rand_index = rand() % root.children.size();
			auto child = root.children[rand_index].get();
			auto result = playout(child);
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
			if (child->nWins / child->nLost > best_child->nWins / best_child->nLost) {
				best_child = child.get();
			}
		}
		auto best_move = best_child->move;
		update(board, best_move, this->id);
		return best_move;
	}
	AI_noob(const std::string& name) : AI(name) {}

	~AI_noob(){}
};
#endif