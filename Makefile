all: main

main: src/main/main.o src/graph/graph.o src/graph/iostream.o
	g++ src/main/main.o src/graph/graph.o src/graph/iostream.o -o bin/main

src/main/main.o: src/main/main.cpp 
	g++ -I include -c src/main/main.cpp -o src/main/main.o

src/graph/graph.o: src/graph/graph.cpp
	g++ -I include -c src/graph/graph.cpp -o src/graph/graph.o

src/graph/iostream.o: src/graph/iostream.cpp
	g++ -I include -c src/graph/iostream.cpp -o src/graph/iostream.o

clean:
	@rm -rf ./**/*o
	@rm -rf ./**/**/*o
