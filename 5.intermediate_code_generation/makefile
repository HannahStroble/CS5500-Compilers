.PHONY: all clean

CXX = /usr/bin/g++
CXXFLAGS = -g -Wall -W -w -pedantic-errors -std=c++11

default: test

test: driver
	echo "DO NOT INCLUDE THIS MAKE FILE"
	@./driver < input.ext

driver: lex tab
	@echo "Compiling tab..."
	@g++ hw5_parser.tab.c -o driver
	@echo "DING! Bisquits are done!"

lex: hw5_parser.l
	@echo "Flexing Parser..."
	@flex hw5_parser.l
	
tab: hw5_parser.y
	@echo "Bison-ing Parser..."
	@bison hw5_parser.y

clean:
	@rm -f hw5_parser.tab.c
	@rm -f lex.yy.c
	@rm -f driver
	@echo "Squeaky clean!"

