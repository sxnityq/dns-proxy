CC := gcc
SRC_DIR := src/
PARSER_DIR := src/parser

all:
	$(CC) -o main $(SRC_DIR)/tools.c $(PARSER_DIR)/io.c $(PARSER_DIR)/lexer.c $(PARSER_DIR)/parser.c $(SRC_DIR)/llist.c $(SRC_DIR)/main.c

.PHONY: clean
clean:
	rm *.o
