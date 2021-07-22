CC = clang++

all:
	$(CC) src/*.cc -flto=full -fuse-ld=lld -falign-functions=64 -ffast-math -lrtaudio -lpthread -O3 -o SimpleEQ 

debug:
	$(CC) src/*.cc -flto=thin -ffast-math -lrtaudio -lpthread -O2 -g -o SimpleEQ-debug
