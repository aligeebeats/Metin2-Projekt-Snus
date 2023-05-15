BIN = ../libthecore.a
CXX = CC
CFLAGS = -w -ggdb -g -O2 -std=c++17 -fstack-protector -static -m32


CPP=buffer.cpp fdwatch.cpp gost.cpp hangul.cpp heart.cpp\
	kstbl.cpp log.cpp main.cpp memcpy.cpp signal.cpp socket.cpp tea.cpp utils.cpp
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