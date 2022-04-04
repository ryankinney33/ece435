#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

// The 6 different types of pieces (and deceased)
enum piece_type {
	pawn,
	knight,
	bishop,
	rook,
	king,
	queen,
	dead // piece has been taken by other team
};

// The 2 different team colors
enum team_color {
	null = 0,
	black = 1,
	white = 2
};

// A chess piece
struct chess_piece {
	// Piece position

	// The type of piece
	enum piece_type id;
	enum team_color color;
	char prefix; // used to put a "b" or "b" if colors are disabled
	char print_char[4]; // the text to be printed for the piece

};

struct chess_board {
	// Information about team and the board
	enum team_color player_color;
	struct chess_piece grid[8][8];
};

#endif /* CHESS_TYPES_H */
