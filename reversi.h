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
int get_opponent_id(int id) {
	return id * -1;
}
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
std::vector<Move> update(int board[N][N], Move move, int id) {
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