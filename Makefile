CPP:=g++-4.6 -Wall -O2 -g -std=c++0x -lm -DLOCAL -march=nocona -Wextra -Wno-unused-parameter -Werror=return-type -Werror=switch -I.


SOURCES=$(shell cat sources | grep .cpp)

entry: $(patsubst %.cpp,%.o,$(SOURCES))
	$(CPP) $^ -o $@

%.o: %.cpp
	$(CPP) -c $< -o $@

monolith: monolith.cpp
	$(CPP) monolith.cpp -o monolith

monolith.cpp: sources $(shell cat sources)
	cat `cat sources` > monolith.cpp
	sed -i "s|^#pragma once$$||g" monolith.cpp
	sed -i "s|^#include \".*$$||g" monolith.cpp

competition: monolith
	cd caia/ayu/bin; ./competition.sh player1 player2 player3 6a0d886 monolith 
