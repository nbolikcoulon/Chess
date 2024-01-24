#include <stdexcept>
#include <string>
#include <array>
#include "board.hpp"


chess::position::position(const std::string &value)
{
	if (value.size() != 2)
		throw std::invalid_argument{value + ": invalid chess position"};

	const auto col = value[0];
	if ((col < 'a') || (col > 'h'))
		throw std::invalid_argument{value + ": invalid chess position"};

	const auto row = value[1];
	if ((row < '1') || (row > '8'))
		throw std::invalid_argument{value + ": invalid chess position"};
		
	_value.col = col - 'a';
	_value.row = row - '1';
}


chess::position::operator coordinates_type() const
{
	return _value;
}


chess::position::operator std::string() const
{
	return std::string{static_cast<char>('a' + _value.col),
			   static_cast<char>('1' + _value.row)};
}


bool chess::position::operator==(const position &pos) const
{
	const auto coords = static_cast<position::coordinates_type>(pos);
	return (_value.col == coords.col && _value.row == coords.row);
}


bool chess::position::coordinates_type::operator==(const position::coordinates_type &pos) const
{
	return (col == pos.col && row == pos.row);
}


bool chess::position::operator!=(const position &pos) const
{
	const auto coords = static_cast<position::coordinates_type>(pos);
	return (_value.col != coords.col || _value.row != coords.row);
}


bool chess::position::coordinates_type::operator!=(const position::coordinates_type &pos) const
{
	return (col != pos.col || row != pos.row);
}


chess::position chess::position::operator+(const std::array<int, 2> &arr) const
{
	std::string new_position_string = std::string{static_cast<char>('a' + _value.col + arr[0]),
					  static_cast<char>('1' + _value.row + arr[1])};
	position new_position = position(new_position_string);

	return new_position;
}
