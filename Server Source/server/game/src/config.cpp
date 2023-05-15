#include "stdafx.h"
#include <sstream>
#ifndef _WIN32
#include <ifaddrs.h>
#endif
#include "constants.h"
#include "utils.h"
#include "log.h"
#include "desc.h"
#include "desc_manager.h"
#include "item_manager.h"
#include "p2p.h"
#include "char.h"
#include "war_map.h"
#include "locale_service.h"
#include "config.h"
#include "db.h"
#include "skill_power.h"

using std::string;

BYTE	g_bChannel = 0;
WORD	mother_port = 50080;
int		passes_per_sec = 25;
WORD	db_port = 0;
WORD	p2p_port = 50900;
char	db_addr[ADDRESS_MAX_LEN + 1];
int		save_event_second_cycle = passes_per_sec * 120;
int		ping_event_second_cycle = passes_per_sec * 60;
bool	g_bNoMoreClient = false;
int		test_server = 0;
bool	guild_mark_server = true;
BYTE	guild_mark_min_level = 3;
bool	no_wander = false;
int		g_iUserLimit = 32768;
char	g_szPublicIP[16] = "0";
char	g_szInternalIP[16] = "0";
int		g_iFullUserCount = 1200;
int		g_iBusyUserCount = 150;
BYTE	g_bAuthServer = false;
string	g_stClientVersion = CLIENT_VERSION;
string	g_stAuthMasterIP;
WORD	g_wAuthMasterPort = 0;

static std::set<DWORD> s_set_dwFileCRC;
static std::set<DWORD> s_set_dwProcessCRC;

string g_stHostname = "";
string g_table_postfix = "";
string g_stQuestDir = "./share/quest";
string g_stDefaultQuestObjectDir = "./share/quest/object";
std::set<string> g_setQuestObjectDir;

extern string g_stLocale;
extern std::string	g_stLocaleFilename;

int SPEEDHACK_LIMIT_COUNT   = 50;
int SPEEDHACK_LIMIT_BONUS   = 80;
int g_iSyncHackLimitCount = 10;
int VIEW_RANGE = 5000;
int VIEW_BONUS_RANGE = 500;
int g_server_id = 0;
string g_strWebMallURL = "www.metin2.xxx";

unsigned int g_uiSpamBlockDuration = 60 * 15;
unsigned int g_uiSpamBlockScore = 100;
unsigned int g_uiSpamReloadCycle = 60 * 10;
int			g_iSpamBlockMaxLevel = 10;
void		LoadStateUserCount();
void		LoadValidCRCList();
bool		g_protectNormalPlayer   = false;

int gPlayerMaxLevel = 99;
bool g_BlockCharCreation = false;
#ifndef _IMPROVED_PACKET_ENCRYPTION_
bool g_isShutdowned = false;
#endif

bool is_string_true(const char * string)
{
	bool	result = 0;
	if (isnhdigit(*string))
	{
		str_to_number(result, string);
		return result > 0 ? true : false;
	}
	else if (LOWER(*string) == 't')
		return true;
	else
		return false;
}

static std::set<int> s_set_map_allows;

bool map_allow_find(int index)
{
	if (g_bAuthServer)
		return false;

	if (s_set_map_allows.find(index) == s_set_map_allows.end())
		return false;

	return true;
}

void map_allow_log()
{
	for (auto i = s_set_map_allows.begin(); i != s_set_map_allows.end(); ++i)
		sys_log(0, "MAP_ALLOW: %d", *i);
}

void map_allow_add(int index)
{
	if (map_allow_find(index) == true)
	{
		fprintf(stdout, "!!! FATAL ERROR !!! multiple MAP_ALLOW setting!!\n");
		exit(1);
	}

	fprintf(stdout, "MAP ALLOW %d\n", index);
	s_set_map_allows.insert(index);
}

void map_allow_copy(long * pl, int size)
{
	int iCount = 0;
	auto it = s_set_map_allows.begin();

	while (it != s_set_map_allows.end())
	{
		int i = *(it++);
		*(pl++) = i;

		if (++iCount > size)
			break;
	}
}

bool GetIPInfo()
{
#ifndef _WIN32
	struct ifaddrs* ifaddrp = NULL;

	if (0 != getifaddrs(&ifaddrp))
	{
		return false;
	}

	for (struct ifaddrs* ifap = ifaddrp; NULL != ifap; ifap = ifap->ifa_next)
	{
		struct sockaddr_in* sai = (struct sockaddr_in*) ifap->ifa_addr;

		if (!ifap->ifa_netmask || sai->sin_addr.s_addr == 0 || sai->sin_addr.s_addr == 16777343)
		{
			continue;
		}
#else
	WSADATA wsa_data;
	char host_name[100];
	HOSTENT* host_ent;
	int n = 0;

	if (WSAStartup(0x0101, &wsa_data))
	{
		return false;
	}

	gethostname(host_name, sizeof(host_name));
	host_ent = gethostbyname(host_name);
	if (host_ent == NULL)
	{
		return false;
	}
	for (; host_ent->h_addr_list[n] != NULL; ++n)
	{
		struct sockaddr_in addr;
		struct sockaddr_in* sai = &addr;
		memcpy(&sai->sin_addr.s_addr, host_ent->h_addr_list[n], host_ent->h_length);
#endif

		char* netip = inet_ntoa(sai->sin_addr);

		if (!strncmp(netip, "10.", 3))
		{
			strlcpy(g_szInternalIP, netip, sizeof(g_szInternalIP));
		}
		else if (g_szPublicIP[0] == '0')
		{
			strlcpy(g_szPublicIP, netip, sizeof(g_szPublicIP));
		}
	}

#ifndef _WIN32
	freeifaddrs(ifaddrp);
#else
	WSACleanup();
#endif

	if (g_szPublicIP[0] != '0')
	{
		return true;
	}
	else
	{
		return false;
	}
}

void config_init(const string& st_localeServiceName)
{
	FILE* fp;

	char buf[256];
	char token_string[256];
	char value_string[256];

	string st_configFileName;

	st_configFileName.reserve(32);
	st_configFileName = "CONFIG";

	if (!st_localeServiceName.empty())
	{
		st_configFileName += ".";
		st_configFileName += st_localeServiceName;
	}

	if (!(fp = fopen(st_configFileName.c_str(), "r")))
	{
		fprintf(stderr, "Can not open [%s]\n", st_configFileName.c_str());
		exit(1);
	}

	if (!GetIPInfo())
	{
		fprintf(stderr, "Can not get public ip address\n");
		exit(1);
	}

	char db_host[2][64], db_user[2][64], db_pwd[2][64], db_db[2][64];
	int mysql_db_port[2];

	for (int n = 0; n < 2; ++n)
	{
		*db_host[n] = '\0';
		*db_user[n] = '\0';
		*db_pwd[n] = '\0';
		*db_db[n] = '\0';
		mysql_db_port[n] = 0;
	}

	char log_host[64], log_user[64], log_pwd[64], log_db[64];
	int log_port = 0;

	*log_host = '\0';
	*log_user = '\0';
	*log_pwd = '\0';
	*log_db = '\0';


	bool isCommonSQL = false;
	bool isPlayerSQL = false;

	FILE* fpOnlyForDB;

	if (!(fpOnlyForDB = fopen(st_configFileName.c_str(), "r")))
	{
		fprintf(stderr, "Can not open [%s]\n", st_configFileName.c_str());
		exit(1);
	}

	while (fgets(buf, 256, fpOnlyForDB))
	{
		parse_token(buf, token_string, value_string);

		TOKEN("hostname")
		{
			g_stHostname = value_string;
			continue;
		}

		TOKEN("channel")
		{
			str_to_number(g_bChannel, value_string);
			continue;
		}

		TOKEN("sql_player")
		{
			const char* line = two_arguments(value_string, db_host[0], sizeof(db_host[0]), db_user[0], sizeof(db_user[0]));
			line = two_arguments(line, db_pwd[0], sizeof(db_pwd[0]), db_db[0], sizeof(db_db[0]));

			if ('\0' != line[0])
			{
				char buf[256];
				one_argument(line, buf, sizeof(buf));
				str_to_number(mysql_db_port[0], buf);
			}

			if (!*db_host[0] || !*db_user[0] || !*db_pwd[0] || !*db_db[0])
			{
				fprintf(stderr, "PLAYER_SQL syntax: logsql <host user password db>\n");
				exit(1);
			}

			char buf[1024];
			snprintf(buf, sizeof(buf), "PLAYER_SQL: %s %s %s %s %d", db_host[0], db_user[0], db_pwd[0], db_db[0], mysql_db_port[0]);
			isPlayerSQL = true;
			continue;
		}

		TOKEN("sql_common")
		{
			const char* line = two_arguments(value_string, db_host[1], sizeof(db_host[1]), db_user[1], sizeof(db_user[1]));
			line = two_arguments(line, db_pwd[1], sizeof(db_pwd[1]), db_db[1], sizeof(db_db[1]));

			if ('\0' != line[0])
			{
				char buf[256];
				one_argument(line, buf, sizeof(buf));
				str_to_number(mysql_db_port[1], buf);
			}

			if (!*db_host[1] || !*db_user[1] || !*db_pwd[1] || !*db_db[1])
			{
				fprintf(stderr, "COMMON_SQL syntax: logsql <host user password db>\n");
				exit(1);
			}

			char buf[1024];
			snprintf(buf, sizeof(buf), "COMMON_SQL: %s %s %s %s %d", db_host[1], db_user[1], db_pwd[1], db_db[1], mysql_db_port[1]);
			isCommonSQL = true;
			continue;
		}

		TOKEN("sql_log")
		{
			const char* line = two_arguments(value_string, log_host, sizeof(log_host), log_user, sizeof(log_user));
			line = two_arguments(line, log_pwd, sizeof(log_pwd), log_db, sizeof(log_db));

			if ('\0' != line[0])
			{
				char buf[256];
				one_argument(line, buf, sizeof(buf));
				str_to_number(log_port, buf);
			}

			if (!*log_host || !*log_user || !*log_pwd || !*log_db)
			{
				fprintf(stderr, "LOG_SQL syntax: logsql <host user password db>\n");
				exit(1);
			}

			char buf[1024];
			snprintf(buf, sizeof(buf), "LOG_SQL: %s %s %s %s %d", log_host, log_user, log_pwd, log_db, log_port);
			continue;
		}
	}
	fclose(fpOnlyForDB);

	if (!isCommonSQL)
	{
		puts("LOAD_COMMON_SQL_INFO_FAILURE:");
		puts("");
		puts("CONFIG:");
		puts("------------------------------------------------");
		puts("COMMON_SQL: HOST USER PASSWORD DATABASE");
		puts("");
		exit(1);
	}

	if (!isPlayerSQL)
	{
		puts("LOAD_PLAYER_SQL_INFO_FAILURE:");
		puts("");
		puts("CONFIG:");
		puts("------------------------------------------------");
		puts("PLAYER_SQL: HOST USER PASSWORD DATABASE");
		puts("");
		exit(1);
	}

	AccountDB::instance().Connect(db_host[1], mysql_db_port[1], db_user[1], db_pwd[1], db_db[1]);

	if (false == AccountDB::instance().IsConnected())
	{
		fprintf(stderr, "cannot start server while no common sql connected\n");
		exit(1);
	}

	printf("-----------------------------------------------\n");
	fprintf(stdout, "CommonSQL connected\n");

	{
		char szQuery[512];
		snprintf(szQuery, sizeof(szQuery), "SELECT mKey, mValue FROM locale");

		std::unique_ptr<SQLMsg> pMsg(AccountDB::instance().DirectQuery(szQuery));

		if (pMsg->Get()->uiNumRows == 0)
		{
			fprintf(stderr, "COMMON_SQL: DirectQuery failed : %s\n", szQuery);
			exit(1);
		}
	}

	AccountDB::instance().SetLocale(g_stLocale);
	AccountDB::instance().ConnectAsync(db_host[1], mysql_db_port[1], db_user[1], db_pwd[1], db_db[1], g_stLocale.c_str());

	DBManager::instance().Connect(db_host[0], mysql_db_port[0], db_user[0], db_pwd[0], db_db[0]);

	if (!DBManager::instance().IsConnected())
	{
		fprintf(stderr, "PlayerSQL.ConnectError\n");
		exit(1);
	}

	fprintf(stdout, "PlayerSQL connected\n");

	if (false == g_bAuthServer)
	{
		LogManager::instance().Connect(log_host, log_port, log_user, log_pwd, log_db);

		if (!LogManager::instance().IsConnected())
		{
			fprintf(stderr, "LogSQL.ConnectError\n");
			exit(1);
		}

		fprintf(stdout, "LogSQL connected\n");
		printf("-----------------------------------------------\n");

		LogManager::instance().BootLog(g_stHostname.c_str(), g_bChannel);
	}

	{
		char szQuery[256];
		snprintf(szQuery, sizeof(szQuery), "SELECT mValue FROM locale WHERE mKey='SKILL_POWER_BY_LEVEL'");
		std::unique_ptr<SQLMsg> pMsg(AccountDB::instance().DirectQuery(szQuery));

		if (pMsg->Get()->uiNumRows == 0)
		{
			fprintf(stderr, "[SKILL_PERCENT] Query failed: %s", szQuery);
			exit(1);
		}

		MYSQL_ROW row;

		row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		const char* p = row[0];
		int cnt = 0;
		char num[128];
		int aiBaseSkillPowerByLevelTable[SKILL_MAX_LEVEL + 1];

		while (*p != '\0' && cnt < (SKILL_MAX_LEVEL + 1))
		{
			p = one_argument(p, num, sizeof(num));
			aiBaseSkillPowerByLevelTable[cnt++] = atoi(num);

			if (*p == '\0')
			{
				if (cnt != (SKILL_MAX_LEVEL + 1))
				{
					fprintf(stderr, "[SKILL_PERCENT] locale table has not enough skill information! (count: %d query: %s)", cnt, szQuery);
					exit(1);
				}
				break;
			}
		}

		for (int job = 0; job < JOB_MAX_NUM * 2; ++job)
		{
			snprintf(szQuery, sizeof(szQuery), "SELECT mValue from locale where mKey='SKILL_POWER_BY_LEVEL_TYPE%d' ORDER BY CAST(mValue AS unsigned)", job);
			std::unique_ptr<SQLMsg> pMsg(AccountDB::instance().DirectQuery(szQuery));

			if (pMsg->Get()->uiNumRows == 0)
			{
				CTableBySkill::instance().SetSkillPowerByLevelFromType(job, aiBaseSkillPowerByLevelTable);
				continue;
			}

			row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			cnt = 0;
			p = row[0];
			int aiSkillTable[SKILL_MAX_LEVEL + 1];

			while (*p != '\0' && cnt < (SKILL_MAX_LEVEL + 1))
			{
				p = one_argument(p, num, sizeof(num));
				aiSkillTable[cnt++] = atoi(num);

				if (*p == '\0')
				{
					if (cnt != (SKILL_MAX_LEVEL + 1))
					{
						fprintf(stderr, "[SKILL_PERCENT] locale table has not enough skill information! (count: %d query: %s)", cnt, szQuery);
						exit(1);
					}
					break;
				}
			}

			CTableBySkill::instance().SetSkillPowerByLevelFromType(job, aiSkillTable);
		}
	}

	log_set_expiration_days(2);
	while (fgets(buf, 256, fp))
	{
		parse_token(buf, token_string, value_string);

		TOKEN("mark_server")
		{
			guild_mark_server = is_string_true(value_string);
			continue;
		}

		TOKEN("mark_min_level")
		{
			str_to_number(guild_mark_min_level, value_string);
			guild_mark_min_level = MINMAX(0, guild_mark_min_level, GUILD_MAX_LEVEL);
			continue;
		}

		TOKEN("port")
		{
			str_to_number(mother_port, value_string);
			continue;
		}

		TOKEN("log_keep_days")
		{
			int i = 0;
			str_to_number(i, value_string);
			log_set_expiration_days(MINMAX(1, i, 90));
			continue;
		}

		TOKEN("passes_per_sec")
		{
			str_to_number(passes_per_sec, value_string);
			continue;
		}

		TOKEN("p2p_port")
		{
			str_to_number(p2p_port, value_string);
			continue;
		}

		TOKEN("db_port")
		{
			str_to_number(db_port, value_string);
			continue;
		}

		TOKEN("db_addr")
		{
			strlcpy(db_addr, value_string, sizeof(db_addr));

			for (int n = 0; n < ADDRESS_MAX_LEN; ++n)
			{
				if (db_addr[n] == ' ')
				{
					db_addr[n] = '\0';
				}
			}
			continue;
		}

		TOKEN("save_event_second_cycle")
		{
			int	cycle = 0;
			str_to_number(cycle, value_string);
			save_event_second_cycle = cycle * passes_per_sec;
			continue;
		}

		TOKEN("ping_event_second_cycle")
		{
			int	cycle = 0;
			str_to_number(cycle, value_string);
			ping_event_second_cycle = cycle * passes_per_sec;
			continue;
		}

		TOKEN("table_postfix")
		{
			g_table_postfix = value_string;
			continue;
		}

		TOKEN("test_server")
		{
			printf("-----------------------------------------------\n");
			printf("TEST_SERVER\n");
			printf("-----------------------------------------------\n");
			str_to_number(test_server, value_string);
			continue;
		}

		TOKEN("shutdowned")
		{
			g_bNoMoreClient = true;
			continue;
		}

		TOKEN("map_allow")
		{
			char* p = value_string;
			string stNum;

			for (; *p; p++)
			{
				if (isnhspace(*p))
				{
					if (stNum.length())
					{
						int	index = 0;
						str_to_number(index, stNum.c_str());
						map_allow_add(index);
						stNum.clear();
					}
				}
				else
				{
					stNum += *p;
				}
			}

			if (stNum.length())
			{
				int	index = 0;
				str_to_number(index, stNum.c_str());
				map_allow_add(index);
			}

			continue;
		}

		TOKEN("no_wander")
		{
			no_wander = true;
			continue;
		}

		TOKEN("user_limit")
		{
			str_to_number(g_iUserLimit, value_string);
			continue;
		}

		TOKEN("auth_server")
		{
			char szIP[32];
			char szPort[32];

			two_arguments(value_string, szIP, sizeof(szIP), szPort, sizeof(szPort));

			if (!*szIP || (!*szPort && strcasecmp(szIP, "master")))
			{
				fprintf(stderr, "AUTH_SERVER: syntax error: <ip|master> <port>\n");
				exit(1);
			}

			g_bAuthServer = true;
			if (!strcasecmp(szIP, "master"))
			{
				printf("-----------------------------------------------\n");
				fprintf(stdout, "Login server was started.\n");
				printf("-----------------------------------------------\n");
			}
			else
			{
				g_stAuthMasterIP = szIP;
				str_to_number(g_wAuthMasterPort, szPort);
				fprintf(stdout, "AUTH_SERVER: master %s %u\n", g_stAuthMasterIP.c_str(), g_wAuthMasterPort);
			}
			continue;
		}

		TOKEN("quest_dir")
		{
			sys_log(0, "QUEST_DIR SETTING : %s", value_string);
			g_stQuestDir = value_string;
		}

		TOKEN("quest_object_dir")
		{
			std::istringstream is(value_string);
			sys_log(0, "QUEST_OBJECT_DIR SETTING : %s", value_string);
			string dir;
			while (!is.eof())
			{
				is >> dir;
				if (is.fail())
				{
					break;
				}
				g_setQuestObjectDir.insert(dir);
				sys_log(0, "QUEST_OBJECT_DIR INSERT : %s", dir.c_str());
			}
		}

		TOKEN("server_id")
		{
			str_to_number(g_server_id, value_string);
		}

		TOKEN("mall_url")
		{
			g_strWebMallURL = value_string;
		}

		TOKEN("bind_ip")
		{
			strlcpy(g_szPublicIP, value_string, sizeof(g_szPublicIP));
		}

		TOKEN("view_range")
		{
			str_to_number(VIEW_RANGE, value_string);
		}

		TOKEN("protect_normal_player")
		{
			str_to_number(g_protectNormalPlayer, value_string);
		}

		TOKEN("pk_protect_level")
		{
			str_to_number(PK_PROTECT_LEVEL, value_string);
		}

		TOKEN("max_level")
		{
			str_to_number(gPlayerMaxLevel, value_string);
			gPlayerMaxLevel = MINMAX(1, gPlayerMaxLevel, PLAYER_MAX_LEVEL_CONST);
		}

		TOKEN("block_char_creation")
		{
			int tmp = 0;

			str_to_number(tmp, value_string);

			if (0 == tmp)
			{
				g_BlockCharCreation = false;
			}
			else
			{
				g_BlockCharCreation = true;
			}

			continue;
		}
	}

	if (g_setQuestObjectDir.empty())
	{
		g_setQuestObjectDir.insert(g_stDefaultQuestObjectDir);
	}

	if (0 == db_port)
	{
		fprintf(stderr, "DB_PORT not configured\n");
		exit(1);
	}

	if (0 == g_bChannel)
	{
		fprintf(stderr, "CHANNEL not configured\n");
		exit(1);
	}

	if (g_stHostname.empty())
	{
		fprintf(stderr, "HOSTNAME must be configured.\n");
		exit(1);
	}

	if (!g_bAuthServer)
	{
		LocaleService_LoadLocaleStringFile();
		printf("-----------------------------------------------\n");
		fprintf(stdout, "%s was started.\n", g_stHostname.c_str());
		printf("-----------------------------------------------\n");
	}

	fclose(fp);

	if ((fp = fopen("CMD", "r")))
	{
		while (fgets(buf, 256, fp))
		{
			char cmd[32], levelname[32];
			int level;

			two_arguments(buf, cmd, sizeof(cmd), levelname, sizeof(levelname));

			if (!*cmd || !*levelname)
			{
				fprintf(stderr, "CMD syntax error: <cmd> <DISABLE | PLAYER | PLAYER | LOW_WIZARD | WIZARD | HIGH_WIZARD | GOD>\n");
				exit(1);
			}

			if (!strcasecmp(levelname, "LOW_WIZARD"))
			{
				level = GM_LOW_WIZARD;
			}
			else if (!strcasecmp(levelname, "WIZARD"))
			{
				level = GM_WIZARD;
			}
			else if (!strcasecmp(levelname, "HIGH_WIZARD"))
			{
				level = GM_HIGH_WIZARD;
			}
			else if (!strcasecmp(levelname, "GOD"))
			{
				level = GM_GOD;
			}
			else if (!strcasecmp(levelname, "IMPLEMENTOR"))
			{
				level = GM_IMPLEMENTOR;
			}
			else if (!strcasecmp(levelname, "PLAYER"))
			{
				level = GM_PLAYER;
			}
			else if (!strcasecmp(levelname, "DISABLE"))
			{
				level = GM_IMPLEMENTOR + 1;
			}
			else
			{
				fprintf(stderr, "CMD syntax error: <cmd> <DISABLE  | PLAYER | LOW_WIZARD | WIZARD | HIGH_WIZARD | GOD>\n");
				exit(1);
			}

			interpreter_set_privilege(cmd, level);
		}

		fclose(fp);
	}

	LoadStateUserCount();
	CWarMapManager::instance().LoadWarMapInfo(NULL);
	if (g_szPublicIP[0] == '0')
	{
		fprintf(stderr, "Can not get public ip address\n");
		exit(1);
	}
}

const char* get_table_postfix()
{
	return g_table_postfix.c_str();
}

void LoadValidCRCList()
{
	s_set_dwProcessCRC.clear();
	s_set_dwFileCRC.clear();

	FILE * fp;
	char buf[256];

	if ((fp = fopen("CRC", "r")))
	{
		while (fgets(buf, 256, fp))
		{
			if (!*buf)
				continue;

			DWORD dwValidClientProcessCRC;
			DWORD dwValidClientFileCRC;

			sscanf(buf, " %u %u ", &dwValidClientProcessCRC, &dwValidClientFileCRC);

			s_set_dwProcessCRC.insert(dwValidClientProcessCRC);
			s_set_dwFileCRC.insert(dwValidClientFileCRC);

			fprintf(stderr, "CLIENT_CRC: %u %u\n", dwValidClientProcessCRC, dwValidClientFileCRC);
		}

		fclose(fp);
	}
}

void LoadStateUserCount()
{
	FILE * fp = fopen("state_user_count", "r");

	if (!fp)
		return;

	fscanf(fp, " %d %d ", &g_iFullUserCount, &g_iBusyUserCount);
	fclose(fp);
}

bool IsValidProcessCRC(DWORD dwCRC)
{
	return s_set_dwProcessCRC.find(dwCRC) != s_set_dwProcessCRC.end();
}

bool IsValidFileCRC(DWORD dwCRC)
{
	return s_set_dwFileCRC.find(dwCRC) != s_set_dwFileCRC.end();
}


