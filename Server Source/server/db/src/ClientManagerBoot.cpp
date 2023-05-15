#include <map>
#include "stdafx.h"
#include "ClientManager.h"
#include "Main.h"
#include "CsvReader.h"
#include "ProtoReader.h"

#ifdef ENABLE_ITEMSHOP
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>
#include <random>
std::string MD5_STRING(const boost::uuids::detail::md5::digest_type& digest)
{
	const auto charDigest = reinterpret_cast<const char*>(&digest);
	std::string result;
	boost::algorithm::hex(charDigest, charDigest + sizeof(boost::uuids::detail::md5::digest_type), std::back_inserter(result));
	return result;
}
#endif

using namespace std;
extern int g_noTXT;

extern int g_test_server;
extern std::string g_stLocaleNameColumn;

bool CClientManager::InitializeTables()
{
	if (g_noTXT)
	{
		if (!InitializeMobTableSQL())
		{
			sys_err("InitializeMobTableSQL FAILED");
			return false;
		}
		if (!InitializeItemTableSQL())
		{
			sys_err("InitializeItemTableSQL FAILED");
			return false;
		}
	}
	else
	{
		if (!InitializeMobTableTXT())
		{
			sys_err("InitializeMobTableTXT FAILED");
			return false;
		}

		if (!InitializeItemTableTXT())
		{
			sys_err("InitializeItemTableTXT FAILED");
			return false;
		}
#ifdef ENABLE_MIRRORING_PROTO
		if (!MirrorMobTableIntoDB())
		{
			sys_err("MirrorMobTableIntoDB FAILED");
			return false;
		}
		if (!MirrorItemTableIntoDB())
		{
			sys_err("MirrorItemTableIntoDB FAILED");
			return false;
		}
#endif
	}

	if (!InitializeShopTable())
	{
		sys_err("InitializeShopTable FAILED");
		return false;
	}

	if (!InitializeSkillTable())
	{
		sys_err("InitializeSkillTable FAILED");
		return false;
	}

	if (!InitializeRefineTable())
	{
		sys_err("InitializeRefineTable FAILED");
		return false;
	}

	if (!InitializeItemAttrTable())
	{
		sys_err("InitializeItemAttrTable FAILED");
		return false;
	}

	if (!InitializeItemRareTable())
	{
		sys_err("InitializeItemRareTable FAILED");
		return false;
	}

	if (!InitializeLandTable())
	{
		sys_err("InitializeLandTable FAILED");
		return false;
	}

	if (!InitializeObjectProto())
	{
		sys_err("InitializeObjectProto FAILED");
		return false;
	}

	if (!InitializeObjectTable())
	{
		sys_err("InitializeObjectTable FAILED");
		return false;
	}

	return true;
}

bool CClientManager::InitializeRefineTable()
{
	char query[2048];

	snprintf(query, sizeof(query),
			"SELECT id, cost, prob, vnum0, count0, vnum1, count1, vnum2, count2,  vnum3, count3, vnum4, count4 FROM refine_proto%s",
			GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
		return true;

	if (m_pRefineTable)
	{
		sys_log(0, "RELOAD: refine_proto");
		delete [] m_pRefineTable;
		m_pRefineTable = NULL;
	}

	m_iRefineTableSize = pRes->uiNumRows;

	m_pRefineTable	= new TRefineTable[m_iRefineTableSize];
	memset(m_pRefineTable, 0, sizeof(TRefineTable) * m_iRefineTableSize);

	TRefineTable* prt = m_pRefineTable;
	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;
		str_to_number(prt->id, data[col++]);
		str_to_number(prt->cost, data[col++]);
		str_to_number(prt->prob, data[col++]);

		for (int i = 0; i < REFINE_MATERIAL_MAX_NUM; i++)
		{
			str_to_number(prt->materials[i].vnum, data[col++]);
			str_to_number(prt->materials[i].count, data[col++]);
			if (prt->materials[i].vnum == 0)
			{
				prt->material_count = i;
				break;
			}
		}
		prt++;
	}
	return true;
}

class FCompareVnum
{
	public:
		bool operator () (const TEntityTable & a, const TEntityTable & b) const
		{
			return (a.dwVnum < b.dwVnum);
		}
};

bool CClientManager::InitializeMobTableTXT()
{
	map<int,const char*> localMap;
	bool isNameFile = true;

	cCsvTable nameData;
	if(!nameData.Load("mob_names.txt",'\t'))
	{
		fprintf(stderr, "Could not load mob_names.txt\n");
		isNameFile = false;
	} 
	else 
	{
		nameData.Next();
		while(nameData.Next()) 
		{
			localMap[atoi(nameData.AsStringByIndex(0))] = nameData.AsStringByIndex(1);
		}
	}

	cCsvTable data;

	if(!data.Load("mob_proto.txt",'\t'))
	{
		fprintf(stderr, "Could not load mob_proto.txt. Wrong file format?\n");
		return false;
	}
	else
	{
		fprintf(stderr, "Mob_proto was loaded from txt.\n");
	}

	data.Next();

	if (!m_vec_mobTable.empty())
	{
		sys_log(0, "RELOAD: mob_proto");
		m_vec_mobTable.clear();
	}

	m_vec_mobTable.resize(data.m_File.GetRowCount()-1);
	memset(&m_vec_mobTable[0], 0, sizeof(TMobTable) * m_vec_mobTable.size());
	TMobTable * mob_table = &m_vec_mobTable[0];

	while (data.Next())
	{
		if (!Set_Proto_Mob_Table(mob_table, data, localMap))
		{
			fprintf(stderr, "Could not process entry.\n");
		}			
		++mob_table;
	}

	sort(m_vec_mobTable.begin(), m_vec_mobTable.end(), FCompareVnum());
	return true;
}

bool CClientManager::InitializeShopTable()
{
	MYSQL_ROW	data;
	int		col;

	static const char * s_szQuery = 
		"SELECT "
		"shop.vnum, "
		"shop.npc_vnum, "
		"shop_item.item_vnum, "
		"shop_item.count "
		"FROM shop LEFT JOIN shop_item "
		"ON shop.vnum = shop_item.shop_vnum ORDER BY shop.vnum, shop_item.item_vnum";

	std::unique_ptr<SQLMsg> pkMsg2(CDBManager::instance().DirectQuery(s_szQuery));

	SQLResult * pRes2 = pkMsg2->Get();

	if (!pRes2->uiNumRows)
	{
		sys_err("InitializeShopTable : Table count is zero.");
		return false;
	}

	std::map<int, TShopTable *> map_shop;

	if (m_pShopTable)
	{
		delete [] (m_pShopTable);
		m_pShopTable = NULL;
	}

	TShopTable * shop_table = m_pShopTable;

	while ((data = mysql_fetch_row(pRes2->pSQLResult)))
	{
		col = 0;

		int iShopVnum = 0;
		str_to_number(iShopVnum, data[col++]);

		if (map_shop.end() == map_shop.find(iShopVnum))
		{
			shop_table = new TShopTable;
			memset(shop_table, 0, sizeof(TShopTable));
			shop_table->dwVnum	= iShopVnum;

			map_shop[iShopVnum] = shop_table;
		}
		else
			shop_table = map_shop[iShopVnum];

		str_to_number(shop_table->dwNPCVnum, data[col++]);

		if (!data[col])
			continue;

		TShopItemTable * pItem = &shop_table->items[shop_table->byItemCount];

		str_to_number(pItem->vnum, data[col++]);
		str_to_number(pItem->count, data[col++]);

		++shop_table->byItemCount;
	}

	m_pShopTable = new TShopTable[map_shop.size()];
	m_iShopTableSize = map_shop.size();

	auto it = map_shop.begin();

	int i = 0;

	while (it != map_shop.end())
	{
		thecore_memcpy((m_pShopTable + i), (it++)->second, sizeof(TShopTable));
		++i;
	}

	return true;
}

bool CClientManager::InitializeQuestItemTable()
{
	using namespace std;

	static const char * s_szQuery = "SELECT vnum, name, %s FROM quest_item_proto ORDER BY vnum";

	char query[1024];
	snprintf(query, sizeof(query), s_szQuery, g_stLocaleNameColumn.c_str());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("query error or no rows: %s", query);
		return false;
	}

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;

		TItemTable tbl;
		memset(&tbl, 0, sizeof(tbl));

		str_to_number(tbl.dwVnum, row[col++]);

		if (row[col])
			strlcpy(tbl.szName, row[col], sizeof(tbl.szName));

		col++;

		if (row[col])
			strlcpy(tbl.szLocaleName, row[col], sizeof(tbl.szLocaleName));

		col++;

		if (m_map_itemTableByVnum.find(tbl.dwVnum) != m_map_itemTableByVnum.end())
		{
			sys_err("QUEST_ITEM_ERROR! %lu vnum already exist! (name %s)", tbl.dwVnum, tbl.szLocaleName);
			continue;
		}

		tbl.bType = ITEM_QUEST;
		tbl.bSize = 1;

		m_vec_itemTable.push_back(tbl);
	}

	return true;
}

bool CClientManager::InitializeItemTableTXT()
{
	bool isNameFile = true;
	map<int,const char*> localMap;
	cCsvTable nameData;
	if(!nameData.Load("item_names.txt",'\t'))
	{
		fprintf(stderr, "Could not load item_names.txt.\n");
		isNameFile = false;
	} 
	else 
	{
		nameData.Next();
		while(nameData.Next()) 
		{
			localMap[atoi(nameData.AsStringByIndex(0))] = nameData.AsStringByIndex(1);
		}
	}
	map<DWORD, TItemTable *> test_map_itemTableByVnum;

	set<int> vnumSet;

	cCsvTable data;
	if(!data.Load("item_proto.txt",'\t'))
	{
		fprintf(stderr, "Could not load item_names.txt.\n");
		return false;
	}
	else
	{
		fprintf(stderr, "Item_proto was loaded from txt.\n");
	}
	data.Next();

	if (!m_vec_itemTable.empty())
	{
		sys_log(0, "RELOAD: item_proto");
		m_vec_itemTable.clear();
		m_map_itemTableByVnum.clear();
	}

	int addNumber = 0;
	while(data.Next()) 
	{
		int vnum = atoi(data.AsStringByIndex(0));
		std::map<DWORD, TItemTable *>::iterator it_map_itemTable;
		it_map_itemTable = test_map_itemTableByVnum.find(vnum);
		if(it_map_itemTable != test_map_itemTableByVnum.end())
		{
			addNumber++;
		}
	}

	data.Destroy();
	if(!data.Load("item_proto.txt",'\t'))
	{
		fprintf(stderr, "Could not load item_names.txt.\n");
		return false;
	}
	data.Next();

	m_vec_itemTable.resize(data.m_File.GetRowCount() - 1 + addNumber);
	memset(&m_vec_itemTable[0], 0, sizeof(TItemTable) * m_vec_itemTable.size());
	int testValue =  m_vec_itemTable.size();

	TItemTable * item_table = &m_vec_itemTable[0];

	while (data.Next())
	{
		int col = 0;

		std::map<DWORD, TItemTable *>::iterator it_map_itemTable;
		it_map_itemTable = test_map_itemTableByVnum.find(atoi(data.AsStringByIndex(col)));
		if(it_map_itemTable == test_map_itemTableByVnum.end()) 
		{
			if (!Set_Proto_Item_Table(item_table, data, localMap))
			{
				fprintf(stderr, "Failed to load item_proto table.\n");
			}
		} 
		else 
		{
			TItemTable *tempTable = it_map_itemTable->second;

			item_table->dwVnum = tempTable->dwVnum;
			strlcpy(item_table->szName, tempTable->szName, sizeof(item_table->szName));
			strlcpy(item_table->szLocaleName, tempTable->szLocaleName, sizeof(item_table->szLocaleName));
			item_table->bType = tempTable->bType;
			item_table->bSubType = tempTable->bSubType;
			item_table->bSize = tempTable->bSize;
			item_table->dwAntiFlags = tempTable->dwAntiFlags;
			item_table->dwFlags = tempTable->dwFlags;
			item_table->dwWearFlags = tempTable->dwWearFlags;
			item_table->dwImmuneFlag = tempTable->dwImmuneFlag;
			item_table->lldGold = tempTable->lldGold;
			item_table->lldShopBuyPrice = tempTable->lldShopBuyPrice;
			item_table->dwRefinedVnum =tempTable->dwRefinedVnum;
			item_table->wRefineSet =tempTable->wRefineSet;
			item_table->bAlterToMagicItemPct = tempTable->bAlterToMagicItemPct;
			item_table->cLimitRealTimeFirstUseIndex = -1;
			item_table->cLimitTimerBasedOnWearIndex = -1;

			int i;

			for (i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
			{
				item_table->aLimits[i].bType = tempTable->aLimits[i].bType;
				item_table->aLimits[i].lValue = tempTable->aLimits[i].lValue;

				if (LIMIT_REAL_TIME_START_FIRST_USE == item_table->aLimits[i].bType)
					item_table->cLimitRealTimeFirstUseIndex = (char)i;

				if (LIMIT_TIMER_BASED_ON_WEAR == item_table->aLimits[i].bType)
					item_table->cLimitTimerBasedOnWearIndex = (char)i;
			}

			for (i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				item_table->aApplies[i].bType = tempTable->aApplies[i].bType;
				item_table->aApplies[i].lValue = tempTable->aApplies[i].lValue;
			}

			for (i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
				item_table->alValues[i] = tempTable->alValues[i];

			item_table->bGainSocketPct = tempTable->bGainSocketPct;
			item_table->sAddonType = tempTable->sAddonType;

			item_table->bWeight  = tempTable->bWeight;

		}
		vnumSet.insert(item_table->dwVnum);
		m_map_itemTableByVnum.insert(std::map<DWORD, TItemTable *>::value_type(item_table->dwVnum, item_table));
		++item_table;
	}

	m_map_itemTableByVnum.clear();

	auto it = m_vec_itemTable.begin();

	while (it != m_vec_itemTable.end())
	{
		TItemTable * item_table = &(*(it++));
		m_map_itemTableByVnum.insert(std::map<DWORD, TItemTable *>::value_type(item_table->dwVnum, item_table));
	}
	sort(m_vec_itemTable.begin(), m_vec_itemTable.end(), FCompareVnum());
	return true;
}


bool CClientManager::InitializeSkillTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT dwVnum, szName, bType, bMaxLevel, dwSplashRange, "
		"szPointOn, szPointPoly, szSPCostPoly, szDurationPoly, szDurationSPCostPoly, "
		"szCooldownPoly, szMasterBonusPoly, setFlag+0, setAffectFlag+0, "
		"szPointOn2, szPointPoly2, szDurationPoly2, setAffectFlag2+0, "
		"szPointOn3, szPointPoly3, szDurationPoly3, szGrandMasterAddSPCostPoly, "
		"bLevelStep, bLevelLimit, prerequisiteSkillVnum, prerequisiteSkillLevel, iMaxHit, szSplashAroundDamageAdjustPoly, eSkillType+0, dwTargetRange "
		"FROM skill_proto%s ORDER BY dwVnum",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from skill_proto");
		return false;
	}

	if (!m_vec_skillTable.empty())
	{
		sys_log(0, "RELOAD: skill_proto");
		m_vec_skillTable.clear();
	}

	m_vec_skillTable.reserve(pRes->uiNumRows);

	MYSQL_ROW	data;
	int		col;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TSkillTable t;
		memset(&t, 0, sizeof(t));

		col = 0;

		str_to_number(t.dwVnum, data[col++]);
		strlcpy(t.szName, data[col++], sizeof(t.szName));
		str_to_number(t.bType, data[col++]);
		str_to_number(t.bMaxLevel, data[col++]);
		str_to_number(t.dwSplashRange, data[col++]);

		strlcpy(t.szPointOn, data[col++], sizeof(t.szPointOn));
		strlcpy(t.szPointPoly, data[col++], sizeof(t.szPointPoly));
		strlcpy(t.szSPCostPoly, data[col++], sizeof(t.szSPCostPoly));
		strlcpy(t.szDurationPoly, data[col++], sizeof(t.szDurationPoly));
		strlcpy(t.szDurationSPCostPoly, data[col++], sizeof(t.szDurationSPCostPoly));
		strlcpy(t.szCooldownPoly, data[col++], sizeof(t.szCooldownPoly));
		strlcpy(t.szMasterBonusPoly, data[col++], sizeof(t.szMasterBonusPoly));

		str_to_number(t.dwFlag, data[col++]);
		str_to_number(t.dwAffectFlag, data[col++]);

		strlcpy(t.szPointOn2, data[col++], sizeof(t.szPointOn2));
		strlcpy(t.szPointPoly2, data[col++], sizeof(t.szPointPoly2));
		strlcpy(t.szDurationPoly2, data[col++], sizeof(t.szDurationPoly2));
		str_to_number(t.dwAffectFlag2, data[col++]);

		strlcpy(t.szPointOn3, data[col++], sizeof(t.szPointOn3));
		strlcpy(t.szPointPoly3, data[col++], sizeof(t.szPointPoly3));
		strlcpy(t.szDurationPoly3, data[col++], sizeof(t.szDurationPoly3));

		strlcpy(t.szGrandMasterAddSPCostPoly, data[col++], sizeof(t.szGrandMasterAddSPCostPoly));

		str_to_number(t.bLevelStep, data[col++]);
		str_to_number(t.bLevelLimit, data[col++]);
		str_to_number(t.preSkillVnum, data[col++]);
		str_to_number(t.preSkillLevel, data[col++]);

		str_to_number(t.lMaxHit, data[col++]);

		strlcpy(t.szSplashAroundDamageAdjustPoly, data[col++], sizeof(t.szSplashAroundDamageAdjustPoly));

		str_to_number(t.bSkillAttrType, data[col++]);
		str_to_number(t.dwTargetRange, data[col++]);

		m_vec_skillTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeItemAttrTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
			"SELECT apply, apply+0, prob, lv1, lv2, lv3, lv4, lv5, weapon, body, wrist, foots, neck, head, shield, ear FROM item_attr%s ORDER BY apply",
			GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from item_attr");
		return false;
	}

	if (!m_vec_itemAttrTable.empty())
	{
		sys_log(0, "RELOAD: item_attr");
		m_vec_itemAttrTable.clear();
	}

	m_vec_itemAttrTable.reserve(pRes->uiNumRows);

	MYSQL_ROW	data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TItemAttrTable t;

		memset(&t, 0, sizeof(TItemAttrTable));

		int col = 0;

		strlcpy(t.szApply, data[col++], sizeof(t.szApply));
		str_to_number(t.dwApplyIndex, data[col++]);
		str_to_number(t.dwProb, data[col++]);
		str_to_number(t.lValues[0], data[col++]);
		str_to_number(t.lValues[1], data[col++]);
		str_to_number(t.lValues[2], data[col++]);
		str_to_number(t.lValues[3], data[col++]);
		str_to_number(t.lValues[4], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_BODY], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_NECK], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_EAR], data[col++]);

		m_vec_itemAttrTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeItemRareTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
			"SELECT apply, apply+0, prob, lv1, lv2, lv3, lv4, lv5, weapon, body, wrist, foots, neck, head, shield, ear FROM item_attr_rare%s ORDER BY apply",
			GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from item_attr_rare");
		return false;
	}

	if (!m_vec_itemRareTable.empty())
	{
		sys_log(0, "RELOAD: item_attr_rare");
		m_vec_itemRareTable.clear();
	}

	m_vec_itemRareTable.reserve(pRes->uiNumRows);

	MYSQL_ROW	data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TItemAttrTable t;

		memset(&t, 0, sizeof(TItemAttrTable));

		int col = 0;

		strlcpy(t.szApply, data[col++], sizeof(t.szApply));
		str_to_number(t.dwApplyIndex, data[col++]);
		str_to_number(t.dwProb, data[col++]);
		str_to_number(t.lValues[0], data[col++]);
		str_to_number(t.lValues[1], data[col++]);
		str_to_number(t.lValues[2], data[col++]);
		str_to_number(t.lValues[3], data[col++]);
		str_to_number(t.lValues[4], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_BODY], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_NECK], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_EAR], data[col++]);

		m_vec_itemRareTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeLandTable()
{
	using namespace building;

	char query[4096];

	snprintf(query, sizeof(query),
		"SELECT id, map_index, x, y, width, height, guild_id, guild_level_limit, price "
		"FROM land%s WHERE enable='YES' ORDER BY id",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_vec_kLandTable.empty())
	{
		sys_log(0, "RELOAD: land");
		m_vec_kLandTable.clear();
	}

	m_vec_kLandTable.reserve(pRes->uiNumRows);

	MYSQL_ROW	data;

	if (pRes->uiNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TLand t;

			memset(&t, 0, sizeof(t));

			int col = 0;

			str_to_number(t.dwID, data[col++]);
			str_to_number(t.lMapIndex, data[col++]);
			str_to_number(t.x, data[col++]);
			str_to_number(t.y, data[col++]);
			str_to_number(t.width, data[col++]);
			str_to_number(t.height, data[col++]);
			str_to_number(t.dwGuildID, data[col++]);
			str_to_number(t.bGuildLevelLimit, data[col++]);
			str_to_number(t.lldPrice, data[col++]);

			m_vec_kLandTable.push_back(t);
		}

	return true;
}

void parse_pair_number_string(const char * c_pszString, std::vector<std::pair<int, int> > & vec)
{
	const char * t = c_pszString;
	const char * p = strchr(t, '/');
	std::pair<int, int> k;

	char szNum[32 + 1];
	char * comma;

	while (p)
	{
		if (isnhdigit(*t))
		{
			strlcpy(szNum, t, MIN(sizeof(szNum), (p-t)+1));

			comma = strchr(szNum, ',');

			if (comma)
			{
				*comma = '\0';
				str_to_number(k.second, comma+1);
			}
			else
				k.second = 0;

			str_to_number(k.first, szNum);
			vec.push_back(k);
		}

		t = p + 1;
		p = strchr(t, '/');
	}

	if (isnhdigit(*t))
	{
		strlcpy(szNum, t, sizeof(szNum));

		comma = strchr(const_cast<char*>(t), ',');

		if (comma)
		{
			*comma = '\0';
			str_to_number(k.second, comma+1);
		}
		else
			k.second = 0;

		str_to_number(k.first, szNum);
		vec.push_back(k);
	}
}

bool CClientManager::InitializeObjectProto()
{
	using namespace building;

	char query[4096];
	snprintf(query, sizeof(query),
			"SELECT vnum, price, materials, upgrade_vnum, upgrade_limit_time, life, reg_1, reg_2, reg_3, reg_4, npc, group_vnum, dependent_group "
			"FROM object_proto%s ORDER BY vnum",
			GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_vec_kObjectProto.empty())
	{
		sys_log(0, "RELOAD: object_proto");
		m_vec_kObjectProto.clear();
	}

	m_vec_kObjectProto.reserve(MAX(0, pRes->uiNumRows));

	MYSQL_ROW	data;

	if (pRes->uiNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TObjectProto t;

			memset(&t, 0, sizeof(t));

			int col = 0;

			str_to_number(t.dwVnum, data[col++]);
			str_to_number(t.lldPrice, data[col++]);

			std::vector<std::pair<int, int> > vec;
			parse_pair_number_string(data[col++], vec);

			for (unsigned int i = 0; i < OBJECT_MATERIAL_MAX_NUM && i < vec.size(); ++i)
			{
				std::pair<int, int> & r = vec[i];

				t.kMaterials[i].dwItemVnum = r.first;
				t.kMaterials[i].dwCount = r.second;
			}

			str_to_number(t.dwUpgradeVnum, data[col++]);
			str_to_number(t.dwUpgradeLimitTime, data[col++]);
			str_to_number(t.lLife, data[col++]);
			str_to_number(t.lRegion[0], data[col++]);
			str_to_number(t.lRegion[1], data[col++]);
			str_to_number(t.lRegion[2], data[col++]);
			str_to_number(t.lRegion[3], data[col++]);

			str_to_number(t.dwNPCVnum, data[col++]);
			str_to_number(t.dwGroupVnum, data[col++]);
			str_to_number(t.dwDependOnGroupVnum, data[col++]);

			t.lNPCX = 0;
			t.lNPCY = MAX(t.lRegion[1], t.lRegion[3])+300;

			m_vec_kObjectProto.push_back(t);
		}

	return true;
}

bool CClientManager::InitializeObjectTable()
{
	using namespace building;

	char query[4096];
	snprintf(query, sizeof(query), "SELECT id, land_id, vnum, map_index, x, y, x_rot, y_rot, z_rot, life FROM object%s ORDER BY id", GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_map_pkObjectTable.empty())
	{
		sys_log(0, "RELOAD: object");
		m_map_pkObjectTable.clear();
	}

	MYSQL_ROW data;

	if (pRes->uiNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TObject * k = new TObject;

			memset(k, 0, sizeof(TObject));

			int col = 0;

			str_to_number(k->dwID, data[col++]);
			str_to_number(k->dwLandID, data[col++]);
			str_to_number(k->dwVnum, data[col++]);
			str_to_number(k->lMapIndex, data[col++]);
			str_to_number(k->x, data[col++]);
			str_to_number(k->y, data[col++]);
			str_to_number(k->xRot, data[col++]);
			str_to_number(k->yRot, data[col++]);
			str_to_number(k->zRot, data[col++]);
			str_to_number(k->lLife, data[col++]);
			m_map_pkObjectTable.insert(std::make_pair(k->dwID, k));
		}

	return true;
}

bool CClientManager::InitializeMobTableSQL()
{
	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT vnum, name, %s, type, rank, battle_type, level, "
		"size+0, ai_flag+0, setRaceFlag+0, setImmuneFlag+0, "
		"on_click, empire, drop_item, resurrection_vnum, folder, "
		"st, dx, ht, iq, damage_min, damage_max, max_hp, regen_cycle, regen_percent, exp, "
		"gold_min, gold_max, def, attack_speed, move_speed, "
		"aggressive_hp_pct, aggressive_sight, attack_range, polymorph_item, "
		"enchant_curse, enchant_slow, enchant_poison, enchant_stun, enchant_critical, enchant_penetrate, "
		"resist_sword, resist_twohand, resist_dagger, resist_bell, resist_fan, resist_bow, "
#ifdef ENABLE_WOLFMAN
		"resist_claw,"
#endif
		"resist_fire, resist_elect, resist_magic, resist_wind, resist_poison, "
#ifdef ENABLE_WOLFMAN
		"resist_bleeding,"
#endif
		"dam_multiply, summon, drain_sp, "
		"skill_vnum0, skill_level0, skill_vnum1, skill_level1, skill_vnum2, skill_level2,"
		"skill_vnum3, skill_level3, skill_vnum4, skill_level4 , sp_berserk, sp_stoneskin, "
		"sp_godspeed, sp_deathblow, sp_revive "
		"FROM mob_proto%s ORDER BY vnum",
		g_stLocaleNameColumn.c_str(), GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("Could not load mob_proto. No results!");
		return false;
	}

	sys_log(0, "MOB_PROTO loading...");

	if (!m_vec_mobTable.empty())
	{
		sys_log(0, "RELOAD: mob_proto");
		m_vec_mobTable.clear();
	}

	m_vec_mobTable.resize(pRes->uiNumRows);
	memset(&m_vec_mobTable[0], 0, sizeof(TMobTable) * m_vec_mobTable.size());
	TMobTable* mob_table = &m_vec_mobTable[0];

	MYSQL_ROW data;
	int col;
	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		col = 0;
		str_to_number(mob_table->dwVnum, data[col++]);
		strlcpy(mob_table->szName, data[col++], sizeof(mob_table->szName));
		strlcpy(mob_table->szLocaleName, data[col++], sizeof(mob_table->szLocaleName));
		str_to_number(mob_table->bType, data[col++]);
		str_to_number(mob_table->bRank, data[col++]);
		str_to_number(mob_table->bBattleType, data[col++]);
		str_to_number(mob_table->bLevel, data[col++]);
		str_to_number(mob_table->bSize, data[col++]);
		str_to_number(mob_table->dwAIFlag, data[col++]);
		str_to_number(mob_table->dwRaceFlag, data[col++]);
		str_to_number(mob_table->dwImmuneFlag, data[col++]);
		str_to_number(mob_table->bOnClickType, data[col++]);
		str_to_number(mob_table->bEmpire, data[col++]);
		str_to_number(mob_table->dwDropItemVnum, data[col++]);
		str_to_number(mob_table->dwResurrectionVnum, data[col++]);
		strlcpy(mob_table->szFolder, data[col++], sizeof(mob_table->szFolder));
		str_to_number(mob_table->bStr, data[col++]);
		str_to_number(mob_table->bDex, data[col++]);
		str_to_number(mob_table->bCon, data[col++]);
		str_to_number(mob_table->bInt, data[col++]);
		str_to_number(mob_table->dwDamageRange[0], data[col++]);
		str_to_number(mob_table->dwDamageRange[1], data[col++]);
		str_to_number(mob_table->dwMaxHP, data[col++]);
		str_to_number(mob_table->bRegenCycle, data[col++]);
		str_to_number(mob_table->bRegenPercent, data[col++]);
		str_to_number(mob_table->dwExp, data[col++]);
		str_to_number(mob_table->dwGoldMin, data[col++]);
		str_to_number(mob_table->dwGoldMax, data[col++]);
		str_to_number(mob_table->wDef, data[col++]);
		str_to_number(mob_table->sAttackSpeed, data[col++]);
		str_to_number(mob_table->sMovingSpeed, data[col++]);
		str_to_number(mob_table->bAggresiveHPPct, data[col++]);
		str_to_number(mob_table->wAggressiveSight, data[col++]);
		str_to_number(mob_table->wAttackRange, data[col++]);
		str_to_number(mob_table->dwPolymorphItemVnum, data[col++]);

		int i;
		for (i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
			str_to_number(mob_table->cEnchants[i], data[col++]);

		for (i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
			str_to_number(mob_table->cResists[i], data[col++]);

		str_to_number(mob_table->fDamMultiply, data[col++]);
		str_to_number(mob_table->dwSummonVnum, data[col++]);
		str_to_number(mob_table->dwDrainSP, data[col++]);

		for (i = 0; i < MOB_SKILL_MAX_NUM; ++i)
		{
			str_to_number(mob_table->Skills[i].dwVnum, data[col++]);
			str_to_number(mob_table->Skills[i].bLevel, data[col++]);
		}

		str_to_number(mob_table->bBerserkPoint, data[col++]);
		str_to_number(mob_table->bStoneSkinPoint, data[col++]);
		str_to_number(mob_table->bGodSpeedPoint, data[col++]);
		str_to_number(mob_table->bDeathBlowPoint, data[col++]);
		str_to_number(mob_table->bRevivePoint, data[col++]);

		++mob_table;
	}

	sort(m_vec_mobTable.begin(), m_vec_mobTable.end(), FCompareVnum());
	return true;
}

bool CClientManager::InitializeItemTableSQL()
{
	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT vnum, vnum_range, name, %s, type, subtype, gold, shop_buy_price, weight, size, flag, wearflag, "
		"antiflag, immuneflag+0, refined_vnum, refine_set, magic_pct, socket_pct, addon_type, "
		"limittype0, limitvalue0, limittype1, limitvalue1, "
		"applytype0, applyvalue0, applytype1, applyvalue1, applytype2, applyvalue2, applytype3, applyvalue3, "
		"value0, value1, value2, value3, value4, value5 "
		"FROM item_proto%s ORDER BY vnum",
		g_stLocaleNameColumn.c_str(), GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("Could not load item_proto. No results!");
		return false;
	}

	sys_log(0, "ITEM_PROTO loading...");

	if (!m_vec_itemTable.empty())
	{
		sys_log(0, "RELOAD: item_proto");
		m_vec_itemTable.clear();
		m_map_itemTableByVnum.clear();
	}

	m_vec_itemTable.resize(pRes->uiNumRows);
	memset(&m_vec_itemTable[0], 0, sizeof(TItemTable) * m_vec_itemTable.size());
	TItemTable* item_table = &m_vec_itemTable[0];

	MYSQL_ROW data;
	int col;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		col = 0;

		str_to_number(item_table->dwVnum, data[col++]);
		str_to_number(item_table->dwVnumRange, data[col++]);
		strlcpy(item_table->szName, data[col++], sizeof(item_table->szName));
		strlcpy(item_table->szLocaleName, data[col++], sizeof(item_table->szLocaleName));
		str_to_number(item_table->bType, data[col++]);
		str_to_number(item_table->bSubType, data[col++]);
		str_to_number(item_table->lldGold, data[col++]);
		str_to_number(item_table->lldShopBuyPrice, data[col++]);
		str_to_number(item_table->bWeight, data[col++]);
		str_to_number(item_table->bSize, data[col++]);
		str_to_number(item_table->dwFlags, data[col++]);
		str_to_number(item_table->dwWearFlags, data[col++]);
		str_to_number(item_table->dwAntiFlags, data[col++]);
		str_to_number(item_table->dwImmuneFlag, data[col++]);
		str_to_number(item_table->dwRefinedVnum, data[col++]);
		str_to_number(item_table->wRefineSet, data[col++]);
		str_to_number(item_table->bAlterToMagicItemPct, data[col++]);
		str_to_number(item_table->bGainSocketPct, data[col++]);
		str_to_number(item_table->sAddonType, data[col++]);

		item_table->cLimitRealTimeFirstUseIndex = -1;
		item_table->cLimitTimerBasedOnWearIndex = -1;

		int i;
		for (i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
		{
			str_to_number(item_table->aLimits[i].bType, data[col++]);
			str_to_number(item_table->aLimits[i].lValue, data[col++]);

			if (LIMIT_REAL_TIME_START_FIRST_USE == item_table->aLimits[i].bType)
				item_table->cLimitRealTimeFirstUseIndex = (char)i;

			if (LIMIT_TIMER_BASED_ON_WEAR == item_table->aLimits[i].bType)
				item_table->cLimitTimerBasedOnWearIndex = (char)i;
		}

		for (i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		{
			str_to_number(item_table->aApplies[i].bType, data[col++]);
			str_to_number(item_table->aApplies[i].lValue, data[col++]);
		}

		for (i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
			str_to_number(item_table->alValues[i], data[col++]);

		m_map_itemTableByVnum.insert(std::map<DWORD, TItemTable*>::value_type(item_table->dwVnum, item_table));
		++item_table;
	}

	sort(m_vec_itemTable.begin(), m_vec_itemTable.end(), FCompareVnum());
	return true;
}

#ifdef ENABLE_MIRRORING_PROTO
bool CClientManager::MirrorMobTableIntoDB()
{
	for (auto it = m_vec_mobTable.begin(); it != m_vec_mobTable.end(); it++)
	{
		const TMobTable& t = *it;
		char query[4096];

		snprintf(query, sizeof(query),
			"replace into mob_proto%s "
			"("
			"vnum, name, %s, type, rank, battle_type, level, scalepct, size, ai_flag, setRaceFlag, setImmuneFlag, "
			"on_click, empire, drop_item, resurrection_vnum, folder, "
			"st, dx, ht, iq, damage_min, damage_max, max_hp, regen_cycle, regen_percent, exp, "
			"gold_min, gold_max, def, attack_speed, move_speed, aggressive_hp_pct, aggressive_sight, attack_range, mob_color ,polymorph_item, " //Fliege

			"enchant_curse, enchant_slow, enchant_poison, enchant_stun, enchant_critical, enchant_penetrate, "
			"resist_sword, resist_twohand, resist_dagger, resist_bell, resist_fan, resist_bow, "
#ifdef ENABLE_WOLFMAN
			"resist_claw, "
#endif
			"resist_fire, resist_elect, resist_magic, resist_wind, resist_poison, "
#ifdef ENABLE_WOLFMAN
			"resist_bleeding, "
#endif
			"dam_multiply, summon, drain_sp, "

			"skill_vnum0, skill_level0, skill_vnum1, skill_level1, skill_vnum2, skill_level2, "
			"skill_vnum3, skill_level3, skill_vnum4, skill_level4, "
			"sp_berserk, sp_stoneskin, sp_godspeed, sp_deathblow, sp_revive"
			") "
			"values ("

			"%u, \"%s\", \"%s\", %u, %u, %u, %u, %u, %u, %u, %u, %u, "
			"%u, %u, %u, %u, '%s', "
			"%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, "
			"%u, %u, %d, %d, %d, %u, %d, %d, %d, %u, "

			"%d, %d, %d, %d, %d, %d, "
			"%d, %d, %d, %d, %d, %d, "
#ifdef ENABLE_WOLFMAN
			"%d, "
#endif
			"%d, %d, %d, %d, %d, "
#ifdef ENABLE_WOLFMAN
			"%d, "
#endif
			"%f, %u, %u, "

			"%u, %u, %u, %u, %u, %u, "
			"%u, %u, %u, %u, "
			"%u, %u, %u, %u, %u"
			")",
			GetTablePostfix(), g_stLocaleNameColumn.c_str(),

			t.dwVnum, t.szName, t.szLocaleName, t.bType, t.bRank, t.bBattleType, t.bLevel, 100, t.bSize, t.dwAIFlag, t.dwRaceFlag, t.dwImmuneFlag,
			t.bOnClickType, t.bEmpire, t.dwDropItemVnum, t.dwResurrectionVnum, t.szFolder,
			t.bStr, t.bDex, t.bCon, t.bInt, t.dwDamageRange[0], t.dwDamageRange[1], t.dwMaxHP, t.bRegenCycle, t.bRegenPercent, t.dwExp,

			t.dwGoldMin, t.dwGoldMax, t.wDef, t.sAttackSpeed, t.sMovingSpeed, t.bAggresiveHPPct, t.wAggressiveSight, t.wAttackRange, t.dwMobColor ,t.dwPolymorphItemVnum,
			t.cEnchants[0], t.cEnchants[1], t.cEnchants[2], t.cEnchants[3], t.cEnchants[4], t.cEnchants[5],
			t.cResists[0], t.cResists[1], t.cResists[2], t.cResists[3], t.cResists[4], t.cResists[5],
#ifdef ENABLE_WOLFMAN
			t.cResists[6],
#endif
			t.cResists[7], t.cResists[8], t.cResists[9], t.cResists[10], t.cResists[11],
#ifdef ENABLE_WOLFMAN
			t.cResists[12],
#endif
			t.fDamMultiply, t.dwSummonVnum, t.dwDrainSP,

			t.Skills[0].dwVnum, t.Skills[0].bLevel, t.Skills[1].dwVnum, t.Skills[1].bLevel, t.Skills[2].dwVnum, t.Skills[2].bLevel,
			t.Skills[3].dwVnum, t.Skills[3].bLevel, t.Skills[4].dwVnum, t.Skills[4].bLevel,
			t.bBerserkPoint, t.bStoneSkinPoint, t.bGodSpeedPoint, t.bDeathBlowPoint, t.bRevivePoint
		);

		CDBManager::instance().AsyncQuery(query);
	}
	return true;
}

bool CClientManager::MirrorItemTableIntoDB()
{
	for (auto it = m_vec_itemTable.begin(); it != m_vec_itemTable.end(); it++)
	{
		const TItemTable& t = *it;
		char query[4096];
		snprintf(query, sizeof(query),
			"replace into item_proto%s ("
			"vnum, type, subtype, name, %s, gold, shop_buy_price, weight, size, "
			"flag, wearflag, antiflag, immuneflag, "
			"refined_vnum, refine_set, magic_pct, specular, socket_pct, addon_type,"
			"limittype0, limitvalue0, limittype1, limitvalue1, "
			"applytype0, applyvalue0, applytype1, applyvalue1, applytype2, applyvalue2, applytype3, applyvalue3, "
			"value0, value1, value2, value3, value4, value5 ) "
			"values ("
			"%u, %u, %u, \"%s\", \"%s\", %lld, %lld, %u, %u, "
			"%u, %u, %u, %u, "
			"%u, %d, %u, %u, %u, %d, "
			"%u, %ld, %u, %ld, "
			"%u, %ld, %u, %ld, %u, %ld, %u, %ld, "
			"%ld, %ld, %ld, %ld, %ld, %ld )",
			GetTablePostfix(), g_stLocaleNameColumn.c_str(),
			t.dwVnum, t.bType, t.bSubType, t.szName, t.szLocaleName, t.lldGold, t.lldShopBuyPrice, t.bWeight, t.bSize,
			t.dwFlags, t.dwWearFlags, t.dwAntiFlags, t.dwImmuneFlag,
			t.dwRefinedVnum, t.wRefineSet, t.bAlterToMagicItemPct, t.bSpecular, t.bGainSocketPct, t.sAddonType,
			t.aLimits[0].bType, t.aLimits[0].lValue, t.aLimits[1].bType, t.aLimits[1].lValue,
			t.aApplies[0].bType, t.aApplies[0].lValue, t.aApplies[1].bType, t.aApplies[1].lValue, t.aApplies[2].bType, t.aApplies[2].lValue, t.aApplies[3].bType, t.aApplies[3].lValue,
			t.alValues[0], t.alValues[1], t.alValues[2], t.alValues[3], t.alValues[4], t.alValues[5]);
		CDBManager::instance().AsyncQuery(query);
	}
	return true;
}
#endif

#ifdef ENABLE_ITEMSHOP
bool CClientManager::InitializePromotionCodes()
{
	char query[2048];

	snprintf(query, sizeof(query),
		"SELECT "
		"CODES.useCount, "
		"CODES.promotion_code, "
		"vnum, "
		"count,"
		"socket0,"
		"socket1,"
		"socket2,"
		"socket3,"
		"socket4,"
		"socket5,"
		"attrtype0,"
		"attrvalue0, "
		"attrtype1,"
		"attrvalue1, "
		"attrtype2,"
		"attrvalue2, "
		"attrtype3,"
		"attrvalue3, "
		"attrtype4,"
		"attrvalue4, "
		"attrtype5,"
		"attrvalue5, "
		"attrtype6, "
		"attrvalue6 "
		"FROM "
		"itemshop.promotion_rewards AS REWARDS "
		"LEFT JOIN itemshop.promotion_codes AS CODES ON REWARDS.code_index = CODES.index "
		"WHERE "
		"REWARDS.code_index = CODES.index AND CODES.state = 'ENABLED' "
		"AND count > 0"
	);

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!m_map_promotionCodeInfo.empty())
	{
		sys_log(0, "RELOAD: itemshop offertimes");
		m_map_promotionCodeInfo.clear();
	}

	if (!pRes->uiNumRows)
	{
		return true;
	}

	MYSQL_ROW data;
	int col = 0;
	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TPromotionItemTable reward;
		col = 0;

		long long llPromotionUseCount;
		char promotion_code[PROMOTION_CODE_MAX_LEN + 1];
		str_to_number(llPromotionUseCount, data[col++]);
		strlcpy(promotion_code, data[col++], sizeof(promotion_code));

		str_to_number(reward.dwVnum, data[col++]);
		str_to_number(reward.wCount, data[col++]);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			str_to_number(reward.alSockets[i], data[col++]);
		}

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
		{
			str_to_number(reward.aAttr[i].bType, data[col++]);
			str_to_number(reward.aAttr[i].sValue, data[col++]);
		}

		m_map_promotionCodeInfo[promotion_code].first = llPromotionUseCount;
		m_map_promotionCodeInfo[promotion_code].second.push_back(reward);
	}

	return true;
}

bool CClientManager::InitializeRedeemedPromotionCodes()
{
	char query[2048];

	snprintf(query, sizeof(query),
		"SELECT "
		"acc_id, "
		"redeemed_code "
		"FROM "
		"itemshop.promotion_redeemed "
	);

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!m_vec_redeemedPromotionCodes.empty())
	{
		sys_log(0, "RELOAD: itemshop redeemed promotion_codes");
		m_vec_redeemedPromotionCodes.clear();
	}

	if (!pRes->uiNumRows)
	{
		return true;
	}

	MYSQL_ROW data;
	int col = 0;
	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		col = 0;

		DWORD accID;
		char code[PROMOTION_CODE_MAX_LEN + 1];
		str_to_number(accID, data[col++]);
		strlcpy(code, data[col++], sizeof(code));

		m_vec_redeemedPromotionCodes[accID].push_back(code);
	}

	return true;
}

bool CClientManager::InitializeItemshopSpecialOffers()
{
	char query[2048];

	snprintf(query, sizeof(query),
		"SELECT "
		"itemIndex, "
		"vnum, "
		"count,"
		"ABS(price), "
		"discountPercent,"
		"limitCount, "
		"category, "
		"UNIX_TIMESTAMP(endTime),"
		"weight,"
		"socket0,"
		"socket1,"
		"socket2,"
		"socket3,"
		"socket4,"
		"socket5,"
		"attrtype0,"
		"attrvalue0, "
		"attrtype1,"
		"attrvalue1, "
		"attrtype2,"
		"attrvalue2, "
		"attrtype3,"
		"attrvalue3, "
		"attrtype4,"
		"attrvalue4, "
		"attrtype5,"
		"attrvalue5, "
		"attrtype6, "
		"attrvalue6, "
		"UNIX_TIMESTAMP(startTime), "
		"UNIX_TIMESTAMP(endTime), "
		"(UNIX_TIMESTAMP(NOW()) >= UNIX_TIMESTAMP(startTime))"
		"FROM "
		"itemshop.itemshop_items AS ITEMS "
		"LEFT JOIN itemshop.itemshop_categories AS CATEGORY ON ITEMS.category = CATEGORY.category_index "
		"WHERE "
		"ITEMS.category = CATEGORY.category_index AND CATEGORY.category_state = 'ENABLED' "
		"AND not limitCount = 0 "
		"AND count > 0 "
		"AND("
		"(UNIX_TIMESTAMP(NOW()) < UNIX_TIMESTAMP(ITEMS.endTime))"
		"AND "
		"(UNIX_TIMESTAMP(ITEMS.startTime) < UNIX_TIMESTAMP(ITEMS.endTime) ))"
		"ORDER BY UNIX_TIMESTAMP(startTime)"
	);

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!m_vec_itemshopSpecialOfferItems.empty())
	{
		sys_log(0, "RELOAD: itemshop offertimes");
		m_vec_itemshopSpecialOfferItems.clear();
	}

	if (!pRes->uiNumRows)
	{
		return true;
	}

	m_vec_itemshopSpecialOfferItems.resize(pRes->uiNumRows);
	memset(&m_vec_itemshopSpecialOfferItems[0], 0, sizeof(TSpecialOfferItems) * m_vec_itemshopSpecialOfferItems.size());
	TSpecialOfferItems* specialoffer_items = &m_vec_itemshopSpecialOfferItems[0];

	MYSQL_ROW data;
	int col;
	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		col = 0;

		// NOTE: Random number -> First non unique after ~20.000 - 40.000 md5 hashes
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(1, 4294967295);

		unsigned long seed = dist(rng);

		// NOTE: To prevent duplicate hashes
		while (std::find(m_vec_usedSeeds.begin(), m_vec_usedSeeds.end(), seed) != m_vec_usedSeeds.end())
		{
			seed = dist(rng);
		}

		m_vec_usedSeeds.push_back(seed);

		boost::uuids::detail::md5 hash;
		boost::uuids::detail::md5::digest_type digest;
		hash.process_bytes((char*)&seed, sizeof(seed));
		hash.get_digest(digest);


		strlcpy(specialoffer_items->item.hash, MD5_STRING(digest).c_str(), sizeof(specialoffer_items->item.hash));

		str_to_number(specialoffer_items->llItemIndex, data[col++]);
		str_to_number(specialoffer_items->item.dwVnum, data[col++]);
		str_to_number(specialoffer_items->item.wCount, data[col++]);
		str_to_number(specialoffer_items->item.ullPrice, data[col++]);
		str_to_number(specialoffer_items->item.byDiscountPercent, data[col++]);
		str_to_number(specialoffer_items->item.llLimitCount, data[col++]);
		str_to_number(specialoffer_items->item.byCategory, data[col++]);
		str_to_number(specialoffer_items->item.end_time, data[col++]);
		str_to_number(specialoffer_items->item.weight, data[col++]);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			str_to_number(specialoffer_items->item.alSockets[i], data[col++]);
		}

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
		{
			str_to_number(specialoffer_items->item.aAttr[i].bType, data[col++]);
			str_to_number(specialoffer_items->item.aAttr[i].sValue, data[col++]);
		}

		str_to_number(specialoffer_items->times.start_time, data[col++]);
		str_to_number(specialoffer_items->times.end_time, data[col++]);
		str_to_number(specialoffer_items->times.is_activ, data[col++]);

		++specialoffer_items;
	}

	return true;
}

bool CClientManager::InitializeItemshopCategoryTable()
{
	char query[2048];

	snprintf(query, sizeof(query),
		"SELECT "
		"category_index, "
		"category_name, "
		"category_icon "
		"FROM "
		"itemshop.itemshop_categories "
		"WHERE category_state = 'ENABLED'"
	);

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (m_pItemshopTableCategories)
	{
		sys_log(0, "RELOAD: itemshop_categories");
		delete[] m_pItemshopTableCategories;
		m_pItemshopTableCategories = NULL;
	}

	if (!pRes->uiNumRows)
	{
		m_iItemshopTableCategorySize = 0;
		m_pItemshopTableCategories = new TItemshopCategoryTable[m_iItemshopTableCategorySize];
		memset(m_pItemshopTableCategories, 0, sizeof(TItemshopCategoryTable) * m_iItemshopTableCategorySize);
		return true;
	}

	m_iItemshopTableCategorySize = pRes->uiNumRows;

	m_pItemshopTableCategories = new TItemshopCategoryTable[m_iItemshopTableCategorySize];
	memset(m_pItemshopTableCategories, 0, sizeof(TItemshopCategoryTable) * m_iItemshopTableCategorySize);

	TItemshopCategoryTable* prt = m_pItemshopTableCategories;
	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;

		str_to_number(prt->index, data[col++]);
		strlcpy(prt->info.category, data[col++], sizeof(prt->info.category));
		strlcpy(prt->info.icon, data[col++], sizeof(prt->info.icon));

		prt++;
	}
	return true;
}

bool CClientManager::InitializeItemshopItemTable()
{
	char query[2048];

	snprintf(query, sizeof(query),
		"SELECT "
		"itemIndex, "
		"vnum, "
		"count,"
		"ABS(price), "
		"discountPercent,"
		"limitCount, "
		"category, "
		"UNIX_TIMESTAMP(endTime),"
		"weight,"
		"socket0,"
		"socket1,"
		"socket2,"
		"socket3,"
		"socket4,"
		"socket5,"
		"attrtype0,"
		"attrvalue0, "
		"attrtype1,"
		"attrvalue1, "
		"attrtype2,"
		"attrvalue2, "
		"attrtype3,"
		"attrvalue3, "
		"attrtype4,"
		"attrvalue4, "
		"attrtype5,"
		"attrvalue5, "
		"attrtype6, "
		"attrvalue6 "
		"FROM "
		"itemshop.itemshop_items AS ITEMS "
		"LEFT JOIN itemshop.itemshop_categories AS CATEGORY ON ITEMS.category = CATEGORY.category_index "
		"WHERE "
		"ITEMS.category = CATEGORY.category_index AND CATEGORY.category_state = 'ENABLED' "
		"AND not limitCount = 0 "
		"AND count > 0 "
		"AND("
		"(UNIX_TIMESTAMP(ITEMS.startTime) <= 0 AND UNIX_TIMESTAMP(ITEMS.endTime) <= 0) OR"
		"(UNIX_TIMESTAMP(NOW()) >= UNIX_TIMESTAMP(ITEMS.startTime) AND UNIX_TIMESTAMP(NOW()) < UNIX_TIMESTAMP(ITEMS.endTime))"
		")"
	);

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	m_ItemshopItems.clear();

	if (m_pItemshopTableItems)
	{
		sys_log(0, "RELOAD: itemshop_items");
		delete[] m_pItemshopTableItems;
		m_pItemshopTableItems = NULL;
	}

	if (!pRes->uiNumRows)
	{
		m_iItemshopTableItemSize = 0;
		m_pItemshopTableItems = new TItemshopItemTable[m_iItemshopTableItemSize];
		memset(m_pItemshopTableItems, 0, sizeof(TItemshopItemTable) * m_iItemshopTableItemSize);
		return true;
	}

	m_iItemshopTableItemSize = pRes->uiNumRows;

	m_pItemshopTableItems = new TItemshopItemTable[m_iItemshopTableItemSize];
	memset(m_pItemshopTableItems, 0, sizeof(TItemshopItemTable) * m_iItemshopTableItemSize);

	TItemshopItemTable* prt = m_pItemshopTableItems;
	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;

		// NOTE: Random number -> First non unique after ~20.000 - 40.000 md5 hashes
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(1, 4294967295);

		unsigned long seed = dist(rng);

		// NOTE: To prevent duplicate hashes
		while (std::find(m_vec_usedSeeds.begin(), m_vec_usedSeeds.end(), seed) != m_vec_usedSeeds.end())
		{
			seed = dist(rng);
		}

		m_vec_usedSeeds.push_back(seed);

		boost::uuids::detail::md5 hash;
		boost::uuids::detail::md5::digest_type digest;
		hash.process_bytes((char*)&seed, sizeof(seed));
		hash.get_digest(digest);
		strlcpy(prt->hash, MD5_STRING(digest).c_str(), sizeof(prt->hash));

		long long llIndex;
		str_to_number(llIndex, data[col++]);
		str_to_number(prt->dwVnum, data[col++]);
		str_to_number(prt->wCount, data[col++]);
		str_to_number(prt->ullPrice, data[col++]);
		str_to_number(prt->byDiscountPercent, data[col++]);
		str_to_number(prt->llLimitCount, data[col++]);
		str_to_number(prt->byCategory, data[col++]);
		str_to_number(prt->end_time, data[col++]);
		str_to_number(prt->weight, data[col++]);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			str_to_number(prt->alSockets[i], data[col++]);
		}

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
		{
			str_to_number(prt->aAttr[i].bType, data[col++]);
			str_to_number(prt->aAttr[i].sValue, data[col++]);
		}

		m_ItemshopItems[prt->hash].first = llIndex;
		m_ItemshopItems[prt->hash].second = *prt;

		prt++;
	}

	m_vec_usedSeeds.clear();

	return true;
}
#endif

