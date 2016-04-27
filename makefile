DIR_SRC = ./src
DIR_OBJ = ./bin/obj
DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = htmlparser.exe

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++

CFLAGS = -std=c++11 -g -Wall
#CFlAGS = -std=c++11 -O3

all: makedir ${BIN_TARGET}

.PHONY:makedir
makedir:
	@mkdir -p ./bin/obj

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ) -o $@

.PHONY:clean
clean:
	-rm -f $(OBJ) $(BIN_TARGET)
