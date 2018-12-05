#DEBUGFLAGS=-g -D_DEBUG
DEBUGFLAGS=-O2
CPPFLAGS=$(DEBUGFLAGS) -Wall -Wno-strict-aliasing -std=gnu++14
OBJS=main.o minheap.o utf8.o huffcode.o

all: huffman

main.o: main.cpp utf8.h utf8.o minheap.h huffcode.h node.h stats.h
	g++ $(CPPFLAGS) -c $< -o $@

huffcode.o: huffcode.cpp huffcode.h minheap.h node.h
	g++ $(CPPFLAGS) -c $< -o $@

minheap.o: minheap.cpp minheap.h node.h
	g++ $(CPPFLAGS) -c $< -o $@

utf8.o: utf8.cpp utf8.h
	g++ $(CPPFLAGS) -c $< -o $@

huffman: $(OBJS)
	g++ $(CPPFLAGS) $(OBJS) -o huffman

clean: cleandocs
	rm -f $(OBJS) huffman

docs:
	doxygen

cleandocs:
	rm -rf html latex # remove Doxygen stuff too

test: huffman
	./huffman -e testtext testtext.z
	./huffman -d testtext.z testtext2
	echo "diff of files:"
	diff -y --suppress-common-lines testtext testtext2
