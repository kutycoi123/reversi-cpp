#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "reversi.h"

void display_board(int board[N][N], const Player* ply1, const Player*ply2, const std::vector<Move>& moves = {}) {
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
			}else if (elem == ply1->id) {
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

		init_board(board);
		this->ply1->score = 2;
		this->ply2->score = 2;
		curr_player_id = 1; // first player

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
			auto possible_moves_ply1 = possible_moves(board, ply1->id);
			display_board(board, ply1.get(), ply2.get(), possible_moves_ply1);
			if (possible_moves_ply1.size() == 0) {
				break;
			}
			auto ply1_move = get_player_move(ply1.get(), possible_moves_ply1);
			auto new_updated_moves = update(board, ply1_move, ply1->id);
			ply1->score += new_updated_moves.size();
			ply2->score -= new_updated_moves.size() - 1;
			ply2->acknowledge(new_updated_moves, ply1->id);


			auto possible_moves_ply2 = possible_moves(board, ply2->id);
			display_board(board, ply1.get(), ply2.get(), possible_moves_ply2);
			if (possible_moves_ply2.size() == 0) {
				break;
			}
			auto ply2_move = get_player_move(ply2.get(), possible_moves_ply2);
			new_updated_moves = update(board, ply2_move, ply2->id);
			ply2->score += new_updated_moves.size();
			ply1->score -= new_updated_moves.size() - 1;
			ply1->acknowledge(new_updated_moves, ply2->id);

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

};





int main(int argc, char**argv) {
	srand (time(NULL));
	//Reversi reversi {std::unique_ptr<Player>(new Human("Human 1")), std::unique_ptr<Player>(new Human("Human 2"))};
	// Reversi reversi {std::unique_ptr<Player>(new Human("Human")),
	// 				std::unique_ptr<Player>(new AI("AI"))};
	Reversi reversi {std::unique_ptr<Player>(new AI("AI")),
				std::unique_ptr<Player>(new Human("Human"))};
	// Reversi reversi {std::unique_ptr<Player>(new AI("AI 1")),
	// 			std::unique_ptr<Player>(new AI("AI 2"))};
	reversi.run();

	return 0; 
}