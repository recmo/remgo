CPP:=g++-4.6 -Wall -O2 -g -std=c++0x -lm -DLOCAL -march=nocona -Wextra -Werror=return-type -Werror=switch -I.


SOURCES=$(shell cat sources | grep .cpp)

entry: $(patsubst %.cpp,%.o,$(SOURCES))
	$(CPP) $^ -o $@

%.o: %.cpp
	$(CPP) -c $< -o $@

monolith: monolith.cpp
	$(CPP) monolith.cpp -o monolith

heuristic: monolith.cpp
	$(CPP) -DHEURISTIC monolith.cpp -o heuristic

monolith.cpp: sources $(shell cat sources)
	cat `cat sources` > monolith.cpp
	sed -i "s|^#pragma once$$||g" monolith.cpp
	sed -i "s|^#include \".*$$||g" monolith.cpp


competition: monolith heuristic
	# cd caia/ayu/bin; ./competition.sh random player1 player2 player3 6a0d886 d930825 monolith heuristic unordered
	cd caia/ayu/bin; ./competition.sh random player1 player2 player3 d930825 d17a66e heuristic monolith
	# cd caia/ayu/bin; ./competition.sh player1 heuristic monolith
	rm -f caia/ayu/refereelogs/*-player3.* caia/ayu/refereelogs/player3-*.*
	grep -R disqualified caia/ayu/refereelogs
	grep -R AYGUR caia/ayu/playerlogs

clean:
	rm *.o

clean-comp:
	rm caia/ayu/*logs*/* caia/ayu/competition-*/*

