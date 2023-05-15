#pragma once
#include "../../libsql/src/AsyncSQL.h"
#include "any_function.h"

class LogManager : public singleton<LogManager>
{
	public:
		LogManager();
		virtual ~LogManager();

		bool		IsConnected();
		bool		Connect(const char * host, const int port, const char * user, const char * pwd, const char * db);
		void		CubeLog(DWORD dwPID, const char* szName, DWORD item_vnum, DWORD item_id, int item_count, bool success);
		void		GMCommandLog(const char * szName, const char * szCommand);
		void		ChangeNameLog(DWORD pid, const char * old_name, const char * new_name, const char * ip);
		void		BootLog(const char * c_pszHostName, BYTE bChannel);
		void		QuestRewardLog(const char * c_pszQuestName, DWORD dwPID, DWORD dwLevel, int iValue1, int iValue2);
		void		ExchangeLogGold(const char* from, const char* to, long long gold, DWORD fromAccID, DWORD toAccID);
		void		ExchangeLogItems(const char* from, const char* to, const char* itemname, DWORD itemid, WORD count, DWORD fromAccID, DWORD toAccID);
		void		RefineLog(const char* name, const char* itemname, DWORD itemid, DWORD newitemid, const char* status);
		void		HackLog(DWORD accountid, const char* name, const char* why);

	private:
		void		Query(const char * c_pszFormat, ...);

		CAsyncSQL	m_sql;
		bool		m_bIsConnect;
};
