#ifndef REVERSI_UTILITY
#define REVERSI_UTILITY
#include <vector>
#include <iostream>
#include <algorithm>
#define INF 2147483647.0
#define N 8

struct Move{
	int i, j;
};	

inline bool operator==(const Move& lhs, const Move& rhs) {
	return lhs.i == rhs.i && lhs.j == rhs.j;
}

inline bool operator!=(const Move& lhs, const Move& rhs) {
	return !(lhs == rhs);
}

static void init_board(int b[N][N]) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			b[i][j] = 0;
		}
	}
	b[N/2-1][N/2-1] = 1;
	b[N/2-1][N/2] = -1;
	b[N/2][N/2-1] = -1;
	b[N/2][N/2] = 1;
}

static int get_opponent_id(int id) {
	return id * -1;
}

static bool find_player_move(int board[N][N], int id, Move start, int i_delta, int j_delta) {
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

static std::vector<Move> possible_moves(int board[N][N], int id) {
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
static int get_winner(int board[N][N]) {
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

static std::vector<Move> update(int board[N][N], Move move, int id) {
	std::vector<Move> new_moves;
	new_moves.push_back(move);
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
	for (const auto& delta : deltas) {
		if (move.i < N && move.i >= 0 && move.j < N && move.j >= 0 && 
			find_player_move(board, id, Move{move.i+delta.first, move.j+delta.second}, delta.first, delta.second)) {
			for (int i = move.i, j = move.j;
				i + delta.first < N && i + delta.first >= 0 && j + delta.second < N && j + delta.second >= 0;
				i += delta.first, j += delta.second) {
				if (board[i+delta.first][j+delta.second] == opponent_id) {
					board[i+delta.first][j+delta.second] = id;
					new_moves.push_back(Move{i+delta.first, j+delta.second});
				}else {
					break;
				}
			}
		}
	}
	return new_moves;
}
#endif