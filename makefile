OBJS = src/*.*pp src/lexical_analyser/*.*pp src/utils/*.*pp 
INCLUDE_DIR = -Isrc
CC = g++
COMPILER_FLAGS = -w
OBJ_NAME = a.out

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(INCLUDE_DIR) -o ./build/$(OBJ_NAME)
clear:
	rm -rf build/$(OBJ_NAME)
run:
	./build/$(OBJ_NAME)
reset:
	make clear && make