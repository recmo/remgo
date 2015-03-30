CPP:=clang++ -Wall -O3 -g -std=c++0x -Wextra -Werror=return-type -Werror=switch -I.

SOURCES=$(shell cat sources | grep .cpp)

remgo: $(patsubst %.cpp,%.o,$(SOURCES))
	$(CPP) $^ -lm -o $@

%.o: %.cpp
	$(CPP) -c $< -o $@

clean:
	rm -f $(patsubst %.cpp,%.o,$(SOURCES))

PACHI1=others/pachi.elf -d 0 -t =500 maximize_score
PACHI2=others/pachi.elf -d 0 -e random
PACHI3=others/pachi.elf -d 0 -e montecarlo -t =5000

test: remgo
	#./twogtp.py --black 'gnugo --chinese-rules --mode=gtp --level=0' --white './remgo' --verbose 2
	./twogtp.py --black '${PACHI3}' --white './remgo' --verbose 2


startBot: remgo
	java -jar others/kgsGtp-3.5.20/kgsGtp.jar kgsGtp.config

stopBot: remgo
	killall -s SIGHUP remgo
