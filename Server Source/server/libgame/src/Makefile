BIN = ../libgame.a
CXX = CC
CFLAGS = -Wall -ggdb -g -O2 -std=c++17 -fstack-protector -static -m32

CPP=attribute.cpp grid.cpp targa.cpp

all: $(BIN)

clean:
	@echo Delete .obj files
	@echo Delete lib file
	@rm -f *.o
	@rm -f $(BIN)

%.o: %.cpp
	@echo Compiling $*
	@$(CXX) $(CFLAGS) -c $^ -o $@

$(BIN): $(CPP:%.cpp=%.o)
	@echo Create static library
	@ar cru $(BIN) $^
	@echo Compiling
	@ranlib $(BIN)
	@rm -rf *.o