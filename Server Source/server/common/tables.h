#pragma once
#include "length.h"

typedef	DWORD IDENT;

enum
{
	HEADER_GD_LOGIN								= 1,
	HEADER_GD_LOGOUT							= 2,
	HEADER_GD_PLAYER_LOAD						= 3,
	HEADER_GD_PLAYER_SAVE						= 4,
	HEADER_GD_PLAYER_CREATE						= 5,
	HEADER_GD_PLAYER_DELETE						= 6,
	HEADER_GD_LOGIN_KEY							= 7,
	HEADER_GD_BOOT								= 9,
	HEADER_GD_PLAYER_COUNT						= 10,
	HEADER_GD_QUEST_SAVE						= 11,
	HEADER_GD_SAFEBOX_LOAD						= 12,
	HEADER_GD_SAFEBOX_SAVE						= 13,
	HEADER_GD_SAFEBOX_CHANGE_SIZE				= 14,
	HEADER_GD_EMPIRE_SELECT						= 15,
	HEADER_GD_SAFEBOX_CHANGE_PASSWORD			= 16,
	HEADER_GD_SAFEBOX_CHANGE_PASSWORD_SECOND	= 17,
	HEADER_GD_DIRECT_ENTER						= 18,
	HEADER_GD_GUILD_SKILL_UPDATE				= 19,
	HEADER_GD_GUILD_EXP_UPDATE					= 20,
	HEADER_GD_GUILD_ADD_MEMBER					= 21,
	HEADER_GD_GUILD_REMOVE_MEMBER				= 22,
	HEADER_GD_GUILD_CHANGE_GRADE				= 23,
	HEADER_GD_GUILD_CHANGE_MEMBER_DATA			= 24,
	HEADER_GD_GUILD_DISBAND						= 25,
	HEADER_GD_GUILD_WAR							= 26,
	HEADER_GD_GUILD_WAR_SCORE					= 27,
	HEADER_GD_GUILD_CREATE						= 28,
	HEADER_GD_ITEM_SAVE							= 30,
	HEADER_GD_ITEM_DESTROY						= 31,
	HEADER_GD_ADD_AFFECT						= 32,
	HEADER_GD_REMOVE_AFFECT						= 33,
	HEADER_GD_HIGHSCORE_REGISTER				= 34,
	HEADER_GD_ITEM_FLUSH						= 35,
	HEADER_GD_PARTY_CREATE						= 36,
	HEADER_GD_PARTY_DELETE						= 37,
	HEADER_GD_PARTY_ADD							= 38,
	HEADER_GD_PARTY_REMOVE						= 39,
	HEADER_GD_PARTY_STATE_CHANGE				= 40,
	HEADER_GD_PARTY_HEAL_USE					= 41,
	HEADER_GD_FLUSH_CACHE						= 42,
	HEADER_GD_RELOAD_PROTO						= 43,
	HEADER_GD_CHANGE_NAME						= 44,
	HEADER_GD_GUILD_CHANGE_LADDER_POINT			= 46,
	HEADER_GD_GUILD_USE_SKILL					= 47,
	HEADER_GD_REQUEST_EMPIRE_PRIV				= 48,
	HEADER_GD_REQUEST_GUILD_PRIV				= 49,
	HEADER_GD_GUILD_DEPOSIT_MONEY				= 51,
	HEADER_GD_GUILD_WITHDRAW_MONEY				= 52,
	HEADER_GD_GUILD_WITHDRAW_MONEY_GIVE_REPLY	= 53,
	HEADER_GD_REQUEST_CHARACTER_PRIV			= 54,
	HEADER_GD_SET_EVENT_FLAG					= 55,
	HEADER_GD_PARTY_SET_MEMBER_LEVEL			= 56,
	HEADER_GD_GUILD_WAR_BET						= 57,
	HEADER_GD_CREATE_OBJECT						= 60,
	HEADER_GD_DELETE_OBJECT						= 61,
	HEADER_GD_UPDATE_LAND						= 62,
	HEADER_GD_MARRIAGE_ADD						= 70,
	HEADER_GD_MARRIAGE_UPDATE					= 71,
	HEADER_GD_MARRIAGE_REMOVE					= 72,
	HEADER_GD_WEDDING_REQUEST					= 73,
	HEADER_GD_WEDDING_READY						= 74,
	HEADER_GD_WEDDING_END						= 75,
	HEADER_GD_AUTH_LOGIN						= 100,
	HEADER_GD_LOGIN_BY_KEY						= 101,
	HEADER_GD_MALL_LOAD							= 107,
	HEADER_GD_MYSHOP_PRICELIST_UPDATE			= 108,
	HEADER_GD_MYSHOP_PRICELIST_REQ				= 109,
	HEADER_GD_BLOCK_CHAT						= 110,
	HEADER_GD_RELOAD_ADMIN						= 115,
	HEADER_GD_BREAK_MARRIAGE					= 116,
	HEADER_GD_REQ_CHANGE_GUILD_MASTER			= 129,
	HEADER_GD_REQ_SPARE_ITEM_ID_RANGE			= 130,
	HEADER_GD_UPDATE_HORSE_NAME					= 131,
	HEADER_GD_REQ_HORSE_NAME					= 132,
	HEADER_GD_DC								= 133,
	HEADER_GD_VALID_LOGOUT						= 134,
	HEADER_GD_DELETE_AWARDID					= 138,
	HEADER_GD_UPDATE_CHANNELSTATUS				= 139,
	HEADER_GD_REQUEST_CHANNELSTATUS				= 140,
#ifndef _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GD_SHUTDOWN							= 149,
#endif
#ifdef ENABLE_ITEMSHOP
	HEADER_GD_RELOAD_ITEMSHOP 					= 200,
	HEADER_GD_BUY_ITEMSHOP 						= 201,
	HEADER_GD_PROMOTION_REDEEM 					= 202,
#endif
	HEADER_GD_SETUP								= 0xff,

	HEADER_DG_NOTICE							= 1,
	HEADER_DG_LOGIN_SUCCESS						= 30,
	HEADER_DG_LOGIN_NOT_EXIST					= 31,
	HEADER_DG_LOGIN_WRONG_PASSWD				= 33,
	HEADER_DG_LOGIN_ALREADY						= 34,
	HEADER_DG_PLAYER_LOAD_SUCCESS				= 35,
	HEADER_DG_PLAYER_LOAD_FAILED				= 36,
	HEADER_DG_PLAYER_CREATE_SUCCESS				= 37,
	HEADER_DG_PLAYER_CREATE_ALREADY				= 38,
	HEADER_DG_PLAYER_CREATE_FAILED				= 39,
	HEADER_DG_PLAYER_DELETE_SUCCESS				= 40,
	HEADER_DG_PLAYER_DELETE_FAILED				= 41,
	HEADER_DG_ITEM_LOAD							= 42,
	HEADER_DG_BOOT								= 43,
	HEADER_DG_QUEST_LOAD						= 44,
	HEADER_DG_SAFEBOX_LOAD						= 45,
	HEADER_DG_SAFEBOX_CHANGE_SIZE				= 46,
	HEADER_DG_SAFEBOX_WRONG_PASSWORD			= 47,
	HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER	= 48,
	HEADER_DG_EMPIRE_SELECT						= 49,
	HEADER_DG_AFFECT_LOAD						= 50,
	HEADER_DG_MALL_LOAD							= 51,
	HEADER_DG_DIRECT_ENTER						= 55,
	HEADER_DG_GUILD_SKILL_UPDATE				= 56,
	HEADER_DG_GUILD_SKILL_RECHARGE				= 57,
	HEADER_DG_GUILD_EXP_UPDATE					= 58,
	HEADER_DG_PARTY_CREATE						= 59,
	HEADER_DG_PARTY_DELETE						= 60,
	HEADER_DG_PARTY_ADD							= 61,
	HEADER_DG_PARTY_REMOVE						= 62,
	HEADER_DG_PARTY_STATE_CHANGE				= 63,
	HEADER_DG_PARTY_HEAL_USE					= 64,
	HEADER_DG_PARTY_SET_MEMBER_LEVEL			= 65,
	HEADER_DG_TIME								= 90,
	HEADER_DG_ITEM_ID_RANGE						= 91,
	HEADER_DG_GUILD_ADD_MEMBER					= 92,
	HEADER_DG_GUILD_REMOVE_MEMBER				= 93,
	HEADER_DG_GUILD_CHANGE_GRADE				= 94,
	HEADER_DG_GUILD_CHANGE_MEMBER_DATA			= 95,
	HEADER_DG_GUILD_DISBAND						= 96,
	HEADER_DG_GUILD_WAR							= 97,
	HEADER_DG_GUILD_WAR_SCORE					= 98,
	HEADER_DG_GUILD_TIME_UPDATE					= 99,
	HEADER_DG_GUILD_LOAD						= 100,
	HEADER_DG_GUILD_LADDER						= 101,
	HEADER_DG_GUILD_SKILL_USABLE_CHANGE			= 102,
	HEADER_DG_GUILD_MONEY_CHANGE				= 103,
	HEADER_DG_GUILD_WITHDRAW_MONEY_GIVE			= 104,
	HEADER_DG_SET_EVENT_FLAG					= 105,
	HEADER_DG_GUILD_WAR_RESERVE_ADD				= 106,
	HEADER_DG_GUILD_WAR_RESERVE_DEL				= 107,
	HEADER_DG_GUILD_WAR_BET						= 108,
	HEADER_DG_RELOAD_PROTO						= 120,
	HEADER_DG_CHANGE_NAME						= 121,
	HEADER_DG_AUTH_LOGIN						= 122,
	HEADER_DG_CHANGE_EMPIRE_PRIV				= 124,
	HEADER_DG_CHANGE_GUILD_PRIV					= 125,
	HEADER_DG_CHANGE_CHARACTER_PRIV				= 127,
	HEADER_DG_CREATE_OBJECT						= 140,
	HEADER_DG_DELETE_OBJECT						= 141,
	HEADER_DG_UPDATE_LAND						= 142,
	HEADER_DG_MARRIAGE_ADD						= 150,
	HEADER_DG_MARRIAGE_UPDATE					= 151,
	HEADER_DG_MARRIAGE_REMOVE					= 152,
	HEADER_DG_WEDDING_REQUEST					= 153,
	HEADER_DG_WEDDING_READY						= 154,
	HEADER_DG_WEDDING_START						= 155,
	HEADER_DG_WEDDING_END						= 156,
	HEADER_DG_MYSHOP_PRICELIST_RES				= 157,
	HEADER_DG_RELOAD_ADMIN						= 158,
	HEADER_DG_BREAK_MARRIAGE					= 159,
	HEADER_DG_ACK_CHANGE_GUILD_MASTER			= 173,
	HEADER_DG_ACK_SPARE_ITEM_ID_RANGE			= 174,
	HEADER_DG_UPDATE_HORSE_NAME 				= 175,
	HEADER_DG_ACK_HORSE_NAME					= 176,
	HEADER_DG_NEED_LOGIN_LOG					= 177,
	HEADER_DG_ITEMAWARD_INFORMER				= 180,
	HEADER_DG_RESPOND_CHANNELSTATUS				= 181,
#ifndef _IMPROVED_PACKET_ENCRYPTION_
	HEADER_DG_SHUTDOWN							= 184,
#endif
#ifdef ENABLE_ITEMSHOP
	HEADER_DG_RELOAD_ITEMSHOP 					= 192,
	HEADER_DG_BUY_ITEMSHOP_ITEM 				= 193,
	HEADER_DG_REFRESH_ITEMSHOP_SINGLE_ITEM 		= 194,
	HEADER_DG_REMOVE_ITEMSHOP_SINGLE_ITEM 		= 195,
	HEADER_DG_ADD_ITEMSHOP_SINGLE_ITEM 			= 196,
	HEADER_DG_PROMOTION_CODE_REDEEM 			= 197,
#endif
	HEADER_DG_MAP_LOCATIONS						= 0xfe,
	HEADER_DG_P2P								= 0xff,
};

#pragma pack(1)

typedef struct SSimplePlayer
{
	DWORD		dwID;
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE		byJob;
	BYTE		byLevel;
	DWORD		dwPlayMinutes;
	BYTE		byST, byHT, byDX, byIQ;
	WORD		wMainPart;
	BYTE		bChangeName;
	WORD		wHairPart;
	DWORD		dwAccePart;
	BYTE		bDummy[4];
	long		x, y;
	long		lAddr;
	WORD		wPort;
	BYTE		skill_group;
} TSimplePlayer;

typedef struct SAccountTable
{
	DWORD			id;
	char			login[LOGIN_MAX_LEN + 1];
	char			passwd[PASSWD_MAX_LEN + 1];
	char			social_id[SOCIAL_ID_MAX_LEN + 1];
	char			status[ACCOUNT_STATUS_MAX_LEN + 1];
	BYTE			bEmpire;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
} TAccountTable;

typedef struct SPacketDGCreateSuccess
{
	BYTE			bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketDGCreateSuccess;

typedef struct TPlayerItemAttribute
{
	BYTE	bType;
	short	sValue;
} TPlayerItemAttribute;

typedef struct SPlayerItem
{
	DWORD	id;
	BYTE	window;
	WORD	pos;
	DWORD	count;
	DWORD	vnum;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute    aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	DWORD	owner;
} TPlayerItem;

typedef struct SQuickslot
{
	BYTE	type;
	BYTE	pos;
} TQuickslot;

typedef struct SPlayerSkill
{
	BYTE	bMasterType;
	BYTE	bLevel;
	time_t	tNextRead;
} TPlayerSkill;

struct	THorseInfo
{
	BYTE	bLevel;
	BYTE	bRiding;
	short	sStamina;
	short	sHealth;
	DWORD	dwHorseHealthDropTime;
};

typedef struct SPlayerTable
{
	DWORD		id;
	char		name[CHARACTER_NAME_MAX_LEN + 1];
	char		ip[IP_ADDRESS_LENGTH + 1];
	WORD		job;
	BYTE		voice;
	BYTE		level;
	BYTE		level_step;
	short		st, ht, dx, iq;
	DWORD		exp;
	long long	gold;
	BYTE		dir;
	INT			x, y, z;
	INT			lMapIndex;
	long		lExitX, lExitY;
	long		lExitMapIndex;
	int			hp;
	int			sp;
	short		sRandomHP;
	short		sRandomSP;
	int			playtime;
	short		stat_point;
	short		skill_point;
	short		sub_skill_point;
	short		horse_skill_point;
	TPlayerSkill skills[SKILL_MAX_NUM];
	TQuickslot  quickslot[QUICKSLOT_MAX_NUM];
	BYTE		part_base;
	DWORD		parts[PART_MAX_NUM];
	short		stamina;
	BYTE		skill_group;
	long		lAlignment;
	short		stat_reset_count;
	THorseInfo	horse;
	DWORD		logoff_interval;
	int			aiPremiumTimes[PREMIUM_MAX_NUM];
} TPlayerTable;

typedef struct SMobSkillLevel
{
	DWORD	dwVnum;
	BYTE	bLevel;
} TMobSkillLevel;

typedef struct SEntityTable
{
	DWORD	dwVnum;
} TEntityTable;

typedef struct SMobTable : public SEntityTable
{
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	char	szLocaleName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bType;
	BYTE	bRank;
	BYTE	bBattleType;
	BYTE	bLevel;
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

typedef struct SSkillTable
{
	DWORD	dwVnum;
	char	szName[32 + 1];
	BYTE	bType;
	BYTE	bMaxLevel;
	DWORD	dwSplashRange;
	char	szPointOn[64];
	char	szPointPoly[100 + 1];
	char	szSPCostPoly[100 + 1];
	char	szDurationPoly[100 + 1];
	char	szDurationSPCostPoly[100 + 1];
	char	szCooldownPoly[100 + 1];
	char	szMasterBonusPoly[100 + 1];
	char	szGrandMasterAddSPCostPoly[100 + 1];
	DWORD	dwFlag;
	DWORD	dwAffectFlag;
	char 	szPointOn2[64];
	char 	szPointPoly2[100 + 1];
	char 	szDurationPoly2[100 + 1];
	DWORD 	dwAffectFlag2;
	char 	szPointOn3[64];
	char 	szPointPoly3[100 + 1];
	char 	szDurationPoly3[100 + 1];
	BYTE	bLevelStep;
	BYTE	bLevelLimit;
	DWORD	preSkillVnum;
	BYTE	preSkillLevel;
	long	lMaxHit; 
	char	szSplashAroundDamageAdjustPoly[100 + 1];
	BYTE	bSkillAttrType;
	DWORD	dwTargetRange;
} TSkillTable;

typedef struct SShopItemTable
{
	DWORD		vnum;
	WORD		count;
    TItemPos	pos;
	long long	price;
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	DWORD 		price_type = 1;
	DWORD		price_vnum = 0;
	SShopItemTable() {
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}
} TShopItemTable;

enum STableExTypes : decltype(TShopItemTable::price_type)
{
	EX_GOLD = 1,
	EX_SECONDARY,
	EX_ITEM,
	EX_EXP,
	EX_MAX,
};

typedef struct SShopTable
{
	DWORD			dwVnum;
	DWORD			dwNPCVnum;
	WORD			byItemCount;
	TShopItemTable	items[SHOP_HOST_ITEM_MAX_NUM];
} TShopTable;

#define QUEST_NAME_MAX_LEN	32
#define QUEST_STATE_MAX_LEN	64

typedef struct SQuestTable
{
	DWORD		dwPID;
	char		szName[QUEST_NAME_MAX_LEN + 1];
	char		szState[QUEST_STATE_MAX_LEN + 1];
	long		lValue;
} TQuestTable;

typedef struct SItemLimit
{
	BYTE	bType;
	long	lValue;
} TItemLimit;

typedef struct SItemApply
{
	BYTE	bType;
	long	lValue;
} TItemApply;

typedef struct SItemTable : public SEntityTable
{
	DWORD		dwVnumRange;
	char        szName[ITEM_NAME_MAX_LEN + 1];
	char		szLocaleName[ITEM_NAME_MAX_LEN + 1];
	BYTE		bType;
	BYTE		bSubType;
	BYTE        bWeight;
	BYTE		bSize;
	DWORD		dwAntiFlags;
	DWORD		dwFlags;
	DWORD		dwWearFlags;
	DWORD		dwImmuneFlag;
	long long	lldGold;
	long long	lldShopBuyPrice;
	TItemLimit	aLimits[ITEM_LIMIT_MAX_NUM];
	TItemApply	aApplies[ITEM_APPLY_MAX_NUM];
	long        alValues[ITEM_VALUES_MAX_NUM];
	long		alSockets[ITEM_SOCKET_MAX_NUM];
	DWORD		dwRefinedVnum;
	WORD		wRefineSet;
	BYTE		bAlterToMagicItemPct;
	BYTE		bSpecular;
	BYTE		bGainSocketPct;
	short int	sAddonType;
	char		cLimitRealTimeFirstUseIndex;
	char		cLimitTimerBasedOnWearIndex;
} TItemTable;

struct TItemAttrTable
{
	TItemAttrTable() :
		dwApplyIndex(0),
		dwProb(0)
	{
		szApply[0] = 0;
		memset(&lValues, 0, sizeof(lValues));
		memset(&bMaxLevelBySet, 0, sizeof(bMaxLevelBySet));
	}

	char    szApply[APPLY_NAME_MAX_LEN + 1];
	DWORD   dwApplyIndex;
	DWORD   dwProb;
	long    lValues[5];
	BYTE    bMaxLevelBySet[ATTRIBUTE_SET_MAX_NUM];
};

typedef struct SConnectTable
{
	char	login[LOGIN_MAX_LEN + 1];
	IDENT	ident;
} TConnectTable;

typedef struct SLoginPacket
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TLoginPacket;

typedef struct SPlayerLoadPacket
{
	DWORD	account_id;
	DWORD	player_id;
	BYTE	account_index;
} TPlayerLoadPacket;

typedef struct SPlayerCreatePacket
{
	char		login[LOGIN_MAX_LEN + 1];
	char		passwd[PASSWD_MAX_LEN + 1];
	DWORD		account_id;
	BYTE		account_index;
	TPlayerTable	player_table;
} TPlayerCreatePacket;

typedef struct SPlayerDeletePacket
{
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	player_id;
	BYTE	account_index;
	char	private_code[8];
} TPlayerDeletePacket;

typedef struct SLogoutPacket
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TLogoutPacket;

typedef struct SPlayerCountPacket
{
	DWORD	dwCount;
} TPlayerCountPacket;

#define SAFEBOX_MAX_NUM			135
#define SAFEBOX_PASSWORD_MAX_LEN	6

typedef struct SSafeboxTable
{
	DWORD		dwID;
	BYTE		bSize;
	long long	lldGold;
	WORD		wItemCount;
} TSafeboxTable;

typedef struct SSafeboxChangeSizePacket
{
	DWORD	dwID;
	BYTE	bSize;
} TSafeboxChangeSizePacket;

typedef struct SSafeboxLoadPacket
{
	DWORD	dwID;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxLoadPacket;

typedef struct SSafeboxChangePasswordPacket
{
	DWORD	dwID;
	char	szOldPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
	char	szNewPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxChangePasswordPacket;

typedef struct SSafeboxChangePasswordPacketAnswer
{
	BYTE	flag;
} TSafeboxChangePasswordPacketAnswer;

typedef struct SEmpireSelectPacket
{
	DWORD	dwAccountID;
	BYTE	bEmpire;
} TEmpireSelectPacket;

typedef struct SPacketGDSetup
{
	char	szPublicIP[16];
	BYTE	bChannel;
	WORD	wListenPort;
	WORD	wP2PPort;
	long	alMaps[32];
	DWORD	dwLoginCount;
	BYTE	bAuthServer;
} TPacketGDSetup;

typedef struct SPacketDGMapLocations
{
	BYTE	bCount;
} TPacketDGMapLocations;

typedef struct SMapLocation
{
	long	alMaps[32];
	char	szHost[MAX_HOST_LENGTH + 1];
	WORD	wPort;
} TMapLocation;

typedef struct SPacketDGP2P
{
	char	szHost[MAX_HOST_LENGTH + 1];
	WORD	wPort;
	BYTE	bChannel;
} TPacketDGP2P;

typedef struct SPacketGDDirectEnter
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	BYTE	index;
} TPacketGDDirectEnter;

typedef struct SPacketDGDirectEnter
{
	TAccountTable accountTable;
	TPlayerTable playerTable;
} TPacketDGDirectEnter;

#ifndef _IMPROVED_PACKET_ENCRYPTION_
typedef struct SPacketShutdown
{
	bool state = false;
} TPacketShutdown;
#endif

typedef struct SPacketGuildSkillUpdate
{
	DWORD	guild_id;
	int		amount;
	BYTE	skill_levels[12];
	BYTE	skill_point;
	BYTE	save;
} TPacketGuildSkillUpdate;

typedef struct SPacketGuildExpUpdate
{
	DWORD	guild_id;
	int		amount;
} TPacketGuildExpUpdate;

typedef struct SPacketGuildChangeMemberData
{
	DWORD	guild_id;
	DWORD	pid;
	DWORD	offer;
	BYTE	level;
	BYTE	grade;
} TPacketGuildChangeMemberData;

typedef struct SPacketDGLoginAlready
{
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketDGLoginAlready;

typedef struct TPacketAffectElement
{
	DWORD	dwType;
	BYTE	bApplyOn;
	long	lApplyValue;
	DWORD	dwFlag;
	long	lDuration;
	long	lSPCost;
} TPacketAffectElement;

typedef struct SPacketGDAddAffect
{
	DWORD	dwPID;
	TPacketAffectElement	elem;
} TPacketGDAddAffect;

typedef struct SPacketGDRemoveAffect
{
	DWORD	dwPID;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGDRemoveAffect;

typedef struct SPacketGDHighscore
{
	DWORD	dwPID;
	long	lValue;
	char	cDir;
	char	szBoard[21];
} TPacketGDHighscore;

typedef struct SPacketPartyCreate
{
	DWORD	dwLeaderPID;
} TPacketPartyCreate;

typedef struct SPacketPartyDelete
{
	DWORD	dwLeaderPID;
} TPacketPartyDelete;

typedef struct SPacketPartyAdd
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bState;
} TPacketPartyAdd;

typedef struct SPacketPartyRemove
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
} TPacketPartyRemove;

typedef struct SPacketPartyStateChange
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bRole;
	BYTE	bFlag;
} TPacketPartyStateChange;

typedef struct SPacketPartySetMemberLevel
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bLevel;
} TPacketPartySetMemberLevel;

typedef struct SPacketGDBoot
{
    DWORD	dwItemIDRange[2];
	char	szIP[16];
} TPacketGDBoot;

typedef struct SPacketGuild
{
	DWORD	dwGuild;
	DWORD	dwInfo;
} TPacketGuild;

typedef struct SPacketGDGuildAddMember
{
	DWORD	dwPID;
	DWORD	dwGuild;
	BYTE	bGrade;
} TPacketGDGuildAddMember;

typedef struct SPacketDGGuildMember
{
	DWORD	dwPID;
	DWORD	dwGuild;
	BYTE	bGrade;
	BYTE	isGeneral;
	BYTE	bJob;
	BYTE	bLevel;
	DWORD	dwOffer;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGGuildMember;

typedef struct SPacketGuildWar
{
	BYTE		bType;
	BYTE		bWar;
	DWORD		dwGuildFrom;
	DWORD		dwGuildTo;
	long long	lWarPrice;
	long		lInitialScore;
} TPacketGuildWar;

typedef struct SPacketGuildWarScore
{
	DWORD	dwGuildGainPoint;
	DWORD	dwGuildOpponent;
	long	lScore;
	long	lBetScore;
} TPacketGuildWarScore;

typedef struct SRefineMaterial
{
	DWORD	vnum;
	int		count;
} TRefineMaterial;

typedef struct SRefineTable
{
	DWORD		id;
	BYTE		material_count;
	long long	cost;
	int			prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SPacketGDChangeName
{
	DWORD	pid;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGDChangeName;

typedef struct SPacketDGChangeName
{
	DWORD	pid;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGChangeName;

typedef struct SPacketGuildLadder
{
	DWORD dwGuild;
	long lLadderPoint;
	long lWin;
	long lDraw;
	long lLoss;
} TPacketGuildLadder;

typedef struct SPacketGuildLadderPoint
{
	DWORD dwGuild;
	long lChange;
} TPacketGuildLadderPoint;

typedef struct SPacketGuildUseSkill
{
	DWORD dwGuild;
	DWORD dwSkillVnum;
	DWORD dwCooltime;
} TPacketGuildUseSkill;

typedef struct SPacketGuildSkillUsableChange
{
	DWORD dwGuild;
	DWORD dwSkillVnum;
	BYTE bUsable;
} TPacketGuildSkillUsableChange;

typedef struct SPacketGDLoginKey
{
	DWORD dwAccountID;
	DWORD dwLoginKey;
} TPacketGDLoginKey;

typedef struct SPacketGDAuthLogin
{
	DWORD	dwID;
	DWORD	dwLoginKey;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szSocialID[SOCIAL_ID_MAX_LEN + 1];
	DWORD	adwClientKey[4];
	int		iPremiumTimes[PREMIUM_MAX_NUM];
} TPacketGDAuthLogin;

typedef struct SPacketGDLoginByKey
{
	char	szLogin[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
	char	szIP[MAX_HOST_LENGTH + 1];
} TPacketGDLoginByKey;

typedef struct SPacketGiveGuildPriv
{
	BYTE type;
	int value;
	DWORD guild_id;
	time_t duration_sec;
} TPacketGiveGuildPriv;
typedef struct SPacketGiveEmpirePriv
{
	BYTE type;
	int value;
	BYTE empire;
	time_t duration_sec;
} TPacketGiveEmpirePriv;
typedef struct SPacketGiveCharacterPriv
{
	BYTE type;
	int value;
	DWORD pid;
} TPacketGiveCharacterPriv;
typedef struct SPacketRemoveGuildPriv
{
	BYTE type;
	DWORD guild_id;
} TPacketRemoveGuildPriv;
typedef struct SPacketRemoveEmpirePriv
{
	BYTE type;
	BYTE empire;
} TPacketRemoveEmpirePriv;

typedef struct SPacketDGChangeCharacterPriv
{
	BYTE type;
	int value;
	DWORD pid;
	BYTE bLog;
} TPacketDGChangeCharacterPriv;

typedef struct SPacketDGChangeGuildPriv
{
	BYTE type;
	int value;
	DWORD guild_id;
	BYTE bLog;
	time_t end_time_sec;
} TPacketDGChangeGuildPriv;

typedef struct SPacketDGChangeEmpirePriv
{
	BYTE type;
	int value;
	BYTE empire;
	BYTE bLog;
	time_t end_time_sec;
} TPacketDGChangeEmpirePriv;

typedef struct SPacketGDGuildMoney
{
	DWORD dwGuild;
	long long lldGold;
} TPacketGDGuildMoney;

typedef struct SPacketDGGuildMoneyChange
{
	DWORD dwGuild;
	long long lldTotalGold;
} TPacketDGGuildMoneyChange;

typedef struct SPacketDGGuildMoneyWithdraw
{
	DWORD dwGuild;
	long long lldChangeGold;
} TPacketDGGuildMoneyWithdraw;

typedef struct SPacketGDGuildMoneyWithdrawGiveReply
{
	DWORD dwGuild;
	long long lldChangeGold;
	BYTE bGiveSuccess;
} TPacketGDGuildMoneyWithdrawGiveReply;

typedef struct SPacketSetEventFlag
{
	char	szFlagName[EVENT_FLAG_NAME_MAX_LEN + 1];
	long	lValue;
} TPacketSetEventFlag;

typedef struct SPacketLoginOnSetup
{
	DWORD   dwID;
	char    szLogin[LOGIN_MAX_LEN + 1];
	char    szSocialID[SOCIAL_ID_MAX_LEN + 1];
	char    szHost[MAX_HOST_LENGTH + 1];
	DWORD   dwLoginKey;
	DWORD   adwClientKey[4];
} TPacketLoginOnSetup;

typedef struct SPacketGDCreateObject
{
	DWORD	dwVnum;
	DWORD	dwLandID;
	INT		lMapIndex;
	INT	 	x, y;
	float	xRot;
	float	yRot;
	float	zRot;
} TPacketGDCreateObject;

typedef struct SGuildReserve
{
	DWORD       dwID;
	DWORD       dwGuildFrom;
	DWORD       dwGuildTo;
	DWORD       dwTime;
	BYTE        bType;
	long long	lWarPrice;
	long        lInitialScore;
	bool        bStarted;
	long long	dwBetFrom;
	long long	dwBetTo;
	long		lPowerFrom;
	long		lPowerTo;
	long		lHandicap;
} TGuildWarReserve;

typedef struct
{
	DWORD	dwWarID;
	char	szLogin[LOGIN_MAX_LEN + 1];
	long long	lldGold;
	DWORD	dwGuild;
} TPacketGDGuildWarBet;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	time_t tMarryTime;
	char szName1[CHARACTER_NAME_MAX_LEN + 1];
	char szName2[CHARACTER_NAME_MAX_LEN + 1];
} TPacketMarriageAdd;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	INT  iLovePoint;
	BYTE  byMarried;
} TPacketMarriageUpdate;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketMarriageRemove;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingRequest;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	DWORD dwMapIndex;
} TPacketWeddingReady;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingStart;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingEnd;


typedef struct SPacketMyshopPricelistHeader
{ 
	DWORD	dwOwnerID;
	WORD	wCount;
} TPacketMyshopPricelistHeader;

typedef struct SItemPriceInfo
{
	DWORD	dwVnum;
	long long	lldPrice;
} TItemPriceInfo;

typedef struct SItemPriceListTable
{
	DWORD	dwOwnerID;
	WORD	wCount;
	TItemPriceInfo	aPriceInfo[SHOP_PRICELIST_MAX_NUM];
} TItemPriceListTable;

typedef struct
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lDuration;
} TPacketBlockChat;

typedef struct TAdminInfo
{
	int m_ID;
	char m_szAccount[32];
	char m_szName[32];
	char m_szContactIP[16];
	char m_szServerIP[16];
	int m_Authority;
} tAdminInfo;

struct tLocale
{
	char szValue[32];
	char szKey[32];
};

typedef struct SPacketReloadAdmin
{
	char szIP[16];
} TPacketReloadAdmin;

typedef struct tChangeGuildMaster
{
	DWORD dwGuildID;
	DWORD idFrom;
	DWORD idTo;
} TPacketChangeGuildMaster;

typedef struct tItemIDRange
{
	DWORD dwMin;
	DWORD dwMax;
	DWORD dwUsableItemIDMin;
} TItemIDRangeTable;

typedef struct tUpdateHorseName
{
	DWORD dwPlayerID;
	char szHorseName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketUpdateHorseName;

typedef struct tDC
{
	char	login[LOGIN_MAX_LEN + 1];
} TPacketDC;

typedef struct tNeedLoginLogInfo
{
	DWORD dwPlayerID;
} TPacketNeedLoginLogInfo;

typedef struct tItemAwardInformer
{
	char	login[LOGIN_MAX_LEN + 1];
	char	command[20];
	unsigned int vnum;
} TPacketItemAwardInfromer;

typedef struct tDeleteAwardID
{
	DWORD dwID;
} TPacketDeleteAwardID;

typedef struct SChannelStatus
{
	short nPort;
	BYTE bStatus;
} TChannelStatus;

#ifdef ENABLE_SWITCHBOT
struct TSwitchbotAttributeAlternativeTable
{
	TPlayerItemAttribute attributes[MAX_NORM_ATTR_NUM];

	bool IsConfigured() const
	{
		for (const auto& it : attributes)
		{
			if (it.bType && it.sValue)
			{
				return true;
			}
		}

		return false;
	}
};

struct TSwitchbotTable
{
	DWORD player_id;
	bool active[SWITCHBOT_SLOT_COUNT];
	bool finished[SWITCHBOT_SLOT_COUNT];
	DWORD items[SWITCHBOT_SLOT_COUNT];
	TSwitchbotAttributeAlternativeTable alternatives[SWITCHBOT_SLOT_COUNT][SWITCHBOT_ALTERNATIVE_COUNT];

	TSwitchbotTable() : player_id(0)
	{
		memset(&items, 0, sizeof(items));
		memset(&alternatives, 0, sizeof(alternatives));
		memset(&active, false, sizeof(active));
		memset(&finished, false, sizeof(finished));
	}
};

struct TSwitchbottAttributeTable
{
	BYTE attribute_set;
	int apply_num;
	long max_value;
};
#endif

#ifdef ENABLE_ITEMSHOP
typedef struct SItemshopCategoryInfo
{
	char					category[ITEMSHOP_CATEGORY_MAX_LEN + 1];
	char					icon[ITEMSHOP_ICON_MAX_LEN + 1];
}TItemshopCategoryInfo;

typedef struct SItemshopCategoryTable
{
	BYTE					index;
	TItemshopCategoryInfo	info;
} TItemshopCategoryTable;

typedef struct SItemshopItemTable
{
	char					hash[ITEMSHOP_HASH_MAX_LEN + 1];
	unsigned long long		ullPrice;
	BYTE					byDiscountPercent;
	long long				llLimitCount;
	BYTE					byCategory;
	DWORD					dwVnum;
	WORD					wCount;
	time_t					end_time;
	long					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	BYTE					weight;
} TItemshopItemTable;

typedef struct SPromotionItemTable
{
	DWORD					dwVnum;
	WORD					wCount;
	long					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPromotionItemTable;

typedef struct SSpecialOfferTimes
{
	time_t					start_time;
	time_t					end_time;
	bool					is_activ;
} TSpecialOfferTimes;

typedef struct SSpecialOfferItems
{
	long long				llItemIndex;
	TSpecialOfferTimes		times;
	TItemshopItemTable		item;
} TSpecialOfferItems;

typedef struct SItemshopCheckBuy
{
	char					hash[ITEMSHOP_HASH_MAX_LEN + 1];
	WORD					wCount;
} TItemshopCheckBuy;

typedef struct SItemshopBuyAnswer
{
	bool		canBuy;
	WORD		wCount;
	char		code[ITEMSHOP_HASH_MAX_LEN + 1];
} TItemshopBuyAnswer;

typedef struct SPromotionRedeem
{
	DWORD					accID;
	char					code[PROMOTION_CODE_MAX_LEN + 1];
} TPromotionRedeem;

typedef struct SPromotionRedeemInfo
{
	BYTE		byRedeemAnswer;
	WORD		reward_count;
} TPromotionRedeemAnswer;

enum
{
	REDEEM_FAIL,
	REDEEM_SUCCESS,
	REDEEM_ALREADY_REDEEMED
};
#endif

#pragma pack()
