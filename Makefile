CC = clang++

all:
	$(CC) src/*.cc -flto=full -fuse-ld=lld -falign-functions=32 -ffast-math -lrtaudio -lpthread -O2 -o SimpleEQ 

debug:
	$(CC) src/*.cc -flto=thin -ffast-math -lrtaudio -lpthread -O2 -g -o SimpleEQ-debug
