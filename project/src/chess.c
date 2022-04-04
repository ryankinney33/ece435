#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "chess.h"

//struct chess_piece board[8][8]; // grid of chess pieces

static int init_piece(struct chess_piece *piece, enum team_color color,
		enum piece_type type)
{
	if (piece == NULL) {
		return -EFAULT; // sanity check
	}

	// Set the information for the piece
	piece->color = color;
	piece->id = type;

	// Write the piece color
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
		case pawn: // pawn
			strcpy(piece->print_char, "\u265f");
			return 0;
		default:
			strcpy(piece->print_char, " ");
			return 0;
	}
}

struct chess_board *init_board(int has_color, int unicode, enum team_color player)
{
	struct chess_board *board = malloc(sizeof(struct chess_board));
	if (board == NULL)
		return NULL;

	// Display information
	board->player_color = player;
	board->use_unicode = unicode;

	// White color
	init_piece(&board->grid[0][0], white, rook); // rooks
	init_piece(&board->grid[0][7], white, rook);
	init_piece(&board->grid[0][1], white, knight); // knights
	init_piece(&board->grid[0][6], white, knight);
	init_piece(&board->grid[0][2], white, bishop); // bishops
	init_piece(&board->grid[0][5], white, bishop);
	init_piece(&board->grid[0][3], white, queen); // queen
	init_piece(&board->grid[0][4], white, king); // king

	// white pawns
	for (int i = 0; i < 8; ++i) {
		init_piece(&board->grid[1][i], white, pawn);
	}

	// empty spaces
	for (int i = 2; i < 6; ++i) {
		for (int j = 0; j < 8; ++j) {
			init_piece(&board->grid[i][j], null, dead);
		}
	}

	// Black color
	init_piece(&board->grid[7][0], black, rook); // rooks
	init_piece(&board->grid[7][7], black, rook);
	init_piece(&board->grid[7][1], black, knight); // knights
	init_piece(&board->grid[7][6], black, knight);
	init_piece(&board->grid[7][2], black, bishop); // bishops
	init_piece(&board->grid[7][5], black, bishop);
	init_piece(&board->grid[7][3], black, queen); // queen
	init_piece(&board->grid[7][4], black, king); // king

	// black pawns
	for (int i = 0; i < 8; ++i) {
		init_piece(&board->grid[6][i], black, pawn);
	}

	return board;
}

struct chess_board *destroy_board(struct chess_board *board)
{
	if (board != NULL)
		free(board);
	return NULL;
}
