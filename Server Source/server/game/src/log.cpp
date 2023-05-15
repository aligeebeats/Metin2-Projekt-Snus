#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "log.h"

#include "char.h"
#include "desc.h"
#include "item.h"
#include "locale_service.h"

static char	__escape_hint[1024];

LogManager::LogManager() : m_bIsConnect(false)
{
}

LogManager::~LogManager()
{
}

bool LogManager::Connect(const char * host, const int port, const char * user, const char * pwd, const char * db)
{
	if (m_sql.Setup(host, user, pwd, db, g_stLocale.c_str(), false, port))
		m_bIsConnect = true;

	return m_bIsConnect;
}

void LogManager::Query(const char * c_pszFormat, ...)
{
	char szQuery[4096];
	va_list args;

	va_start(args, c_pszFormat);
	vsnprintf(szQuery, sizeof(szQuery), c_pszFormat, args);
	va_end(args);

	if (test_server)
		sys_log(0, "LOG: %s", szQuery);

	m_sql.AsyncQuery(szQuery);
}

bool LogManager::IsConnected()
{
	return m_bIsConnect;
}

void LogManager::CubeLog(DWORD dwPID, const char* szName, DWORD item_vnum, DWORD item_id, int item_count, bool success)
{
	Query("INSERT DELAYED INTO cube%s (pid, date, name, item_vnum, item_id, item_count, success) VALUES(%u, NOW(), '%s', %u, %u, %u, %u)", get_table_postfix(), dwPID, szName, item_vnum, item_id, item_count, success?1:0);
}

void LogManager::ChangeNameLog(DWORD pid, const char *old_name, const char *new_name, const char *ip)
{
	Query("INSERT DELAYED INTO change_name%s (pid, old_name, new_name, date, ip) VALUES(%u, '%s', '%s', NOW(), '%s') ", get_table_postfix(), pid, old_name, new_name, ip);
}

void LogManager::GMCommandLog(const char* szName, const char* szCommand)
{
	m_sql.EscapeString(__escape_hint, sizeof(__escape_hint), szCommand, strlen(szCommand));

	Query("INSERT DELAYED INTO command%s (date, name, command) VALUES(NOW(), '%s', '%s') ", get_table_postfix(), szName, __escape_hint);
}

void LogManager::BootLog(const char * c_pszHostName, BYTE bChannel)
{
	Query("INSERT INTO boot(date, hostname, channel) VALUES(NOW(), '%s', %d)", c_pszHostName, bChannel);
}

void LogManager::QuestRewardLog(const char * c_pszQuestName, DWORD dwPID, DWORD dwLevel, int iValue1, int iValue2)
{
	Query("INSERT INTO quest_reward%s VALUES('%s',%u,%u,2,%u,%u,NOW())", get_table_postfix(), c_pszQuestName, dwPID, dwLevel, iValue1, iValue2);
}

void LogManager::ExchangeLogGold(const char* from, const char* to, long long gold, DWORD fromAccID, DWORD toAccID)
{
	Query("INSERT INTO exchange_gold%s (date, `from`, `to`, gold ,fromAccID, toAccID) VALUES(NOW(), '%s', '%s', %lld, %d, %d)", get_table_postfix(), from, to, gold, fromAccID, toAccID);
}

void LogManager::ExchangeLogItems(const char* from, const char* to, const char* itemname, DWORD itemid, WORD count, DWORD fromAccID, DWORD toAccID)
{
	Query("INSERT INTO exchange_items%s (date, `from`, `to`, itemname, itemid, count, fromAccID, toAccID) VALUES(NOW(), '%s', '%s', '%s', %d, %d, %d, %d)", get_table_postfix(), from, to, itemname, itemid, count, fromAccID, toAccID);
}

void LogManager::RefineLog(const char* name, const char* itemname, DWORD itemid, DWORD newitemid, const char* status)
{
	Query("INSERT INTO refine%s (date, name, itemname, itemid, newitemid, status) VALUES(NOW(), '%s', '%s', %d, %d, '%s')", get_table_postfix(), name, itemname, itemid, newitemid, status);
}

void LogManager::HackLog(DWORD accountid, const char* name, const char* why)
{
	Query("INSERT INTO hack%s (date, accountid, name, why) VALUES(NOW(), %d, '%s', '%s')", get_table_postfix(), accountid, name, why);
}
