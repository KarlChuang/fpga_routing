all: main

main: src/main/main.o src/graph/graph.o src/graph/iostream.o src/graph/advancedTraverse.o
	g++ src/main/main.o src/graph/graph.o src/graph/iostream.o src/graph/advancedTraverse.o -o bin/main

src/main/main.o: src/main/main.cpp include
	g++ -I include -c src/main/main.cpp -o src/main/main.o

src/graph/graph.o: src/graph/graph.cpp include
	g++ -I include -c src/graph/graph.cpp -o src/graph/graph.o

src/graph/iostream.o: src/graph/iostream.cpp include
	g++ -I include -c src/graph/iostream.cpp -o src/graph/iostream.o

src/graph/advancedTraverse.o: src/graph/advancedTraverse.cpp include
	g++ -I include -c src/graph/advancedTraverse.cpp -o src/graph/advancedTraverse.o

clean:
	@rm -rf ./**/*o
	@rm -rf ./**/**/*o
