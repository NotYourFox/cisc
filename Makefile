FILES = ./src/main.c ./src/argparser/argparser.c ./src/utils/utils.c

all:
	gcc -g -o cisc $(FILES)
