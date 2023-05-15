#include "stdafx.h"
#include "locale_service.h"
#include "constants.h"
#include "utils.h"
#include "mob_manager.h"
#include "config.h"
#include "skill_power.h"

using namespace std;

string g_stLocale = "latin1";
string g_stServiceName;
string g_stServiceBasePath = "share";
string g_stServiceMapPath = g_stServiceBasePath + "/map";
string g_stLocaleFilename = g_stServiceBasePath + "/locale_string.txt";
BYTE PK_PROTECT_LEVEL = 15;
string g_stLocal = "";

int check_name(const char* str)
{
	const char* tmp;

	if (!str || !*str)
	{
		return 0;
	}

	if (strlen(str) < 2)
	{
		return 0;
	}

	for (tmp = str; *tmp; ++tmp)
	{
		if (isdigit(*tmp) || isalpha(*tmp))
		{
			continue;
		}
		else
		{
			return 0;
		}
	}

	char szTmp[256];
	str_lower(str, szTmp, sizeof(szTmp));

	if (CMobManager::instance().Get(szTmp, false))
	{
		return 0;
	}

	return 1;
}

void LocaleService_LoadLocaleStringFile()
{
	if (!g_stLocaleFilename.empty())
	{
		locale_init(g_stLocaleFilename.c_str());
		fprintf(stderr, "LocaleService %s\n", g_stLocaleFilename.c_str());
	}
}

const std::string& LocaleService_GetBasePath()
{
	return g_stServiceBasePath;
}

const std::string& LocaleService_GetMapPath()
{
	return g_stServiceMapPath;
}

const std::string& LocaleService_GetQuestPath()
{
	return g_stQuestDir;
}
