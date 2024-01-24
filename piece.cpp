#include <string>
#include "board.hpp"


chess::piece::piece(const piece_type value_type, const piece_color value_color)
{
	type = value_type;
	color = value_color;

	list_of_moves = {};
}

chess::piece::piece(const std::string s)
{
        if      (s == "P") {
		type = chess::piece_type::pawn;
		color = chess::piece_color::black;
	} else if (s == "N") {
		type = chess::piece_type::knight;
		color = chess::piece_color::black;
        } else if (s == "B") {
		type = chess::piece_type::bishop;
		color = chess::piece_color::black;
        } else if (s == "R") {
		type = chess::piece_type::rook;
		color = chess::piece_color::black;
        } else if (s == "Q") {
		type = chess::piece_type::queen;
		color = chess::piece_color::black;
        } else if (s == "K") {
		type = chess::piece_type::king;
		color = chess::piece_color::black;
	} else if (s == "p") {
		type = chess::piece_type::pawn;
		color = chess::piece_color::white;
        } else if (s == "n") {
		type = chess::piece_type::knight;
		color = chess::piece_color::white;
	} else if (s == "b") {
		type = chess::piece_type::bishop;
		color = chess::piece_color::white;
	} else if (s == "r") {
		type = chess::piece_type::rook;
		color = chess::piece_color::white;
        } else if (s == "q") {
		type = chess::piece_type::queen;
		color = chess::piece_color::white;
        } else if (s == "k") {
		type = chess::piece_type::king;
		color = chess::piece_color::white;
	} else throw std::invalid_argument("Invalid piece type");

	list_of_moves = {};
}


bool chess::piece::operator==(const piece &p) const
{
	return (type == p.type && color == p.color);
}


bool chess::piece::operator!=(const piece &p) const
{
	return (type != p.type || color != p.color);
}


void chess::piece::empty_list_of_moves()
{
	list_of_moves = {};
}


void chess::piece::add_move(position pos)
{
	list_of_moves.push_back(pos);
}
