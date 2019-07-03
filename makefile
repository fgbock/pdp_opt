BIN = Binary
SRC = Source
run: optimizer.o parser.o heuristic.o
	g++ -o optimizer.exe $(BIN)\optimizer.o $(BIN)\parser.o $(BIN)\heuristic.o
	
optimizer.o: $(SRC)\optimizer.cpp $(SRC)\parser.h
	g++ -o $(BIN)\optimizer.o -c $(SRC)\optimizer.cpp

parser.o: $(SRC)\parser.cpp $(SRC)\parser.h
	g++ -o $(BIN)\parser.o -c $(SRC)\parser.cpp
	
heuristic.o: $(SRC)\heuristic.cpp $(SRC)\heuristic.h
	g++ -o $(BIN)\heuristic.o -c $(SRC)\heuristic.cpp
	
clean:
	rm optimizer.exe $(BIN)\optimizer.o $(BIN)\parser.o $(BIN)\heuristic.o