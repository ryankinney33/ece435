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
	white, // may not actually be white and black
	black,
	null,
};

// A chess piece
struct chess_piece {
	// Piece position

	// The type of piece
	enum piece_type id;
	enum team_color color;

	char print_char[4]; // the text to be printed for the piece

};

#endif /* CHESS_TYPES_H */
