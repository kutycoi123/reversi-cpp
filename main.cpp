#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <time.h>
#include <stdlib.h>

#include "reversi.h"

/* Display the game board */
void display_board(int board[N][N], const Player* ply1, const Player*ply2, const std::vector<Move>& moves = {}) {

	std::cout << " ";
	for (int i = 0; i < N; ++i) {
		std::cout << " " << i;
	}
	std::cout << "\n";
	for (int i = 0; i < N*2+1; ++i) {
		if (i % 2 == 0) {
			std::cout << " -----------------\n";
		}else{
			std::cout << (i-1)/2;
			for (int j = 0; j < N; ++j) {
				auto elem = board[(i-1)/2][j];
				std::cout << "|";
				Move m{(i-1)/2, j};
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
	}
	std::cout << "Scores:  " << ply1->name << ": " << ply1->score  << "  "
			  << ply2->name << ": " << ply2->score << "\n";	

}
/* Main class for Reversi game */
struct Reversi {
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
			if (std::find(moves.begin(), moves.end(), move) == moves.end()) {
				std::cout << "Invalid move. Please choose another move\n";
				continue;
			}
			return move;
		}

	}

	/* Start then game */
	void run() {
		std::cout << "\n============ Game started!!! ===============\n";
		std::cout << "Symbols meaning:\n";
		std::cout << "    X: First player's move\n";
		std::cout << "    O: Second player's move\n";
		std::cout << "    #: Valid move that current player can take in their turns\n\n";
		while (true) {
			/* Player 1 turn */ 
			auto no_more_moves = false;
			auto possible_moves_ply1 = possible_moves(board, ply1->id);
			display_board(board, ply1.get(), ply2.get(), possible_moves_ply1);
			std::cout << "Current player: " << ply1->name << "\n";
			if (possible_moves_ply1.size() != 0) {
				auto ply1_move = get_player_move(ply1.get(), possible_moves_ply1);
				auto new_updated_moves = update(board, ply1_move, ply1->id);
				ply1->score += new_updated_moves.size();
				ply2->score -= new_updated_moves.size() - 1;
				ply2->acknowledge(new_updated_moves, ply1->id);
				std::cout << ply1->name << " made a move: " << "(" << ply1_move.i << "," << ply1_move.j << ")\n";
			} else {
				no_more_moves = true;
				std::cout << ply1->name << " has no valid moves to make\n";
			}
			std::cout << "\n";

			/* Player 2 turn */
			auto possible_moves_ply2 = possible_moves(board, ply2->id);
			display_board(board, ply1.get(), ply2.get(), possible_moves_ply2);
			std::cout << "Current player: " << ply2->name << "\n";						
			if (possible_moves_ply2.size() != 0) {
				auto ply2_move = get_player_move(ply2.get(), possible_moves_ply2);
				auto new_updated_moves = update(board, ply2_move, ply2->id);
				ply2->score += new_updated_moves.size();
				ply1->score -= new_updated_moves.size() - 1;
				ply1->acknowledge(new_updated_moves, ply2->id);		
				std::cout << ply2->name << " made a move: " << "(" << ply2_move.i << "," << ply2_move.j << ")\n";						
			} else {
				if (no_more_moves){
					std::cout << "No more valid moves for both players. Game end!!!\n";
					break;
				}else {
					std::cout << ply1->name << " has no valid moves to make\n";
				}
			}
			std::cout << "\n";
		}
		/* Print out scores of 2 players */
		auto ply1_score = ply1->score, ply2_score = ply2->score;
		if (ply1_score > ply2_score) {
			std::cout << "Player " << ply1->name << " has won with " << ply1_score << " scores \n";
			winner = ply1.get();
		} else if (ply1_score < ply2_score) {
			std::cout << "Player " << ply2->name << " has won with " << ply2_score << " scores \n";
			winner = ply2.get();
		} else {
			std::cout << "Draw!!!\n";
		}
	}
};


void human_vs_AI() {
	int option;
	while (true) {
		std::cout << "Choose 1 for human to go first, 2 for AI to go first: ";
		std::cin >> option;
		if (option == 1) {
			Reversi reversi(std::unique_ptr<Player>(new Human("Human")), 
							std::unique_ptr<Player>(new AI_MCTS("AI pure mcts")));
			reversi.run();
			break;
		} else if (option == 2) {
			Reversi reversi(std::unique_ptr<Player>(new AI_MCTS("AI pure mcts")), 
							std::unique_ptr<Player>(new Human("Human")));
			reversi.run();
			break;
		} else {
			std::cout << "Option not found\n";
		}
	}	
}
void AI_vs_AI() {
	Reversi reversi(std::unique_ptr<Player>(new AI_HEURISTIC("AI mcts with heuristic")), 
					std::unique_ptr<Player>(new AI_MCTS("AI pure mcts")));
	reversi.run();
}
int main(int argc, char**argv) {
	srand (time(NULL));
	int option;
	std::cout << "================= Welcome to Reversi ===================\n";
	std::cout << "1. Human vs AI\n";
	std::cout << "2. AI vs AI\n";
	while (true) {
		std::cout << "Please choose option 1 or 2: ";
		std::cin >> option;
		if (option == 1) {
			human_vs_AI();
			break;
		} else if (option == 2) {
			AI_vs_AI();
			break;
		} else {
			std::cout << "Option not found\n";
		}
	}
}

// Comment out above main function and 
// uncomment below code if you want to let both AI play against with each other for 100 games
/*
#include <thread>
#include <mutex>
int ai_heuristic_wins = 0;
int ai_mcts_wins = 0;
int draws = 0;
int NGAMES = 10;
std::mutex ai_heuristic_wins_mut;
std::mutex ai_mcts_wins_mut;
std::mutex draws_mut;

void run_reversi_thread() {
	Reversi reversi {std::unique_ptr<Player>(new AI_HEURISTIC("AI heuristic"))
					,std::unique_ptr<Player>(new AI_MCTS("AI mcts"))};
	reversi.run();
	if (reversi.winner == NULL) {
		std::lock_guard<std::mutex> guard(draws_mut);
		draws++;
	}else if (reversi.winner->id == 1) {
		std::lock_guard<std::mutex> guard(ai_heuristic_wins_mut);
		ai_heuristic_wins++;
	}else{
		std::lock_guard<std::mutex> guard(ai_mcts_wins_mut);
		ai_mcts_wins++;
	}
}
int main(int argc, char**argv) {
	srand (time(NULL));
	std::vector<std::thread> threads;
	for (int i = 0; i < NGAMES; ++i){
		std::cout << "===================== NEW GAME =========================\n";
		threads.push_back(std::thread(run_reversi_thread));
	}
	for (int i = 0; i < threads.size(); ++i) {
		threads[i].join();
	}
	std::cout << "AI heuristic mcts wins: " << ai_heuristic_wins << "\n";
	std::cout << "AI mcts wins: " << ai_mcts_wins << "\n";
	std::cout << "Draws: " << draws << "\n";
	return 0; 
}
*/
