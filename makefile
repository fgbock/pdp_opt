BIN = Binary
SRC = Source
FLG = -fopenmp
COMPILER = g++

run: optimizer.o parser.o heuristic.o
	$(COMPILER) $(FLG) -o optimizer.exe $(BIN)\optimizer.o $(BIN)\parser.o $(BIN)\heuristic.o
	
optimizer.o: $(SRC)\optimizer.cpp $(SRC)\parser.h
	$(COMPILER) $(FLG) -o $(BIN)\optimizer.o -c $(SRC)\optimizer.cpp

parser.o: $(SRC)\parser.cpp $(SRC)\parser.h
	$(COMPILER) $(FLG) -o $(BIN)\parser.o -c $(SRC)\parser.cpp
	
heuristic.o: $(SRC)\heuristic.cpp $(SRC)\heuristic.h
	$(COMPILER) $(FLG) -o $(BIN)\heuristic.o -c $(SRC)\heuristic.cpp
	
clean:
	rm optimizer.exe $(BIN)\optimizer.o $(BIN)\parser.o $(BIN)\heuristic.o