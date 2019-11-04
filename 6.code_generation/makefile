.PHONY: all clean

CXX = /usr/bin/g++
CXXFLAGS = -g -Wall -W -w -pedantic-errors -std=c++11

default: driver

test: driver
	echo "DO NOT INCLUDE THIS MAKE FILE"
	@./driver < input.txt

driver: lex.yy.c hw6.tab.c
	@echo "Compiling"
	@g++ hw6.tab.c -o driver
	@echo "DING! Biscuits are done!"

lex.yy.c: hw6.l
	@echo "Flexing Parser..."
	@flex hw6.l
	@echo "Flexed!"
	
hw6.tab.c: hw6.y
	@echo "Bison-ing Parser..."
	@bison hw6.y
	@echo "Bisoned!"
clean:
	@echo "Scrubbing..."
	@rm -f hw6.tab.c
	@rm -f lex.yy.c
	@rm -f driver
	@echo "Squeaky clean!"

