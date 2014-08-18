CPP:=g++-4.6 -Wall -O2 -g -std=c++0x -lm -DLOCAL -march=nocona -Wextra -Wno-unused-parameter -Werror=return-type -Werror=switch 

main: main.cpp
	$(CPP) main.cpp -o main

competition: main
	cd caia/ayu/bin; ./competition.sh player1 player2 player3 main
