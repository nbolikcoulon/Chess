TARGET = chess

INCLUDES = board.hpp

SOURCES = main.cpp board.cpp position.cpp piece.cpp constants.cpp moves.cpp

CXXFLAGS = -std=c++17 -Wall -Wextra -Wpointer-arith -Wmissing-declarations -Werror -O2 -g


###################################################################################################


$(TARGET): $(SOURCES:.cpp=.o)
	g++ -o $@ $(CXXFLAGS) $^

clean:
	@rm -vf $(TARGET) *.o

run: $(TARGET)
	@./$(TARGET)

%.o: %.cpp
	g++ -c $< $(CXXFLAGS) -o $@


###################################################################################################


main.o: main.cpp $(INCLUDES)
board.o: board.cpp $(INCLUDES)
position.o: position.cpp $(INCLUDES)
piece.o: piece.cpp $(INCLUDES)
constants.o: constants.cpp $(INCLUDES)
moves.o: moves.cpp $(INCLUDES)
