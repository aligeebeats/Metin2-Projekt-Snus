#pragma once

class CPythonNonPlayer : public CSingleton<CPythonNonPlayer>
{
	public:
		enum  EClickEvent
		{
			ON_CLICK_EVENT_NONE		= 0,
			ON_CLICK_EVENT_BATTLE	= 1,
			ON_CLICK_EVENT_SHOP		= 2,
			ON_CLICK_EVENT_TALK		= 3,
			ON_CLICK_EVENT_VEHICLE	= 4,

			ON_CLICK_EVENT_MAX_NUM,
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

		enum EMobAIFlags
		{
			AIFLAG_AGGRESSIVE = (1 << 0),
			AIFLAG_NOMOVE = (1 << 1),
			AIFLAG_COWARD = (1 << 2),
			AIFLAG_NOATTACKSHINSU = (1 << 3),
			AIFLAG_NOATTACKJINNO = (1 << 4),
			AIFLAG_NOATTACKCHUNJO = (1 << 5),
			AIFLAG_ATTACKMOB = (1 << 6),
			AIFLAG_BERSERK = (1 << 7),
			AIFLAG_STONESKIN = (1 << 8),
			AIFLAG_GODSPEED = (1 << 9),
			AIFLAG_DEATHBLOW = (1 << 10),
			AIFLAG_REVIVE = (1 << 11),
		};

		enum EMobRank
		{
			MOB_RANK_PAWN = 0,
			MOB_RANK_S_PAWN = 1,
			MOB_RANK_KNIGHT = 2,
			MOB_RANK_S_KNIGHT = 3,
			MOB_RANK_BOSS = 4,
			MOB_RANK_KING = 5,
		};

		#define MOB_ATTRIBUTE_MAX_NUM	12
		#define MOB_SKILL_MAX_NUM		5

#pragma pack(push)
#pragma pack(1)
		typedef struct SMobSkillLevel
		{
			DWORD       dwVnum;
			BYTE        bLevel;
		} TMobSkillLevel;

		typedef struct SMobTable
		{
			DWORD       dwVnum;
			char        szName[CHARACTER_NAME_MAX_LEN + 1]; 
			char        szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

			BYTE        bType;
			BYTE        bRank;
			BYTE        bBattleType;
			BYTE        bLevel;
			BYTE		bScale;
			BYTE        bSize;

			DWORD       dwGoldMin;
			DWORD       dwGoldMax;
			DWORD       dwExp;
			DWORD       dwMaxHP;
			BYTE        bRegenCycle;
			BYTE        bRegenPercent;
			WORD        wDef;

			DWORD       dwAIFlag;
			DWORD       dwRaceFlag;
			DWORD       dwImmuneFlag;

			BYTE        bStr, bDex, bCon, bInt;
			DWORD       dwDamageRange[2];

			short       sAttackSpeed;
			short       sMovingSpeed;
			BYTE        bAggresiveHPPct;
			WORD        wAggressiveSight;
			WORD        wAttackRange;

			char        cEnchants[MOB_ENCHANTS_MAX_NUM];
			char        cResists[MOB_RESISTS_MAX_NUM];

			DWORD       dwResurrectionVnum;
			DWORD       dwDropItemVnum;

			BYTE        bMountCapacity;
			BYTE        bOnClickType;

			BYTE        bEmpire;
			char        szFolder[64 + 1];
			float       fDamMultiply;
			DWORD       dwSummonVnum;
			DWORD       dwDrainSP;
			DWORD		dwMonsterColor;
		    DWORD       dwPolymorphItemVnum;

			TMobSkillLevel	Skills[MOB_SKILL_MAX_NUM];

		    BYTE		bBerserkPoint;
			BYTE		bStoneSkinPoint;
			BYTE		bGodSpeedPoint;
			BYTE		bDeathBlowPoint;
			BYTE		bRevivePoint;
		} TMobTable;
#pragma pack(pop)

		typedef std::list<TMobTable *> TMobTableList;
		typedef std::map<DWORD, TMobTable *> TNonPlayerDataMap;

	public:
		CPythonNonPlayer(void);
		virtual ~CPythonNonPlayer(void);

		void Clear();
		void Destroy();

		bool				LoadNonPlayerData(const char * c_szFileName);

		const TMobTable *	GetTable(DWORD dwVnum);
		bool				GetName(DWORD dwVnum, const char ** c_pszName);
		bool				GetInstanceType(DWORD dwVnum, BYTE* pbType);
		BYTE				GetEventType(DWORD dwVnum);
		BYTE				GetEventTypeByVID(DWORD dwVID);
		DWORD				GetMonsterColor(DWORD dwVnum);
		const char*			GetMonsterName(DWORD dwVnum);
		DWORD				GetMonsterType(DWORD dwVnum);
		DWORD				GetMonsterRank(DWORD dwVnum);
		BYTE				GetMobLevel(DWORD dwVnum);
		DWORD				GetMobAIFlag(DWORD dwVnum);
#ifdef ENABLE_TARGET_INFO
		DWORD				GetMonsterMaxHP(DWORD dwVnum);
		DWORD				GetMonsterRaceFlag(DWORD dwVnum);
		DWORD				GetMonsterLevel(DWORD dwVnum);
		DWORD				GetMonsterDamage1(DWORD dwVnum);
		DWORD				GetMonsterDamage2(DWORD dwVnum);
		DWORD				GetMonsterExp(DWORD dwVnum);
		float				GetMonsterDamageMultiply(DWORD dwVnum);
		DWORD				GetMonsterST(DWORD dwVnum);
		DWORD				GetMonsterDX(DWORD dwVnum);
		bool				IsMonsterStone(DWORD dwVnum);
		char				GetMonsterResist(DWORD dwVnum, BYTE byResist);
#endif
		void				GetMatchableMobList(int iLevel, int iInterval, TMobTableList * pMobTableList);
		BYTE				GetMonsterScalePercent(DWORD dwVnum);
	protected:
		TNonPlayerDataMap	m_NonPlayerDataMap;
};