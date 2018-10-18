DEBUGFLAGS=-g -D_DEBUG
CPPFLAGS=$(DEBUGFLAGS) -Wall -pedantic -std=c++14
OBJS=main.o minheap.o utf8.o huffcode.o

all: huffman

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

huffman: $(OBJS)
	g++ $(CPPFLAGS) $(OBJS) -o huffman
	rm -f $(OBJS)

clean: cleandocs
	rm -f $(OBJS) huffman

docs:
	doxygen

cleandocs:
	rm -rf html latex # remove Doxygen stuff too

test: huffman
	./huffman -e testtext testtext.z
	./huffman -d testtext.z testtext2
