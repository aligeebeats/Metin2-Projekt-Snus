CC = CC

CFLAGS = -w -g -ggdb -pipe -O2 -std=c++17 -fstack-protector-all -m32
TARGET = ../game

##############################################################################
# libgame
INCDIR += -I../../libgame/src
LIBDIR += -L../../libgame

# libpoly
INCDIR += -I../../libpoly/src
LIBDIR += -L../../libpoly

# libsql
INCDIR += -I../../libsql/src
LIBDIR += -L../../libsql

# libthecore
INCDIR += -I../../libthecore/src
LIBDIR += -L../../libthecore

#	Mysql
INCDIR += -I../../../extern/FreeBSD/mysql
LIBDIR += -L../../../extern/FreeBSD/mysql/lib

#	liblua
INCDIR += -I../../../extern/FreeBSD/lua
LIBDIR += -L../../../extern/FreeBSD/lua/lib

#	Boost
INCDIR += -I../../../extern/FreeBSD/boost
LIBDIR += -L../../../extern/FreeBSD/boost/lib

#	lzo
INCDIR += -I../../../extern/FreeBSD/lzo
LIBDIR += -L../../../extern/FreeBSD/lzo/lib

#	CryptoPP
#INCDIR += -I../../../extern/FreeBSD/cryptopp
#LIBDIR += -L../../../extern/FreeBSD/cryptopp/lib
#LIBS   += -lcryptopp

#	devil
INCDIR += -I../../../extern/FreeBSD/devil
LIBDIR += -L../../../extern/FreeBSD/devil/lib

LIBS += -lmysqlclient -lz -pthread -lgame -lpoly -lsql -lthecore -llua -llualib -lmd -lm -lssl -pthread -lIL -lpng -ltiff -lmng -llcms -ljpeg -lcrypto
##############################################################################
CPP = vid.cpp\
	minilzo.cpp\
	FSM.cpp\
	MarkConvert.cpp\
	MarkImage.cpp\
	MarkManager.cpp\
	OXEvent.cpp\
	ani.cpp\
	cipher.cpp\
	battle.cpp\
	blend_item.cpp\
	buffer_manager.cpp\
	building.cpp\
	char.cpp\
	char_affect.cpp\
	char_battle.cpp\
	char_change_empire.cpp\
	char_horse.cpp\
	char_item.cpp\
	char_manager.cpp\
	char_quickslot.cpp\
	char_resist.cpp\
	char_skill.cpp\
	char_state.cpp\
	PetSystem.cpp\
	cmd.cpp\
	cmd_emotion.cpp\
	cmd_general.cpp\
	cmd_gm.cpp\
	cmd_oxevent.cpp\
	config.cpp\
	constants.cpp\
	crc32.cpp\
	cube.cpp\
	db.cpp\
	desc.cpp\
	desc_client.cpp\
	desc_manager.cpp\
	desc_p2p.cpp\
	dungeon.cpp\
	entity.cpp\
	entity_view.cpp\
	event.cpp\
	event_queue.cpp\
	exchange.cpp\
	file_loader.cpp\
	fishing.cpp\
	gm.cpp\
	guild.cpp\
	guild_manager.cpp\
	guild_war.cpp\
	horse_rider.cpp\
	horsename_manager.cpp\
	input.cpp\
	input_auth.cpp\
	input_db.cpp\
	input_login.cpp\
	input_main.cpp\
	input_p2p.cpp\
	input_udp.cpp\
	item.cpp\
	item_addon.cpp\
	item_attribute.cpp\
	item_manager.cpp\
	item_manager_idrange.cpp\
	locale.cpp\
	locale_service.cpp\
	log.cpp\
	login_data.cpp\
	lzo_manager.cpp\
	main.cpp\
	marriage.cpp\
	messenger_manager.cpp\
	mining.cpp\
	mob_manager.cpp\
	motion.cpp\
	p2p.cpp\
	packet_info.cpp\
	party.cpp\
	polymorph.cpp\
	priv_manager.cpp\
	pvp.cpp\
	questevent.cpp\
	questlua.cpp\
	questlua_affect.cpp\
	questlua_building.cpp\
	questlua_danceevent.cpp\
	questlua_dungeon.cpp\
	questlua_game.cpp\
	questlua_global.cpp\
	questlua_guild.cpp\
	questlua_horse.cpp\
	questlua_pet.cpp\
	questlua_item.cpp\
	questlua_marriage.cpp\
	questlua_npc.cpp\
	questlua_oxevent.cpp\
	questlua_party.cpp\
	questlua_pc.cpp\
	questlua_quest.cpp\
	questlua_target.cpp\
	questmanager.cpp\
	questnpc.cpp\
	questpc.cpp\
	refine.cpp\
	regen.cpp\
	safebox.cpp\
	sectree.cpp\
	sectree_manager.cpp\
	sequence.cpp\
	shop.cpp\
	skill.cpp\
	start_position.cpp\
	target.cpp\
	text_file_loader.cpp\
	trigger.cpp\
	utils.cpp\
	vector.cpp\
	war_map.cpp\
	wedding.cpp\
	xmas_event.cpp\
	map_location.cpp\
	BlueDragon.cpp\
	BlueDragon_Binder.cpp\
	DragonLair.cpp\
	questlua_dragonlair.cpp\
	skill_power.cpp\
	affect.cpp\
	buff_on_attributes.cpp\
	dragon_soul_table.cpp\
	DragonSoul.cpp\
	group_text_parse_tree.cpp\
	char_dragonsoul.cpp\
	questlua_dragonsoul.cpp\
	shop_manager.cpp\
	shopEx.cpp\
	item_manager_read_tables.cpp\
	new_switchbot.cpp\
	char_hunting.cpp\
	buff_npc_system.cpp\
	itemshop.cpp
###############################################################################
CPPOBJS	= $(CPP:%.cpp=$(OBJDIR)/%.o)

OBJDIR = .obj
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

default: $(TARGET)

clean:
	@echo Clean done!
	@find . -type f | xargs -n 5 touch
	@rm -rf $(OBJDIR)
	@rm -rf $(TARGET)

$(OBJDIR)/%.o: %.cpp
	@echo compile $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

$(TARGET): $(CPPOBJS)
	@echo linking....
	@$(CC) $(CFLAGS) $(LIBDIR) $(CPPOBJS) $(LIBS) -o $(TARGET)