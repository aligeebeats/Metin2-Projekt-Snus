#pragma once
#include <boost/unordered_map.hpp>
#include "../../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#include "cube.h"
#include "mining.h"
#include <unordered_map>

class CBuffOnAttributes;
class CPetSystem;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
class CBuffNPCActor;
#endif

#define INSTANT_FLAG_DEATH_PENALTY	(1 << 0)
#define INSTANT_FLAG_SHOP			(1 << 1)
#define INSTANT_FLAG_EXCHANGE		(1 << 2)
#define INSTANT_FLAG_STUN			(1 << 3)
#define INSTANT_FLAG_NO_REWARD		(1 << 4)

#define AI_FLAG_NPC			(1 << 0)
#define AI_FLAG_AGGRESSIVE	(1 << 1)
#define AI_FLAG_HELPER		(1 << 2)
#define AI_FLAG_STAYZONE	(1 << 3)

#define SET_OVER_TIME(ch, time)	(ch)->SetOverTime(time)

extern int g_nPortalLimitTime;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
#ifdef ENABLE_WOLFMAN
	MAIN_RACE_WOLFMAN_M,
#endif
	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
#ifdef ENABLE_WOLFMAN
	BLEEDING_LENGTH = 30,
#endif
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
};

enum
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
	FLY_QUIVER_ATTACK_NORMAL,
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
#ifdef ENABLE_WOLFMAN
	DAMAGE_TYPE_BLEEDING,
#endif
};

enum EPointTypes
{
	POINT_NONE,
	POINT_LEVEL,
	POINT_VOICE,
	POINT_EXP,
	POINT_NEXT_EXP,
	POINT_HP,
	POINT_MAX_HP,
	POINT_SP,
	POINT_MAX_SP,
	POINT_STAMINA,
	POINT_MAX_STAMINA,
	POINT_GOLD,
	POINT_ST,
	POINT_HT,
	POINT_DX,
	POINT_IQ,
	POINT_DEF_GRADE,
	POINT_ATT_SPEED,
	POINT_ATT_GRADE,
	POINT_MOV_SPEED,
	POINT_CLIENT_DEF_GRADE,
	POINT_CASTING_SPEED,
	POINT_MAGIC_ATT_GRADE,
	POINT_MAGIC_DEF_GRADE,
	POINT_EMPIRE_POINT,
	POINT_LEVEL_STEP,
	POINT_STAT,
	POINT_SUB_SKILL,
	POINT_SKILL,
	POINT_WEAPON_MIN,
	POINT_WEAPON_MAX,
	POINT_PLAYTIME,
	POINT_HP_REGEN,
	POINT_SP_REGEN, 
	POINT_BOW_DISTANCE,
	POINT_HP_RECOVERY,
	POINT_SP_RECOVERY,
	POINT_POISON_PCT,
	POINT_STUN_PCT,
	POINT_SLOW_PCT,
	POINT_CRITICAL_PCT,
	POINT_PENETRATE_PCT,
	POINT_CURSE_PCT,
	POINT_ATTBONUS_HUMAN,
	POINT_ATTBONUS_ANIMAL,
	POINT_ATTBONUS_ORC,
	POINT_ATTBONUS_MILGYO,
	POINT_ATTBONUS_UNDEAD,
	POINT_ATTBONUS_DEVIL,
	POINT_ATTBONUS_INSECT,
	POINT_ENCHANT_FIRE,
	POINT_ENCHANT_ICE,
	POINT_ATTBONUS_DESERT,
	POINT_ATTBONUS_MONSTER,
	POINT_ATTBONUS_WARRIOR,
	POINT_ATTBONUS_ASSASSIN,
	POINT_ATTBONUS_SURA,
	POINT_ATTBONUS_SHAMAN,
	POINT_ATTBONUS_TREE,
	POINT_RESIST_WARRIOR,
	POINT_RESIST_ASSASSIN,
	POINT_RESIST_SURA,
	POINT_RESIST_SHAMAN,
	POINT_STEAL_HP,
	POINT_STEAL_SP,
	POINT_MANA_BURN_PCT,
	POINT_DAMAGE_SP_RECOVER,
	POINT_BLOCK,
	POINT_DODGE,
	POINT_RESIST_SWORD,
	POINT_RESIST_TWOHAND,
	POINT_RESIST_DAGGER,
	POINT_RESIST_BELL,
	POINT_RESIST_FAN,
	POINT_RESIST_BOW,
	POINT_RESIST_FIRE,
	POINT_RESIST_ELEC,
	POINT_RESIST_MAGIC,
	POINT_RESIST_WIND,
	POINT_REFLECT_MELEE,
	POINT_REFLECT_CURSE,
	POINT_POISON_REDUCE,
	POINT_KILL_SP_RECOVER,
	POINT_EXP_DOUBLE_BONUS,
	POINT_GOLD_DOUBLE_BONUS,
	POINT_ITEM_DROP_BONUS,
	POINT_POTION_BONUS,
	POINT_KILL_HP_RECOVERY,
	POINT_IMMUNE_STUN,
	POINT_IMMUNE_SLOW,
	POINT_IMMUNE_FALL,
	POINT_PARTY_ATTACKER_BONUS,
	POINT_PARTY_TANKER_BONUS,
	POINT_ATT_BONUS,
	POINT_DEF_BONUS,
	POINT_ATT_GRADE_BONUS,
	POINT_DEF_GRADE_BONUS,
	POINT_MAGIC_ATT_GRADE_BONUS,
	POINT_MAGIC_DEF_GRADE_BONUS,
	POINT_RESIST_NORMAL_DAMAGE,
	POINT_HIT_HP_RECOVERY,
	POINT_HIT_SP_RECOVERY,
	POINT_MANASHIELD,
	POINT_PARTY_BUFFER_BONUS,
	POINT_PARTY_SKILL_MASTER_BONUS,
	POINT_HP_RECOVER_CONTINUE,
	POINT_SP_RECOVER_CONTINUE,
	POINT_STEAL_GOLD,
	POINT_POLYMORPH,
	POINT_MOUNT,
	POINT_PARTY_HASTE_BONUS,
	POINT_PARTY_DEFENDER_BONUS,
	POINT_STAT_RESET_COUNT,
	POINT_HORSE_SKILL,
	POINT_MALL_ATTBONUS,
	POINT_MALL_DEFBONUS,
	POINT_MALL_EXPBONUS,
	POINT_MALL_ITEMBONUS,
	POINT_MALL_GOLDBONUS,
	POINT_MAX_HP_PCT,
	POINT_MAX_SP_PCT,
	POINT_SKILL_DAMAGE_BONUS,
	POINT_NORMAL_HIT_DAMAGE_BONUS,
	POINT_SKILL_DEFEND_BONUS,
	POINT_NORMAL_HIT_DEFEND_BONUS,
	POINT_PC_BANG_EXP_BONUS,
	POINT_PC_BANG_DROP_BONUS,
	POINT_RAMADAN_CANDY_BONUS_EXP,
	POINT_ENERGY,
	POINT_ENERGY_END_TIME,
	POINT_COSTUME_ATTR_BONUS,
	POINT_MAGIC_ATT_BONUS_PER,
	POINT_MELEE_MAGIC_ATT_BONUS_PER,
	POINT_RESIST_ICE,
	POINT_RESIST_EARTH,
	POINT_RESIST_DARK,
	POINT_RESIST_CRITICAL,
	POINT_RESIST_PENETRATE,
#ifdef ENABLE_WOLFMAN
	POINT_BLEEDING_REDUCE,
	POINT_BLEEDING_PCT,
	POINT_ATTBONUS_WOLFMAN,
	POINT_RESIST_WOLFMAN,
	POINT_RESIST_CLAW,
#endif
	POINT_RESIST_MAGIC_REDUCTION,
	POINT_RESIST_HUMAN,
	POINT_ENCHANT_ELEC,
	POINT_ENCHANT_WIND,
	POINT_ENCHANT_EARTH,
	POINT_ENCHANT_DARK,
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE			= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER			= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
};

enum EPickupModes
{
	PICKUP_MODE_SINGLE,
	PICKUP_MODE_ALL,
};

enum EPickupBlock
{
	PICKUP_BLOCK_WEAPON = (1 << 0),
	PICKUP_BLOCK_ARMOR	= (1 << 1),
	PICKUP_BLOCK_HEAD	= (1 << 2),
	PICKUP_BLOCK_SHIELD	= (1 << 3),
	PICKUP_BLOCK_WRIST	= (1 << 4),
	PICKUP_BLOCK_FOOTS	= (1 << 5),
	PICKUP_BLOCK_NECK	= (1 << 6),
	PICKUP_BLOCK_EAR	= (1 << 7),
	PICKUP_BLOCK_ETC	= (1 << 8),
};

struct DynamicCharacterPtr {
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o)
		: is_pc(o.is_pc), id(o.id) {}

	LPCHARACTER Get() const; 

	void Reset() {
		is_pc = false;
		id = 0;
	}

	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs) {
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}

	DynamicCharacterPtr& operator=(LPCHARACTER character);
	operator LPCHARACTER() const {
		return Get();
	}

	bool is_pc;
	uint32_t id;
};

typedef struct character_point
{
	long long	points[POINT_MAX_NUM];
	BYTE		job;
	BYTE		voice;
	BYTE		level;
	DWORD		exp;
	long long	gold;
	int			hp;
	int			sp;
	int			iRandomHP;
	int			iRandomSP;
	int			stamina;
	BYTE		skill_group;
} CHARACTER_POINT;

typedef struct character_point_instant
{
	long long		points[POINT_MAX_NUM];
	float			fRot;
	int				iMaxHP;
	int				iMaxSP;
	long			position;
	long			instant_flag;
	DWORD			dwAIFlag;
	DWORD			dwImmuneFlag;
	DWORD			dwLastShoutPulse;
	DWORD			parts[PART_MAX_NUM];
	LPITEM			pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
	WORD			wItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
	LPITEM			pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
	WORD			wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];
#ifdef ENABLE_SWITCHBOT
	LPITEM			pSwitchbotItems[SWITCHBOT_SLOT_COUNT];
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	LPITEM 			pBuffEquipmentItem[BUFF_WINDOW_SLOT_MAX_NUM];
#endif
	WORD			pAcceSlots[ACCE_SLOT_MAX_NUM];
	LPITEM			pCubeItems[CUBE_MAX_NUM];
	LPCHARACTER		pCubeNpc;
	LPCHARACTER		battle_victim;
	BYTE			gm_level;
	BYTE			bBasePart;
	int				iMaxStamina;
	BYTE			bBlockMode;
	BYTE			bPickupMode;
	WORD			wPickupBlockFlag;
	int				iDragonSoulActiveDeck;
	LPENTITY		m_pDragonSoulRefineWindowOpener;
} CHARACTER_POINT_INSTANT;

#define TRIGGERPARAM		LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE	type;
	int		(*func) (TRIGGERPARAM);
	long	value;
} TRIGGER;

class CTrigger
{
	public:
		CTrigger() : bType(0), pFunc(NULL)
		{
		}

		BYTE	bType;
		int	(*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

struct TSkillUseInfo
{
	int	    iHitCount;
	int	    iMaxHitCount;
	int	    iSplashCount;
	DWORD   dwNextSkillUsableTime;
	int	    iRange;
	bool    bUsed;
	bool    bSkillCD;
	DWORD   dwHitCount;
	DWORD   dwVID;
	bool    isGrandMaster;

	boost::unordered_map<VID, size_t> TargetVIDMap;

	TSkillUseInfo()
		: iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false), bSkillCD(false), dwHitCount(0),
		dwVID(0), isGrandMaster(false)
   	{}

	bool    HitOnce(DWORD dwVnum = 0);
	bool    IsSkillCooldown(DWORD dwVnum, float fSkillPower);
	bool    UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD   GetMainTargetVID() const	{ return dwVID; }
	void    SetMainTargetVID(DWORD vid) { dwVID=vid; }
	void    ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CShop;
typedef class CShop * LPSHOP;
class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);

namespace marriage
{
	class WeddingMap;
}
enum e_overtime
{
	OT_NONE,
	OT_3HOUR,
	OT_5HOUR,
};

class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
	protected:
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);

	public:
		LPCHARACTER		FindCharacterInView(const char * name, bool bFindPCOnly);
		void			UpdatePacket();

	protected:
		CStateTemplate<CHARACTER>	m_stateMove;
		CStateTemplate<CHARACTER>	m_stateBattle;
		CStateTemplate<CHARACTER>	m_stateIdle;

	public:
		virtual void	StateMove();
		virtual void	StateBattle();
		virtual void	StateIdle();
		virtual void	StateFlag();
		virtual void	StateFlagBase();
		void			StateHorse();

	protected:
		void			__StateIdle_Monster();
		void			__StateIdle_Stone();
		void			__StateIdle_NPC();

	public:
		DWORD GetAIFlag() const	{ return m_pointsInstant.dwAIFlag; }
		void			SetAggressive();
		bool			IsAggressive() const;
		void			SetCoward();
		bool			IsCoward() const;
		void			CowardEscape();
		void			SetNoAttackShinsu();
		bool			IsNoAttackShinsu() const;
		void			SetNoAttackChunjo();
		bool			IsNoAttackChunjo() const;
		void			SetNoAttackJinno();
		bool			IsNoAttackJinno() const;
		void			SetAttackMob();
		bool			IsAttackMob() const;
		virtual void	BeginStateEmpty();
		virtual void	EndStateEmpty() {}
		void			RestartAtSamePos();

	protected:
		DWORD				m_dwStateDuration;

	public:
		CHARACTER();
		virtual ~CHARACTER();

		void			Create(const char * c_pszName, DWORD vid, bool isPC);
		void			Destroy();
		void			Disconnect(const char * c_pszReason);

	protected:
		void			Initialize();

	public:
		DWORD			GetPlayerID() const	{ return m_dwPlayerID; }
		void			SetPlayerProto(const TPlayerTable * table);
		void			CreatePlayerProto(TPlayerTable & tab);
		void			SetProto(const CMob * c_pkMob);
		WORD			GetRaceNum() const;
		WORD			GetPlayerRace() const;
		void			Save();
		void			SaveReal();
		void			FlushDelayedSaveItem();
		const char*		GetName() const;
		const VID &		GetVID() const		{ return m_vid;		}
		void			SetName(const std::string& name) { m_stName = name; }
		void			SetRace(BYTE race);
		bool			ChangeSex();
		DWORD			GetAID() const;
		int				GetChangeEmpireCount() const;
		void			SetChangeEmpireCount();
		int				ChangeEmpire(BYTE empire);
		BYTE			GetJob() const;
		BYTE			GetCharType() const;
		bool			IsPC() const		{ return GetDesc() ? true : false; }
		bool			IsNPC()	const		{ return m_bCharType != CHAR_TYPE_PC; }
		bool			IsMonster()	const	{ return m_bCharType == CHAR_TYPE_MONSTER; }
		bool			IsStone() const		{ return m_bCharType == CHAR_TYPE_STONE; }
		bool			IsDoor() const		{ return m_bCharType == CHAR_TYPE_DOOR; } 
		bool			IsBuilding() const	{ return m_bCharType == CHAR_TYPE_BUILDING;  }
		bool			IsWarp() const		{ return m_bCharType == CHAR_TYPE_WARP; }
		bool			IsGoto() const		{ return m_bCharType == CHAR_TYPE_GOTO; }
		DWORD			GetLastShoutPulse() const	{ return m_pointsInstant.dwLastShoutPulse; }
		void			SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
		int				GetLevel() const		{ return m_points.level;	}
		void			SetLevel(BYTE level);
		BYTE			GetGMLevel() const;
		BOOL 			IsGM() const;
		void			SetGMLevel(); 
		DWORD			GetExp() const		{ return m_points.exp;	}
		void			SetExp(DWORD exp)	{ m_points.exp = exp;	}
		DWORD			GetNextExp() const;
		LPCHARACTER		DistributeExp();
		void			DistributeHP(LPCHARACTER pkKiller);
		void			DistributeSP(LPCHARACTER pkKiller, int iMethod=0);
		void			SetPosition(int pos);
		bool			IsPosition(int pos) const	{ return m_pointsInstant.position == pos ? true : false; }
		int				GetPosition() const		{ return m_pointsInstant.position; }
		void			SetPart(BYTE bPartPos, DWORD dwVal);
		DWORD			GetPart(BYTE bPartPos) const;
		DWORD			GetOriginalPart(BYTE bPartPos) const;
		void			SetHP(int hp)		{ m_points.hp = hp; }
		int				GetHP() const		{ return m_points.hp; }
		void			SetSP(int sp)		{ m_points.sp = sp; }
		int				GetSP() const		{ return m_points.sp; }
		void			SetStamina(int stamina)	{ m_points.stamina = stamina; }
		int				GetStamina() const		{ return m_points.stamina; }
		void			SetMaxHP(int iVal)	{ m_pointsInstant.iMaxHP = iVal; }
		int				GetMaxHP() const	{ return m_pointsInstant.iMaxHP; }
		void			SetMaxSP(int iVal)	{ m_pointsInstant.iMaxSP = iVal; }
		int				GetMaxSP() const	{ return m_pointsInstant.iMaxSP; }
		void			SetMaxStamina(int iVal)	{ m_pointsInstant.iMaxStamina = iVal; }
		int				GetMaxStamina() const	{ return m_pointsInstant.iMaxStamina; }
		void			SetRandomHP(int v)	{ m_points.iRandomHP = v; }
		void			SetRandomSP(int v)	{ m_points.iRandomSP = v; }
		int				GetRandomHP() const	{ return m_points.iRandomHP; }
		int				GetRandomSP() const	{ return m_points.iRandomSP; }
		int				GetHPPct() const;
		void			SetRealPoint(BYTE idx, int val);
		int				GetRealPoint(BYTE idx) const;
		void			SetPoint(WORD idx, long long val);
		long long		GetPoint(WORD idx) const;
		int				GetLimitPoint(WORD idx) const;
		int				GetPolymorphPoint(BYTE idx) const;
		const TMobTable &GetMobTable() const;
		BYTE			GetMobRank() const;
		BYTE			GetMobBattleType() const;
		BYTE			GetMobSize() const;
		DWORD			GetMobDamageMin() const;
		DWORD			GetMobDamageMax() const;
		WORD			GetMobAttackRange() const;
		DWORD			GetMobDropItemVnum() const;
		float			GetMobDamageMultiply() const;
		bool			IsBerserker() const;
		bool			IsBerserk() const;
		void			SetBerserk(bool mode);
		bool			IsStoneSkinner() const;
		bool			IsGodSpeeder() const;
		bool			IsGodSpeed() const;
		void			SetGodSpeed(bool mode);
		bool			IsDeathBlower() const;
		bool			IsDeathBlow() const;
		bool			IsReviver() const;
		bool			HasReviverInParty() const;
		bool			IsRevive() const;
		void			SetRevive(bool mode);
		bool			IsRaceFlag(DWORD dwBit) const;
		bool			IsSummonMonster() const;
		DWORD			GetSummonVnum() const;
		DWORD			GetPolymorphItemVnum() const;
		DWORD			GetMonsterDrainSPPoint() const;
		void			MainCharacterPacket();
		void			ComputePoints();
		void			ComputeBattlePoints();
		void			PointChange(BYTE type, long long amount, bool bAmount = false, bool bBroadcast = false);
		void			PointsPacket();
		void			ApplyPoint(BYTE bApplyType, int iVal);
		void			CheckMaximumPoints();
		bool			Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false);
		void			Sitdown(int is_ground);
		void			Standup();
		void			SetRotation(float fRot);
		void			SetRotationToXY(long x, long y);
		float			GetRotation() const	{ return m_pointsInstant.fRot; }
		void			MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet);
		void			Motion(BYTE motion, LPCHARACTER victim = NULL);
		void			ChatPacket(BYTE type, const char *format, ...);
		void			ResetPoint(int iLv);
		void			SetBlockMode(BYTE bFlag);
		void			SetBlockModeForce(BYTE bFlag);
		bool			IsBlockMode(BYTE bFlag) const	{ return (m_pointsInstant.bBlockMode & bFlag)?true:false; }
		void			LoadPickup();
		void			SetPickupMode(BYTE bMode);
		BYTE			GetPickupMode() const { return m_pointsInstant.bPickupMode; };
		void			SetPickupBlockFlag(WORD wFlag);
		bool			IsPickupBlocked(WORD wFlag) const { return (m_pointsInstant.wPickupBlockFlag & wFlag) ? true : false; }
		bool			IsPickupBlockedItem(DWORD dwVnum);
		bool			IsPolymorphed() const		{ return m_dwPolymorphRace>0; }
		bool			IsPolyMaintainStat() const	{ return m_bPolyMaintainStat; }
		void			SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
		DWORD			GetPolymorphVnum() const	{ return m_dwPolymorphRace; }
		int				GetPolymorphPower() const;
		void			fishing();
		void			fishing_take();
		void			mining(LPCHARACTER chLoad);
		void			mining_cancel();
		void			mining_take();
		void			ResetPlayTime(DWORD dwTimeRemain = 0);
		void			CreateFly(BYTE bType, LPCHARACTER pkVictim);
		void			ResetChatCounter();
		BYTE			IncreaseChatCounter();
		BYTE			GetChatCounter() const;
		BYTE			GetSkillGradeByLevel(BYTE bLevel);

	protected:
		DWORD			m_dwPolymorphRace;
		bool			m_bPolyMaintainStat;
		DWORD			m_dwLoginPlayTime;
		DWORD			m_dwPlayerID;
		VID				m_vid;
		std::string		m_stName;
		BYTE			m_bCharType;
		CHARACTER_POINT	m_points;
		CHARACTER_POINT_INSTANT	m_pointsInstant;
		int				m_iMoveCount;
		DWORD			m_dwPlayStartTime;
		BYTE			m_bAddChrState;
		bool			m_bSkipSave;
		BYTE			m_bChatCounter;

	public:
		bool			IsStateMove() const			{ return IsState((CState&)m_stateMove); }
		bool			IsStateIdle() const			{ return IsState((CState&)m_stateIdle); }
		bool			IsWalking() const			{ return m_bNowWalking || GetStamina()<=0; }
		void			SetWalking(bool bWalkFlag)	{ m_bWalking=bWalkFlag; }
		void			SetNowWalking(bool bWalkFlag);	
		void			ResetWalking()			{ SetNowWalking(m_bWalking); }
		bool			Goto(long x, long y);
		void			Stop();
		bool			CanMove() const;
		void			SyncPacket();
		bool			Sync(long x, long y);
		bool			Move(long x, long y);
		void			OnMove(bool bIsAttack = false);
		DWORD			GetMotionMode() const;
		float			GetMoveMotionSpeed() const;
		float			GetMoveSpeed() const;
		void			CalculateMoveDuration();
		void			SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime=0, int iRot=-1);
		DWORD			GetCurrentMoveDuration() const	{ return m_dwMoveDuration; }
		DWORD			GetWalkStartTime() const	{ return m_dwWalkStartTime; }
		DWORD			GetLastMoveTime() const		{ return m_dwLastMoveTime; }
		DWORD			GetLastAttackTime() const	{ return m_dwLastAttackTime; }
		void			SetLastAttacked(DWORD time);
		bool			SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
		bool			IsSyncOwner(LPCHARACTER ch) const;
		bool			WarpSet(long x, long y, long lRealMapIndex = 0);
		void			SetWarpLocation(long lMapIndex, long x, long y);
		void			WarpEnd();
		const PIXEL_POSITION & GetWarpPosition() const { return m_posWarp; }
		bool			WarpToPID(DWORD dwPID);
		void			SaveExitLocation();
		void			ExitToSavedLocation();
		void			StartStaminaConsume();
		void			StopStaminaConsume();
		bool			IsStaminaConsume() const;
		bool			IsStaminaHalfConsume() const;
		void			ResetStopTime();
		DWORD			GetStopTime() const;

	protected:
		void			ClearSync();
		float			m_fSyncTime;
		LPCHARACTER		m_pkChrSyncOwner;
		CHARACTER_LIST	m_kLst_pkChrSyncOwned;
		PIXEL_POSITION	m_posDest;
		PIXEL_POSITION	m_posStart;
		PIXEL_POSITION	m_posWarp;
		long			m_lWarpMapIndex;
		PIXEL_POSITION	m_posExit;
		long			m_lExitMapIndex;
		DWORD			m_dwMoveStartTime;
		DWORD			m_dwMoveDuration;
		DWORD			m_dwLastMoveTime;
		DWORD			m_dwLastAttackTime;
		DWORD			m_dwWalkStartTime;
		DWORD			m_dwStopTime;
		bool			m_bWalking;
		bool			m_bNowWalking;
		bool			m_bStaminaConsume;

	public:
		void			SyncQuickslot(BYTE bType, BYTE bOldPos, BYTE bNewPos);
		bool			GetQuickslot(BYTE pos, TQuickslot ** ppSlot);
		bool			SetQuickslot(BYTE pos, TQuickslot & rSlot);
		bool			DelQuickslot(BYTE pos);
		bool			SwapQuickslot(BYTE a, BYTE b);
		void			ChainQuickslotItem(LPITEM pItem, BYTE bType, BYTE bOldPos);

	protected:
		TQuickslot		m_quickslot[QUICKSLOT_MAX_NUM];

	public:
		void			StartAffectEvent();
		void			ClearAffect(bool bSave=false);
		void			ComputeAffect(CAffect * pkAff, bool bAdd);
		bool			AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube = false);
		void			RefreshAffect();
		bool			RemoveAffect(DWORD dwType);
		bool			IsAffectFlag(DWORD dwAff) const;
		bool			UpdateAffect();
		int				ProcessAffect();
		void			LoadAffect(DWORD dwCount, TPacketAffectElement * pElements);
		void			SaveAffect();
		bool			IsLoadedAffect() const	{ return m_bIsLoadedAffect; }		
		bool			IsGoodAffect(BYTE bAffectType) const;
		void			RemoveGoodAffect();
		void			RemoveBadAffect();
		CAffect *		FindAffect(DWORD dwType, BYTE bApply=APPLY_NONE) const;
		const std::list<CAffect *> & GetAffectContainer() const	{ return m_list_pkAffect; }
		bool			RemoveAffect(CAffect * pkAff);

	protected:
		bool			m_bIsLoadedAffect;
		TAffectFlag		m_afAffectFlag;
		std::list<CAffect *>	m_list_pkAffect;

	public:
		void			SetParty(LPPARTY pkParty);
		LPPARTY			GetParty() const	{ return m_pkParty; }

		bool			RequestToParty(LPCHARACTER leader);
		void			DenyToParty(LPCHARACTER member);
		void			AcceptToParty(LPCHARACTER member);
		void			PartyInvite(LPCHARACTER pchInvitee);
		void			PartyInviteAccept(LPCHARACTER pchInvitee);
		void			PartyInviteDeny(DWORD dwPID);
		bool			BuildUpdatePartyPacket(TPacketGCPartyUpdate & out);
		int				GetLeadershipSkillLevel() const;
		bool			CanSummon(int iLeaderShip);
		void			SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

	protected:
		void			PartyJoin(LPCHARACTER pkLeader);

		enum PartyJoinErrCode 
		{
			PERR_NONE		= 0,
			PERR_SERVER,
			PERR_DUNGEON,
			PERR_OBSERVER,
			PERR_LVBOUNDARY,
			PERR_LOWLEVEL,
			PERR_HILEVEL,
			PERR_ALREADYJOIN,
			PERR_PARTYISFULL,
			PERR_SEPARATOR,
			PERR_DIFFEMPIRE,
			PERR_MAX
		};

		static PartyJoinErrCode	IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);
		static PartyJoinErrCode	IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);
		LPPARTY			m_pkParty;
		DWORD			m_dwLastDeadTime;
		LPEVENT			m_pkPartyRequestEvent;

		typedef std::map< DWORD, LPEVENT >	EventMap;
		EventMap		m_PartyInviteEventMap;

	public:
		void			SetDungeon(LPDUNGEON pkDungeon);
		LPDUNGEON		GetDungeon() const	{ return m_pkDungeon; }
		LPDUNGEON		GetDungeonForce() const;
	protected:
		LPDUNGEON	m_pkDungeon;
		int			m_iEventAttr;

	public:
		void			SetGuild(CGuild * pGuild);
		CGuild*			GetGuild() const	{ return m_pGuild; }
		void			SetWarMap(CWarMap* pWarMap);
		CWarMap*		GetWarMap() const	{ return m_pWarMap; }

	protected:
		CGuild *		m_pGuild;
		DWORD			m_dwUnderGuildWarInfoMessageTime;
		CWarMap *		m_pWarMap;

	public:
		bool			CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false);
		bool			IsItemLoaded() const	{ return m_bItemLoaded; }
		void			SetItemLoaded()	{ m_bItemLoaded = true; }
		void			ClearItem();
#ifdef HIGHLIGHT_SLOT
		void			SetItem(TItemPos Cell, LPITEM item, bool bWereMine = false);
#else
		void			SetItem(TItemPos Cell, LPITEM item);
#endif
		LPITEM			GetItem(TItemPos Cell) const;
		LPITEM			GetInventoryItem(WORD wCell) const;
		bool			IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;
#ifdef ENABLE_SPECIAL_INVENTORY
		bool			IsEmptySpecialItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;
#endif	
		void			SetWear(WORD wCell, LPITEM item);
		LPITEM			GetWear(WORD wCell) const;
		void			UseSilkBotary(void);
		void			UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);
		bool			UseItemEx(LPITEM item, TItemPos DestCell);
		bool			UseItem(TItemPos Cell, TItemPos DestCell = NPOS);
		bool			IsRefineThroughGuild() const;
		CGuild *		GetRefineGuild() const;
		long long		ComputeRefineFee(long long lldCost, int iMultiply = 5) const;
		void			PayRefineFee(long long lldTotalMoney);
		void			SetRefineNPC(LPCHARACTER ch);
		bool			RefineItem(LPITEM pkItem, LPITEM pkTarget);
		bool			DestroyItem(TItemPos Cell);
		bool			DropItem(TItemPos Cell,  WORD wCount=0);
		bool			GiveRecallItem(LPITEM item);
		void			ProcessRecallItem(LPITEM item);
		void			EffectPacket(int enumEffectType);
		void			SpecificEffectPacket(const char filename[128]);
		bool			DoRefine(LPITEM item, bool bMoneyOnly = false);
		bool			DoRefineWithScroll(LPITEM item);
		bool			RefineInformation(WORD wCell, BYTE bType, int iAdditionalCell = -1);
		void			SetRefineMode(int iAdditionalCell = -1);
		void			ClearRefineMode();
		bool			GiveItem(LPCHARACTER victim, TItemPos Cell);
		bool			CanReceiveItem(LPCHARACTER from, LPITEM item) const;
		void			ReceiveItem(LPCHARACTER from, LPITEM item);
		bool			GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector <DWORD> &dwItemVnums, 
							std::vector <DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count);

		bool			MoveItem(TItemPos pos, TItemPos change_pos, WORD num);
		bool			PickupItem(LPITEM item, DWORD vid = 0);
		bool			EquipItem(LPITEM item, int iCandidateCell = -1);
		bool			UnequipItem(LPITEM item);
		bool			CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);
		bool			CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS, bool checkCostume = true);
		void			AcceRefineCheckin(BYTE acceSlot, TItemPos currentCell);
		void			AcceRefineCheckout(BYTE acceSlot);
		void			AcceRefineAccept(int windowType);
		void			AcceRefineCancel();
		void			AcceClose();
		bool			SwapItem(WORD wCell, WORD wDestCell);
		LPITEM			AutoGiveItem(DWORD dwItemVnum, WORD wCount=1, int iRarePct = -1, bool bMsg = true);
		void			AutoGiveItem(LPITEM item, bool longOwnerShip = false);
#ifdef ENABLE_SPECIAL_INVENTORY
		int				GetEmptyInventory(LPITEM item) const;
#else
		int				GetEmptyInventory(BYTE size) const;
#endif
		int				GetEmptyDragonSoulInventory(LPITEM pItem) const;
		void			CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;
		int				CountEmptyInventory() const;
		int				CountSpecifyItem(DWORD vnum, int iExceptionCell = -1) const;
		void			RemoveSpecifyItem(DWORD vnum, DWORD count = 1, int iExceptionCell = -1);
		LPITEM			FindSpecifyItem(DWORD vnum) const;
		LPITEM			FindItemByID(DWORD id) const;
		int				CountSpecifyTypeItem(BYTE type) const;
		void			RemoveSpecifyTypeItem(BYTE type, DWORD count = 1);
		bool			IsEquipUniqueItem(DWORD dwItemVnum) const;
		bool			IsEquipUniqueGroup(DWORD dwGroupVnum) const;
		void			SendEquipment(LPCHARACTER ch);

	protected:
		void			SendMyShopPriceListCmd(DWORD dwItemVnum, long long dwItemPrice);
		bool			m_bNoOpenedShop;
		bool			m_bItemLoaded;
		int				m_iRefineAdditionalCell;
		bool			m_bUnderRefine;
		DWORD			m_dwRefineNPCVID;

	public:
		long long		GetGold() const		{ return m_points.gold;	}
		void			SetGold(long long gold)	{ m_points.gold = gold;	}
		bool			DropGold(long long gold);
		long long		GetAllowedGold() const;
		void			GiveGold(long long lldRemain);

	public:
		void			SetShop(LPSHOP pkShop);
		LPSHOP			GetShop() const { return m_pkShop; }
		void			ShopPacket(BYTE bSubHeader);
		void			SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
		LPCHARACTER		GetShopOwner() const { return m_pkChrShopOwner;}
		void			OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, WORD wItemCount);
		LPSHOP			GetMyShop() const { return m_pkMyShop; }
		void			CloseMyShop();

	protected:
		LPSHOP			m_pkShop;
		LPSHOP			m_pkMyShop;
		std::string		m_stShopSign;
		LPCHARACTER		m_pkChrShopOwner;

	public:
		bool			ExchangeStart(LPCHARACTER victim);
		void			SetExchange(CExchange * pkExchange);
		CExchange *		GetExchange() const	{ return m_pkExchange;	}

	protected:
		CExchange *		m_pkExchange;

	public:
		struct TBattleInfo
		{
			int iTotalDamage;
			int iAggro;

			TBattleInfo(int iTot, int iAggr)
				: iTotalDamage(iTot), iAggro(iAggr)
				{}
		};
		typedef std::map<VID, TBattleInfo>	TDamageMap;

		typedef struct SAttackLog
		{
			DWORD	dwVID;
			DWORD	dwTime;
		} AttackLog;

		bool				Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		bool				__Profile__Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		void				DeathPenalty(BYTE bExpLossPercent);
		void				ReviveInvisible(int iDur);
		bool				Attack(LPCHARACTER pkVictim, BYTE bType = 0);
		bool				IsAlive() const		{ return m_pointsInstant.position == POS_DEAD ? false : true; }
		bool				CanFight() const;
		bool				CanBeginFight() const;
		void				BeginFight(LPCHARACTER pkVictim);
		bool				CounterAttack(LPCHARACTER pkChr);
		bool				IsStun() const;
		void				Stun();
		bool				IsDead() const;
		void				Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead=false);
		void				Reward(bool bItemDrop);
		void				RewardGold(LPCHARACTER pkAttacker);
		bool				Shoot(BYTE bType);
		void				FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader);
		void				ForgetMyAttacker();
		void				AggregateMonster();
		void				AttractRanger();
		void				PullMonster();
		int					GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount = 1);
		void				UseArrow(LPITEM pkArrow, DWORD dwArrowCount);
		void				AttackedByPoison(LPCHARACTER pkAttacker);
		void				RemovePoison();
#ifdef ENABLE_WOLFMAN
		void				AttackedByBleeding(LPCHARACTER pkAttacker);
		void				RemoveBleeding();
#endif
		void				AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
		void				RemoveFire();
		void				UpdateAlignment(int iAmount);
		int					GetAlignment() const;
		int					GetRealAlignment() const;
		void				ShowAlignment(bool bShow);
		void				SetKillerMode(bool bOn);
		bool				IsKillerMode() const;
		void				UpdateKillerMode();
		BYTE				GetPKMode() const;
		void				SetPKMode(BYTE bPKMode);
		void				ItemDropPenalty(LPCHARACTER pkKiller);
		void				UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int dam);

	public:
		void SetComboSequence(BYTE seq);
		BYTE GetComboSequence() const;
		void SetLastComboTime(DWORD time);
		DWORD GetLastComboTime() const;
		int GetValidComboInterval() const;
		void SetValidComboInterval(int interval);
		BYTE GetComboIndex() const;
		void IncreaseComboHackCount(int k = 1);
		void ResetComboHackCount();
		void SkipComboAttackByTime(int interval);
		DWORD GetSkipComboAttackByTime() const;

	protected:
		BYTE m_bComboSequence;
		DWORD m_dwLastComboTime;
		int m_iValidComboInterval;
		BYTE m_bComboIndex;
		int m_iComboHackCount;
		DWORD m_dwSkipComboAttackByTime;

	protected:
		void				UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int dam, TBattleInfo & info);
		void				ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);
		DWORD				m_dwFlyTargetID;
		std::vector<DWORD>	m_vec_dwFlyTargets;
		TDamageMap			m_map_kDamage;
		DWORD				m_dwKillerPID;
		int					m_iAlignment;
		int					m_iRealAlignment;
		int					m_iKillerModePulse;
		BYTE				m_bPKMode;
		DWORD				m_dwLastVictimSetTime;
		int					m_iMaxAggro;

	public:
		void				SetStone(LPCHARACTER pkChrStone);
		void				ClearStone();
		void				DetermineDropMetinStone();
		DWORD				GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
		BYTE				GetDropMetinStonePct() const { return m_bDropMetinStonePct; }

	protected:
		LPCHARACTER			m_pkChrStone;
		CHARACTER_SET		m_set_pkChrSpawnedBy;
		DWORD				m_dwDropMetinStone;
		BYTE				m_bDropMetinStonePct;

	public:
		enum
		{
			SKILL_UP_BY_POINT,
			SKILL_UP_BY_BOOK,
			SKILL_UP_BY_TRAIN,
			SKILL_UP_BY_QUEST,
		};

		void				SkillLevelPacket();
		void				SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
		bool				SkillLevelDown(DWORD dwVnum);
		bool				UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
		bool                IsSkillCooldown(DWORD dwVnum, float fSkillPower) { return m_SkillUseInfo[dwVnum].IsSkillCooldown(dwVnum, fSkillPower) ? true : false; }
		void				ResetSkill();
		void				SetSkillLevel(DWORD dwVnum, BYTE bLev);
		int					GetUsedSkillMasterType(DWORD dwVnum);
		bool				IsLearnableSkill(DWORD dwSkillVnum) const;
		bool				CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
		bool				CanUseSkill(DWORD dwSkillVnum) const;
		bool				IsUsableSkillMotion(DWORD dwMotionIndex) const;
		int					GetSkillLevel(DWORD dwVnum) const;
		int					GetSkillMasterType(DWORD dwVnum) const;
		int					GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;
		time_t				GetSkillNextReadTime(DWORD dwVnum) const;
		void				SetSkillNextReadTime(DWORD dwVnum, time_t time);
		void				SkillLearnWaitMoreTimeMessage(DWORD dwVnum);
		void				ComputePassiveSkill(DWORD dwVnum);
		int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
		int					ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
		void				SetSkillGroup(BYTE bSkillGroup); 
		BYTE				GetSkillGroup() const		{ return m_points.skill_group; }
		int					ComputeCooltime(int time);
		void				GiveRandomSkillBook();
		void				DisableCooltime();
		bool				LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
		bool				LearnGrandMasterSkill(DWORD dwSkillVnum);

	private:
		bool				m_bDisableCooltime;
		DWORD				m_dwLastSkillTime;

	public:
		bool				HasMobSkill() const;
		size_t				CountMobSkill() const;
		const TMobSkillInfo * GetMobSkill(unsigned int idx) const;
		bool				CanUseMobSkill(unsigned int idx) const;
		bool				UseMobSkill(unsigned int idx);
		void				ResetMobSkillCooltime();
	protected:
		DWORD				m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];

	public:
		void				StartMuyeongEvent();
		void				StopMuyeongEvent();

	private:
		LPEVENT				m_pkMuyeongEvent;

	public:
		int					GetChainLightningIndex() const { return m_iChainLightingIndex; }
		void				IncChainLightningIndex() { ++m_iChainLightingIndex; }
		void				AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.insert(ch); }
		void				ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
		int					GetChainLightningMaxCount() const;
		const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

	private:
		int					m_iChainLightingIndex;
		CHARACTER_SET m_setExceptChainLighting;

	public:
		void				SetAffectedEunhyung();
		void				ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
		bool				GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

	private:
		DWORD				m_dwAffectedEunhyungLevel;

	protected:
		TPlayerSkill*					m_pSkillLevels;
		boost::unordered_map<BYTE, int>		m_SkillDamageBonus;
		std::map<int, TSkillUseInfo>	m_SkillUseInfo;

	public:
		void			AssignTriggers(const TMobTable * table);
		LPCHARACTER		GetVictim() const;
		void			SetVictim(LPCHARACTER pkVictim);
		LPCHARACTER		GetNearestVictim(LPCHARACTER pkChr);
		LPCHARACTER		GetProtege() const;
		bool			Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
		bool			Return();
		bool			IsGuardNPC() const;
		bool			IsChangeAttackPosition(LPCHARACTER target) const;
		void			ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR;}
		void			SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time();}
		bool			OnIdle();
		void			OnAttack(LPCHARACTER pkChrAttacker);
		void			OnClick(LPCHARACTER pkChrCauser);
		VID				m_kVIDVictim;

	protected:
		DWORD			m_dwLastChangeAttackPositionTime;
		CTrigger		m_triggerOnClick;

	protected:
		LPCHARACTER		m_pkChrTarget;
		CHARACTER_SET	m_set_pkChrTargetedBy;

	public:
		void				SetTarget(LPCHARACTER pkChrTarget);
		void				BroadcastTargetPacket();
		void				ClearTarget();
		void				CheckTarget();
		LPCHARACTER			GetTarget() const { return m_pkChrTarget; }

	public:
		int					GetSafeboxSize() const;
		void				QuerySafeboxSize();
		void				SetSafeboxSize(int size);
		CSafebox *			GetSafebox() const;
		void				LoadSafebox(int iSize, long long lldGold, int iItemCount, TPlayerItem * pItems);
		void				ChangeSafeboxSize(BYTE bSize);
		void				CloseSafebox();
		void				ReqSafeboxLoad(const char* pszPassword);
		void				CancelSafeboxLoad( void ) { m_bOpeningSafebox = false; }
		void				SetMallLoadTime(int t) { m_iMallLoadTime = t; }
		int					GetMallLoadTime() const { return m_iMallLoadTime; }
		CSafebox *			GetMall() const;
		void				LoadMall(int iItemCount, TPlayerItem * pItems);
		void				CloseMall();
		void				SetSafeboxOpenPosition();
		float				GetDistanceFromSafeboxOpen() const;

	protected:
		CSafebox *			m_pkSafebox;
		int					m_iSafeboxSize;
		int					m_iSafeboxLoadTime;
		bool				m_bOpeningSafebox;
		CSafebox *			m_pkMall;
		int					m_iMallLoadTime;
		PIXEL_POSITION		m_posSafeboxOpen;

	public:
		void				MountVnum(DWORD vnum);
		DWORD				GetMountVnum() const { return m_dwMountVnum; }
		DWORD				GetLastMountTime() const { return m_dwMountTime; }
		bool				CanUseHorseSkill();
		virtual	void		SetHorseLevel(int iLevel);
		virtual	bool		StartRiding();
		virtual	bool		StopRiding();
		virtual	DWORD		GetMyHorseVnum() const;
		virtual	void		HorseDie();
		virtual bool		ReviveHorse();
		virtual void		SendHorseInfo();
		virtual	void		ClearHorseInfo();
		void				HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0, const char* name = 0);
		LPCHARACTER			GetHorse() const			{ return m_chHorse; }
		LPCHARACTER			GetRider() const;
		void				SetRider(LPCHARACTER ch);
		bool				IsRiding() const;

	public:
		CPetSystem*			GetPetSystem()				{ return m_petSystem; }

	protected:
		CPetSystem*			m_petSystem;

	public:

	protected:
		LPCHARACTER			m_chHorse;
		LPCHARACTER			m_chRider;
		DWORD				m_dwMountVnum;
		DWORD				m_dwMountTime;
		BYTE				m_bSendHorseLevel;
		BYTE				m_bSendHorseHealthGrade;
		BYTE				m_bSendHorseStaminaGrade;

	public:
		void 				SetEmpire(BYTE bEmpire);
		BYTE				GetEmpire() const { return m_bEmpire; }

	protected:
		BYTE				m_bEmpire;

	public:
		void				SetRegen(LPREGEN pkRegen);

	protected:
		PIXEL_POSITION		m_posRegen;
		float				m_fRegenAngle;
		LPREGEN				m_pkRegen;
		size_t				regen_id_;

	public:
		bool				CannotMoveByAffect() const;
		bool				IsImmune(DWORD dwImmuneFlag);
		void                UpdateImmuneFlags();
		void				SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }

	protected:
		void				ApplyMobAttribute(const TMobTable* table);

	public:
		void				SetQuestNPCID(DWORD vid);
		DWORD				GetQuestNPCID() const { return m_dwQuestNPCVID; }
		LPCHARACTER			GetQuestNPC() const;
		void				SetQuestItemPtr(LPITEM item);
		void				ClearQuestItemPtr();
		LPITEM				GetQuestItemPtr() const;
		void				SetQuestBy(DWORD dwQuestVnum)	{ m_dwQuestByVnum = dwQuestVnum; }
		DWORD				GetQuestBy() const			{ return m_dwQuestByVnum; }
		int					GetQuestFlag(const std::string& flag) const;
		void				SetQuestFlag(const std::string& flag, int value);
		void				ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

	private:
		DWORD				m_dwQuestNPCVID;
		DWORD				m_dwQuestByVnum;
		LPITEM				m_pQuestItem;

	public:
		bool				StartStateMachine(int iPulse = 1);
		void				StopStateMachine();
		void				UpdateStateMachine(DWORD dwPulse);
		void				SetNextStatePulse(int iPulseNext);
		void				UpdateCharacter(DWORD dwPulse);

	protected:
		DWORD				m_dwNextStatePulse;

	public:
		LPCHARACTER			GetMarryPartner() const;
		void				SetMarryPartner(LPCHARACTER ch);
		int					GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);
		void				SetWeddingMap(marriage::WeddingMap* pMap);
		marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

	private:
		marriage::WeddingMap* m_pWeddingMap;
		LPCHARACTER			m_pkChrMarried;

	public:
		void				StartWarpNPCEvent();

	public:
		void				StartSaveEvent();
		void				StartRecoveryEvent();
		void				StartCheckSpeedHackEvent();
		void				StartDestroyWhenIdleEvent();
		LPEVENT				m_pkDeadEvent;
		LPEVENT				m_pkStunEvent;
		LPEVENT				m_pkSaveEvent;
		LPEVENT				m_pkRecoveryEvent;
		LPEVENT				m_pkTimedEvent;
		LPEVENT				m_pkFishingEvent;
		LPEVENT				m_pkAffectEvent;
		LPEVENT				m_pkPoisonEvent;
#ifdef ENABLE_WOLFMAN
		LPEVENT				m_pkBleedingEvent;
#endif
		LPEVENT				m_pkFireEvent;
		LPEVENT				m_pkWarpNPCEvent;
		LPEVENT				m_pkMiningEvent;
		LPEVENT				m_pkWarpEvent;
		LPEVENT				m_pkCheckSpeedHackEvent;
		LPEVENT				m_pkDestroyWhenIdleEvent;
		LPEVENT				m_pkPetSystemUpdateEvent;

		bool IsWarping() const { return m_pkWarpEvent ? true : false; }

		bool				m_bHasPoisoned;
#ifdef ENABLE_WOLFMAN
		bool				m_bHasBled;
#endif
		const CMob *		m_pkMobData;
		CMobInstance *		m_pkMobInst;
		std::map<int, LPEVENT> m_mapMobSkillEvent;
		friend struct FuncSplashDamage;
		friend struct FuncSplashAffect;
		friend class CFuncShoot;

	public:
		int				GetPremiumRemainSeconds(BYTE bType) const;

	private:
		int	m_aiPremiumTimes[PREMIUM_MAX_NUM];

	public :
		bool ItemProcess_Hair(LPITEM item, int iDestCell);

	public :
		void ClearSkill();
		void ClearSubSkill();
		bool ResetOneSkill(DWORD dwVnum);

	private :
		void SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag);

	public:
		bool	IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
		void 	SetOpenSafebox(bool b) { m_isOpenSafebox = b; }
		int		GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
		void	SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }

	private:
		bool	m_isOpenSafebox;

	public:
		int		GetSkillPowerByLevel(int level, bool bMob = false) const;
		int		GetRefineTime() const { return m_iRefineTime; }
		void	SetRefineTime() { m_iRefineTime = thecore_pulse(); } 
		int		m_iRefineTime;
		int		GetExchangeTime() const { return m_iExchangeTime; }
		void	SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
		int		m_iExchangeTime;
		int 	m_iMyShopTime;
		int		GetMyShopTime() const	{ return m_iMyShopTime; }
		void	SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }
		bool	IsHack(bool bSendMsg = true, bool bCheckShopOwner = true, int limittime = g_nPortalLimitTime);
		void	Say(const std::string & s);

	public:
		bool ItemProcess_Polymorph(LPITEM item);
		LPITEM*	GetCubeItem() { return m_pointsInstant.pCubeItems; }
		bool IsCubeOpen () const	{ return (m_pointsInstant.pCubeNpc?true:false); }
		void SetCubeNpc(LPCHARACTER npc)	{ m_pointsInstant.pCubeNpc = npc; }
		bool CanDoCube() const;

	private:
		e_overtime m_eOverTime;

	public:
		bool IsOverTime(e_overtime e) const { return (e == m_eOverTime); }
		void SetOverTime(e_overtime e) { m_eOverTime = e; }

	private:
		int		m_deposit_pulse;

	public:
		void	UpdateDepositPulse();
		bool	CanDeposit() const;

	private:
		void	__OpenPrivateShop();

	public:
		struct AttackedLog
		{
			DWORD 	dwPID;
			DWORD	dwAttackedTime;
			
			AttackedLog() : dwPID(0), dwAttackedTime(0)
			{
			}
		};

		AttackLog	m_kAttackLog;
		AttackedLog m_AttackedLog;
		int			m_speed_hack_count;

	private :
		std::string m_strNewName;

	public :
		const std::string GetNewName() const { return this->m_strNewName; }
		void SetNewName(const std::string name) { this->m_strNewName = name; }

	public :
		void GoHome();

	private :
		std::set<DWORD>	m_known_guild;

	public :
		void SendGuildName(CGuild* pGuild);
		void SendGuildName(DWORD dwGuildID);

	private :
		DWORD m_dwLogOffInterval;

	public :
		DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }

	public:
		bool UnEquipSpecialRideUniqueItem ();
		bool CanWarp () const;

	private:
		DWORD m_dwLastGoldDropTime;

	public:
		void AutoRecoveryItemProcess (const EAffectTypes);

	public:
		void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
		void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

	private:
		void BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue);
		void BuffOnAttr_ClearAll();

		typedef std::map <BYTE, CBuffOnAttributes*> TMapBuffOnAttrs;
		TMapBuffOnAttrs m_map_buff_on_attrs;

	public:
		void SetArmada() { cannot_dead = true; }
		void ResetArmada() { cannot_dead = false; }

	private:
		bool cannot_dead;

	private:
		bool m_bIsPet;

	public:
		void SetPet() { m_bIsPet = true; }
		bool IsPet() const
		{
			return m_bIsPet;
		}

	private:
		float m_fAttMul;
		float m_fDamMul;

	public:
		float GetAttMul() { return this->m_fAttMul; }
		void SetAttMul(float newAttMul) {this->m_fAttMul = newAttMul; }
		float GetDamMul() { return this->m_fDamMul; }
		void SetDamMul(float newDamMul) {this->m_fDamMul = newDamMul; }

	private:
		bool IsValidItemPosition(TItemPos Pos) const;

	private:
		unsigned int itemAward_vnum;
		char		 itemAward_cmd[20];

	public:
		unsigned int GetItemAward_vnum() { return itemAward_vnum; }
		char*		 GetItemAward_cmd() { return itemAward_cmd;	  }
		void		 SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
		void		 SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd,cmd); }

	public:
		void	DragonSoul_Initialize();
		bool	DragonSoul_IsQualified() const;
		void	DragonSoul_GiveQualification();
		int		DragonSoul_GetActiveDeck() const;
		bool	DragonSoul_IsDeckActivated() const;
		bool	DragonSoul_ActivateDeck(int deck_idx);
		void	DragonSoul_DeactivateAll();
		void	DragonSoul_CleanUp();

	public:
		bool		DragonSoul_RefineWindow_Open(LPENTITY pEntity);
		bool		DragonSoul_RefineWindow_Close();
		LPENTITY	DragonSoul_RefineWindow_GetOpener() { return  m_pointsInstant.m_pDragonSoulRefineWindowOpener; }
		bool		DragonSoul_RefineWindow_CanRefine();

	private:
		timeval		m_tvLastSyncTime;
		int			m_iSyncHackCount;
	public:
		void			SetLastSyncTime(const timeval &tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
		const timeval&	GetLastSyncTime() { return m_tvLastSyncTime; }
		void			SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount;}
		int				GetSyncHackCount() { return m_iSyncHackCount; }

	protected:
		int m_iLastPMPulse;
		int m_iPMCounter;
		int m_iLastItemDestroyPulse;

	private:
		BYTE m_AcceWindowType;

	public:
		void SetAcceWindowType(int windowType) { m_AcceWindowType = windowType; }
		int GetAcceWindowType() { return m_AcceWindowType; }
		bool IsAcceWindowOpen() const { return m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT] != WORD_MAX || m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT] != WORD_MAX; }
		void ClearPMCounter(void) { m_iPMCounter = 0; }
		void IncreasePMCounter(void) { m_iPMCounter++; }
		void SetLastPMPulse(void);
		int  GetPMCounter(void)   const { return m_iPMCounter; }
		int  GetLastPMPulse(void) const { return m_iLastPMPulse; }

#ifdef ENABLE_TARGET_INFO
	private:
		DWORD	dwLastTargetInfoPulse;

	public:
		DWORD	GetLastTargetInfoPulse() const { return dwLastTargetInfoPulse; }
		void	SetLastTargetInfoPulse(DWORD pulse) { dwLastTargetInfoPulse = pulse; }
#endif
		bool IsNearWater() const;

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	protected:
		CBuffNPCActor*	m_buffNPCSystem;
		
	public:
		CBuffNPCActor*	GetBuffNPCSystem() { return m_buffNPCSystem; }

		void			LoadBuffNPC();

		void			SetBuffNPC(bool isActive)	{ m_bIsBuffNPC = isActive;}
		bool			IsBuffNPC() const { return m_bIsBuffNPC; }

		void			SendBuffLevelUpEffect(int vid, int type, int value, int amount);
		void			SendBuffNPCUseSkillPacket(int skill_vnum, int skill_level);

		DWORD		BuffGetNextExp() const;
		DWORD		BuffGetNextExpByLevel(int level) const;

		LPITEM		GetBuffWear(UINT bCell) const;
		LPITEM		GetBuffEquipmentItem(WORD wCell) const;
		bool			IsBuffEquipUniqueItem(DWORD dwItemVnum) const;
		bool 			CheckBuffEquipmentPositionAvailable(int iWearCell);
		bool 			EquipBuffItem(BYTE cell, LPITEM item);
		bool 			UnequipBuffItem(BYTE cell, LPITEM item);
		bool 			IsBuffEquipEmpty();

		void			SetBuffWearWeapon(DWORD vnum) { m_dwBuffWeapon = vnum; }
		int				GetBuffWearWeapon() { return m_dwBuffWeapon; }
		void			SetBuffWearHead(DWORD value0) { m_dwBuffHead = value0; }
		int				GetBuffWearHead() { return m_dwBuffHead; }
		void			SetBuffWearArmor(DWORD vnum) { m_dwBuffArmor = vnum; }
		int				GetBuffWearArmor() { return m_dwBuffArmor; }

		void			SetBuffNPCInt(int value) { m_dwBuffNPCInt = value; }
		DWORD		GetBuffNPCInt() { return m_dwBuffNPCInt; }

		void			SetBuffHealValue(int value) { m_buffSkillHealHPValue = value; }
		DWORD		GetBuffHealValue() { return m_buffSkillHealHPValue; }
		
		void			SetBuffSealVID(int vid) { m_sealVID = vid; }
		int				GetBuffSealVID() { return m_sealVID; }
		
		LPEVENT	m_pkBuffNPCSystemUpdateEvent;
		LPEVENT	m_pkBuffNPCSystemExpireEvent;

	private:
		bool		m_bIsBuffNPC;
		DWORD	m_dwBuffNPCVid;
		DWORD	m_dwBuffNPCInt;
		int			m_sealVID;
		int			m_buffSkillHealHPValue;
		
		DWORD	m_dwBuffWeapon;
		DWORD	m_dwBuffHead;
		DWORD	m_dwBuffArmor;
#endif

#ifdef ENABLE_HUNTING_SYSTEM
	public:
		void CheckHunting(bool isLevelUp = 0);
		void OpenHuntingWindowMain();
		void OpenHuntingWindowSelect();
		void OpenHuntingWindowReward();
		
		void UpdateHuntingMission(DWORD dwMonsterVnum);
		void ReciveHuntingRewards();
		void SetCachedRewards();

		void SetRewardRandomItemFromTable();
		void SendRandomItemPacket(bool IsSelectWindow);
		
		int GetRaceTable();
		int GetMinMaxMoney(DWORD missionLevel, bool AskMax);
		int GetRandomMoney(DWORD missionLevel);
		int GetMinMaxExp(DWORD missionLevel, bool AskMax);
		int GetRandomExp(DWORD missionLevel);
#endif

#ifdef ENABLE_ITEMSHOP
	public:
		void	SendItemshopItemsPacket(
			const std::unordered_map <BYTE, std::vector<TItemshopItemTable>> items,
			const std::unordered_map <BYTE, TItemshopCategoryInfo> categories
		);
		void	SendPromotionRewardPacket(BYTE byAnswer, std::vector<TPromotionItemTable>);
		void	SendItemshopSingleItemRefreshPacket(TItemshopItemTable* item);
		void	SendItemshopSingleItemRemovePacket(TItemshopItemTable* item);
		void	SendItemshopSingleItemAddPacket(TItemshopItemTable* item, TItemshopCategoryInfo category);
		void	SendItemshopCoinPacket();
		bool	CanUseItemshop() const { return  m_iItemshopCooldownTime < thecore_pulse(); }
		void	SetItemshopCooldown(int pulse) { m_iItemshopCooldownTime = pulse; }
		bool	CanUsePromotionCode() const { return  m_iPromotionCodeCooldownTime < thecore_pulse(); }
		void	SetPromotionCodedown(int pulse) { m_iPromotionCodeCooldownTime = pulse; }
	protected:
		int		m_iItemshopCooldownTime;
		int		m_iPromotionCodeCooldownTime;
#endif

};
ESex GET_SEX(LPCHARACTER ch);
