# Makefile
CC = g++
CCFLAGS = -g
STD = -std=c++11

all: main.o
	$(CC) $(CCFLAGS) -o main main.o -lncursesw

clean:
	rm -f *.o

%.o : %.cpp	
	$(CC) $(STD) $(CCFLAGS) -c $<