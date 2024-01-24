#include <iostream>
#include "board.hpp"
#include <string>

int main()
{
	auto b = chess::board();

	b.move_piece(chess::position("e2"), chess::position("e4"));
	b.move_piece(chess::position("e7"), chess::position("e5"));
	b.move_piece(chess::position("f1"), chess::position("c4"));
	b.move_piece(chess::position("a7"), chess::position("a6"));
	b.move_piece(chess::position("d1"), chess::position("f3"));
	b.move_piece(chess::position("a6"), chess::position("a5"));
	b.move_piece(chess::position("f3"), chess::position("f7"));
}
