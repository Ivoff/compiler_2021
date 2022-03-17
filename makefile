OBJS = src/*.*pp src/lexical_analyser/*.*pp src/syntactical_analyser/*.*pp src/syntactical_analyser/parse_tree/*.*pp src/semantic_generator/*.*pp src/semantic_generator/symbol_table/*.*pp src/semantic_generator/semantic_actions_table/*.*pp src/semantic_generator/code_generator/*.*pp src/semantic_recursive/*.*pp src/misc/*.*pp
INCLUDE_DIR = -Isrc
CC = g++
COMPILER_FLAGS = -w -g -std=c++11
OBJ_NAME = a.out

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(INCLUDE_DIR) -o ./build/$(OBJ_NAME)
clear:
	rm -rf build/$(OBJ_NAME)
run:
	./build/$(OBJ_NAME)
reset:
	make clear && make

debug:
	gdb ./build/$(OBJ_NAME)