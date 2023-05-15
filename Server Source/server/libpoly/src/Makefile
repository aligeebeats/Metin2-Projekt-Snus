BIN = ../libpoly.a
CXX = CC
CFLAGS = -Wall -ggdb -std=c++17 -g -O2 -fstack-protector -static -m32


CPP=Base.cpp Poly.cpp Symbol.cpp SymTable.cpp
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