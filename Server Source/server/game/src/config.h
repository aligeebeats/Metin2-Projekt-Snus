#pragma once

enum
{
	ADDRESS_MAX_LEN = 15
};

void						config_init(const std::string& st_localeServiceName);
extern char					sql_addr[256];
extern WORD					mother_port;
extern WORD					p2p_port;
extern char					db_addr[ADDRESS_MAX_LEN + 1];
extern WORD					db_port;
extern int					passes_per_sec;
extern int					save_event_second_cycle;
extern int					ping_event_second_cycle;
extern int					test_server;
extern bool					guild_mark_server;
extern BYTE					guild_mark_min_level;
extern bool					g_bNoMoreClient;
extern BYTE					g_bChannel;
extern bool					map_allow_find(int index);
extern void					map_allow_copy(long * pl, int size);
extern bool					no_wander;
extern int					g_iUserLimit;
extern time_t				g_global_time;
const char *				get_table_postfix();
extern std::string			g_stHostname;
extern std::string			g_stLocale;
extern std::string			g_stLocaleFilename;
extern char					g_szPublicIP[16];
extern char					g_szInternalIP[16];
extern int					(*is_twobyte) (const char * str);
extern int					(check_name) (const char * str);
extern int					g_iFullUserCount;
extern int					g_iBusyUserCount;
extern void					LoadStateUserCount();
extern BYTE					g_bAuthServer;
extern BYTE					PK_PROTECT_LEVEL;
extern void					LoadValidCRCList();
extern bool					IsValidProcessCRC(DWORD dwCRC);
extern bool					IsValidFileCRC(DWORD dwCRC);
extern std::string			g_stAuthMasterIP;
extern WORD					g_wAuthMasterPort;
extern std::string			g_stClientVersion;
extern std::string			g_stQuestDir;
extern std::set<std::string> g_setQuestObjectDir;
extern int					SPEEDHACK_LIMIT_COUNT;
extern int 					SPEEDHACK_LIMIT_BONUS;
extern int					g_iSyncHackLimitCount;
extern int					g_server_id;
extern std::string			g_strWebMallURL;
extern int					VIEW_RANGE;
extern int					VIEW_BONUS_RANGE;
extern bool					g_protectNormalPlayer;
extern int					gPlayerMaxLevel;
extern bool					g_BlockCharCreation;
#ifndef _IMPROVED_PACKET_ENCRYPTION_
extern bool					g_isShutdowned;
#endif
