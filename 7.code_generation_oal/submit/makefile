.PHONY: all clean

CXX = /usr/bin/g++
CXXFLAGS = -g -Wall -W -w -pedantic-errors -std=c++11

# Define Sources and Headers based off of extentions
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

OBJECTS = $(SOURCES:%.cpp=%.o)

default: test

test: driver
	@./driver input.txt > oal_source.txt

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

driver: $(OBJECTS)
	@$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	
debug: $(OBJECTS)
	@$(CXX) $(CXXFLAGS) -g3 $(OBJECTS) -o $@

clean:
	-@rm -f core
	-@rm -f driver
	-@rm -f debug
	-@rm -f depend
	-@rm -f $(OBJECTS)

# Automatically generate dependencies and include them in Makefile
depend: $(SOURCES) $(HEADERS)
	@$(CXX) -MM *.cpp > $@


-include depend