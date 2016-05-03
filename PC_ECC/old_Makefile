CXX=g++
LD=g++
CFLAGS=-Wall -pedantic -g
LIBS=

rs232.o: rs232.h rs232.c
	$(CXX) $(CFLAGS) -c -o rs232.o rs232.c 

main.o: main.cpp rs232.h
	$(CXX) $(CFLAGS) -c -o main.o main.cpp

all: rs232.o  main.o
	$(LD) -o try rs232.o main.o $(LIBS)

clean:
	rm main.o try rs232.o
