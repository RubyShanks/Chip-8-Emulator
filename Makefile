all:
	g++ -I src/include -L src/lib -o p1 p1.cpp -lmingw32 -lSDL2main -lSDL2
