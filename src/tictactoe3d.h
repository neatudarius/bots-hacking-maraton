#include <cstring>

#define NMAX 4

#define NO_PLAYER 0
#define CLIENT 1
#define SERVER 2

#define RES_WIN_CLIENT 1
#define RES_WIN_SERVER 2
#define RES_TIE 3
#define RES_CONTINUE 4

struct small_matrix {
	int sm[5][5];
};

typedef pair<int, int> Pair;

struct TicTacToe3D {
	small_matrix field[12];
	small_matrix macroboard;
	
	int next_x, next_y;
	int currentPlayer;

	TicTacToe3D() {
		for (int k = 1; k <= 9; ++k) {
			for (int i = 1; i <= 3; ++i) {
				for (int j = 1; j <= 3; ++j) {
					field[k].sm[i][j] = NO_PLAYER;
				}
			}
		}

		for (int i = 1; i <= 3; ++i) {
			for (int j = 1; j <= 3; ++j) {
				macroboard.sm[i][j] = NO_PLAYER;
			}
		}

		int i_big = 5;
		int j_big = 5;


		next_x = 2;
		next_y = 2;
		
		move(CLIENT, i_big, j_big);
	}


	Pair convert_big2small(Pair p) {
		int i_big = p.first, j_big = p.second;

		int i_small = i_big;
		int j_small = j_big;

		while (i_small > 3) i_small -= 3;
		while (j_small > 3) j_small -= 3;

		return Pair(i_small, j_small);
	}

	Pair convert_big2macro(Pair p) {
		int i_big = p.first, j_big = p.second;

		int i_macro = i_big / 3 + !(i_big % 3 == 0);
		int j_macro = j_big / 3 + !(j_big % 3 == 0);

		return Pair(i_macro, j_macro);		
	}

	int convert_big2square(Pair p) {
		p = convert_big2macro(p);

		int i = p.first, j = p.second;

		return (i - 1) * 3 + j;
	}

	bool player_won_small(int player, small_matrix& matrix) {
		int cnt_row[4] = {0, 0, 0};
		int cnt_col[4] = {0, 0, 0};
		int cnt_diag[4] = {0, 0, 0};

		for (int i = 1; i <= 3; ++i) {
			for (int j = 1; j <= 3; ++j) {
				if (matrix.sm[i][j] == player) {
					cnt_row[i]++;
					cnt_col[j]++;

					if (i == j) {
						cnt_diag[0]++;
					}

					if (j == 4 - i) {
						cnt_diag[1]++;
					}
				}
			}
		}

		for (int i = 1; i <= 3; ++i) {
			if (cnt_row[i] == 3 || cnt_col[i] == 3) {
				return true;
			}
		}

		return cnt_diag[0] == 3 || cnt_diag[1] == 3;
	}

	void update_macroboard() {
		int k = 0;
		for (int i = 1; i <= 3; ++i) {
			for (int j = 1; j <= 3; ++j) {
				k++;
				if (player_won_small(CLIENT, field[k])) {
					macroboard.sm[i][j] = CLIENT;
				} else 	if (player_won_small(SERVER, field[k])) {
					macroboard.sm[i][j] = SERVER;
				} else {
					macroboard.sm[i][j] = NO_PLAYER;
				}

				// cout << macroboard.sm[i][j] << ' ';
			}
			// cout << '\n';
		}

		// cout << "next: " << next_x << " " << next_y << '\n';
	}

	bool player_won_game(int player) {
		return player_won_small(player, macroboard);
	}


	bool is_done() {
		return player_won_game(CLIENT) || player_won_game(SERVER);
	}

	bool check_is_valid(int i_big, int j_big) {
		if (!i_big || !j_big || i_big > 9 || j_big > 9) {
			return false;
		}

		int k = convert_big2square(Pair(i_big, j_big));

		auto p_small = convert_big2small({i_big, j_big });
		int i_small = p_small.first, j_small = p_small.second;

		auto p_macro = convert_big2macro({i_big, j_big });
		int i_macro = p_macro.first, j_macro = p_macro.second;

		// verificat tot pt small
		if (field[k].sm[i_small][j_small] != NO_PLAYER ||  macroboard.sm[i_macro][j_macro] != NO_PLAYER ) {
			// cout << "small is busy\n";
			return false;
		}

		// celula pe care o vreau eu este in cea 3x3 in care trebuie
		if ((i_macro == next_x && j_macro == next_y) && macroboard.sm[next_x][next_y] == NO_PLAYER) {
			// cout << "cell is in target and target is not conq\n";
			return true;
		}


		// celula pe care o vreau eu NU este in cea 3x3 in care trebuie, dar cea care trebuie e FULL
		if (!(i_macro == next_x && j_macro == next_y) && macroboard.sm[next_x][next_y] != NO_PLAYER) {
			// cout << "cell is not in target and but target is conq" << i_macro << " " << j_macro << " " << next_x << " " << next_y <<   "\n";
			return true;
		}

		return false;
	}

	int move(int player, int i_big, int j_big) {
		this->currentPlayer = player;

		auto p = convert_big2small(Pair(i_big, j_big));
		int i_small = p.first;
		int j_small = p.second;
		int k = convert_big2square(Pair(i_big, j_big));

		field[k].sm[i_small][j_small] = player;
		// if (player_won_small(player, field[k].sm)) {
			
		// }

		next_x = i_small;
		next_y = j_small;
		update_macroboard();

		return 0;
	}

	int get_val(int i_big, int j_big) {
		auto p = convert_big2small(Pair(i_big, j_big));
		int i_small = p.first;
		int j_small = p.second;

		auto p_macro = convert_big2macro(Pair(i_big, j_big));
		int i_macro = p_macro.first, j_macro = p_macro.second;


		int k = convert_big2square(Pair(i_big, j_big));

		// cout << i_big << ' ' << j_big << ':' << k << ':' <<  i_small << ' ' << j_small << ':'  << i_macro << ' ' << j_macro << '\n';
		// cout << "\n";

		return field[k].sm[i_small][j_small];
	}

	string to_string() {
		string s = "";

		for (int i = 1; i <= 9; ++i) {
			for (int j = 1; j <= 9; ++j) {
				s += (get_val(i, j) + '0');
				s += " ";

				if (j % 3 == 0) {
					s += "\t";
				}
			}
			s += "\n";
			if (i % 3 == 0) {
				s += "\n";
			}
		}

		return string(s);
	}
};

