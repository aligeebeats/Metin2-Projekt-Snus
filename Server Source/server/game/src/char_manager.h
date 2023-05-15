#pragma once
#include "../../common/stl.h"
#include "../../common/length.h"
#include "vid.h"

class CDungeon;
class CHARACTER;
class CharacterVectorInteractor;

class CHARACTER_MANAGER : public singleton<CHARACTER_MANAGER>
{
	public:
		typedef boost::unordered_map<std::string, LPCHARACTER> NAME_MAP;

		CHARACTER_MANAGER();
		virtual ~CHARACTER_MANAGER();

		void                    Destroy();

		void			GracefulShutdown();

		DWORD			AllocVID();

		LPCHARACTER             CreateCharacter(const char * name, DWORD dwPID = 0);
		void DestroyCharacter(LPCHARACTER ch);

		void DestroyCharacterInMap(long lMapIndex);

		void			Update(int iPulse);
		LPCHARACTER		SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion = false, int iRot = -1, bool bShow = true);
		LPCHARACTER		SpawnMobRange(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, bool bIsException=false, bool bSpawnMotion = false , bool bAggressive = false);
		LPCHARACTER		SpawnGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen = NULL, bool bAggressive_ = false, LPDUNGEON pDungeon = NULL);
		bool			SpawnGroupGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen = NULL, bool bAggressive_ = false, LPDUNGEON pDungeon = NULL);
		bool			SpawnMoveGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, int tx, int ty, LPREGEN pkRegen = NULL, bool bAggressive_ = false);
		LPCHARACTER		SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex);

		void			SelectStone(LPCHARACTER pkChrStone);

		NAME_MAP &		GetPCMap() { return m_map_pkPCChr; }

		LPCHARACTER		Find(DWORD dwVID);
		LPCHARACTER		Find(const VID & vid);
		LPCHARACTER		FindPC(const char * name);
		LPCHARACTER		FindByPID(DWORD dwPID);

		bool			AddToStateList(LPCHARACTER ch);
		void			RemoveFromStateList(LPCHARACTER ch);
		void			DelayedSave(LPCHARACTER ch);
		bool			FlushDelayedSave(LPCHARACTER ch);
		void			ProcessDelayedSave();

		template<class Func>	Func for_each_pc(Func f);

		void			KillLog(DWORD dwVnum);

		void			RegisterRaceNum(DWORD dwVnum);
		void			RegisterRaceNumMap(LPCHARACTER ch);
		void			UnregisterRaceNumMap(LPCHARACTER ch);
		bool			GetCharactersByRaceNum(DWORD dwRaceNum, CharacterVectorInteractor & i);

		LPCHARACTER		FindSpecifyPC(unsigned int uiJobFlag, long lMapIndex, LPCHARACTER except=NULL, int iMinLevel = 1, int iMaxLevel = PLAYER_MAX_LEVEL_CONST);

		void			SetMobItemRate(int value)	{ m_iMobItemRate = value; }
		void			SetMobDamageRate(int value)	{ m_iMobDamageRate = value; }
		void			SetMobGoldAmountRate(int value)	{ m_iMobGoldAmountRate = value; }
		void			SetMobGoldDropRate(int value)	{ m_iMobGoldDropRate = value; }
		void			SetMobExpRate(int value)	{ m_iMobExpRate = value; }

		void			SetMobItemRatePremium(int value)	{ m_iMobItemRatePremium = value; }
		void			SetMobGoldAmountRatePremium(int value)	{ m_iMobGoldAmountRatePremium = value; }
		void			SetMobGoldDropRatePremium(int value)	{ m_iMobGoldDropRatePremium = value; }
		void			SetMobExpRatePremium(int value)		{ m_iMobExpRatePremium = value; }

		void			SetUserDamageRatePremium(int value)	{ m_iUserDamageRatePremium = value; }
		void			SetUserDamageRate(int value ) { m_iUserDamageRate = value; }
		int			GetMobItemRate(LPCHARACTER ch);
		int			GetMobDamageRate(LPCHARACTER ch);
		int			GetMobGoldAmountRate(LPCHARACTER ch);
		int			GetMobGoldDropRate(LPCHARACTER ch);
		int			GetMobExpRate(LPCHARACTER ch);

		int			GetUserDamageRate(LPCHARACTER ch);
		void		SendScriptToMap(long lMapIndex, const std::string & s); 

		bool			BeginPendingDestroy();
		void			FlushPendingDestroy();

	private:
		int					m_iMobItemRate;
		int					m_iMobDamageRate;
		int					m_iMobGoldAmountRate;
		int					m_iMobGoldDropRate;
		int					m_iMobExpRate;

		int					m_iMobItemRatePremium;
		int					m_iMobGoldAmountRatePremium;
		int					m_iMobGoldDropRatePremium;
		int					m_iMobExpRatePremium;

		int					m_iUserDamageRate;
		int					m_iUserDamageRatePremium;
		int					m_iVIDCount;

		boost::unordered_map<DWORD, LPCHARACTER> m_map_pkChrByVID;
		boost::unordered_map<DWORD, LPCHARACTER> m_map_pkChrByPID;
		NAME_MAP			m_map_pkPCChr;

		char				dummy1[1024];
		CHARACTER_SET		m_set_pkChrState;
		CHARACTER_SET		m_set_pkChrForDelayedSave;
		LPCHARACTER			m_pkChrSelectedStone;

		std::map<DWORD, DWORD> m_map_dwMobKillCount;

		std::set<DWORD>		m_set_dwRegisteredRaceNum;
		std::map<DWORD, CHARACTER_SET> m_map_pkChrByRaceNum;

		bool				m_bUsePendingDestroy;
		CHARACTER_SET		m_set_pkChrPendingDestroy;

#ifdef ENABLE_ITEMSHOP
	public:
		void			SendItemshopSingleRemoveItem(TItemshopItemTable* item);
		void			SendItemshopSingleAddItem(TItemshopItemTable* item);
		void			SendItemshopSingleItemRefresh(TItemshopItemTable* item);
		void			SendItemshopItems();
#endif

};

	template<class Func>	
Func CHARACTER_MANAGER::for_each_pc(Func f)
{
	for (auto it = m_map_pkChrByPID.begin(); it != m_map_pkChrByPID.end(); ++it)
		f(it->second);

	return f;
}

class CharacterVectorInteractor : public CHARACTER_VECTOR
{
	public:
		CharacterVectorInteractor() : m_bMyBegin(false) { }

		CharacterVectorInteractor(const CHARACTER_SET & r);
		virtual ~CharacterVectorInteractor();

	private:
		bool m_bMyBegin;
};

#define M2_DESTROY_CHARACTER(ptr) CHARACTER_MANAGER::instance().DestroyCharacter(ptr)
