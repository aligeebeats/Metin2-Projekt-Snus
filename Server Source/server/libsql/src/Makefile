BIN = ../libsql.a
CXX = CC
CFLAGS += -w -Wall -ggdb -g -O2 -std=c++17 -fstack-protector -static -m32

#####################################################
CPP=AsyncSQL.cpp Semaphore.cpp Statement.cpp Tellwait.cpp
INCDIR += -I../../../extern/FreeBSD/mysql

all: $(BIN)
clean:
	@echo Delete .obj files
	@echo Delete lib file
	@rm -f *.o
	@rm -f $(BIN)

%.o: %.cpp
	@echo Compiling $*
	@$(CXX) $(INCDIR) $(CFLAGS) -c $^ -o $@

$(BIN): $(CPP:%.cpp=%.o)
	@echo Create static library
	@ar cru $(BIN) $^
	@echo Compiling
	@ranlib $(BIN)
	@rm -rf *.o