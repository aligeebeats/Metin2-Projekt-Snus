#pragma once
#include "sectree_manager.h"

class CParty;

class CDungeon
{
	typedef boost::unordered_map<LPPARTY, int> TPartyMap;
	typedef std::map<std::string, LPCHARACTER> TUniqueMobMap;

	public:
	typedef uint32_t IdType;

	~CDungeon();

	IdType GetId() const { return m_id; }
	void	Notice(const char* msg);
	void	JoinParty(LPPARTY pParty);
	void	QuitParty(LPPARTY pParty);
	void	Join(LPCHARACTER ch);
	void	IncMember(LPCHARACTER ch);
	void	DecMember(LPCHARACTER ch);
	void	Purge();
	void	KillAll();
	void	IncMonster() { m_iMonsterCount++; sys_log(0, "MonsterCount %d", m_iMonsterCount); }
	void	DecMonster() { m_iMonsterCount--; CheckEliminated(); }
	int		CountMonster() { return m_iMonsterCount; }
	int		CountRealMonster();
	void	IncPartyMember(LPPARTY pParty, LPCHARACTER ch);
	void	DecPartyMember(LPPARTY pParty, LPCHARACTER ch);
	void	IncKillCount(LPCHARACTER pkKiller, LPCHARACTER pkVictim);
	int		GetKillMobCount();
	int		GetKillStoneCount();
	bool	IsUsePotion();
	bool	IsUseRevive();
	void	UsePotion(LPCHARACTER ch);
	void	UseRevive(LPCHARACTER ch);
	long	GetMapIndex() { return m_lMapIndex; }
	void	Spawn(DWORD vnum, const char* pos);
	LPCHARACTER	SpawnMob(DWORD vnum, int x, int y, int dir = 0);
	LPCHARACTER	SpawnMob_ac_dir(DWORD vnum, int x, int y, int dir = 0);
	LPCHARACTER	SpawnGroup(DWORD vnum, long x, long y, float radius, bool bAggressive=false, int count=1);
	void	SpawnNameMob(DWORD vnum, int x, int y, const char* name);
	void	SpawnGotoMob(long lFromX, long lFromY, long lToX, long lToY);
	void	SpawnRegen(const char* filename, bool bOnce = true);
	void	AddRegen(LPREGEN regen);
	void	ClearRegen();
	bool	IsValidRegen(LPREGEN regen, size_t regen_id);
	void	SetUnique(const char* key, DWORD vid);
	void	SpawnMoveUnique(const char* key, DWORD vnum, const char* pos_from, const char* pos_to);
	void	SpawnMoveGroup(DWORD vnum, const char* pos_from, const char* pos_to, int count=1);
	void	SpawnUnique(const char* key, DWORD vnum, const char* pos);
	void	SpawnStoneDoor(const char* key, const char* pos);
	void	SpawnWoodenDoor(const char* key, const char* pos);
	void	KillUnique(const std::string& key);
	void	PurgeUnique(const std::string& key);
	bool	IsUniqueDead(const std::string& key);
	float	GetUniqueHpPerc(const std::string& key);
	DWORD	GetUniqueVid(const std::string& key);
	void	DeadCharacter(LPCHARACTER ch);
	void	UniqueSetMaxHP(const std::string& key, int iMaxHP);
	void	UniqueSetHP(const std::string& key, int iHP);
	void	UniqueSetDefGrade(const std::string& key, int iGrade);
	void	SendDestPositionToParty(LPPARTY pParty, long x, long y);
	void	CheckEliminated();
	void	JumpAll(long lFromMapIndex, int x, int y);
	void	WarpAll(long lFromMapIndex, int x, int y);
	void	JumpParty(LPPARTY pParty, long lFromMapIndex, int x, int y);
	void	ExitAll();
	void	ExitAllToStartPosition();
	void	JumpToEliminateLocation();
	void	SetExitAllAtEliminate(long time);
	void	SetWarpAtEliminate(long time, long lMapIndex, int x, int y, const char* regen_file);
	int		GetFlag(std::string name);
	void	SetFlag(std::string name, int value);

	typedef std::vector <std::pair <DWORD, int> > ItemGroup;
	void	CreateItemGroup (std::string& group_name, ItemGroup& item_group);
	const ItemGroup* GetItemGroup (std::string& group_name);
	template <class Func> Func ForEachMember(Func f);
	bool IsAllPCNearTo( int x, int y, int dist );

	protected:
	CDungeon(IdType id, long lOriginalMapIndex, long lMapIndex);
	void	Initialize();

	private:
	IdType 		m_id;
	DWORD		m_lOrigMapIndex;
	DWORD		m_lMapIndex;

	CHARACTER_SET	    m_set_pkCharacter;
	std::map<std::string, int>  m_map_Flag;
	typedef std::map<std::string, ItemGroup> ItemGroupMap;
	ItemGroupMap	m_map_ItemGroup;
	TPartyMap		m_map_pkParty;
	TAreaMap&		m_map_Area;
	TUniqueMobMap	m_map_UniqueMob;

	int			m_iMobKill;
	int			m_iStoneKill;
	bool		m_bUsePotion;
	bool		m_bUseRevive;
	int			m_iMonsterCount;
	bool		m_bExitAllAtEliminate;
	bool		m_bWarpAtEliminate;
	int			m_iWarpDelay;
	long		m_lWarpMapIndex;
	long		m_lWarpX;
	long		m_lWarpY;
	std::string	m_stRegenFile;

	std::vector<LPREGEN> m_regen;

	LPEVENT		deadEvent;
	LPEVENT exit_all_event_;
	LPEVENT jump_to_event_;
	size_t regen_id_;

	friend class CDungeonManager;
	friend EVENTFUNC(dungeon_dead_event);
	friend EVENTFUNC(dungeon_exit_all_event);
	friend EVENTFUNC(dungeon_jump_to_event);
	LPPARTY m_pParty;
	public :
	void SetPartyNull();
};

class CDungeonManager : public singleton<CDungeonManager>
{
	typedef std::map<CDungeon::IdType, LPDUNGEON> TDungeonMap;
	typedef std::map<long, LPDUNGEON> TMapDungeon;

	public:
	CDungeonManager();
	virtual ~CDungeonManager();

	LPDUNGEON	Create(long lOriginalMapIndex);
	void		Destroy(CDungeon::IdType dungeon_id);
	LPDUNGEON	Find(CDungeon::IdType dungeon_id);
	LPDUNGEON	FindByMapIndex(long lMapIndex);

	private:
	TDungeonMap	m_map_pkDungeon;
	TMapDungeon m_map_pkMapDungeon;
	CDungeon::IdType next_id_;
};
