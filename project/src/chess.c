#include <stddef.h>
#include <errno.h>
#include <string.h>

#include "chess.h"

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

	// Copy the characters into the string
	if (type == dead) {
		strcpy(piece->print_char, " ");
		return 0;
	}

	if (color == black) {
		switch (type) {
			case king:
				strcpy(piece->print_char, "\u265a");
				return 0;
			case queen:
				strcpy(piece->print_char, "\u265b");
				return 0;
			case rook:
				strcpy(piece->print_char, "\u265c");
				return 0;
			case bishop:
				strcpy(piece->print_char, "\u265d");
				return 0;
			case knight:
				strcpy(piece->print_char, "\u265e");
				return 0;
			default: // pawn
				strcpy(piece->print_char, "\u265f");
				return 0;
		}
	}

	// white pieces
	switch (type) {
		case king:
			strcpy(piece->print_char, "\u2654");
			return 0;
		case queen:
			strcpy(piece->print_char, "\u2655");
			return 0;
		case rook:
			strcpy(piece->print_char, "\u2656");
			return 0;
		case bishop:
			strcpy(piece->print_char, "\u2657");
			return 0;
		case knight:
			strcpy(piece->print_char, "\u2658");
			return 0;
		default: // pawn
			strcpy(piece->print_char, "\u2659");
			return 0;
	}
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
