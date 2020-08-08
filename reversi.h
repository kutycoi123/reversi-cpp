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




struct AI_MCTS : Player {
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

struct AI_HEURISTIC : AI_MCTS {
	Move next_move(const std::vector<Move>& possible_moves) override {
		auto best_move = monte_carlo_tree_search_with_heu(board, id);
		update(board, best_move, this->id);
		return best_move;		
	}
	AI_HEURISTIC(const std::string& name) : AI_MCTS(name) {

	}

	~AI_HEURISTIC(){}
};
#endif