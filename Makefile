CXX = clang++
LD=ld.lld
rtaudio:
	cd ./external/rtaudio && ./autogen.sh CXX=$(CXX) LD=$(LD) && make
all:
	$(CXX) src/*.cc -flto=full -Iexternal/rtaudio -Lexternal/rtaudio -lrtaudio -lpthread -O2 -o SimpleEQ
	strip -s SimpleEQ 

debug:
	$(CXX) src/*.cc -flto=thin -ffast-math -lrtaudio -lpthread -O2 -g -o SimpleEQ-debug
