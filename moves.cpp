#include <iostream>
#include <stdexcept>
#include <string>
#include "board.hpp"

void chess::board::_find_pawn_moves(piece &pawn, const position pos)
{
	pawn.empty_list_of_moves();

	auto color = pawn.color;
	int move_direction;
	if (color == chess::piece_color::white) move_direction =  1;
	if (color == chess::piece_color::black) move_direction = -1;
	
	std::vector<chess::position> possible_new_pos;

	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {0, move_direction});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {0, 2*move_direction});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {1, move_direction});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-1, move_direction});

	auto pos_iter = possible_new_pos.begin();
	for (; pos_iter < possible_new_pos.end(); pos_iter++) {
		if (_pawn_moves(pos, *pos_iter)) {
			if (_pawn_unblocked(pos, *pos_iter)) {
				if (!_king_is_check(pos, *pos_iter)) {
					pawn.add_move(*pos_iter);
				}
			}
		}
	}
}


void chess::board::_find_knight_moves(piece &knight, const position pos) {
	knight.empty_list_of_moves();

	std::vector<chess::position> possible_new_pos;

	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {1, 2});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {1, -2});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {2, 1});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {2, -1});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-1, 2});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-1, -2});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-2, 1});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-2, -1});

	auto pos_iter = possible_new_pos.begin();
	for (; pos_iter < possible_new_pos.end(); pos_iter++) {
		if (!_king_is_check(pos, *pos_iter)) {
			knight.add_move(*pos_iter);
		}
	}
}


void chess::board::_find_bishop_moves(piece &bishop, const position pos) {
	bishop.empty_list_of_moves();

	std::vector<chess::position> possible_new_pos;

	for (int x = 1; x < 8; x++) {
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {x, x});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-x, -x});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {x, -x});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-x, x});
	}

	auto pos_iter = possible_new_pos.begin();
	for (; pos_iter < possible_new_pos.end(); pos_iter++) {
		if (_bishop_unblocked(pos, *pos_iter)) {
			if (!_king_is_check(pos, *pos_iter)) {
				bishop.add_move(*pos_iter);
			}
		}
	}
}


void chess::board::_find_rook_moves(piece &rook, const position pos) {
	rook.empty_list_of_moves();

	std::vector<chess::position> possible_new_pos;

	for (int x = 1; x < 8; x++) {
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {x, 0});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-x, 0});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {0, x});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {0, -x});
	}

	auto pos_iter = possible_new_pos.begin();
	for (; pos_iter < possible_new_pos.end(); pos_iter++) {
		if (_rook_unblocked(pos, *pos_iter)) {
			if (!_king_is_check(pos, *pos_iter)) {
				rook.add_move(*pos_iter);
			}
		}
	}
}


void chess::board::_find_queen_moves(piece &queen, const position pos) {
	queen.empty_list_of_moves();

	std::vector<chess::position> possible_new_pos;

	for (int x = 1; x < 8; x++) {
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {x, x});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-x, -x});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {x, -x});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-x, x});

		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {x, 0});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-x, 0});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {0, x});
		possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {0, -x});
	}

	auto pos_iter = possible_new_pos.begin();
	for (; pos_iter < possible_new_pos.end(); pos_iter++) {
		if (_queen_unblocked(pos, *pos_iter)) {
			if (!_king_is_check(pos, *pos_iter)) {
				queen.add_move(*pos_iter);
			}
		}
	}
}


void chess::board::_find_king_moves(piece &king, const position pos) {
	king.empty_list_of_moves();

	std::vector<chess::position> possible_new_pos;

	for (int x = -1; x < 2; x++) {
		for (int y = -1; y < 2; y++) {
			if ((x != 0) || (y != 0)) {
				possible_new_pos = _add_possible_position(possible_new_pos, pos,
							std::array<int, 2> {x, y});
			}
		}
	}

	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {2, 0});
	possible_new_pos = _add_possible_position(possible_new_pos, pos, std::array<int, 2> {-2, 0});
	
	auto pos_iter = possible_new_pos.begin();
	for (; pos_iter < possible_new_pos.end(); pos_iter++) {
		if (_king_moves(pos, *pos_iter)) {
			if (_king_unblocked(pos, *pos_iter)) {
				if (!_king_is_check(pos, *pos_iter)) {
					king.add_move(*pos_iter);
				}
			}
		}
	}
}


std::vector<chess::position> chess::board::_add_possible_position(std::vector<position> vec,
						const position pos, const std::array<int, 2> arr) {
	try {
	chess::position new_position = pos + arr;

	const auto piece = *get_piece_at(pos);
	auto color = piece.color;

	auto target_content = get_piece_at(new_position);
	if (!target_content) {
		vec.push_back(new_position);
	} else if ((target_content->color != color) && (target_content->type != chess::piece_type::king)) {
		vec.push_back(new_position);
	}
	}
	catch ( ... ) {
	}

	return vec;
}


bool chess::board::_pawn_moves(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	if (get_piece_at(old)->color == piece_color::white) {
		if (coord_pos.col == coord_old.col) {
			if (coord_pos.row == coord_old.row + 1) {
				return true;
			}
			if ((coord_pos.row == coord_old.row + 2) && (coord_pos.row == 3)) {
				return true;
			}
			return false;
		}
		if ((abs(coord_pos.col - coord_old.col) == 1) && (coord_pos.row == coord_old.row + 1)) {
			if (get_piece_at(pos)) {
				return true;
			}
			if (_en_passant_is_possible(pos)) {
				return true;
			}
			return false;
		}
	} else {
		if (coord_pos.col == coord_old.col) {
			if (coord_pos.row == coord_old.row - 1) {
				return true;
			}
			if ((coord_pos.row == coord_old.row - 2) && (coord_pos.row == 4)) {
				return true;
			}
			return false;
		}
		if ((abs(coord_pos.col - coord_old.col) == 1) && (coord_pos.row == coord_old.row - 1)) {
			if (get_piece_at(pos)) {
				return true;
			}
			if (_en_passant_is_possible(pos)) {
				return true;
			}
			return false;
		}
	}

	return false;
}


bool chess::board::_en_passant_is_possible(position pos)
{
	if (_moves_record.empty()) {
		return false;
	}

	int pos_color;
	if (_player_to_play == piece_color::white) pos_color = 1;
	else pos_color = 0;

	// last move is a pawn move
	if (static_cast<char>(_moves_record.back()[pos_color][2]) != '-') {
		return false;
	}

	// two-square pawm move from the opponent
	char opponent_row_start = static_cast<char>(_moves_record.back()[pos_color][1]);
	char opponent_row_end = static_cast<char>(_moves_record.back()[pos_color][4]);
	if (abs(static_cast<int>(opponent_row_start - opponent_row_end)) != 2) {
		return false;
	}

	char opponent_col = static_cast<char>(_moves_record.back()[pos_color][0]);
	auto coord_opponent_pawn_push = static_cast<position::coordinates_type>(position(std::string{opponent_col,
											opponent_row_end}));
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	if (abs(coord_opponent_pawn_push.row - coord_pos.row) != 1) {
		return false;
	}
	if (coord_pos.col != coord_opponent_pawn_push.col) {
		return false;
	}

	return true;
}


bool chess::board::_king_moves(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	if (abs(coord_pos.col - coord_old.col) < 2 && abs(coord_pos.row - coord_old.row) < 2) {
		return true;
	}

	auto pos_iter = _moves_record.begin();

	// king-side castling
	if (pos == old + std::array<int, 2> {2, 0}) {
		if (_data[7][coord_pos.row].value() != piece{piece_type::rook, _player_to_play}) {
			return false;
		}

		if (_player_to_play == piece_color::white) {
			for (; pos_iter != _moves_record.end(); pos_iter++) {
				std::size_t found_king = (*pos_iter)[0].find("Ke1");
				std::size_t found_rook = (*pos_iter)[0].find("Rh1");
				if (found_king || found_rook) {
					return false;
				}
			}
			return true;
		}
		
		if (_player_to_play == piece_color::black) {
			for (; pos_iter != _moves_record.end(); pos_iter++) {
				std::size_t found_king = (*pos_iter)[1].find("Ke8");
				std::size_t found_rook = (*pos_iter)[1].find("Rh8");
				if (found_king || found_rook) {
					return false;
				}
			}
			return true;
		}
	}

	// queen-side castling
	if (pos == old + std::array<int, 2> {-2, 0}) {
		if (_data[0][coord_pos.row].value() != piece{piece_type::rook, _player_to_play}) {
			return false;
		}

		if (_player_to_play == piece_color::white) {
			for (; pos_iter != _moves_record.end(); pos_iter++) {
				std::size_t found_king = (*pos_iter)[0].find("Ke1");
				std::size_t found_rook = (*pos_iter)[0].find("Ra1");
				if (found_king || found_rook) {
					return false;
				}
			}
			return true;
		}

		if (_player_to_play == piece_color::black) {
			for (; pos_iter != _moves_record.end(); pos_iter++) {
				std::size_t found_king = (*pos_iter)[1].find("Ke8");
				std::size_t found_rook = (*pos_iter)[1].find("Ra8");
				if (found_king || found_rook) {
					return false;
				}
			}
			return true;
		}
	}

	return false;
}


bool chess::board::_pawn_unblocked(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	if (abs(coord_pos.row - coord_old.row) == 1) {
		if ((get_piece_at(pos)) && (coord_pos.col == coord_old.col))  {
			return false;
		}
		return true;
	}

	auto passed_square = static_cast<position::coordinates_type>(old);
	passed_square.row = (coord_old.row + coord_pos.row) / 2;

	if (get_piece_at(passed_square)) {
		return false;
	}
	
	return true;
}


bool chess::board::_bishop_unblocked(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	int slope_col;
	int slope_row;
	((coord_pos.col - coord_old.col) > 0) ? (slope_col = 1) : (slope_col = -1);
	((coord_pos.row - coord_old.row) > 0) ? (slope_row = 1) : (slope_row = -1);

	auto passed_square = static_cast<position::coordinates_type>(old);
	passed_square.col += slope_col;
	passed_square.row += slope_row;

	if (passed_square == coord_pos) return true;

	for (; passed_square != coord_pos; passed_square.col += slope_col, passed_square.row += slope_row) {
		if (get_piece_at(passed_square)) {
			return false;
		}
	}

	return true;
}


bool chess::board::_rook_unblocked(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	int direction_col;
	int direction_row;
	switch (coord_pos.row - coord_old.row) {
	case 0:
		direction_col = (coord_pos.col - coord_old.col) / abs(coord_pos.col - coord_old.col);
		direction_row = 0;
		break;
	default:
		direction_col = 0;
		direction_row = (coord_pos.row - coord_old.row) / abs(coord_pos.row - coord_old.row);
		break;
	}

	auto passed_square = static_cast<position::coordinates_type>(old);
	passed_square.col += direction_col;
	passed_square.row += direction_row;
	for (; passed_square != coord_pos; passed_square.col += direction_col, passed_square.row += direction_row) {
		if (get_piece_at(passed_square)) {
			return false;
		}
	}

	return true;
}


bool chess::board::_queen_unblocked(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	if ((coord_old.row != coord_pos.row) && (coord_old.col != coord_pos.col)) {
		if (_bishop_unblocked(old, pos)) {
			return true;
		}
		return false;
	}

	if (_rook_unblocked(old, pos)) {
		return true;
	}

	return false;
}


bool chess::board::_king_unblocked(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	int check_col = std::max(0, coord_pos.col - 1);
	int check_row = std::max(0, coord_pos.row - 1);

	int limit_col = std::min(8, coord_pos.col + 1);
	int limit_row = std::min(8, coord_pos.row + 1);

	for (; check_col < limit_col; check_col++) {
		for (; check_row < limit_row; check_row++) {
			if (_data[check_col][check_row]->type == piece_type::king) {
				return false;
			}
		}
	}

	if (abs(coord_pos.col - coord_old.col) < 2) {
		return true;
	}

	int direction_col;
	int number_squares;
	(coord_pos.col - coord_old.col > 0) ? (direction_col = 1, number_squares = 2) : (direction_col = -1, number_squares = 3);

	auto passed_square = static_cast<position::coordinates_type>(old);
	passed_square.col += direction_col;
	for (; number_squares != 0; number_squares--)	{
		if (get_piece_at(passed_square)) {
			return false;
		}

		const auto intermediate_pos = position(std::string{static_cast<char>('a' + passed_square.col),
								   static_cast<char>('1' + passed_square.row)});
		if (_king_is_check(old, intermediate_pos)) {
			return false;
		}

		passed_square.col += direction_col;
	}

	return true;
}
