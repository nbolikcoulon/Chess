#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP

#include <array>
#include <vector>
#include <map>
#include <optional>
#include <string>


namespace chess {
	enum class piece_type {
		pawn,
		knight,
		bishop,
		rook,
		queen,
		king,
	};

	enum class piece_color {
		white,
		black,
	};

	enum class result {
		playing,
		draw,
		white_victory,
		black_victory,
	};

	class position {
	public:
		struct coordinates_type {
			int col;
			int row;

			bool operator== (const coordinates_type &) const;
			bool operator!= (const coordinates_type &) const;
		};

		explicit position(const std::string &value);

		explicit operator coordinates_type() const;
		explicit operator std::string() const;

		bool operator== (const position &) const;
		bool operator!= (const position &) const;
		position operator+ (const std::array<int, 2> &) const;

	private:
		coordinates_type _value;
	};

	class piece {
	public:
		piece_type type;
		piece_color color;
		std::vector<position> list_of_moves;

		explicit piece(const piece_type value_type, const piece_color value_color);
		explicit piece(const std::string s);

		char16_t as_unicode_char() const;

		bool operator== (const piece &) const;
		bool operator!= (const piece &) const;

		void empty_list_of_moves();
		void add_move(chess::position pos);
	};

	class board {
	public:
	        const std::string initial_position = "RNBQKBNR"
        	                                     "PPPPPPPP"
                	                             "        "
                        	                     "        "
                                	             "        "
                                        	     "        "
	                                             "pppppppp"
        	                                     "rnbqkbnr";

		explicit board(const std::string &value, piece_color color);
		board(const std::string &value) : board(value, piece_color::white) {};
		board() : board(initial_position) {};

		result game_status;

		std::optional<piece> get_piece_at(position::coordinates_type pos) const;
		std::optional<piece> get_piece_at(position pos) const;
		std::optional<piece> get_piece_at(std::string pos_string) const;

		void move_piece(position old, position pos);

		void print() const;
		void save() const;

	private:
		using column_type = std::array<std::optional<piece>, 8>;

		std::array<column_type, 8> _data;
		std::vector<std::string> _list_of_white_positions;
		std::vector<std::string> _list_of_black_positions;
		std::vector<std::array<column_type, 8>> _previous_boards;

		piece_color _player_to_play;

		// keeps track of moves in the form "Ke1-Ke2", first column is white
		std::vector<std::array<std::string, 2>> _moves_record;
		int _50_move_count;

		static std::map<piece_type, std::string> _piece_names;

		void _find_possible_moves();
		void _save_move(const position old, const position pos);
		bool _enough_pieces_on_board();
		bool _three_fold_rep();
		bool _50_move_rule(const position pos, const std::optional<piece> piece_in_target_pos);
		void _update_board_status(const position old, const position pos,
				const piece_color color, const std::optional<piece> piece_in_target_pos);

		piece& _get_ref_piece_at(position::coordinates_type pos);
		piece& _get_ref_piece_at(position pos);
		piece& _get_ref_piece_at(std::string pos_string);

		void _find_pawn_moves(piece &pawn, const position pos);
		void _find_knight_moves(piece &knight, const position pos);
		void _find_bishop_moves(piece &bishop, const position pos);
		void _find_rook_moves(piece &rook, const position pos);
		void _find_queen_moves(piece &queen, const position pos);
		void _find_king_moves(piece &king, const position pos);

		std::vector<position> _add_possible_position(std::vector<position> vec,
							const position pos, const std::array<int, 2> arr);

		bool _move_can_be_done(const position old, const position pos);

		bool _king_is_check(const position old, const position pos);
		bool _king_is_check(const piece_color color);

		bool _pawn_moves(const position old, const position pos);
		bool _en_passant_is_possible(const position pos);
		bool _king_moves(const position old, const position pos);

		bool _pawn_unblocked(const position old, const position pos);
		bool _bishop_unblocked(const position old, const position pos);
		bool _rook_unblocked(const position old, const position pos);
		bool _queen_unblocked(const position old, const position pos);
		bool _king_unblocked(const position old, const position pos);
	};
};


#endif
