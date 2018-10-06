CPPFLAGS=-g -O2 -Wall -pedantic -std=c++14
OBJS=main.o

all: huffman

%.o: %.cpp
	gcc $(CPPFLAGS) -c $< -o $@

huffman: $(OBJS)
	gcc $(CPPFLAGS) $(OBJS) -o huffman

clean: cleandocs
	rm -f $(OBJS) huffman

docs:
	doxygen

cleandocs:
	rm -rf html latex # remove Doxygen stuff too
