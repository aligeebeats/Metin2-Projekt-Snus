#pragma once
#include "../GameLib/ItemData.h"

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects(0) {}
	SAffects(const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}
	int operator = (const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}

	BOOL IsAffect(BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects(const DWORD & c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;
};

extern std::string g_strGuildSymbolPathName;

const DWORD c_Name_Max_Length = 64;
const DWORD c_FileName_Max_Length = 128;
const DWORD c_Short_Name_Max_Length = 32;

const DWORD c_Inventory_Page_Size = 5*9;
const DWORD c_Inventory_Page_Count = 4;
const DWORD c_ItemSlot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;

const DWORD c_Equipment_Start = c_ItemSlot_Count;

const DWORD c_Equipment_Body = c_Equipment_Start + CItemData::WEAR_BODY;
const DWORD c_Equipment_Head = c_Equipment_Start + CItemData::WEAR_HEAD;
const DWORD c_Equipment_Shoes = c_Equipment_Start + CItemData::WEAR_FOOTS;
const DWORD c_Equipment_Wrist = c_Equipment_Start + CItemData::WEAR_WRIST;
const DWORD c_Equipment_Weapon = c_Equipment_Start + CItemData::WEAR_WEAPON;
const DWORD c_Equipment_Neck = c_Equipment_Start + CItemData::WEAR_NECK;
const DWORD c_Equipment_Ear = c_Equipment_Start + CItemData::WEAR_EAR;
const DWORD c_Equipment_Unique1 = c_Equipment_Start + CItemData::WEAR_UNIQUE1;
const DWORD c_Equipment_Unique2 = c_Equipment_Start + CItemData::WEAR_UNIQUE2;
const DWORD c_Equipment_Arrow = c_Equipment_Start + CItemData::WEAR_ARROW;
const DWORD c_Equipment_Shield = c_Equipment_Start + CItemData::WEAR_SHIELD;
const DWORD c_Equipment_Ring1 = c_Equipment_Start + CItemData::WEAR_RING1;
const DWORD c_Equipment_Ring2 = c_Equipment_Start + CItemData::WEAR_RING2;
const DWORD c_Equipment_Belt = c_Equipment_Start + CItemData::WEAR_BELT;
const DWORD c_Equipment_Pendant = c_Equipment_Start + CItemData::WEAR_PENDANT;

const DWORD c_Equipment_Count = CItemData::WEAR_PENDANT;
const DWORD c_Equipment_End = c_Equipment_Pendant;

const DWORD c_Costume_Slot_Start = c_Equipment_End;
const DWORD	c_Costume_Slot_Body = c_Equipment_Start + CItemData::WEAR_COSTUME_BODY;
const DWORD	c_Costume_Slot_Hair = c_Equipment_Start + CItemData::WEAR_COSTUME_HAIR;
const DWORD	c_Costume_Slot_Acce = c_Equipment_Start + CItemData::WEAR_COSTUME_ACCE;
const DWORD	c_Costume_Slot_Weapon = c_Equipment_Start + CItemData::WEAR_COSTUME_WEAPON;
const DWORD	c_Costume_Slot_Mount = c_Equipment_Start + CItemData::WEAR_COSTUME_MOUNT;
const DWORD	c_Costume_Slot_Pet = c_Equipment_Start + CItemData::WEAR_COSTUME_PET;

const DWORD c_Costume_Slot_Count = CItemData::WEAR_MAX_NUM - c_Equipment_Count;
const DWORD c_Costume_Slot_End = c_Costume_Slot_Start + c_Costume_Slot_Count;
const DWORD c_Wear_Max = CItemData::WEAR_MAX_NUM;


enum EDragonSoulDeckType
{
	DS_DECK_1,
	DS_DECK_2,
	DS_DECK_MAX_NUM = 2,
};

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
	DRAGON_SOUL_GRADE_MYTH,
	DRAGON_SOUL_GRADE_MAX,

};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};

#ifdef WJ_ENABLE_TRADABLE_ICON
enum ETopWindowTypes
{
	ON_TOP_WND_NONE,
	ON_TOP_WND_SHOP,
	ON_TOP_WND_EXCHANGE,
	ON_TOP_WND_SAFEBOX,
	ON_TOP_WND_PRIVATE_SHOP,
	ON_TOP_WND_ITEM_COMB,
	ON_TOP_WND_PET_FEED,

	ON_TOP_WND_MAX,
};
#endif

const DWORD c_DragonSoul_Equip_Start = c_ItemSlot_Count + c_Wear_Max;
const DWORD c_DragonSoul_Equip_Slot_Max = 6;
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + c_DragonSoul_Equip_Slot_Max * DS_DECK_MAX_NUM;

const DWORD c_DragonSoul_Equip_Reserved_Count = c_DragonSoul_Equip_Slot_Max * 3;		

#ifdef ENABLE_SPECIAL_INVENTORY

enum ESpecialInventoryTypes
{
	INVENTORY_TYPE_INVENTORY,
	INVENTORY_TYPE_SKILLBOOK,
	INVENTORY_TYPE_STONE,
	INVENTORY_TYPE_MATERIAL,
	INVENTORY_TYPE_COUNT,
};

enum ESpecialInventorySize
{
	SPECIAL_INVENTORY_WIDTH = 5,
	SPECIAL_INVENTORY_HEIGHT = 9,
	SPECIAL_INVENTORY_PAGE_SIZE = SPECIAL_INVENTORY_WIDTH * SPECIAL_INVENTORY_HEIGHT,
};
#endif


const DWORD c_Belt_Inventory_Slot_Start = c_DragonSoul_Equip_End + c_DragonSoul_Equip_Reserved_Count;
const DWORD c_Belt_Inventory_Width = 4;
const DWORD c_Belt_Inventory_Height= 4;
const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;

const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size;
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;


#if defined(ENABLE_NEW_EQUIPMENT_SYSTEM) || defined(ENABLE_SPECIAL_INVENTORY)
const DWORD c_Special_Inventory_Slot_Start = c_Belt_Inventory_Slot_End;
const DWORD c_Special_Inventory_Skillbook_Slot_Start = c_Special_Inventory_Slot_Start;
const DWORD c_Special_Inventory_Skillbook_Slot_End = c_Special_Inventory_Skillbook_Slot_Start + SPECIAL_INVENTORY_PAGE_SIZE * c_Inventory_Page_Count;
const DWORD c_Special_Inventory_Stone_Slot_Start = c_Special_Inventory_Skillbook_Slot_End;
const DWORD c_Special_Inventory_Stone_Slot_End = c_Special_Inventory_Stone_Slot_Start + SPECIAL_INVENTORY_PAGE_SIZE * c_Inventory_Page_Count;
const DWORD c_Special_Inventory_Material_Slot_Start = c_Special_Inventory_Stone_Slot_End;
const DWORD c_Special_Inventory_Material_Slot_End = c_Special_Inventory_Material_Slot_Start + SPECIAL_INVENTORY_PAGE_SIZE * c_Inventory_Page_Count;
const DWORD c_Special_Inventory_Slot_End = c_Special_Inventory_Material_Slot_End;

const DWORD c_Inventory_Count = c_Special_Inventory_Slot_End;

#elif defined(ENABLE_NEW_EQUIPMENT_SYSTEM)
const DWORD c_Inventory_Count = c_Belt_Inventory_Slot_End;
#else
const DWORD c_Inventory_Count = c_DragonSoul_Inventory_End;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM 
const DWORD c_Buff_Equipment_Slot_Start = 0;
const DWORD c_Buff_Equipment_Slot_End = c_Buff_Equipment_Slot_Start + CItemData::BUFF_WINDOW_SLOT_MAX_NUM;
#endif

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
#ifdef ENABLE_SWITCHBOT
	SLOT_TYPE_SWITCHBOT,
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	SLOT_TYPE_BUFF_EQUIPMENT,
#endif
	SLOT_TYPE_MAX,
};

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
#ifdef ENABLE_SWITCHBOT
	SWITCHBOT,
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	BUFF_EQUIPMENT,
#endif
	ACCE_REFINE,
	GROUND,
	WINDOW_TYPE_MAX,
};

enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};

#pragma pack (push, 1)
#define WORD_MAX 0xffff

#ifdef ENABLE_SWITCHBOT
enum ESwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	MAX_NORM_ATTR_NUM = 5,
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
	ATTRIBUTE_SET_MAX_NUM,
};

#endif

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
    SItemPos ()
    {
		window_type =     INVENTORY;
		cell = WORD_MAX;
    }
	SItemPos (BYTE _window_type, WORD _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

	bool IsValidCell()
	{
		switch (window_type)
		{
		case INVENTORY:
			return cell < c_Inventory_Count;
			break;
		case EQUIPMENT:
			return cell < c_DragonSoul_Equip_End;
			break;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DS_INVENTORY_MAX_NUM);
			break;

#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_COUNT;
			break;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case BUFF_EQUIPMENT:
			return cell < (CItemData::BUFF_WINDOW_SLOT_MAX_NUM);
			break;
#endif

		default:
			return false;
		}
	}
	bool IsEquipCell()
	{
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			return (c_Equipment_Start + c_Wear_Max > cell) && (c_Equipment_Start <= cell);
			break;

		case BELT_INVENTORY:
		case DRAGON_SOUL_INVENTORY:
			return false;
			break;

		default:
			return false;
		}
	}

	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator!=(const struct SItemPos& rhs) const
	{
		return (window_type != rhs.window_type) || (cell != rhs.cell);
	}

	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;

const TItemPos NPOS(RESERVED_WINDOW, WORD_MAX);

#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;

#define FILE_MAX_LEN 128

enum
{
#ifndef ENABLE_EXTENDED_SOCKETS
	ITEM_SOCKET_SLOT_MAX_NUM = 3,
#else
	ITEM_SOCKET_SLOT_MAX_NUM = 6,
	ITEM_STONES_MAX_NUM = 3, //If you are extending stones in item, change to 6. If you do not want more than 3 stones, keep 3.
#endif
	ITEM_ATTRIBUTE_SLOT_MAX_NUM = 7,
};

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	BYTE Type;
	BYTE Position;
} TQuickSlot;

typedef struct TPlayerItemAttribute
{
    BYTE        bType;
    short       sValue;
} TPlayerItemAttribute;

typedef struct packet_item
{
    DWORD       vnum;
    WORD        count;
	DWORD		flags;
	DWORD		anti_flags;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TItemData;

typedef struct packet_shop_item
{
    DWORD       vnum;
    long long	price;
    WORD        count;
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	DWORD 		price_type = 1;
	DWORD 		price_vnum = 0;
} TShopItemData;

#pragma pack(pop)

inline float GetSqrtDistance(int ix1, int iy1, int ix2, int iy2)
{
	float dx, dy;

	dx = float(ix1 - ix2);
	dy = float(iy1 - iy2);

	return sqrtf(dx*dx + dy*dy);
}

void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName(const char * c_szFontName);
CResource* DefaultFont_GetResource();
CResource* DefaultItalicFont_GetResource();

void SetGuildSymbolPath(const char * c_szPathName);
const char * GetGuildSymbolFileName(DWORD dwGuildID);
BYTE SlotTypeToInvenType(BYTE bSlotType);
BYTE ApplyTypeToPointType(BYTE bApplyType);
