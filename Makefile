CPP:=clang++ -Wall -O3 -g -std=c++0x -Wextra -Werror=return-type -Werror=switch -I.

SOURCES=$(shell cat sources | grep .cpp)

remgo: $(patsubst %.cpp,%.o,$(SOURCES))
	$(CPP) $^ -lm -o $@

%.o: %.cpp
	$(CPP) -c $< -o $@

clean:
	rm -f $(patsubst %.cpp,%.o,$(SOURCES))
