#include <iostream>
#include <stdexcept>
#include <string>
#include "board.hpp"


std::map<chess::piece_type, std::string> chess::board::_piece_names{{chess::piece_type::pawn, ""}, 
								    {chess::piece_type::knight, "N"},
								    {chess::piece_type::bishop, "B"},
								    {chess::piece_type::rook, "R"},
								    {chess::piece_type::queen, "Q"},
								    {chess::piece_type::king, "K"}};

char16_t chess::piece::as_unicode_char() const
{
	if (color == chess::piece_color::white) {
		switch (type) {
		case piece_type::pawn:
			return u'\u2659';
			break;
		case piece_type::knight:
			return u'\u2658';
			break;
		case piece_type::bishop:
			return u'\u2657';
			break;
		case piece_type::rook:
			return u'\u2656';
			break;
		case piece_type::queen:
			return u'\u2655';
			break;
		case piece_type::king:
			return u'\u2654';
			break;
		default:
			throw std::logic_error{"Invalid piece type"};
		}
	} else if (color == chess::piece_color::black) {
		switch (type) {
		case piece_type::pawn:
			return u'\u265f';
			break;
		case piece_type::knight:
			return u'\u265e';
			break;
		case piece_type::bishop:
			return u'\u265d';
			break;
		case piece_type::rook:
			return u'\u265c';
			break;
		case piece_type::queen:
			return u'\u265b';
			break;
		case piece_type::king:
			return u'\u265a';
			break;
		default:
			throw std::logic_error{"Invalid piece type"};
		}
	}

	throw std::logic_error{"Invalid piece color"};
}
