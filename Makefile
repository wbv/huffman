CPPFLAGS=-g -Wall -pedantic -std=c++14
OBJS=main.o

all: huffman

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

huffman: $(OBJS)
	g++ $(CPPFLAGS) $(OBJS) -o huffman

clean: cleandocs
	rm -f $(OBJS) huffman

docs:
	doxygen

cleandocs:
	rm -rf html latex # remove Doxygen stuff too

tests:
	echo -ne "\0a\x55\x54\x53\x52b\x44\x43\x42\x41\0" > testnoflag.dat
	echo -ne "\1a\x55\x54\x53\x52b\x44\x43\x42\x41\0\x33\x32\x31\x30\0" \
	  > testflag.dat
