#include <stddef.h>
#include "chess.h"
#include "display.h"
#include <errno.h>

struct chess_piece board[8][8]; // grid of chess pieces

static int init_piece(struct chess_piece *piece, enum team_color color,
		enum piece_type type)
{
	if (piece == NULL) {
		return -EFAULT; // sanity check
	}

	// Set the information for the piece
	piece->color = color;
	piece->id = type;
	return 0;
}

int init_board(int has_color, enum team_color player) {
	// "Player" team color
	init_piece(&board[7][0], player, rook); // rooks
	init_piece(&board[7][7], player, rook);
	init_piece(&board[7][1], player, knight); // knights
	init_piece(&board[7][6], player, knight);
	init_piece(&board[7][2], player, bishop); // bishops
	init_piece(&board[7][5], player, bishop);
	init_piece(&board[7][3], player, queen); // queen
	init_piece(&board[7][4], player, king); // king

	// pawns
	for (int i = 0; i < 8; ++i) {
		init_piece(&board[6][i], player, pawn);
	}

	// empty spaces
	for (int i = 2; i < 6; ++i) {
		for (int j = 0; j < 8; ++j) {
			init_piece(&board[i][j], null, dead);
		}
	}

	// Other team color
	if (player == white) {
		player = black;
	} else {
		player = white;
	}
	init_piece(&board[0][0], player, rook); // rooks
	init_piece(&board[0][7], player, rook);
	init_piece(&board[0][1], player, knight); // knights
	init_piece(&board[0][6], player, knight);
	init_piece(&board[0][2], player, bishop); // bishops
	init_piece(&board[0][5], player, bishop);
	init_piece(&board[0][3], player, queen); // queen
	init_piece(&board[0][4], player, king); // king

	// pawns
	for (int i = 0; i < 8; ++i) {
		init_piece(&board[1][i], player, pawn);
	}

	return 0;
}
