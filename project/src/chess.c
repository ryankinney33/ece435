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
	// White color
	init_piece(&board[0][0], white, rook); // rooks
	init_piece(&board[0][7], white, rook);
	init_piece(&board[0][1], white, knight); // knights
	init_piece(&board[0][6], white, knight);
	init_piece(&board[0][2], white, bishop); // bishops
	init_piece(&board[0][5], white, bishop);
	init_piece(&board[0][3], white, queen); // queen
	init_piece(&board[0][4], white, king); // king

	// pawns
	for (int i = 0; i < 8; ++i) {
		init_piece(&board[1][i], player, pawn);
	}

	// empty spaces
	for (int i = 2; i < 6; ++i) {
		for (int j = 0; j < 8; ++j) {
			init_piece(&board[i][j], null, dead);
		}
	}

	// Black color
	init_piece(&board[7][0], black, rook); // rooks
	init_piece(&board[7][7], black, rook);
	init_piece(&board[7][1], black, knight); // knights
	init_piece(&board[7][6], black, knight);
	init_piece(&board[7][2], black, bishop); // bishops
	init_piece(&board[7][5], black, bishop);
	init_piece(&board[7][3], black, queen); // queen
	init_piece(&board[7][4], black, king); // king

	// pawns
	for (int i = 0; i < 8; ++i) {
		init_piece(&board[6][i], black, pawn);
	}

	return 0;
}
