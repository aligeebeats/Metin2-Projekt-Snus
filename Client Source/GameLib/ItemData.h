#pragma once

#include "../eterLib/GrpSubImage.h"
#include "../eterGrnLib/Thing.h"

class CItemData
{
	public:
		enum
		{
			ITEM_NAME_MAX_LEN = 24,
			ITEM_LIMIT_MAX_NUM = 2,
			ITEM_VALUES_MAX_NUM = 6,
			ITEM_SMALL_DESCR_MAX_LEN = 256,
			ITEM_APPLY_MAX_NUM = 4,
			ITEM_SOCKET_MAX_NUM = 3,
#ifdef ENABLE_SHINING_SYSTEM
			ITEM_SHINING_MAX_COUNT = 3,
#endif
		};

		enum EItemType
		{
			ITEM_TYPE_NONE,
			ITEM_TYPE_WEAPON,
			ITEM_TYPE_ARMOR,
			ITEM_TYPE_USE,
			ITEM_TYPE_AUTOUSE,
			ITEM_TYPE_MATERIAL,
			ITEM_TYPE_SPECIAL,
			ITEM_TYPE_TOOL,
			ITEM_TYPE_LOTTERY,
			ITEM_TYPE_ELK,
			ITEM_TYPE_METIN,
			ITEM_TYPE_CONTAINER,
			ITEM_TYPE_FISH,
			ITEM_TYPE_ROD,
			ITEM_TYPE_RESOURCE,
			ITEM_TYPE_CAMPFIRE,
			ITEM_TYPE_UNIQUE,
			ITEM_TYPE_SKILLBOOK,
			ITEM_TYPE_QUEST,
			ITEM_TYPE_POLYMORPH,
			ITEM_TYPE_TREASURE_BOX,
			ITEM_TYPE_TREASURE_KEY,
			ITEM_TYPE_SKILLFORGET,
			ITEM_TYPE_GIFTBOX,
			ITEM_TYPE_PICK,
			ITEM_TYPE_HAIR,
			ITEM_TYPE_TOTEM,
			ITEM_TYPE_BLEND,
			ITEM_TYPE_COSTUME,
			ITEM_TYPE_DS,
			ITEM_TYPE_SPECIAL_DS,
			ITEM_TYPE_EXTRACT,
			ITEM_TYPE_SECONDARY_COIN,
			ITEM_TYPE_RING,
			ITEM_TYPE_BELT,
			ITEM_TYPE_GACHA,
			ITEM_TYPE_MAX_NUM,				
		};

		enum EWeaponSubTypes
		{
			WEAPON_SWORD,
			WEAPON_DAGGER,
			WEAPON_BOW,
			WEAPON_TWO_HANDED,
			WEAPON_BELL,
			WEAPON_FAN,
			WEAPON_ARROW,
			WEAPON_MOUNT_SPEAR,
#ifdef ENABLE_WOLFMAN
			WEAPON_CLAW,
#endif
			WEAPON_QUIVER,
			WEAPON_BOUQUET,
			WEAPON_NUM_TYPES,

			WEAPON_NONE = WEAPON_NUM_TYPES+1,
		};

		enum EMaterialSubTypes
		{
			MATERIAL_LEATHER,
			MATERIAL_BLOOD,
			MATERIAL_ROOT,
			MATERIAL_NEEDLE,
			MATERIAL_JEWEL,
			MATERIAL_DS_REFINE_NORMAL, 
			MATERIAL_DS_REFINE_BLESSED, 
			MATERIAL_DS_REFINE_HOLLY,
		};

		enum EArmorSubTypes
		{
			ARMOR_BODY,
			ARMOR_HEAD,
			ARMOR_SHIELD,
			ARMOR_WRIST,
			ARMOR_FOOTS,
		    ARMOR_NECK,
			ARMOR_EAR,
			ARMOR_PENDANT,
			ARMOR_NUM_TYPES
		};

		enum ECostumeSubTypes
		{
			COSTUME_BODY,
			COSTUME_HAIR,
			COSTUME_ACCE,
			COSTUME_WEAPON,
			COSTUME_MOUNT,
			COSTUME_PET,
			COSTUME_NUM_TYPES,
		};

		enum EUseSubTypes
		{
			USE_POTION,
			USE_TALISMAN,
			USE_TUNING,
			USE_MOVE,
			USE_TREASURE_BOX,
			USE_MONEYBAG,
			USE_BAIT,
			USE_ABILITY_UP,
			USE_AFFECT,
			USE_CREATE_STONE,
			USE_SPECIAL,
			USE_POTION_NODELAY,
			USE_CLEAR,
			USE_INVISIBILITY,
			USE_DETACHMENT,
			USE_BUCKET,
			USE_POTION_CONTINUE,
			USE_CLEAN_SOCKET,
			USE_CHANGE_ATTRIBUTE,
			USE_ADD_ATTRIBUTE,
			USE_ADD_ACCESSORY_SOCKET,
			USE_PUT_INTO_ACCESSORY_SOCKET,
			USE_ADD_ATTRIBUTE2,
			USE_RECIPE,
			USE_CHANGE_ATTRIBUTE2,
			USE_BIND,
			USE_UNBIND,
			USE_TIME_CHARGE_PER,
			USE_TIME_CHARGE_FIX,
			USE_PUT_INTO_BELT_SOCKET,
			USE_PUT_INTO_RING_SOCKET,
			USE_CHANGE_COSTUME_ATTR,
			USE_RESET_COSTUME_ATTR,
			USE_FLOWER,
		};

		enum EGatchaType
		{
			USE_GACHA,
			GEM_LUCKY_BOX_GACHA,
			SPECIAL_LUCKY_BOX_GACHA,
		};

		enum EDragonSoulSubType
		{
			DS_SLOT1,
			DS_SLOT2,
			DS_SLOT3,
			DS_SLOT4,
			DS_SLOT5,
			DS_SLOT6,
			DS_SLOT_NUM_TYPES = 6,
		};

		enum EMetinSubTypes
		{
			METIN_NORMAL,
			METIN_GOLD,
		};

		enum ELimitTypes
		{
			LIMIT_NONE,

			LIMIT_LEVEL,
			LIMIT_STR,
			LIMIT_DEX,
			LIMIT_INT,
			LIMIT_CON,
			LIMIT_REAL_TIME = 7,						
			LIMIT_REAL_TIME_START_FIRST_USE,
			LIMIT_TIMER_BASED_ON_WEAR,
			LIMIT_MAX_NUM
		};

		enum EItemAntiFlag
		{
			ITEM_ANTIFLAG_FEMALE        = (1 << 0),
			ITEM_ANTIFLAG_MALE          = (1 << 1),
			ITEM_ANTIFLAG_WARRIOR       = (1 << 2),
			ITEM_ANTIFLAG_ASSASSIN      = (1 << 3),
			ITEM_ANTIFLAG_SURA          = (1 << 4),
			ITEM_ANTIFLAG_SHAMAN        = (1 << 5),
			ITEM_ANTIFLAG_GET           = (1 << 6),
			ITEM_ANTIFLAG_DROP          = (1 << 7),
			ITEM_ANTIFLAG_SELL          = (1 << 8),
			ITEM_ANTIFLAG_EMPIRE_A      = (1 << 9),
			ITEM_ANTIFLAG_EMPIRE_B      = (1 << 10),
			ITEM_ANTIFLAG_EMPIRE_R      = (1 << 11),
			ITEM_ANTIFLAG_SAVE          = (1 << 12),
			ITEM_ANTIFLAG_GIVE          = (1 << 13),
			ITEM_ANTIFLAG_PKDROP        = (1 << 14),
			ITEM_ANTIFLAG_STACK         = (1 << 15),
			ITEM_ANTIFLAG_MYSHOP        = (1 << 16),
			ITEM_ANTIFLAG_SAFEBOX		= (1 << 17),
#ifdef ENABLE_WOLFMAN
			ITEM_ANTIFLAG_WOLFMAN		= (1 << 18),
#endif
		};

		enum EItemFlag
		{
			ITEM_FLAG_REFINEABLE        = (1 << 0),
			ITEM_FLAG_SAVE              = (1 << 1),
			ITEM_FLAG_STACKABLE         = (1 << 2),
			ITEM_FLAG_COUNT_PER_1GOLD   = (1 << 3),
			ITEM_FLAG_SLOW_QUERY        = (1 << 4),
			ITEM_FLAG_RARE              = (1 << 5),
			ITEM_FLAG_UNIQUE            = (1 << 6),
			ITEM_FLAG_MAKECOUNT			= (1 << 7),
			ITEM_FLAG_IRREMOVABLE		= (1 << 8),
			ITEM_FLAG_CONFIRM_WHEN_USE	= (1 << 9),
			ITEM_FLAG_QUEST_USE         = (1 << 10),
			ITEM_FLAG_QUEST_USE_MULTIPLE= (1 << 11),
			ITEM_FLAG_UNUSED03          = (1 << 12),
			ITEM_FLAG_LOG               = (1 << 13),
			ITEM_FLAG_APPLICABLE		= (1 << 14),
		};

		enum EWearPositions
		{
			WEAR_BODY,
			WEAR_HEAD,
			WEAR_FOOTS,
			WEAR_WRIST,
			WEAR_WEAPON,
			WEAR_NECK,
			WEAR_EAR,
			WEAR_UNIQUE1,
			WEAR_UNIQUE2,
			WEAR_ARROW,
			WEAR_SHIELD,
			WEAR_RING1,
			WEAR_RING2,
			WEAR_BELT,
			WEAR_PENDANT,
			WEAR_COSTUME_BODY,
			WEAR_COSTUME_HAIR,
			WEAR_COSTUME_ACCE,
			WEAR_COSTUME_WEAPON,
			WEAR_COSTUME_MOUNT,
			WEAR_COSTUME_PET,
			WEAR_MAX_NUM,
		};

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		enum EBuffWindowSlots
		{
			BUFF_WEAR_WEAPON,				// 0
			BUFF_WEAR_HEAD,					// 1
			BUFF_WEAR_BODY,					// 2
			BUFF_WEAR_UNIQUE,				// 3

			BUFF_SLOT_SKILLBOOK,			// 4
			BUFF_SLOT_SKILLBOOK_COMMON_1,	// 5
			BUFF_SLOT_SKILLBOOK_COMMON_2,	// 6

			BUFF_SLOT_CHANGE_NAME,			// 7
			BUFF_SLOT_CHANGE_SEX,			// 8
			BUFF_SLOT_RESET_SKILL,			// 9

			BUFF_WINDOW_SLOT_MAX_NUM,
		};
#endif

		enum EItemWearableFlag
		{
			WEARABLE_BODY			= (1 << 0),
			WEARABLE_HEAD			= (1 << 1),
			WEARABLE_FOOTS			= (1 << 2),
			WEARABLE_WRIST			= (1 << 3),
			WEARABLE_WEAPON			= (1 << 4),
			WEARABLE_NECK			= (1 << 5),
			WEARABLE_EAR			= (1 << 6),
			WEARABLE_UNIQUE			= (1 << 7),
			WEARABLE_ARROW			= (1 << 8),
			WEARABLE_SHIELD			= (1 << 9),
			WEARABLE_RING			= (1 << 10),
			WEARABLE_BELT			= (1 << 11),
			WEARABLE_PENDANT		= (1 << 12),
			WEARABLE_COSTUME_BODY	= (1 << 13),
			WEARABLE_COSTUME_HAIR	= (1 << 14),
			WEARABLE_COSTUME_ACCE	= (1 << 15),
			WEARABLE_COSTUME_WEAPON = (1 << 16),
			WEARABLE_COSTUME_MOUNT	= (1 << 17),
			WEARABLE_COSTUME_PET	= (1 << 18),
		};

		enum EApplyTypes
		{
			APPLY_NONE,                
			APPLY_MAX_HP,              
			APPLY_MAX_SP,              
			APPLY_CON,                 
			APPLY_INT,                 
			APPLY_STR,                 
			APPLY_DEX,                 
			APPLY_ATT_SPEED,           
			APPLY_MOV_SPEED,           
			APPLY_CAST_SPEED,          
			APPLY_HP_REGEN,            
			APPLY_SP_REGEN,            
			APPLY_POISON_PCT,          
			APPLY_STUN_PCT,            
			APPLY_SLOW_PCT,            
			APPLY_CRITICAL_PCT,        
			APPLY_PENETRATE_PCT,       
			APPLY_ATTBONUS_HUMAN,      
			APPLY_ATTBONUS_ANIMAL,     
			APPLY_ATTBONUS_ORC,        
			APPLY_ATTBONUS_MILGYO,     
			APPLY_ATTBONUS_UNDEAD,     
			APPLY_ATTBONUS_DEVIL,      
			APPLY_STEAL_HP,            
			APPLY_STEAL_SP,            
			APPLY_MANA_BURN_PCT,       
			APPLY_DAMAGE_SP_RECOVER,   
			APPLY_BLOCK,               
			APPLY_DODGE,               
			APPLY_RESIST_SWORD,        
			APPLY_RESIST_TWOHAND,      
			APPLY_RESIST_DAGGER,       
			APPLY_RESIST_BELL,         
			APPLY_RESIST_FAN,          
			APPLY_RESIST_BOW,          
			APPLY_RESIST_FIRE,         
			APPLY_RESIST_ELEC,         
			APPLY_RESIST_MAGIC,        
			APPLY_RESIST_WIND,         
			APPLY_REFLECT_MELEE,       
			APPLY_REFLECT_CURSE,       
			APPLY_POISON_REDUCE,       
			APPLY_KILL_SP_RECOVER,     
			APPLY_EXP_DOUBLE_BONUS,    
			APPLY_GOLD_DOUBLE_BONUS,   
			APPLY_ITEM_DROP_BONUS,     
			APPLY_POTION_BONUS,        
			APPLY_KILL_HP_RECOVER,     
			APPLY_IMMUNE_STUN,         
			APPLY_IMMUNE_SLOW,         
			APPLY_IMMUNE_FALL,         
			APPLY_SKILL,               
			APPLY_BOW_DISTANCE,        
			APPLY_ATT_GRADE_BONUS,     
			APPLY_DEF_GRADE_BONUS,     
			APPLY_MAGIC_ATT_GRADE,     
			APPLY_MAGIC_DEF_GRADE,     
			APPLY_CURSE_PCT,           
			APPLY_MAX_STAMINA,			
			APPLY_ATT_BONUS_TO_WARRIOR,
			APPLY_ATT_BONUS_TO_ASSASSIN,
			APPLY_ATT_BONUS_TO_SURA,    
			APPLY_ATT_BONUS_TO_SHAMAN,  
			APPLY_ATT_BONUS_TO_MONSTER, 
			APPLY_MALL_ATTBONUS,        
			APPLY_MALL_DEFBONUS,        
			APPLY_MALL_EXPBONUS,        
			APPLY_MALL_ITEMBONUS,       
			APPLY_MALL_GOLDBONUS,       
			APPLY_MAX_HP_PCT,           
			APPLY_MAX_SP_PCT,           
			APPLY_SKILL_DAMAGE_BONUS,   
			APPLY_NORMAL_HIT_DAMAGE_BONUS,
			APPLY_SKILL_DEFEND_BONUS,
			APPLY_NORMAL_HIT_DEFEND_BONUS,
			APPLY_EXTRACT_HP_PCT,
			APPLY_PC_BANG_EXP_BONUS,	
			APPLY_PC_BANG_DROP_BONUS,	
			APPLY_RESIST_WARRIOR,		
			APPLY_RESIST_ASSASSIN ,		
			APPLY_RESIST_SURA,			
			APPLY_RESIST_SHAMAN,		
			APPLY_ENERGY,				
			APPLY_DEF_GRADE,			
			APPLY_COSTUME_ATTR_BONUS,	
			APPLY_MAGIC_ATTBONUS_PER,	
			APPLY_MELEE_MAGIC_ATTBONUS_PER,
			APPLY_RESIST_ICE,
			APPLY_RESIST_EARTH,
			APPLY_RESIST_DARK,
			APPLY_ANTI_CRITICAL_PCT,
			APPLY_ANTI_PENETRATE_PCT,
#ifdef ENABLE_WOLFMAN
			APPLY_BLEEDING_REDUCE,
			APPLY_BLEEDING_PCT,
			APPLY_ATTBONUS_WOLFMAN,
			APPLY_RESIST_WOLFMAN,
			APPLY_RESIST_CLAW,
#endif
			APPLY_RESIST_MAGIC_REDUCTION,
			APPLY_RESIST_HUMAN,
			APPLY_ACCEDRAIN_RATE,
			APPLY_ATTBONUS_ELEC,
			APPLY_ATTBONUS_FIRE,
			APPLY_ATTBONUS_ICE,
			APPLY_ATTBONUS_WIND,
			APPLY_ATTBONUS_EARTH,
			APPLY_ATTBONUS_DARK,
   			MAX_APPLY_NUM,
		};

		enum EImmuneFlags
		{
			IMMUNE_PARA         = (1 << 0),
			IMMUNE_CURSE        = (1 << 1),
			IMMUNE_STUN         = (1 << 2),
			IMMUNE_SLEEP        = (1 << 3),
			IMMUNE_SLOW         = (1 << 4),
			IMMUNE_POISON       = (1 << 5),
			IMMUNE_TERROR       = (1 << 6),
		};

#pragma pack(push)
#pragma pack(1)
		typedef struct SItemLimit
		{
			BYTE        bType;
			long        lValue;
		} TItemLimit;

		typedef struct SItemApply
		{
			BYTE        bType;
			long        lValue;
		} TItemApply;

		typedef struct SItemTable
		{
			DWORD       dwVnum;
			DWORD       dwVnumRange;
			char        szName[ITEM_NAME_MAX_LEN + 1];
			char        szLocaleName[ITEM_NAME_MAX_LEN + 1];
			BYTE        bType;
			BYTE        bSubType;
			
			BYTE        bWeight;
			BYTE        bSize;
			
			DWORD       dwAntiFlags;
			DWORD       dwFlags;
			DWORD       dwWearFlags;
			DWORD       dwImmuneFlag;
						
			DWORD       dwIBuyItemPrice;			
			DWORD		dwISellItemPrice;
			
			TItemLimit  aLimits[ITEM_LIMIT_MAX_NUM];
			TItemApply  aApplies[ITEM_APPLY_MAX_NUM];
			long        alValues[ITEM_VALUES_MAX_NUM];
			long        alSockets[ITEM_SOCKET_MAX_NUM];
			DWORD       dwRefinedVnum;
			WORD		wRefineSet;
			BYTE        bAlterToMagicItemPct;
			BYTE		bSpecular;
			BYTE        bGainSocketPct;
		} TItemTable;

#pragma pack(pop)

	public:
		CItemData();
		virtual ~CItemData();

		void Clear();
		void SetSummary(const std::string& c_rstSumm);
		void SetDescription(const std::string& c_rstDesc);

		CGraphicThing * GetModelThing();
		CGraphicThing * GetSubModelThing();
		CGraphicThing * GetDropModelThing();
		CGraphicSubImage * GetIconImage();

		DWORD GetAttachingDataCount();
		BOOL GetCollisionDataPointer(DWORD dwIndex, const NRaceData::TAttachingData ** c_ppAttachingData);
		BOOL GetAttachingDataPointer(DWORD dwIndex, const NRaceData::TAttachingData ** c_ppAttachingData);

		const TItemTable*	GetTable() const;
		DWORD GetIndex() const;
		const char * GetName() const;
		const char * GetDescription() const;
		const char * GetSummary() const;
		BYTE GetType() const;
		BYTE GetSubType() const;
		UINT GetRefine() const;
		const char* GetUseTypeString() const;
		DWORD GetWeaponType() const;
		BYTE GetSize() const;
		BOOL IsAntiFlag(DWORD dwFlag) const;
		BOOL IsFlag(DWORD dwFlag) const;
		BOOL IsWearableFlag(DWORD dwFlag) const;
		BOOL HasNextGrade() const;
		BOOL GetRefinedVnum() const;
		DWORD GetWearFlags() const;
		DWORD GetIBuyItemPrice() const;
		DWORD GetISellItemPrice() const;
		BOOL GetLimit(BYTE byIndex, TItemLimit * pItemLimit) const;
		BOOL GetApply(BYTE byIndex, TItemApply * pItemApply) const;
		long GetValue(BYTE byIndex) const;
		long GetSocket(BYTE byIndex) const;
		long SetSocket(BYTE byIndex,DWORD value);
		int GetSocketCount() const;
		DWORD GetIconNumber() const;

		UINT	GetSpecularPoweru() const;
		float	GetSpecularPowerf() const;
	
		BOOL IsEquipment() const;

		void SetDefaultItemData(const char * c_szIconFileName, const char * c_szModelFileName  = NULL);
		void SetItemTableData(TItemTable * pItemTable);

#ifdef ENABLE_SHINING_SYSTEM
		typedef struct SItemShiningTable {
			char szShinings[ITEM_SHINING_MAX_COUNT][256];
		public:
			bool Any() const
			{
				for (int i = 0; i < CItemData::ITEM_SHINING_MAX_COUNT; i++)
				{
					if (strcmp(szShinings[i], ""))
					{
						return true;
					}
				}
				return false;
			}
		} TItemShiningTable;
		void SetItemShiningTableData(BYTE bIndex, const char* szEffectname);
		CItemData::TItemShiningTable GetItemShiningTable() { return m_ItemShiningTable; }
#endif

	protected:
		void __LoadFiles();
		void __SetIconImage(const char * c_szFileName);

	protected:
		std::string m_strModelFileName;
		std::string m_strSubModelFileName;
		std::string m_strDropModelFileName;
		std::string m_strIconFileName;
		std::string m_strDescription;
		std::string m_strSummary;
		std::vector<std::string> m_strLODModelFileNameVector;

		CGraphicThing * m_pModelThing;
		CGraphicThing * m_pSubModelThing;
		CGraphicThing * m_pDropModelThing;
		CGraphicSubImage * m_pIconImage;
		std::vector<CGraphicThing *> m_pLODModelThingVector;

		NRaceData::TAttachingDataVector m_AttachingDataVector;
		DWORD		m_dwVnum;
		TItemTable m_ItemTable;
#ifdef ENABLE_SHINING_SYSTEM
		TItemShiningTable m_ItemShiningTable;
#endif
		
	public:
		static void DestroySystem();

		static CItemData* New();
		static void Delete(CItemData* pkItemData);

		static CDynamicPool<CItemData>		ms_kPool;

	protected:
		typedef struct SItemScaleTable
		{
			D3DXVECTOR3 v3Scale[NRaceData::SEX_MAX_NUM][NRaceData::JOB_MAX_NUM];
			float fScaleParticle[NRaceData::SEX_MAX_NUM][NRaceData::JOB_MAX_NUM];
		} TItemScaleTable;
		TItemScaleTable m_ItemScaleTable;

	public:
		float GetItemParticleScale(BYTE bJob, BYTE bSex);
		void SetItemTableScaleData(BYTE bJob, BYTE bSex, float fScaleX, float fScaleY, float fScaleZ, float fScaleParticle);
		D3DXVECTOR3& GetItemScaleVector(BYTE bJob, BYTE bSex);
};
