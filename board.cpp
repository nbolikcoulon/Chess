#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "board.hpp"

chess::board::board(const std::string &value, piece_color color)
{
	if (value.size() != 64) {
		throw std::invalid_argument(value + " is not a valid board.");
	}

	std::size_t black_king_pos = value.find("K");
	if (black_king_pos == std::string::npos) {
		throw std::invalid_argument("The board must contain a black king");
	} else if (value.find("K", black_king_pos+1) != std::string::npos) {
		throw std::invalid_argument("The board must contain a single black king");
	}

	std::size_t white_king_pos = value.find("k");
	if (white_king_pos == std::string::npos) {
		throw std::invalid_argument("The board must contain a white king");
	} else if (value.find("k", white_king_pos+1) != std::string::npos) {
		throw std::invalid_argument("The board must contain a single white king");
	}

	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			const std::string square_content = value.substr(column + 8*row, 1);
			if (square_content != " ") {
				_data[column][7 - row].emplace(chess::piece(square_content));

				std::string position_string = std::string{static_cast<char>('h' -  column),
									  static_cast<char>('8' -  row)};

				if (_data[column][7 - row]->color == chess::piece_color::white) {
					_list_of_white_positions.push_back(position_string);
				} else if (_data[column][7 - row]->color == chess::piece_color::black) {
					_list_of_black_positions.push_back(position_string);
				} else {
					throw std::invalid_argument("Invalid piece type found");
				}
			}
		}
	}

	game_status = result::playing;
	_moves_record = {};
	_50_move_count = 0;
	_player_to_play = color;
	_previous_boards = {_data};

	_find_possible_moves();
}


void chess::board::_find_possible_moves()
{
	std::vector<std::string>::iterator position_iterator;
	std::vector<std::string>::iterator end;
	if (_player_to_play == chess::piece_color::white) {
		position_iterator = _list_of_white_positions.begin();
		end = _list_of_white_positions.end();
	} else {
		position_iterator = _list_of_black_positions.begin();
		end = _list_of_black_positions.end();
	}

	for (; position_iterator < end; position_iterator++) {
		const auto pos = chess::position(*position_iterator);
		auto& piece = _get_ref_piece_at(pos);

		if (piece.type == chess::piece_type::pawn) 	  _find_pawn_moves(piece, pos);
		else if (piece.type == chess::piece_type::knight) _find_knight_moves(piece, pos);
		else if (piece.type == chess::piece_type::bishop) _find_bishop_moves(piece, pos);
		else if (piece.type == chess::piece_type::rook)   _find_rook_moves(piece, pos);
		else if (piece.type == chess::piece_type::queen)  _find_queen_moves(piece, pos);
		else if (piece.type == chess::piece_type::king)   _find_king_moves(piece, pos);
	}
}


std::optional<chess::piece> chess::board::get_piece_at(position::coordinates_type pos) const
{
	return _data[pos.col][pos.row];
}


std::optional<chess::piece> chess::board::get_piece_at(position pos) const
{
	const auto coords = static_cast<position::coordinates_type>(pos);

	return _data[coords.col][coords.row];
}


std::optional<chess::piece> chess::board::get_piece_at(std::string pos_string) const
{
	const auto pos = chess::position(pos_string);
	const auto coords = static_cast<position::coordinates_type>(pos);

	return _data[coords.col][coords.row];
}


chess::piece& chess::board::_get_ref_piece_at(position::coordinates_type pos)
{
	if (_data[pos.col][pos.row]) return _data[pos.col][pos.row].value();
	else throw std::invalid_argument("No piece at the given position");
}


chess::piece& chess::board::_get_ref_piece_at(position pos)
{
	const auto coords = static_cast<position::coordinates_type>(pos);

	if (_data[coords.col][coords.row]) return _data[coords.col][coords.row].value();
	else throw std::invalid_argument("No piece at the given position");
}


chess::piece& chess::board::_get_ref_piece_at(std::string pos_string)
{
	const auto pos = chess::position(pos_string);
	const auto coords = static_cast<position::coordinates_type>(pos);

	if (_data[coords.col][coords.row]) return _data[coords.col][coords.row].value();
	else throw std::invalid_argument("No piece at the given position");
}


void chess::board::move_piece(position old, position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	if (!_move_can_be_done(old, pos)) {
		throw std::logic_error("Chess rules do not allow this move");
	}

	auto moved_piece = get_piece_at(old);
	auto piece_in_target_pos = get_piece_at(pos);

	_data[coord_pos.col][coord_pos.row].emplace(_data[coord_old.col][coord_old.row].value());
	_data[coord_old.col][coord_old.row].reset();

	_save_move(old, pos);
	_update_board_status(old, pos, moved_piece->color, piece_in_target_pos);
	auto game_can_continue = _enough_pieces_on_board();
	if (!game_can_continue) {
		std::cout << "Draw\n";
		game_status = result::draw;
		save();
	}
	if (_three_fold_rep()) {
		std::cout << "Draw\n";
		game_status = result::draw;
		save();
	}
	if (_50_move_rule(pos, piece_in_target_pos)) {
		std::cout << "Draw\n";
		game_status = result::draw;
		save();
	}
}


void chess::board::_save_move(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	const auto moved_piece = get_piece_at(pos);

	auto piece_character = _piece_names[moved_piece->type];
	std::string old_string = std::string{static_cast<char>('a' +  coord_old.col),
					     static_cast<char>('1' +  coord_old.row)};
	std::string pos_string = std::string{static_cast<char>('a' +  coord_pos.col),
					     static_cast<char>('1' +  coord_pos.row)};

	std::string move_played = piece_character + old_string + "-" + piece_character + pos_string;

	if (moved_piece->color == piece_color::white) {
		std::array<std::string, 2> added_move = {move_played, ""};
		_moves_record.push_back(added_move);
	} else {
		_moves_record.back()[1] = move_played;
	}
}


void chess::board::_update_board_status(const position old, const position pos, const piece_color color_moved_piece, const std::optional<piece> piece_in_target_pos)
{
	// update saved boards for 3x repetition check
	if (piece_in_target_pos) {
		_previous_boards = {};
	} else if (get_piece_at(pos)->type == piece_type::pawn) {
		_previous_boards = {};
	}

	_previous_boards.push_back(_data);

	// update piece lists
	if (color_moved_piece == chess::piece_color::white) {
		auto pos_iterator = std::find(_list_of_white_positions.begin(),
					      _list_of_white_positions.end(),
					      static_cast<std::string>(old));
		_list_of_white_positions.erase(pos_iterator);
		_list_of_white_positions.push_back(static_cast<std::string>(pos));

		if (piece_in_target_pos) {
			auto pos_iterator = std::find(_list_of_black_positions.begin(),
						      _list_of_black_positions.end(),
						      static_cast<std::string>(pos));
			_list_of_black_positions.erase(pos_iterator);
		}

		_player_to_play = chess::piece_color::black;
		_find_possible_moves();

		bool black_can_move = false;
		auto pos_iter = _list_of_black_positions.begin();
		for (; pos_iter < _list_of_black_positions.end(); pos_iter++) {
			auto black_piece = get_piece_at(*pos_iter);
			if (size(black_piece->list_of_moves) > 0) {
				black_can_move = true;
				break;
			}
		}

		if (!black_can_move) {
			if (_king_is_check(piece_color::black)) {
				std::cout << "Game over. White wins\n";
				game_status = result::white_victory;
			} else {
				std::cout << "Draw\n";
				game_status = result::draw;
			}
			save();
		}
	} else {
		auto pos_iterator = std::find(_list_of_black_positions.begin(),
						_list_of_black_positions.end(),
						static_cast<std::string>(old));
		_list_of_black_positions.erase(pos_iterator);
		_list_of_black_positions.push_back(static_cast<std::string>(pos));

		if (piece_in_target_pos) {
			auto pos_iterator = std::find(_list_of_white_positions.begin(),
						      _list_of_white_positions.end(),
						      static_cast<std::string>(pos));
			_list_of_white_positions.erase(pos_iterator);
		}

		_player_to_play = chess::piece_color::white;
		_find_possible_moves();

		bool white_can_move = false;
		auto pos_iter = _list_of_white_positions.begin();
		for (; pos_iter < _list_of_white_positions.end(); pos_iter++) {
			auto white_piece = get_piece_at(*pos_iter);
			if (size(white_piece->list_of_moves) > 0) {
				white_can_move = true;
				break;
			}
		}

		if (!white_can_move) {
			if (_king_is_check(chess::piece_color::white)) {
				std::cout << "Game over. Black wins\n";
				game_status = result::black_victory;
			} else {
				std::cout << "Draw\n";
				game_status = result::draw;
			}
			save();
		}
	}
}


bool chess::board::_enough_pieces_on_board()
{
	if ((size(_list_of_white_positions) == 1) && (size(_list_of_black_positions) == 1)) return false;

	int number_white_bishop_l = 0;
	int number_white_bishop_d = 0;
	int number_white_knight = 0;
	for (auto pos_iterator = _list_of_white_positions.begin(); pos_iterator != _list_of_white_positions.end(); pos_iterator++ ) {
		auto piece_at_pos = get_piece_at(*pos_iterator);
		if (piece_at_pos->type == piece_type::queen) return true;
		if (piece_at_pos->type == piece_type::rook) return true;
		if (piece_at_pos->type == piece_type::pawn) return true;
		if (piece_at_pos->type == piece_type::knight) {
			number_white_knight += 1;
		} else if (piece_at_pos->type == piece_type::bishop) {
			if (((*pos_iterator)[0] - 'a') % 2 == 0) {
				if (((*pos_iterator)[1] - '1') % 2 == 0) {
					number_white_bishop_d += 1;
				} else {
					number_white_bishop_l += 1;
				}
			} else {
				if (((*pos_iterator)[1] - '1') % 2 == 0) {
					number_white_bishop_l += 1;
				} else {
					number_white_bishop_d += 1;
				}
			}
		}
	}

	if ((number_white_bishop_l > 0) && (number_white_bishop_d > 0)) return true;
	if ((number_white_knight > 0) && ((number_white_bishop_l > 0) || (number_white_bishop_d > 0))) return true;
	if (size(_list_of_black_positions) == 1) return false;

	int number_black_bishop_l = 0;
	int number_black_bishop_d = 0;
	int number_black_knight = 0;
	for (auto pos_iterator = _list_of_black_positions.begin(); pos_iterator != _list_of_black_positions.end(); pos_iterator++) {
		auto piece_at_pos = get_piece_at(*pos_iterator);
		if (piece_at_pos->type == piece_type::queen) return true;
		if (piece_at_pos->type == piece_type::rook) return true;
		if (piece_at_pos->type == piece_type::pawn) return true;
		if (piece_at_pos->type == piece_type::knight) {
			number_black_knight += 1;
		} else if (piece_at_pos->type == piece_type::bishop) {
			if (((*pos_iterator)[0] - 'a') % 2 == 0) {
				if (((*pos_iterator)[0] - '1') % 2 == 0) {
					number_black_bishop_d += 1;
				} else {
					number_black_bishop_l += 1;
				}
			} else {
				if (((*pos_iterator)[0] - '1') % 2 == 0) {
					number_black_bishop_l += 1;
				} else {
					number_black_bishop_d += 1;
				}
			}
		}
	}

	if ((number_black_bishop_l > 0) && (number_black_bishop_d > 0)) return true;
	if ((number_black_knight > 0) && ((number_black_bishop_l > 0) || (number_black_bishop_d > 0))) return true;

	return false;
}


bool chess::board::_three_fold_rep()
{
	if (_previous_boards.size() < 3) return false;

	auto last_board = _previous_boards.back();
	int count_repetition = 1;

	for (auto b = _previous_boards.begin(); b != (_previous_boards.end() - 1); b++) {
		if (*b == last_board) count_repetition += 1;
	}

	if (count_repetition == 3) return true;
	return false;
}


bool chess::board::_50_move_rule(const position pos, const std::optional<piece> piece_in_target_pos)
{
	if (piece_in_target_pos) {
		_50_move_count = 1;
		return false;
	}

	if (get_piece_at(pos)->type == piece_type::pawn) {
		_50_move_count = 1;
		return false;
	}

	if (get_piece_at(pos)->color == piece_color::black) {
		_50_move_count += 1;
	
		if (_50_move_count == 50) return true;
	}

	return false;
}


void chess::board::print() const
{
	std::cout << "\n";

        // print top line indicating the columns labels
        std::cout << "  |";
        for (int i = 0; i < 8; std::cout << "   " << static_cast<char>('a' + i++) << "  |") {}
        std::cout << "\n";

        for (int row = 0; row < 8; row++) {
                std::cout << 8-row << " |";
                for (int column = 0; column < 8; column++) {
                        auto content = _data[column][7 - row];
                        if (content) {
                                std::cout << " " << content->as_unicode_char() << " |";
                        } else {
                                std::cout << "      |";
                        }
		}
                std::cout << "\n";
        }
}


void chess::board::save() const
{
	auto move_iter = _moves_record.begin();
	int move_count = 1;
	for (; move_iter != _moves_record.end() - 1; move_iter++, move_count++) {
		std::cout << "\n";
		std::cout << move_count << "\t";
		std::cout << (*move_iter)[0] << "\t";
		std::cout << (*move_iter)[1];
	}

	switch (game_status) {
	case result::playing:
		std::cout << "\n";
		std::cout << move_count << "\t";
		std::cout << (*move_iter)[0] << "\t";
		std::cout << (*move_iter)[1] << "\n";
		break;
	case result::draw:
		std::cout << "\n";
		std::cout << move_count << "\t";
		std::cout << (*move_iter)[0] << "\t";
		std::cout << (*move_iter)[1] << "\n";
		std::cout << "\t" << "1/2" << "\t" << "1/2\n";
		break;
	case result::white_victory:
		std::cout << "\n";
		std::cout << move_count << "\t";
		std::cout << (*move_iter)[0] << "##\n";
		break;
	case result::black_victory:
		std::cout << "\n";
		std::cout << move_count << "\t";
		std::cout << (*move_iter)[0] << "\t";
		std::cout << (*move_iter)[1] << "##\n";
		break;
	}
}


bool chess::board::_move_can_be_done(const position old, const position pos)
{
	if (pos == old) return false;

	auto moved_piece = get_piece_at(old);

	if (!moved_piece) return false;
	if (moved_piece->color != _player_to_play) return false;

	auto move_list = moved_piece->list_of_moves;
	auto found_move = std::find(move_list.begin(), move_list.end(), pos);

	if (found_move != move_list.end()) {
		return true;
	} else {
		return false;
	}
}


bool chess::board::_king_is_check(const position old, const position pos)
{
	const auto coord_old = static_cast<position::coordinates_type>(old);
	const auto coord_pos = static_cast<position::coordinates_type>(pos);

	const auto captured_piece = get_piece_at(pos);

	_data[coord_pos.col][coord_pos.row].emplace(_data[coord_old.col][coord_old.row].value());
	if (old != pos) {
		_data[coord_old.col][coord_old.row].reset();
	}

	// get the position of the king
	std::vector<std::string> list_positions_player;
	std::vector<std::string> list_positions_opponent;

	switch (get_piece_at(coord_pos)->color) {
	case piece_color::white:
		list_positions_player = _list_of_white_positions;
		list_positions_opponent = _list_of_black_positions;
		break;
	case piece_color::black:
		list_positions_player = _list_of_black_positions;
		list_positions_opponent = _list_of_white_positions;
		break;
	default:
		throw std::logic_error{"Invalid piece color found"};
	}

	position king_pos = pos;
	for (auto p = list_positions_player.begin(); p != list_positions_player.end(); p++) {
		if (get_piece_at(*p)->type == piece_type::king) {
			king_pos = position(*p);
			break;
		}
	}
	const auto king_coord = static_cast<position::coordinates_type>(king_pos);

	//check for capture of the king
	bool king_is_check = false;
	for (auto p = list_positions_opponent.begin(); p != list_positions_opponent.end(); p++) {
		auto piece_check = *get_piece_at(*p);
		auto p_pos = position(*p);
		auto p_coord = static_cast<position::coordinates_type>(p_pos);

		switch (piece_check.type) {
		case piece_type::pawn:
			int direction_pawn;
			if (get_piece_at(king_pos)->color == piece_color::white) direction_pawn =  1;
			if (get_piece_at(king_pos)->color == piece_color::black) direction_pawn = -1;

			if ((abs(p_coord.col - king_coord.col) == 1) && ((p_coord.row - king_coord.row) == direction_pawn)) {
										king_is_check = true;
			}
			break;
		case piece_type::knight:
			if ((abs(p_coord.col - king_coord.col) == 2) && (abs(p_coord.row - king_coord.row) == 1)) {
										king_is_check = true;
			}
			if ((abs(p_coord.col - king_coord.col) == 1) && (abs(p_coord.row - king_coord.row) == 2)) {
										king_is_check = true;
			}
			break;
		case piece_type::bishop:
			if (abs(p_coord.col - king_coord.col) == abs(p_coord.row - king_coord.row)) {
				if (_bishop_unblocked(p_pos, king_pos)) 	king_is_check = true;
			}
			break;
		case piece_type::rook:
			if ((p_coord.col == king_coord.col) || (p_coord.row == king_coord.row)) {
				if (_rook_unblocked(p_pos, king_pos))		king_is_check = true;
			}
			break;
		case piece_type::queen:
			if (abs(p_coord.col - king_coord.col) == abs(p_coord.row - king_coord.row)) {
				if (_bishop_unblocked(p_pos, king_pos)) 	king_is_check = true;
			}
			if ((p_coord.col == king_coord.col) || (p_coord.row == king_coord.row)) {
				if (_rook_unblocked(p_pos, king_pos))		king_is_check = true;
			}
			break;
		case piece_type::king:
			break;
		default:
			_data[coord_old.col][coord_old.row].emplace(_data[coord_pos.col][coord_pos.row].value());
			if (captured_piece) {
				_data[coord_pos.col][coord_pos.row] = *captured_piece;
			} else {
				_data[coord_pos.col][coord_pos.row].reset();
			}
			
			throw std::logic_error{"Invalid piece type found"};
		}

	}

	// put the board back to its state
	_data[coord_old.col][coord_old.row].emplace(_data[coord_pos.col][coord_pos.row].value());
	if (captured_piece) {
		_data[coord_pos.col][coord_pos.row].emplace(*captured_piece);
	} else {
		_data[coord_pos.col][coord_pos.row].reset();
	}
	
	// return result
	if (king_is_check) return true;
	else return false;
}


bool chess::board::_king_is_check(const piece_color color)
{
	if (color == piece_color::white) {
		auto white_pos = static_cast<chess::position>(_list_of_white_positions[0]);
		return _king_is_check(white_pos, white_pos);
	} else {
		auto black_pos = static_cast<chess::position>(_list_of_black_positions[0]);
		return _king_is_check(black_pos, black_pos);
	}
}
