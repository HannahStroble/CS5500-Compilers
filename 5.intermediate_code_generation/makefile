.PHONY: all clean

CXX = /usr/bin/g++
CXXFLAGS = -g -Wall -W -w -pedantic-errors -std=c++11

default: driver

test: driver
	echo "DO NOT INCLUDE THIS MAKE FILE"
	@./driver < input.ext

driver: lex.yy.c hw5_parser.tab.c
	@echo "Compiling tab..."
	@g++ hw5_parser.tab.c -o driver
	@echo "DING! Bisquits are done!"

lex.yy.c: hw5_parser.l
	@echo "Flexing Parser..."
	@flex hw5_parser.l
	
hw5_parser.tab.c: hw5_parser.y
	@echo "Bison-ing Parser..."
	@bison hw5_parser.y

clean:
	@rm -f hw5_parser.tab.c
	@rm -f lex.yy.c
	@rm -f driver
	@echo "Squeaky clean!"

