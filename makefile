
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=%.o)

SF = -std=c++14 -Wall -Wextra -g
LF = -lSDL2

parallel:
	@make -j8 --silent build

build: $(OBJ)
	@echo "-- Linking --"
	@g++ $^ $(LF) -obuild

%.o: %.cpp
	@echo "-- Generating $@ --"
	@g++ $(SF) -c $< -o $@

run: parallel
	@./build

clean:
	@rm *.o