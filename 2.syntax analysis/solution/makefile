.PHONY: all clean

CXX = /usr/bin/g++
CXXFLAGS = -g -Wall -W -w -pedantic-errors -std=c++11

# Define Sources and Headers based off of extentions
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

OBJECTS = $(SOURCES:%.cpp=%.o)

default: driver

%.o: %.cpp
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

driver: $(OBJECTS)
	@echo "Building $@"
	@$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@echo ""
	@echo "Everything worked"
	@echo ""

clean:
	-@rm -f core
	-@rm -f driver
	-@rm -f depend
	-@rm -f $(OBJECTS)

# Automatically generate dependencies and include them in Makefile
depend: $(SOURCES) $(HEADERS)
	@echo "Generating dependencies"
	@$(CXX) -MM *.cpp > $@


-include depend