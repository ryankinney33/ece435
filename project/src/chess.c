#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "chess.h"

static void init_piece(struct chess_piece *piece, enum team_color color,
		enum piece_type type, int use_unicode)
{
	// Set the information for the piece
	piece->color = color;
	piece->id = type;

	if (color == black)
		piece->prefix = 'b';
	else if (color == white)
		piece->prefix = 'w';
	else
		piece->prefix = ' ';

	// Write the piece color
	switch (type) {
		case king:
			strcpy(piece->print_char, (use_unicode)? "\u265a" : "K");
			return;
		case queen:
			strcpy(piece->print_char, (use_unicode)? "\u265b" : "Q");
			return;
		case rook:
			strcpy(piece->print_char, (use_unicode)? "\u265c" : "R");
			return;
		case bishop:
			strcpy(piece->print_char, (use_unicode)? "\u265d" : "B");
			return;
		case knight:
			strcpy(piece->print_char, (use_unicode)? "\u265e" : "N");
			return;
		case pawn:
			strcpy(piece->print_char, (use_unicode)? "\u265f" : "P");
			return;
		default:
			strcpy(piece->print_char, " ");
			return;
	}
}

struct chess_board *init_board(int unicode, enum team_color player)
{
	// Sanity check
	if (player == null)
		return NULL;

	struct chess_board *board = malloc(sizeof(struct chess_board));
	if (board == NULL) {
		perror("malloc");
		return NULL;
	}

	// Display information
	board->player_color = player;

	// White color
	init_piece(&board->grid[0][0], white, rook, unicode); // rooks
	init_piece(&board->grid[0][7], white, rook, unicode);
	init_piece(&board->grid[0][1], white, knight, unicode); // knights
	init_piece(&board->grid[0][6], white, knight, unicode);
	init_piece(&board->grid[0][2], white, bishop, unicode); // bishops
	init_piece(&board->grid[0][5], white, bishop, unicode);
	init_piece(&board->grid[0][3], white, queen, unicode); // queen
	init_piece(&board->grid[0][4], white, king, unicode); // king

	// white pawns
	for (int i = 0; i < 8; ++i) {
		init_piece(&board->grid[1][i], white, pawn, unicode);
	}

	// empty spaces
	for (int i = 2; i < 6; ++i) {
		for (int j = 0; j < 8; ++j) {
			init_piece(&board->grid[i][j], null, dead, unicode);
		}
	}

	// Black color
	init_piece(&board->grid[7][0], black, rook, unicode); // rooks
	init_piece(&board->grid[7][7], black, rook, unicode);
	init_piece(&board->grid[7][1], black, knight, unicode); // knights
	init_piece(&board->grid[7][6], black, knight, unicode);
	init_piece(&board->grid[7][2], black, bishop, unicode); // bishops
	init_piece(&board->grid[7][5], black, bishop, unicode);
	init_piece(&board->grid[7][3], black, queen, unicode); // queen
	init_piece(&board->grid[7][4], black, king, unicode); // king

	// black pawns
	for (int i = 0; i < 8; ++i) {
		init_piece(&board->grid[6][i], black, pawn, unicode);
	}

	return board;
}

struct chess_board *destroy_board(struct chess_board *board)
{
	if (board != NULL)
		free(board);
	return NULL;
}
