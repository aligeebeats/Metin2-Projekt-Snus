CC = CC
CFLAGS += -ggdb -w -g -O2 -pipe -std=c++17 -static -fno-stack-protector -m32
TARGET = ../db

############################################################################
#  LibTheCore
INCDIR += -I/../../libthecore/src
LIBDIR += -L../../libthecore

# LibGame
INCDIR += -I../../libgame/src
LIBDIR += -L../../libgame

#  LibSql
INCDIR += -I../../libsql/src
LIBDIR += -L../../libsql

#  LibPoly
INCDIR += -I../../libpoly/src
LIBDIR += -L../../libpoly

#  Mysql
INCDIR += -I../../../extern/FreeBSD/mysql
LIBDIR += -L../../../extern/FreeBSD/mysql/lib

#  Boost
INCDIR += -I../../../extern/FreeBSD/boost
LIBDIR += -L../../../extern/FreeBSD/boost/lib

LIBS += -lmysqlclient -lz -lthecore -lsql -lpoly -lgame -lm -lssl -lcrypto -pthread
############################################################################

CPP = Config.cpp\
	NetBase.cpp\
	Peer.cpp\
	PeerBase.cpp\
	Main.cpp\
	DBManager.cpp\
	Cache.cpp\
	LoginData.cpp\
	ClientManager.cpp\
	ClientManagerPlayer.cpp\
	ClientManagerLogin.cpp\
	ClientManagerBoot.cpp\
	ClientManagerParty.cpp\
	ClientManagerGuild.cpp\
	GuildManager.cpp\
	PrivManager.cpp\
	ItemAwardManager.cpp\
	ClientManagerEventFlag.cpp\
	Marriage.cpp\
	ItemIDRangeManager.cpp\
	ClientManagerHorseName.cpp\
	ProtoReader.cpp\
	CsvReader.cpp 

############################################################################
OBJDIR = .obj

$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)
OBJS = $(CPP:%.cpp=$(OBJDIR)/%.o)

default: $(TARGET)

clean:
	@echo Clean done!
	@find . -type f | xargs -n 5 touch
	@rm -rf $(OBJDIR)
	@rm -rf $(TARGET)

$(TARGET): $(OBJS)
	@echo linking ...
	@$(CC) $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS) -o $(TARGET)

$(OBJDIR)/%.o: %.cpp
	@echo compile $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@