CC = clang++

all:
	$(CC) src/*.cc -ffast-math -lrtaudio -lpthread -O2 -o SimpleEQ 

debug:
	$(CC) src/*.cc -ffast-math -lrtaudio -lpthread -O2 -g -o SimpleEQ-debug
