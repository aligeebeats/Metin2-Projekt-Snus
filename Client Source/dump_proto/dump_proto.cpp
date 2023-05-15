#include "StdAfx.h"
#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include "CsvFile.h"
#include "ItemCSVReader.h"
#include "../EterBase/lzo.h"

#ifdef ENABLE_LZ4_PACK_COMPRESSION
#pragma comment ( lib, "liblz4_static.lib")
#endif

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

enum EMisc
{
	MOB_SKILL_MAX_NUM		= 5,
};

enum EMobEnchants
{
	MOB_ENCHANT_CURSE,
	MOB_ENCHANT_SLOW,
	MOB_ENCHANT_POISON,
	MOB_ENCHANT_STUN,
	MOB_ENCHANT_CRITICAL,
	MOB_ENCHANT_PENETRATE,
	MOB_ENCHANTS_MAX_NUM
};

enum EMobResists
{
	MOB_RESIST_SWORD,
	MOB_RESIST_TWOHAND,
	MOB_RESIST_DAGGER,
	MOB_RESIST_BELL,
	MOB_RESIST_FAN,
	MOB_RESIST_BOW,
#ifdef ENABLE_WOLFMAN
	MOB_RESIST_CLAW,
#endif
	MOB_RESIST_FIRE,
	MOB_RESIST_ELECT,
	MOB_RESIST_MAGIC,
	MOB_RESIST_WIND,
	MOB_RESIST_POISON,
#ifdef ENABLE_WOLFMAN
	MOB_RESIST_BLEEDING,
#endif
	MOB_RESISTS_MAX_NUM
};

#pragma pack(1)
typedef struct SMobSkillLevel
{
	DWORD	dwVnum;
	BYTE	bLevel;
} TMobSkillLevel;
#pragma pack()

#pragma pack(1)
typedef struct SMobTable
{
	DWORD	dwVnum;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	char	szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

	BYTE	bType;
	BYTE	bRank;
	BYTE	bBattleType;
	BYTE	bLevel;
	BYTE	bScale;
	BYTE	bSize;

	DWORD	dwGoldMin;
	DWORD	dwGoldMax;
	DWORD	dwExp;
	DWORD	dwMaxHP;
	BYTE	bRegenCycle;
	BYTE	bRegenPercent;
	WORD	wDef;

	DWORD	dwAIFlag;
	DWORD	dwRaceFlag;
	DWORD	dwImmuneFlag;

	BYTE	bStr, bDex, bCon, bInt;
	DWORD	dwDamageRange[2];

	short	sAttackSpeed;
	short	sMovingSpeed;
	BYTE	bAggresiveHPPct;
	WORD	wAggressiveSight;
	WORD	wAttackRange;

	char	cEnchants[MOB_ENCHANTS_MAX_NUM];
	char	cResists[MOB_RESISTS_MAX_NUM];

	DWORD	dwResurrectionVnum;
	DWORD	dwDropItemVnum;

	BYTE	bMountCapacity;
	BYTE	bOnClickType;

	BYTE	bEmpire;
	char	szFolder[64 + 1];

	float	fDamMultiply;

	DWORD	dwSummonVnum;
	DWORD	dwDrainSP;
	DWORD	dwMobColor;
	DWORD	dwPolymorphItemVnum;

	TMobSkillLevel Skills[MOB_SKILL_MAX_NUM];

	BYTE	bBerserkPoint;
	BYTE	bStoneSkinPoint;
	BYTE	bGodSpeedPoint;
	BYTE	bDeathBlowPoint;
	BYTE	bRevivePoint;
} TMobTable;
#pragma pack()

TMobTable * m_pMobTable = nullptr;
int m_iMobTableSize = 0;

enum EItemMisc
{
	ITEM_NAME_MAX_LEN			= 24,
	ITEM_VALUES_MAX_NUM			= 6,
	ITEM_SMALL_DESCR_MAX_LEN	= 256,
	ITEM_LIMIT_MAX_NUM			= 2,
	ITEM_APPLY_MAX_NUM			= 4,
	ITEM_SOCKET_MAX_NUM			= 3,
	ITEM_MAX_COUNT				= 5000,
	ITEM_ATTRIBUTE_MAX_NUM		= 7,
	ITEM_ATTRIBUTE_MAX_LEVEL	= 5,
	ITEM_AWARD_WHY_MAX_LEN		= 50,
	REFINE_MATERIAL_MAX_NUM		= 5,
	ITEM_ELK_VNUM				= 50026,
};

#pragma pack(1)
typedef struct SItemLimit
{
	BYTE	bType;
	long	lValue;
} TItemLimit;
#pragma pack()

#pragma pack(1)
typedef struct SItemApply
{
	BYTE	bType;
	long	lValue;
} TItemApply;
#pragma pack()

#pragma pack(1)
typedef struct
{
	DWORD       dwVnum;
	DWORD		dwVnumRange;
	char        szName[ITEM_NAME_MAX_LEN + 1];
	char	szLocaleName[ITEM_NAME_MAX_LEN + 1];
	BYTE	bType;
	BYTE	bSubType;

	BYTE        bWeight;
	BYTE	bSize;

	DWORD	dwAntiFlags;
	DWORD	dwFlags;
	DWORD	dwWearFlags;
	DWORD	dwImmuneFlag;

	DWORD       dwGold;
	DWORD       dwShopBuyPrice;

	TItemLimit	aLimits[ITEM_LIMIT_MAX_NUM];
	TItemApply	aApplies[ITEM_APPLY_MAX_NUM];
	long        alValues[ITEM_VALUES_MAX_NUM];
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	DWORD	dwRefinedVnum;
	WORD	wRefineSet;
	BYTE	bAlterToMagicItemPct;
	BYTE	bSpecular;
	BYTE	bGainSocketPct;
} TClientItemTable;

#pragma pack()
bool	operator < (const TClientItemTable& lhs, const TClientItemTable& rhs)
{
	return lhs.dwVnum < rhs.dwVnum;
}

TClientItemTable* m_pItemTable = nullptr;
int m_iItemTableSize = 0;

bool Set_Proto_Mob_Table(TMobTable *mobTable, cCsvTable &csvTable, std::map<int,const char*> &nameMap)
{
	int col = 0;
	mobTable->dwVnum               = atoi(csvTable.AsStringByIndex(col++));
	strncpy(mobTable->szName, csvTable.AsStringByIndex(col++), CHARACTER_NAME_MAX_LEN);
	std::map<int,const char*>::iterator it;
	it = nameMap.find(mobTable->dwVnum);
	if (it != nameMap.end()) 
	{
		const char * localeName = it->second;
		strncpy(mobTable->szLocaleName, localeName, sizeof (mobTable->szLocaleName));
	} 
	else 
	{
		strncpy(mobTable->szLocaleName, mobTable->szName, sizeof (mobTable->szLocaleName));
	}
	int rankValue = get_Mob_Rank_Value(csvTable.AsStringByIndex(col++));
	mobTable->bRank                = rankValue;

	int typeValue = get_Mob_Type_Value(csvTable.AsStringByIndex(col++));
	mobTable->bType                = typeValue;

	int battleTypeValue = get_Mob_BattleType_Value(csvTable.AsStringByIndex(col++));
	mobTable->bBattleType          = battleTypeValue;

	mobTable->bLevel		= atoi(csvTable.AsStringByIndex(col++));

	mobTable->bScale		= atoi(csvTable.AsStringByIndex(col++));

	int sizeValue = get_Mob_Size_Value(csvTable.AsStringByIndex(col++));
	mobTable->bSize                = sizeValue;

	int aiFlagValue = get_Mob_AIFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwAIFlag             = aiFlagValue;
	col++;

	int raceFlagValue = get_Mob_RaceFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwRaceFlag           = raceFlagValue;

	int immuneFlagValue = get_Mob_ImmuneFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwImmuneFlag         = immuneFlagValue;

	mobTable->bEmpire              = atoi(csvTable.AsStringByIndex(col++));

	strncpy(mobTable->szFolder, csvTable.AsStringByIndex(col++), sizeof(mobTable->szFolder));

	mobTable->bOnClickType         = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bStr                 = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bDex                 = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bCon                 = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bInt                 = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwDamageRange[0]     = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwDamageRange[1]     = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwMaxHP              = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bRegenCycle          = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bRegenPercent        = atoi(csvTable.AsStringByIndex(col++));
	col++;
	col++;
	mobTable->dwExp                = atoi(csvTable.AsStringByIndex(col++));
	mobTable->wDef                 = atoi(csvTable.AsStringByIndex(col++));
	mobTable->sAttackSpeed         = atoi(csvTable.AsStringByIndex(col++));
	mobTable->sMovingSpeed         = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bAggresiveHPPct      = atoi(csvTable.AsStringByIndex(col++));
	mobTable->wAggressiveSight	= atoi(csvTable.AsStringByIndex(col++));
	mobTable->wAttackRange		= atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwDropItemVnum	= atoi(csvTable.AsStringByIndex(col++));
	col++;

	for (int i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
		mobTable->cEnchants[i] = atoi(csvTable.AsStringByIndex(col++));

	for (int i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
		mobTable->cResists[i] = atoi(csvTable.AsStringByIndex(col++));

	mobTable->fDamMultiply		= atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwSummonVnum		= atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwDrainSP		= atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwMobColor		= atoi(csvTable.AsStringByIndex(col++));
	
	return true;
}

bool BuildMobTable()
{
	fprintf(stderr, "sizeof(TMobTable): %u\n", sizeof(TMobTable));

	bool isNameFile = true;
	std::map<int,const char*> localMap;
	cCsvTable nameData;

	if(!nameData.Load("mob_names.txt",'\t'))
	{
		fprintf(stderr, "mob_names.txt not found\n");
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
		fprintf(stderr, "mob_proto.txt not found.\n");
		return false;
	}
	data.Next();

	if (m_pMobTable)
	{
		free(m_pMobTable);
		m_pMobTable = NULL;
	}

	m_iMobTableSize = data.m_File.GetRowCount()-1;

	m_pMobTable = (TMobTable*)calloc(m_iMobTableSize, sizeof(TMobTable));
	memset(m_pMobTable, 0, sizeof(TMobTable) * m_iMobTableSize);

	TMobTable * mob_table = m_pMobTable;

	while (data.Next())
	{
		if (!Set_Proto_Mob_Table(mob_table, data, localMap))
		{
			fprintf(stderr, "몹 프로토 테이블 셋팅 실패.\n");			
		}

#ifdef _DEBUG
		fprintf(stdout, "MOB #%-5d %-16s %-16s sight: %u color %u[%d]\n", mob_table->dwVnum, mob_table->szName, mob_table->szLocaleName, mob_table->wAggressiveSight, mob_table->dwMobColor, 0);
#endif

		++mob_table;
	}
	return true;
}

DWORD g_adwMobProtoKey[4] =
{   
	4813894,
	18955,
	552631,             
	6822045
};

void SaveMobProto()
{   
	FILE * fp;          

	fp = fopen("mob_proto", "wb");

	if (!fp)
	{ 
		printf("cannot open %s for writing\n", "mob_proto");
		return;
	}

	DWORD fourcc = MAKEFOURCC('M', 'M', 'P', 'T');
	fwrite(&fourcc, sizeof(DWORD), 1, fp);      

	DWORD dwElements = m_iMobTableSize;
	fwrite(&dwElements, sizeof(DWORD), 1, fp);

	CLZObject zObj;     

	printf("sizeof(TMobTable) %d\n", sizeof(TMobTable));

	if (!CLZO::instance().CompressEncryptedMemory(zObj, m_pMobTable, sizeof(TMobTable) * m_iMobTableSize, g_adwMobProtoKey))
	{
		printf("cannot compress\n");
		fclose(fp);
		return;
	}

	const CLZObject::THeader & r = zObj.GetHeader();

	printf("MobProto count %u\n%u --Compress--> %u --Encrypt--> %u, GetSize %u\n",
			m_iMobTableSize, r.dwRealSize, r.dwCompressedSize, r.dwEncryptSize, zObj.GetSize());

	DWORD dwDataSize = zObj.GetSize();
	fwrite(&dwDataSize, sizeof(DWORD), 1, fp);
	fwrite(zObj.GetBuffer(), dwDataSize, 1, fp);

	fclose(fp);
}

void LoadMobProto()
{
	FILE * fp;
	DWORD fourcc, tableSize, dataSize;

	fp = fopen("mob_proto", "rb");

	fread(&fourcc, sizeof(DWORD), 1, fp);
	fread(&tableSize, sizeof(DWORD), 1, fp);
	fread(&dataSize, sizeof(DWORD), 1, fp);
	BYTE * data = (BYTE *) malloc(dataSize);

	if (data)
	{
		fread(data, dataSize, 1, fp);

		CLZObject zObj;

		if (CLZO::instance().Decompress(zObj, data, g_adwMobProtoKey))
		{
			printf("real_size %u\n", zObj.GetSize());

			for (DWORD i = 0; i < tableSize; ++i)
			{
				TMobTable & rTable = *((TMobTable *) zObj.GetBuffer() + i);
#ifdef _DEBUG
				printf("%u %s\n", rTable.dwVnum, rTable.szName);
#endif
			}
		}

		free(data);
	}

	fclose(fp);
}

bool Set_Proto_Item_Table(TClientItemTable *itemTable, cCsvTable &csvTable, std::map<int,const char*> &nameMap)
{
	{
		std::string s(csvTable.AsStringByIndex(0));
		int pos = s.find("~");

		if (std::string::npos == pos)
		{
			itemTable->dwVnum = atoi(s.c_str());
			if (0 == itemTable->dwVnum)
			{
				printf ("INVALID VNUM %s\n", s.c_str());
				return false;
			}
			itemTable->dwVnumRange = 0;
		}
		else
		{
			std::string s_start_vnum (s.substr(0, pos));
			std::string s_end_vnum (s.substr(pos +1 ));

			int start_vnum = atoi(s_start_vnum.c_str());
			int end_vnum = atoi(s_end_vnum.c_str());
			if (0 == start_vnum || (0 != end_vnum && end_vnum < start_vnum))
			{
				printf ("INVALID VNUM RANGE%s\n", s.c_str());
				return false;
			}
			itemTable->dwVnum = start_vnum;
			itemTable->dwVnumRange = end_vnum - start_vnum;
		}
	}
	int col = 1;
	strncpy(itemTable->szName, csvTable.AsStringByIndex(col++), ITEM_NAME_MAX_LEN);
	std::map<int,const char*>::iterator it;
	it = nameMap.find(itemTable->dwVnum);
	if (it != nameMap.end())
	{
		const char * localeName = it->second;
		strncpy(itemTable->szLocaleName, localeName, sizeof(itemTable->szLocaleName));
	} 
	else 
	{
		strncpy(itemTable->szLocaleName, itemTable->szName, sizeof(itemTable->szLocaleName));
	}
	itemTable->bType = get_Item_Type_Value(csvTable.AsStringByIndex(col++));
	itemTable->bSubType = get_Item_SubType_Value(itemTable->bType, csvTable.AsStringByIndex(col++));
	itemTable->bSize = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dwAntiFlags = get_Item_AntiFlag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwFlags = get_Item_Flag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwWearFlags = get_Item_WearFlag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwImmuneFlag = get_Item_Immune_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwGold = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dwShopBuyPrice = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dwRefinedVnum = atoi(csvTable.AsStringByIndex(col++));
	itemTable->wRefineSet = atoi(csvTable.AsStringByIndex(col++));
	itemTable->bAlterToMagicItemPct = atoi(csvTable.AsStringByIndex(col++));

	int i;

	for (i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		itemTable->aLimits[i].bType = get_Item_LimitType_Value(csvTable.AsStringByIndex(col++));
		itemTable->aLimits[i].lValue = atoi(csvTable.AsStringByIndex(col++));
	}

	for (i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		itemTable->aApplies[i].bType = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col++));
		itemTable->aApplies[i].lValue = atoi(csvTable.AsStringByIndex(col++));
	}

	for (i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
		itemTable->alValues[i] = atoi(csvTable.AsStringByIndex(col++));

	itemTable->bSpecular = atoi(csvTable.AsStringByIndex(col++));
	itemTable->bGainSocketPct = atoi(csvTable.AsStringByIndex(col++));
	col++;
	
	itemTable->bWeight = 0;

	return true;
}

bool BuildItemTable()
{
	fprintf(stderr, "sizeof(TClientItemTable): %u\n", sizeof(TClientItemTable));

	bool isNameFile = true;
	std::map<int,const char*> localMap;
	cCsvTable nameData;

	if(!nameData.Load("item_names.txt",'\t'))
	{
		fprintf(stderr, "item_names.txt not found.\n");
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

	if (m_pItemTable)
	{
		free(m_pItemTable);
		m_pItemTable = nullptr;
	}

	data.Destroy();
	if(!data.Load("item_proto.txt",'\t'))
	{
		fprintf(stderr, "item_proto.txt not found.\n");
		return false;
	}
	data.Next();

	m_iItemTableSize = data.m_File.GetRowCount()-1;
	m_pItemTable = (TClientItemTable*)calloc(m_iItemTableSize, sizeof(TClientItemTable));
	memset(m_pItemTable, 0, sizeof(TClientItemTable) * m_iItemTableSize);

	TClientItemTable* item_table = m_pItemTable;

	while (data.Next())
	{
		if (!Set_Proto_Item_Table(item_table, data, localMap))
		{
			fprintf(stderr, "Set_Proto_Item_Table failed.\n");			
		}

#ifdef _DEBUG
		fprintf(stdout, "ITEM #%-5u %-24s %-24s VAL: %ld %ld %ld %ld %ld %ld WEAR %u ANTI %u IMMUNE %u REFINE %u\n",
				item_table->dwVnum,
				item_table->szName,
				item_table->szLocaleName,
				item_table->alValues[0],
				item_table->alValues[1],
				item_table->alValues[2],
				item_table->alValues[3],
				item_table->alValues[4],
				item_table->alValues[5],
				item_table->dwWearFlags,
				item_table->dwAntiFlags,
				item_table->dwImmuneFlag,
				item_table->dwRefinedVnum);
#endif
		++item_table;
	}

	return true;
}

DWORD g_adwItemProtoKey[4] =
{
	173217,
	72619434,
	408587239,
	27973291
};  

void SaveItemProto()
{
	FILE * fp;

	fp = fopen("item_proto", "wb");

	if (!fp)
	{
		printf("cannot open %s for writing\n", "item_proto");
		return;
	}   

	DWORD fourcc = MAKEFOURCC('M', 'I', 'P', 'X');
	fwrite(&fourcc, sizeof(DWORD), 1, fp);

	DWORD dwVersion = 0x00000001;
	fwrite(&dwVersion, sizeof(DWORD), 1, fp);

	DWORD dwStride = sizeof(TClientItemTable);
	fwrite(&dwStride, sizeof(DWORD), 1, fp); 

	DWORD dwElements = m_iItemTableSize;
	fwrite(&dwElements, sizeof(DWORD), 1, fp);

	CLZObject zObj;
	std::vector <TClientItemTable> vec_item_table (&m_pItemTable[0], &m_pItemTable[m_iItemTableSize - 1]);
	std::sort(&m_pItemTable[0], &m_pItemTable[0] + m_iItemTableSize);

	if (!CLZO::instance().CompressEncryptedMemory(zObj, m_pItemTable, sizeof(TClientItemTable) * m_iItemTableSize, g_adwItemProtoKey))
	{
		printf("cannot compress\n");
		fclose(fp);
		return;
	}   

	const CLZObject::THeader & r = zObj.GetHeader();

	printf("Elements %d\n%u --Compress--> %u --Encrypt--> %u, GetSize %u\n",
			m_iItemTableSize,
			r.dwRealSize,
			r.dwCompressedSize,
			r.dwEncryptSize,
			zObj.GetSize());

	DWORD dwDataSize = zObj.GetSize();
	fwrite(&dwDataSize, sizeof(DWORD), 1, fp);
	fwrite(zObj.GetBuffer(), dwDataSize, 1, fp);

	fclose(fp);

	fp = fopen("item_proto", "rb");

	if (!fp)
	{
		printf("Error!!\n");
		return;
	}

	fread(&fourcc, sizeof(DWORD), 1, fp);
	fread(&dwElements, sizeof(DWORD), 1, fp);

	printf("Elements Check %u fourcc match %d\n", dwElements, fourcc == MAKEFOURCC('M', 'I', 'P', 'T'));
	fclose(fp);
}

int main(int argc, char ** argv)
{
	CLZO lzo_manager;
	
	if (BuildMobTable())
	{
		SaveMobProto();
		LoadMobProto();
		std::cout << "BuildMobTable working normal" << std::endl;
	}

	if (BuildItemTable())
	{
		SaveItemProto();
		std::cout << "BuildItemTable working normal" << std::endl;
	}
	return 0;
}
