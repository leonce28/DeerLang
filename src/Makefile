# Makefile for CMM

ifeq ($(OS), Windows_NT)

all:
	if not exist ..\bin (mkdir ..\bin)
	g++ -DNDEBUG -O3 -std=gnu++17 -o ..\bin\test CMM.cpp -lboost_program_options

debug:
	if not exist ..\bin (mkdir ..\bin)
	g++ -g -std=gnu++17 -Wall -o ..\bin\test CMM.cpp -lboost_program_options

clean:
	if exist ..\bin\test.exe (del /F ..\bin\test.exe)

else

all:
	mkdir -p ../bin
	g++ -DNDEBUG -O3 -std=gnu++17 -o ../bin/test CMM.cpp -lboost_program_options

debug:
	mkdir -p ../bin
	g++ -g -std=gnu++17 -Wall -o ../bin/test CMM.cpp -lboost_program_options

clean:
	rm -f ../bin/test

endif
