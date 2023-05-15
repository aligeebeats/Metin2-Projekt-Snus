#include "stdafx.h"
#include <stack>
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "xmas_event.h"
#include "marriage.h"
#include "polymorph.h"
#include "blend_item.h"
#include "safebox.h"
#include "shop.h"
#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "PetSystem.h"

#ifdef ENABLE_SWITCHBOT
#include "new_switchbot.h"
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
#include "buff_npc_system.h"
#endif

const int ITEM_BROKEN_METIN_VNUM = 28960;
const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator()(LPENTITY pEnt)
	{
		if (pEnt->IsType(ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[(DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};

static bool IS_SUMMON_ITEM(int vnum)
{
	switch (vnum)
	{
		case 22000:
		case 22010:
		case 22011:
		case 22020:
		case ITEM_MARRIAGE_RING:
			return true;
	}

	return false;
}

static bool IS_MONKEY_DUNGEON(int map_index)
{
	switch (map_index)
	{
		case 5:
		case 25:
		case 45:
		case 108:
		case 109:
			return true;;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE(int map_index)
{
	if (IS_MONKEY_DUNGEON(map_index))
		return false;

	switch (map_index)
	{
		case 66 :
		case 71 :
		case 72 :
		case 73 :
		case 193 :
#if 0
		case 184 :
		case 185 :
		case 186 :
		case 187 :
		case 188 :
		case 189 :
#endif
		case 216 :
		case 217 :
		case 208 :
			return false;
	}

	if (map_index > 10000) return false;

	return true;
}

static bool FN_check_item_socket(LPITEM item)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
			return false;
	}

	return true;
}

static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}
static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE==GET_SEX(ch))
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)) 
	{
		if (SEX_FEMALE==GET_SEX(ch))
			return false;
	}

	return true;
}

bool CHARACTER::CanHandleItem(bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
			return false;

	if (GetMyShop())
		return false;

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
			return false;

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
		return false;

	if (IsWarping())
		return false;

	if (IsAcceWindowOpen())
	{
		return false;
	}

	return true;
}

LPITEM CHARACTER::GetInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}
LPITEM CHARACTER::GetItem(TItemPos Cell) const
{
	if (!IsValidItemPosition(Cell))
		return NULL;
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pItems[wCell];
	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pDSItems[wCell];

#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid switchbot item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pSwitchbotItems[wCell];
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		if(wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid Buff-EQ item cell %d >= %d", wCell, BUFF_WINDOW_SLOT_MAX_NUM);
			return NULL;
		}
		return m_pointsInstant.pBuffEquipmentItem[wCell];
#endif

	default:
		return NULL;
	}
	return NULL;
}

#ifdef HIGHLIGHT_SLOT
void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem, bool bWereMine)
#else
void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem)
#endif
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	if ((unsigned long)((CItem*)pItem) == 0xff || (unsigned long)((CItem*)pItem) == 0xffffffff)
	{
		sys_err("!!! FATAL ERROR !!! item == 0xff (char: %s cell: %u)", GetName(), wCell);
		core_dump();
		return;
	}

	if (pItem && pItem->GetOwner())
	{
		assert(!"GetOwner exist");
		return;
	}

	switch(window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		{
			if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
			{
				sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
				return;
			}

			LPITEM pOld = m_pointsInstant.pItems[wCell];

			if (pOld)
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				if (wCell < INVENTORY_MAX_NUM || Cell.IsSpecialInventoryPosition())
#else
				if (wCell < INVENTORY_MAX_NUM)
#endif
				{	
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
#ifdef ENABLE_SPECIAL_INVENTORY		 
							if (!TItemPos(INVENTORY, p).IsSpecialInventoryPosition())
							{
								continue;
							}
#else
							continue;
#endif

						if (m_pointsInstant.pItems[p] && m_pointsInstant.pItems[p] != pOld)
							continue;

						m_pointsInstant.wItemGrid[p] = 0;
					}
				}
				else
					m_pointsInstant.wItemGrid[wCell] = 0;
			}

			if (pItem)
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				if (wCell < INVENTORY_MAX_NUM || Cell.IsSpecialInventoryPosition())
#else
				if (wCell < INVENTORY_MAX_NUM)
#endif
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
#ifdef ENABLE_SPECIAL_INVENTORY		 
							if (!TItemPos(INVENTORY, p).IsSpecialInventoryPosition())
							{
								continue;
							}
#else
							continue;
#endif

						m_pointsInstant.wItemGrid[p] = wCell + 1;
					}
				}
				else
					m_pointsInstant.wItemGrid[wCell] = wCell + 1;
			}

			m_pointsInstant.pItems[wCell] = pItem;
		}
		break;

	case DRAGON_SOUL_INVENTORY:
		{
			LPITEM pOld = m_pointsInstant.pDSItems[wCell];

			if (pOld)
			{
				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						if (m_pointsInstant.pDSItems[p] && m_pointsInstant.pDSItems[p] != pOld)
							continue;

						m_pointsInstant.wDSItemGrid[p] = 0;
					}
				}
				else
					m_pointsInstant.wDSItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					sys_err("CHARACTER::SetItem: invalid DS item cell %d", wCell);
					return;
				}

				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						m_pointsInstant.wDSItemGrid[p] = wCell + 1;
					}
				}
				else
					m_pointsInstant.wDSItemGrid[wCell] = wCell + 1;
			}

			m_pointsInstant.pDSItems[wCell] = pItem;
		}
		break;

#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
	{
		LPITEM pOld = m_pointsInstant.pSwitchbotItems[wCell];
		if (pItem && pOld)
		{
			return;
		}

		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::SetItem: invalid switchbot item cell %d", wCell);
			return;
		}

		if (pItem)
		{
			CSwitchbotManager::Instance().RegisterItem(GetPlayerID(), pItem->GetID(), wCell);
		}
		else
		{
			CSwitchbotManager::Instance().UnregisterItem(GetPlayerID(), wCell);
		}

		m_pointsInstant.pSwitchbotItems[wCell] = pItem;
	}
	break;
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		{
			if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM) {
				sys_err("CHARACTER::SetItem: invalid BUFF_EQUIPMENT item cell %d", wCell);
				return;
			}
			LPITEM pOld = m_pointsInstant.pBuffEquipmentItem[wCell];

			if (pOld && pItem) {
				return;
			}

			m_pointsInstant.pBuffEquipmentItem[wCell] = pItem;
			if (GetBuffNPCSystem() != NULL) {
				if (GetBuffNPCSystem()->IsSummoned()) {
					GetBuffNPCSystem()->UpdateBuffEquipment();
				}
			}
		}
		break;	
#endif

	default:
		sys_err ("Invalid Inventory type %d", window_type);
		return;
	}

	if (GetDesc())
	{
		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;

			pack.count = pItem->GetCount();
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags	= pItem->GetAntiFlag();
#ifdef HIGHLIGHT_SLOT
			pack.highlight = !bWereMine || (Cell.window_type == DRAGON_SOUL_INVENTORY);
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif


			thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
			thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
			pack.vnum = 0;
			memset(pack.alSockets, 0, sizeof(pack.alSockets));
			memset(pack.aAttr, 0, sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell(this, wCell);
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
				pItem->SetWindow(INVENTORY);
#ifdef ENABLE_SPECIAL_INVENTORY
			else if (wCell >= SPECIAL_INVENTORY_SLOT_START && wCell < SPECIAL_INVENTORY_SLOT_END)
				pItem->SetWindow(INVENTORY);
#endif
			else
				pItem->SetWindow(EQUIPMENT);
			break;
		case DRAGON_SOUL_INVENTORY:
			pItem->SetWindow(DRAGON_SOUL_INVENTORY);
			break;
#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			pItem->SetWindow(SWITCHBOT);
			break;
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case BUFF_EQUIPMENT:
			pItem->SetWindow(BUFF_EQUIPMENT);
			break;
#endif
		}
	}
}

LPITEM CHARACTER::GetWear(WORD wCell) const
{
	if (wCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::GetWear: invalid wear cell %d", wCell);
		return NULL;
	}

	return m_pointsInstant.pItems[INVENTORY_MAX_NUM + wCell];
}

void CHARACTER::SetWear(WORD wCell, LPITEM item)
{
	if (wCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
		return;
	}

	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + wCell), item);

	if (!item && wCell == WEAR_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;
	
	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = GetInventoryItem(i)))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#ifdef ENABLE_SWITCHBOT
	for (i = 0; i < SWITCHBOT_SLOT_COUNT; ++i)
	{
		if ((item = GetItem(TItemPos(SWITCHBOT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	for (i = 0; i < BUFF_WINDOW_SLOT_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(BUFF_EQUIPMENT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
}

bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	switch (Cell.window_type)
	{
	case INVENTORY:
		{
			WORD wCell = Cell.cell;
			++iExceptionCell;

			if (Cell.IsBeltInventoryPosition())
			{
				LPITEM beltItem = GetWear(WEAR_BELT);

				if (NULL == beltItem)
					return false;

				if (false == CBeltInventoryHelper::IsAvailableCell(wCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
					return false;

				if (m_pointsInstant.wItemGrid[wCell])
				{
					if (m_pointsInstant.wItemGrid[wCell] == iExceptionCell)
						return true;

					return false;
				}

				if (bSize == 1)
					return true;

			}
#ifdef ENABLE_SPECIAL_INVENTORY
			else if (Cell.IsSpecialInventoryPosition())
			{
				return IsEmptySpecialItemGrid(Cell, bSize, iExceptionCell);
			}
#endif
			else if (wCell >= INVENTORY_MAX_NUM)
				return false;

			if (m_pointsInstant.wItemGrid[wCell])
			{
				if (m_pointsInstant.wItemGrid[wCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;
					BYTE bPage = wCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM);

					do
					{
						BYTE p = wCell + (INVENTORY_WIDTH * j);

						if (p >= INVENTORY_MAX_NUM)
							return false;

						if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) != bPage)
							return false;

						if (m_pointsInstant.wItemGrid[p])
							if (m_pointsInstant.wItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
				else
					return false;
			}
			if (1 == bSize)
				return true;
			else
			{
				int j = 1;
				BYTE bPage = wCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM);

				do
				{
					BYTE p = wCell + (INVENTORY_WIDTH * j);

					if (p >= INVENTORY_MAX_NUM)
						return false;

					if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) != bPage)
						return false;

					if (m_pointsInstant.wItemGrid[p])
						if (m_pointsInstant.wItemGrid[p] != iExceptionCell)
							return false;
				}
				while (++j < bSize);

				return true;
			}
		}
		break;
	case DRAGON_SOUL_INVENTORY:
		{
			WORD wCell = Cell.cell;
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				return false;

			iExceptionCell++;

			if (m_pointsInstant.wDSItemGrid[wCell])
			{
				if (m_pointsInstant.wDSItemGrid[wCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;

					do
					{
						BYTE p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							return false;

						if (m_pointsInstant.wDSItemGrid[p])
							if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
				else
					return false;
			}

			if (1 == bSize)
				return true;
			else
			{
				int j = 1;

				do
				{
					BYTE p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						return false;

					if (m_pointsInstant.wItemGrid[p])
						if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
							return false;
				}
				while (++j < bSize);

				return true;
			}
		}
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		{
		WORD wCell = Cell.cell;
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			return false;
		}

		if (m_pointsInstant.pSwitchbotItems[wCell])
		{
			return false;
		}

		return true;
		}
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		{
			UINT wCell = Cell.cell;
			if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
			{
				return false;
			}

			if (m_pointsInstant.pBuffEquipmentItem[wCell])
			{
				return false;
			}

		return true;
		}
#endif
	}
	return false;
}

#ifdef ENABLE_SPECIAL_INVENTORY

bool CHARACTER::IsEmptySpecialItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	if (bSize > 1)
	{
		return false;
	}

	WORD wCell = Cell.cell;

	SpecialInventoryRange range = SpecialInventoryRange(Cell.GetSpecialInventoryType());
	if (wCell < range.start || wCell >= range.end)
	{
		return false;
	}

	if (m_pointsInstant.wItemGrid[wCell])
	{
		return m_pointsInstant.wItemGrid[wCell] == iExceptionCell;
	}

	return true;
}

int CHARACTER::GetEmptyInventory(LPITEM item) const
{
	int start = 0;
	int end = INVENTORY_MAX_NUM;

	if (item->GetSpecialInventoryType() != -1)
	{
		SpecialInventoryRange range = SpecialInventoryRange(item->GetSpecialInventoryType());
		start = range.start;
		end = range.end;
	}

	for (int i = start; i < end; ++i)
	{
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), item->GetSize()))
		{
			return i;
		}
	}

	return -1;
}
#else
int CHARACTER::GetEmptyInventory(BYTE size) const
{
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
		{
			return i;
		}
	}

	return -1;
}
#endif

int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return -1;
	if (!DragonSoul_IsQualified())
	{
		return -1;
	}
	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
		return -1;

	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
			return i + wBaseCell;

	return -1;
}

void CHARACTER::CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize(DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy(m_pointsInstant.wDSItemGrid, m_pointsInstant.wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

int CHARACTER::CountEmptyInventory() const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem(i))
			count += GetInventoryItem(i)->GetSize();

	return (INVENTORY_MAX_NUM - count);
}

void TransformRefineItem(LPITEM pkOldItem, LPITEM pkNewItem)
{
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
	}
	else
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket(i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}

	}
	pkOldItem->CopyAttributeTo(pkNewItem);
}

void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");
	}
}

void CHARACTER::SetRefineNPC(LPCHARACTER ch)
{
	if ( ch != NULL )
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}
	
	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	DWORD result_vnum = item->GetRefinedVnum();

	long long cost = ComputeRefineFee(prt->cost);

	int RefineChance = GetQuestFlag("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Free advancement is only for weapons up to level 20."));
			return false;
		}

		cost = 0;
		SetQuestFlag("main_quest_lv7.refine_chance", RefineChance - 1);
	}

	if (item->GetCount() > 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot refine a stacked item."));
	}

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No advancement possible."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can't be made better."));
		return false;
	}

	if (GetGold() < cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang for an advancement."));
		return false;
	}

	if (!bMoneyOnly && !RefineChance)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
			if (CountSpecifyItem(prt->materials[i].vnum, item->GetCell()) < prt->materials[i].count)
			{
				if (test_server)
				{
					ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
				}
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough material for an advancement."));
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count, item->GetCell());
	}

	int prob = number(1, 100);

	if (IsRefineThroughGuild() || bMoneyOnly)
		prob -= 10;

	if (prob <= prt->prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			DWORD olditem = item->GetID();
			const char* itemname = item->GetName();

			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			WORD wCell = item->GetCell();
			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, wCell)); 
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			LogManager::instance().RefineLog(GetName(), itemname, olditem, pkNewItem->GetID(), "SUCESS");

			sys_log(0, "Refine Success %d", cost);
			sys_log(0, "PayPee %d", cost);
			PayRefineFee(cost);
			sys_log(0, "PayPee End %d", cost);
		}
		else
		{
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		}
	}
	else
	{
		LogManager::instance().RefineLog(GetName(), item->GetName(), item->GetID(), 0, "FAILED");
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");
		PayRefineFee(cost);
	}

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL = 0,
	HYUNIRON_CHN   = 1,
	YONGSIN_SCROLL = 2,
	MUSIN_SCROLL   = 3,
	YAGONG_SCROLL  = 4,
	MEMO_SCROLL	   = 5,
	BDRAGON_SCROLL	= 6,
};

bool CHARACTER::DoRefineWithScroll(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	LPITEM pkItemScroll;

	if (m_iRefineAdditionalCell < 0)
		return false;

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);

	if (!pkItemScroll)
		return false;

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
		return false;

	if (pkItemScroll->GetVnum() == item->GetVnum())
		return false;

	DWORD result_vnum = item->GetRefinedVnum();
	DWORD result_fail_vnum = item->GetRefineFromVnum();

	if (item->GetCount() > 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot refine a stacked item."));
	}

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No advancement possible."));
		return false;
	}

	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Advancement scroll isn't available for you at the moment."));
			return false;
		}
	}

	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue(1))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't advance with this Scroll."));
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot upgrade this item."));
			return false;
		}
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can't be made better."));
		return false;
	}

	if (GetGold() < prt->cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang for an advancement."));
		return false;
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
		if (CountSpecifyItem(prt->materials[i].vnum, item->GetCell()) < prt->materials[i].count)
		{
			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
			}
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough material for an advancement."));
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count, item->GetCell());

	int prob = number(1, 100);
	int success_prob = prt->prob;
	bool bDestroyWhenFail = false;

	const char* szRefineType = "SCROLL";

	if (pkItemScroll->GetValue(0) == HYUNIRON_CHN || 
		pkItemScroll->GetValue(0) == YONGSIN_SCROLL || 
		pkItemScroll->GetValue(0) == YAGONG_SCROLL)
	{
		const char hyuniron_prob[9] = { 100, 75, 65, 55, 45, 40, 35, 25, 20 };

		const char yagong_prob[9] = { 100, 100, 90, 80, 70, 60, 50, 30, 20 };

		if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			success_prob = hyuniron_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			success_prob = yagong_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else
		{
			if (!pkItemScroll->GetValue(0) == HYUNIRON_CHN)
			{
				sys_err("REFINE : Unknown refine scroll item. Value0: %d", pkItemScroll->GetValue(0));
			}
		}

		if (test_server) 
		{
			ChatPacket(CHAT_TYPE_INFO, "[Only Test] Success_Prob %d, RefineLevel %d ", success_prob, item->GetRefineLevel());
		}
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
			bDestroyWhenFail = true;

		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
		{
			szRefineType = "HYUNIRON";
		}
		else if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			szRefineType = "GOD_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			szRefineType = "YAGONG_SCROLL";
		}
	}

	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}

	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		success_prob = 100;
		szRefineType = "MEMO_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		success_prob = 80;
		szRefineType = "BDRAGON_SCROLL";
	}

	pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);

	if (prob <= success_prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			DWORD olditem = item->GetID();
			const char* itemname = item->GetName();

			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			WORD wCell = item->GetCell();

			NotifyRefineSuccess(this, item, szRefineType);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, wCell)); 
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			LogManager::instance().RefineLog(GetName(), itemname, olditem, pkNewItem->GetID(), "SUCESS");
			PayRefineFee(prt->cost);
		}
		else
		{
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
			DWORD olditem = item->GetID();
			const char* itemname = item->GetName();

			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			WORD wCell = item->GetCell();
			NotifyRefineFail(this, item, szRefineType, -1);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, wCell)); 
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			LogManager::instance().RefineLog(GetName(), itemname, olditem, pkNewItem->GetID(), "FAILED");
			PayRefineFee(prt->cost);
		}
		else
		{
			sys_err("cannot create item %u", result_fail_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else
	{
		NotifyRefineFail(this, item, szRefineType);
		
		PayRefineFee(prt->cost);
	}

	return true;
}

bool CHARACTER::RefineInformation(WORD wCell, BYTE bType, int iAdditionalCell)
{
#ifdef ENABLE_SPECIAL_INVENTORY
	if (wCell > INVENTORY_MAX_NUM)
	{
		if (!(wCell >= SPECIAL_INVENTORY_STONE_START && wCell < SPECIAL_INVENTORY_SLOT_END))
		{
			return false;
		}
	}
#else
	if (wCell > INVENTORY_MAX_NUM)
	{
		return false;
	}
#endif

	LPITEM item = GetInventoryItem(wCell);

	if (!item)
		return false;

	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag("deviltower_zone.can_refine"))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only be rewarded once for the Demon Tower Quest."));
		return false;
	}

	TPacketGCRefineInformation p;

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = wCell;
	p.src_vnum = item->GetVnum();
	p.result_vnum = item->GetRefinedVnum();
	p.type = bType;

	if (p.result_vnum == 0)
	{
		sys_err("RefineInformation p.result_vnum == 0");
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can't be made better."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can't be advanced this way."));
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't combine identical Advancement Scrolls."));
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Blessing Scroll and Magic Metal can be combined"));
				return false;
			}
		}
	}

	CRefineManager & rm = CRefineManager::instance();

	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());

	if (!prt)
	{
		sys_err("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can't be made better."));
		return false;
	}

	if (GetQuestFlag("main_quest_lv7.refine_chance") > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Free advancement is only for weapons up to level 20."));
			return false;
		}
		p.cost = 0;
	}
	else
		p.cost = ComputeRefineFee(prt->cost);

	p.prob = prt->prob;
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy(&p.materials, prt->materials, sizeof(prt->materials));
	}

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(iAdditionalCell);
	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;

	if (pkItem->GetSubType() == USE_TUNING)
	{
		if (pkItem->GetValue(0) == MUSIN_SCROLL)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
		else if (pkItem->GetValue(0) == HYUNIRON_CHN)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_HYUNIRON, pkItem->GetCell());
		else if (pkItem->GetValue(0) == BDRAGON_SCROLL)
		{
			if (pkTarget->GetRefineSet() != 702) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_BDRAGON, pkItem->GetCell());
		}
		else
		{
			if (pkTarget->GetRefineSet() == 501) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_SCROLL, pkItem->GetCell());
		}
	}
	else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET(pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
	{
		bool bHasMetinStone = false;

#ifdef ENABLE_EXTENDED_SOCKETS
		for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
		{
			long socket = pkTarget->GetSocket(i);
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				bHasMetinStone = true;
				break;
			}
		}

		if (bHasMetinStone)
		{
#ifdef ENABLE_EXTENDED_SOCKETS
			for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
			{
				long socket = pkTarget->GetSocket(i);
				if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				{
					AutoGiveItem(socket);
					pkTarget->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
				}
			}
			pkItem->SetCount(pkItem->GetCount() - 1);
			return true;
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no Spirit Stone that can be removed."));
			return false;
		}
	}

	return false;
}

EVENTFUNC(kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "kill_campfire_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) 
	{
		return 0;
	}
	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

bool CHARACTER::GiveRecallItem(LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't save this position."));
		return false;
	}

	int pos;

	if (item->GetCount() == 1)
	{
		item->SetSocket(0, GetX());
		item->SetSocket(1, GetY());
	}
#ifdef ENABLE_SPECIAL_INVENTORY
	else if ((pos = GetEmptyInventory(item)) != -1)
#else
	else if ((pos = GetEmptyInventory(item->GetSize())) != -1)
#endif
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket(0, GetX());
			item2->SetSocket(1, GetY());
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

			item->SetCount(item->GetCount() - 1);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in the inventory."));
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem(LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex(item->GetSocket(0), item->GetSocket(1))) == 0)
		return;

	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
		case 301:
		case 302:
		case 303:
		case 304:
			if( GetLevel() < 90 )
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is lower than the Level of the Item."));
				return;
			}
			else
				break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't teleport to a safe position in a foreign Kingdom."));
		item->SetSocket(0, 0);
		item->SetSocket(1, 0);
	}
	else
	{
		sys_log(1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket(0), item->GetSocket(1));
		WarpSet(item->GetSocket(0), item->GetSocket(1));
		item->SetCount(item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop()
{
	unsigned bodyPart = GetPart(PART_MAIN);
	switch (bodyPart)
	{
		case 0:
		case 1:
		case 2:
			ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
			break;
		default:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("To open a shop, you have to take off the armor."));
			break;
	}
}

void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, long long dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %lld", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}

void CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*)(p + 1);

	if (!p->wCount)
		SendMyShopPriceListCmd(1, 0);
	else {
		for (int idx = 0; idx < p->wCount; idx++)
			SendMyShopPriceListCmd(pInfo[ idx ].dwVnum, pInfo[ idx ].lldPrice);
	}

	__OpenPrivateShop();
}

void CHARACTER::UseSilkBotary(void)
{
	if (m_bNoOpenedShop) {
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof(DWORD));
		m_bNoOpenedShop = false;
	} else {
		__OpenPrivateShop();
	}
}

int CalculateConsume(LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT	= 30;
	static const int WARP_MIN_LIFE_PERCENT	= 10;

	int consumeLife = 0;
	{
		const int curLife		= ch->GetHP();
		const int needPercent	= WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;
		if (curLife < needLife)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough HP."));
			return -1;
		}

		consumeLife = needLife;
		const int minPercent	= WARP_MIN_LIFE_PERCENT;
		const int minLife	= ch->GetMaxHP() * minPercent / 100;
		if (curLife - needLife < minLife)
			consumeLife = curLife - minLife;

		if (consumeLife < 0)
			consumeLife = 0;
	}
	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Spell Points to use this."));
		return -1;
	}

	return needSP;
}

bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limitValue)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is lower than the Level of the Item."));
					return false;
				}
				break;

			case LIMIT_REAL_TIME_START_FIRST_USE:
				iLimitRealtimeStartFirstUseFlagIndex = i;
				break;

			case LIMIT_TIMER_BASED_ON_WEAR:
				iLimitTimerBasedOnWearFlagIndex = i;
				break;
		}
	}

	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		if (0 == item->GetSocket(1))
		{
			long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[iLimitRealtimeStartFirstUseFlagIndex].lValue;

			if (0 == duration)
				duration = 60 * 60 * 24 * 7;

			item->SetSocket(0, time(0) + duration);
			item->StartRealTimeExpireEvent();
		}	

		if (false == item->IsEquipped())
			item->SetSocket(1, item->GetSocket(1) + 1);		
	}

	switch (item->GetType())
	{
		case ITEM_HAIR:
			return ItemProcess_Hair(item, wDestCell);

		case ITEM_POLYMORPH:
			return ItemProcess_Polymorph(item);

		case ITEM_QUEST:
			if (IsObserverMode() == true)
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use this Item in a duel."));
					return false;
				}
			}

			if (!IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
			{
				if (item->GetSIGVnum() == 0)
				{
					quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
				}
				else
				{
					quest::CQuestManager::instance().SIGUse(GetPlayerID(), item->GetSIGVnum(), item, false);
				}
			}
			break;

		case ITEM_CAMPFIRE:
			{
				float fx, fy;
				GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

				LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy));

				if (!tree)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't make a campfire here."));
					return false;
				}

				if (tree->IsAttr((long)(GetX()+fx), (long)(GetY()+fy), ATTR_WATER))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't make a campfire under water."));
					return false;
				}

				LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy), 0, false, number(0, 359));

				char_event_info* info = AllocEventInfo<char_event_info>();

				info->ch = campfire;

				campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

				item->SetCount(item->GetCount() - 1);
			}
			break;

		case ITEM_UNIQUE:
			{
				switch (item->GetSubType())
				{
					case USE_ABILITY_UP:
						{
							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
									EffectPacket(SE_DXUP_PURPLE);
									break;

								case APPLY_ATT_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true, true);
									EffectPacket(SE_SPEEDUP_GREEN);
									break;

								case APPLY_STR:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_RESIST_MAGIC:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS, 
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;
							}
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					default:
						{
							if (item->GetSubType() == USE_SPECIAL)
							{
								sys_log(0, "ITEM_UNIQUE: USE_SPECIAL %u", item->GetVnum());

								switch (item->GetVnum())
								{
									case 71049:
										{
											UseSilkBotary();
										}
										break;
								}
							}
							else
							{
								if (!item->IsEquipped())
									EquipItem(item);
								else
									UnequipItem(item);
							}
						}
						break;
				}
			}
			break;

		case ITEM_COSTUME:
		case ITEM_WEAPON:
		case ITEM_ARMOR:
		case ITEM_ROD:
		case ITEM_RING:
		case ITEM_BELT:
		case ITEM_PICK:
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;

		case ITEM_DS:
			{
				if (!item->IsEquipped())
					return false;
				return DSManager::instance().PullOut(this, NPOS, item);
			break;
			}
		case ITEM_SPECIAL_DS:
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;
		
		case ITEM_FISH:
			{
				if (item->GetSubType() == FISH_ALIVE)
					fishing::UseFish(this, item);
			}
			break;

		case ITEM_TREASURE_BOX:
			{
				return false;
			}
			break;

		case ITEM_TREASURE_KEY:
			{
				LPITEM item2;

				if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging())
					return false;

				if (item2->GetType() != ITEM_TREASURE_BOX)
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("This Item can't be opened with a Key."));
					return false;
				}

				if (item->GetValue(0) == item2->GetValue(0))
				{
					DWORD dwBoxVnum = item2->GetVnum();
					std::vector <DWORD> dwVnums;
					std::vector <DWORD> dwCounts;
					std::vector <LPITEM> item_gets(0);
					int count = 0;

					if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					{
						ITEM_MANAGER::instance().RemoveItem(item);
						ITEM_MANAGER::instance().RemoveItem(item2);
					}
					else
					{
						ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("This Key seems not to fit into the lock."));
						return false;
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("This Key seems not to fit into the lock."));
					return false;
				}
			}
			break;

		case ITEM_GIFTBOX:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if ((dwBoxVnum > 51500 && dwBoxVnum < 52000) || (dwBoxVnum >= 50255 && dwBoxVnum <= 50260))
				{
					if(!(this->DragonSoul_IsQualified()))
					{
						ChatPacket(CHAT_TYPE_INFO,LC_TEXT("Before you open the Cor Draconis, you have to complete the Dragon Stone quest and activate the Dragon Stone Alchemy."));
						return false;
					}
				}

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					item->SetCount(item->GetCount()-1);
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("You received nothing."));
					return false;
				}
			}
			break;

		case ITEM_SKILLFORGET:
			{
				if (!item->GetSocket(0))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					return false;
				}

				DWORD dwVnum = item->GetSocket(0);

				if (SkillLevelDown(dwVnum))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You lowered your Skill Level."));
				}
				else
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't lower your Skill Level."));
			}
			break;

		case ITEM_SKILLBOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
					return false;
				}

				DWORD dwVnum = 0;

				if (item->GetVnum() == 50300)
				{
					dwVnum = item->GetSocket(0);
				}
				else
				{
					dwVnum = item->GetValue(0);
				}

				if (0 == dwVnum)
				{
					ITEM_MANAGER::instance().RemoveItem(item);

					return false;
				}

				if (true == LearnSkillByBook(dwVnum))
					{
						item->SetCount(item->GetCount()-1);
				 
						int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
				 
						SetSkillNextReadTime(dwVnum, get_global_time() + iReadDelay);
					}
				}
				break;

		case ITEM_USE:
			{
				if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
				{
					if (test_server)
						sys_log (0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());

					int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
					int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
					int apply_value = item->GetValue(2);
					int apply_duration = item->GetValue(1);

					switch (item->GetSubType())
					{
						case USE_ABILITY_UP:
							if (FindAffect(affect_type, apply_type))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The effect is already working."));
								return false;
							}

							{
								switch (item->GetValue(0))
								{
									case APPLY_MOV_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
										break;

									case APPLY_ATT_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
										break;

									case APPLY_STR:
									case APPLY_DEX:
									case APPLY_CON:
									case APPLY_INT:
									case APPLY_CAST_SPEED:
									case APPLY_RESIST_MAGIC:
									case APPLY_ATT_GRADE_BONUS:
									case APPLY_DEF_GRADE_BONUS:
										AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
										break;
								}
							}

							if (GetDungeon())
								GetDungeon()->UsePotion(this);

							if (GetWarMap())
								GetWarMap()->UsePotion(this, item);

							item->SetCount(item->GetCount() - 1);
							break;

					case USE_AFFECT :
						{
							if (!IsLoadedAffect())
							{
								return false;
							}

							if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The effect is already working."));
							}
							else
							{

								AddAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false, true);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_POTION_NODELAY:
						{
							bool used = false;

							if (item->GetValue(0) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (GetDungeon())
									GetDungeon()->UsePotion(this);

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								item->SetCount(item->GetCount() - 1);
							}
						}
						break;
					}

					return true;
				}

				if (test_server)
				{
					 sys_log (0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
				}

				switch (item->GetSubType())
				{
					case USE_TIME_CHARGE_PER:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}

							if (pDestItem->IsDragonSoul())
							{
								int ret;

								if (item->GetVnum() == DRAGON_HEART_VNUM)
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
								}
								else
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								}
								if (ret > 0)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds have been added."), ret);
									item->SetCount(item->GetCount() - 1);
									return true;
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot recharge this Dragon Stone."));
									return false;
								}
							}
							else
								return false;
						}
						break;
					case USE_TIME_CHARGE_FIX:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}

							if (pDestItem->IsDragonSoul())
							{
								int ret = pDestItem->GiveMoreTime_Fix(item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));

								if (ret)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds have been added."), ret);
									item->SetCount(item->GetCount() - 1);
									return true;
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot recharge this Dragon Stone."));
									return false;
								}
							}
							else
								return false;
						}
						break;
					case USE_SPECIAL:
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
						 if (item->GetVnum() == ASLAN_BUFF_NPC_SEAL_VNUM)
						{
							if(GetBuffNPCSystem()->IsActive()) {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN"));
								return false;
							}
							
							if (item->GetSocket(0) == 0) {
								SetBuffSealVID(item->GetCell());
								
								TPacketGCBuffNPCAction packet;
								packet.bHeader = HEADER_GC_BUFF_NPC_ACTION;
								packet.bAction = 0;
								packet.dValue0 = 0;
								packet.dValue1 = 0;
								packet.dValue2 = 0;
								packet.dValue3 = 0;
								GetDesc()->Packet(&packet, sizeof(packet));
							}
							else
							{
								DBManager::instance().DirectQuery("UPDATE player.player_buff_npc SET player_name='%s', player_id=%d WHERE id=%d ", GetName(), GetPlayerID(), item->GetSocket(0));
								item->RemoveFromCharacter();
								GetBuffNPCSystem()->LoadBuffNPC();
								SetQuestFlag("buff_npc.is_summon", 1);
								GetBuffNPCSystem()->Summon();
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_ACTIVATE_SEAL"));
							}
						}
#endif
						switch (item->GetVnum())
						{
							case ITEM_CLEAR_ACCE:
								{
									LPITEM pDestItem = GetItem(DestCell);
									if (NULL == pDestItem)
										return false;

									if (pDestItem->GetSubType() != COSTUME_ACCE)
										return false;

									if (pDestItem->IsEquipped())
										return false;

									if (pDestItem->GetSocket(1) == 0)
										return false;

									pDestItem->ClearAttribute();
									pDestItem->SetSocket(1, 0);
									item->SetCount(item->GetCount() - 1);
								}
								break;

							case ITEM_NOG_POCKET:
								{
									if (FindAffect(AFFECT_NOG_ABILITY))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The effect is already working."));
										return false;
									}
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									EffectPacket(SE_DXUP_PURPLE);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;

							case ITEM_RAMADAN_CANDY:
								{
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;

							case ITEM_MARRIAGE_RING:
								{
									marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
									if (pMarriage)
									{
										int consumeSP = CalculateConsumeSP(this);

										if (consumeSP < 0)
											return false;

										PointChange(POINT_SP, -consumeSP, false);

										WarpToPID(pMarriage->GetOther(GetPlayerID()));
									}
									else
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't wear a Wedding Ring if you aren't married."));
								}
								break;

							case UNIQUE_ITEM_CAPE_OF_COURAGE:
							case 70057:
							case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
								AggregateMonster();
								// item->SetCount(item->GetCount()-1);
								break;

							case UNIQUE_ITEM_WHITE_FLAG:
								ForgetMyAttacker();
								item->SetCount(item->GetCount()-1);
								break;

							case UNIQUE_ITEM_TREASURE_BOX:
								break;

							case 30093:
							case 30094:
							case 30095:
							case 30096:
								{
									const int MAX_BAG_INFO = 26;
									static struct LuckyBagInfo
									{
										DWORD count;
										int prob;
										DWORD vnum;
									} b1[MAX_BAG_INFO] =
									{
										{ 1000,	302,	1 },
										{ 10,	150,	27002 },
										{ 10,	75,	27003 },
										{ 10,	100,	27005 },
										{ 10,	50,	27006 },
										{ 10,	80,	27001 },
										{ 10,	50,	27002 },
										{ 10,	80,	27004 },
										{ 10,	50,	27005 },
										{ 1,	10,	50300 },
										{ 1,	6,	92 },
										{ 1,	2,	132 },
										{ 1,	6,	1052 },
										{ 1,	2,	1092 },
										{ 1,	6,	2082 },
										{ 1,	2,	2122 },
										{ 1,	6,	3082 },
										{ 1,	2,	3122 },
										{ 1,	6,	5052 },
										{ 1,	2,	5082 },
										{ 1,	6,	7082 },
										{ 1,	2,	7122 },
										{ 1,	1,	11282 },
										{ 1,	1,	11482 },
										{ 1,	1,	11682 },
										{ 1,	1,	11882 },
									};

									struct LuckyBagInfo b2[MAX_BAG_INFO] =
									{
										{ 1000,	302,	1 },
										{ 10,	150,	27002 },
										{ 10,	75,	27002 },
										{ 10,	100,	27005 },
										{ 10,	50,	27005 },
										{ 10,	80,	27001 },
										{ 10,	50,	27002 },
										{ 10,	80,	27004 },
										{ 10,	50,	27005 },
										{ 1,	10,	50300 },
										{ 1,	6,	92 },
										{ 1,	2,	132 },
										{ 1,	6,	1052 },
										{ 1,	2,	1092 },
										{ 1,	6,	2082 },
										{ 1,	2,	2122 },
										{ 1,	6,	3082 },
										{ 1,	2,	3122 },
										{ 1,	6,	5052 },
										{ 1,	2,	5082 },
										{ 1,	6,	7082 },
										{ 1,	2,	7122 },
										{ 1,	1,	11282 },
										{ 1,	1,	11482 },
										{ 1,	1,	11682 },
										{ 1,	1,	11882 },
									};
	
									LuckyBagInfo * bi = NULL;
									bi = b1;

									int pct = number(1, 1000);

									int i;
									for (i=0;i<MAX_BAG_INFO;i++)
									{
										if (pct <= bi[i].prob)
											break;
										pct -= bi[i].prob;
									}
									if (i>=MAX_BAG_INFO)
										return false;

									if (bi[i].vnum == 50300)
									{
										GiveRandomSkillBook();
									}
									else if (bi[i].vnum == 1)
									{
										PointChange(POINT_GOLD, 1000, true);
									}
									else
									{
										AutoGiveItem(bi[i].vnum, bi[i].count);
									}
									ITEM_MANAGER::instance().RemoveItem(item);
								}
								break;

							case 27989:
							case 76006:
								{
									LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

									if (pMap != NULL)
									{
										item->SetSocket(0, item->GetSocket(0) + 1);

										FFindStone f;
										pMap->for_each(f);

										if (f.m_mapStone.size() > 0)
										{
											auto stone = f.m_mapStone.begin();

											DWORD max = UINT_MAX;
											LPCHARACTER pTarget = stone->second;

											while (stone != f.m_mapStone.end())
											{
												DWORD dist = (DWORD)DISTANCE_SQRT(GetX()-stone->second->GetX(), GetY()-stone->second->GetY());

												if (dist != 0 && max > dist)
												{
													max = dist;
													pTarget = stone->second;
												}
												stone++;
											}

											if (pTarget != NULL)
											{
												int val = 3;

												if (max < 10000) val = 2;
												else if (max < 70000) val = 1;

												ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
														(int)GetDegreeFromPositionXY(GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is currently no Metin Stone on this map."));
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is currently no Metin Stone on this map."));
										}

										if (item->GetSocket(0) >= 6)
										{
											ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
											ITEM_MANAGER::instance().RemoveItem(item);
										}
									}
									break;
								}
								break;

							case 27996:
								item->SetCount(item->GetCount() - 1);
								break;

							case 27987:
								{
									item->SetCount(item->GetCount() - 1);

									int r = number(1, 100);

									if (r <= 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is a Stone inside the Clam."));
										AutoGiveItem(27990);
									}
									else
									{
										const int prob_table[] =
										{
											70, 83, 91
										};

										if (r <= prob_table[0])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Clam completely vanished."));
										}
										else if (r <= prob_table[1])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is a White pearl inside the Clam."));
											AutoGiveItem(27992);
										}
										else if (r <= prob_table[2])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is a Blue Pearl inside the Clam."));
											AutoGiveItem(27993);
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is a Red Pearl inside the Clam."));
											AutoGiveItem(27994);
										}
									}
								}
								break;

							case 71013:
								CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50100:
							case 50101:
							case 50102:
							case 50103:
							case 50104:
							case 50105:
							case 50106:
								CreateFly(item->GetVnum() - 50100 + FLY_FIREWORK1, this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50200:
								{
									__OpenPrivateShop();
								}
								break;

							case fishing::FISH_MIND_PILL_VNUM:
								AddAffect(AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20*60, 0, true);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50301:
							case 50302:
							case 50303:
								{
									if (IsPolymorphed() == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your state as long as you are transformed."));
										return false;
									}

									int lv = GetSkillLevel(SKILL_LEADERSHIP);

									if (lv < item->GetValue(0))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("It isn't easy to understand this Book."));
										return false;
									}

									if (lv >= item->GetValue(1))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Book won't help you."));
										return false;
									}

									if (LearnSkillByBook(SKILL_LEADERSHIP))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

										SetSkillNextReadTime(SKILL_LEADERSHIP, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50304:
							case 50305:
							case 50306:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
										return false;
										
									}
									if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need Level 30 to understand this."));
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need Level 50 to understand this."));
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) >= 2)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't train any more Combos."));
										return false;
									}

									int iPct = item->GetValue(0);

									if (LearnSkillByBook(SKILL_COMBO, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

										SetSkillNextReadTime(SKILL_COMBO, get_global_time() + iReadDelay);
									}
								}
								break;
							case 50311:
							case 50312:
							case 50313:
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already speak this language fluently."));
									return false;
								}
								break;

							case 50061 :
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
										return false;
										
									}
									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 10)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't raise your Skills anymore."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50314: case 50315: case 50316:
							case 50323: case 50324:
							case 50325: case 50326:
								{
									if (IsPolymorphed() == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your state as long as you are transformed."));
										return false;
									}
									
									int iSkillLevelLowLimit = item->GetValue(0);
									int iSkillLevelHighLimit = item->GetValue(1);
									int iPct = MINMAX(0, item->GetValue(2), 100);
									int iLevelLimit = item->GetValue(3);
									DWORD dwSkillVnum = 0;
									
									switch (item->GetVnum())
									{
										case 50314: case 50315: case 50316:
											dwSkillVnum = SKILL_POLYMORPH;
											break;

										case 50323: case 50324:
											dwSkillVnum = SKILL_ADD_HP;
											break;

										case 50325: case 50326:
											dwSkillVnum = SKILL_RESIST_PENETRATE;
											break;

										default:
											return false;
									}

									if (0 == dwSkillVnum)
										return false;

									if (GetLevel() < iLevelLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to raise your Level to understand this Book."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't raise your Skills anymore."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("It isn't easy to understand this Book."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't train further with this Book."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50902:
							case 50903:
							case 50904:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
										return false;
										
									}
									DWORD dwSkillVnum = SKILL_CREATE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum)>=40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't raise your Skills anymore."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);

										if (test_server) 
										{
											ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Success to learn skill ");
										}
									}
									else
									{
										if (test_server) 
										{
											ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Failed to learn skill ");
										}
									}
								}
								break;

							case ITEM_MINING_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
										return false;
										
									}
									DWORD dwSkillVnum = SKILL_MINING;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum)>=40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't raise your Skills anymore."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case ITEM_HORSE_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
										return false;
										
									}
									DWORD dwSkillVnum = SKILL_HORSE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetLevel() < 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have the right Level for Rider Training."));
										return false;
									}

									if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
									{
										if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
										{
											RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You escaped the Evil Ghost Curse with the help of an Exorcism Scroll."));
										}
										else
										{
											SkillLearnWaitMoreTimeMessage(GetSkillNextReadTime(dwSkillVnum) - get_global_time());
											return false;
										}
									}

									if (GetPoint(POINT_HORSE_SKILL) >= 20 || 
											GetSkillLevel(SKILL_HORSE_WILDATTACK) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60 ||
											GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read more Riding Guides."));
										return false;
									}

									if (number(1, 100) <= iPct)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You received Riding Points after reading the Riding Guide."));
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can raise your Riding Skill Level through received Points."));
										PointChange(POINT_HORSE_SKILL, 1);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

										if (!test_server)
											SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not understand the Riding Guide."));
									}

									ITEM_MANAGER::instance().RemoveItem(item);
								}
								break;

							case 70102:
							case 70103:
								{
									if (GetAlignment() >= 0)
										return false;

									int delta = MIN(-GetAlignment(), item->GetValue(0));

									sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);

									UpdateAlignment(delta);
									item->SetCount(item->GetCount() - 1);

									if (delta / 10 > 0)
									{
										ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("Your mind is clear. You can concentrate well now."));
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Rank raised up to %d ."), delta/10);
									}
								}
								break;

							case 71107:
								{
									int val = item->GetValue(0);
									int interval = item->GetValue(1);
									quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
									int last_use_time = pPC->GetFlag("mythical_peach.last_use_time");

									if (!pPC)
										return false;

									if (get_global_time() - last_use_time < interval * 60 * 60)
									{
										if (test_server == false)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use it now."));
											return false;
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Test server time limit passed "));
										}
									}
									
									if (GetAlignment() == 200000)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your rank  cannot raise any more."));
										return false;
									}
									
									if (200000 - GetAlignment() < val * 10)
									{
										val = (200000 - GetAlignment()) / 10;
									}
									
									int old_alignment = GetAlignment() / 10;

									UpdateAlignment(val*10);
									
									item->SetCount(item->GetCount()-1);
									pPC->SetFlag("mythical_peach.last_use_time", get_global_time());

									ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("Your mind is clear. You can concentrate well now."));
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Rank raised up to %d ."), val);
								}
								break;

							case 71109:
							case 72719:
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() == true)
										return false;

									if (item2->GetSocketCount() == 0)
										return false;

									if (item2->IsEquipped())
										return false;

									switch( item2->GetType() )
									{
										case ITEM_WEAPON:
											break;
										case ITEM_ARMOR:
											switch (item2->GetSubType())
											{
											case ARMOR_EAR:
											case ARMOR_WRIST:
											case ARMOR_NECK:
											case ARMOR_PENDANT:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No metin stone to take out."));
												return false;
											}
											break;

										default:
											return false;
									}

									std::stack<long> socket;

									for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										socket.push(item2->GetSocket(i));

									int idx = ITEM_SOCKET_MAX_NUM - 1;

									while (socket.size() > 0)
									{
										if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
											break;

										idx--;
										socket.pop();
									}

									if (socket.size() == 0)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No metin stone to take out."));
										return false;
									}

									LPITEM pItemReward = AutoGiveItem(socket.top());

									if (pItemReward != NULL)
									{
										item2->SetSocket(idx, 1);
										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case 70201:
							case 70202:
							case 70203:
							case 70204:
							case 70205:
							case 70206:
								{
									if (GetPart(PART_HAIR) >= 1001)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot dye or bleach your current Hairstyle."));
									}
									else
									{
										quest::CQuestManager& q = quest::CQuestManager::instance();
										quest::PC* pPC = q.GetPC(GetPlayerID());

										if (pPC)
										{
											int last_dye_level = pPC->GetFlag("dyeing_hair.last_dye_level");

											if (last_dye_level == 0 ||
													last_dye_level+3 <= GetLevel() ||
													item->GetVnum() == 70201)
											{
												SetPart(PART_HAIR, item->GetVnum() - 70201);

												if (item->GetVnum() == 70201)
													pPC->SetFlag("dyeing_hair.last_dye_level", 0);
												else
													pPC->SetFlag("dyeing_hair.last_dye_level", GetLevel());

												item->SetCount(item->GetCount() - 1);
												UpdatePacket();
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to reach Level %d to be able to dye your Hair again."), last_dye_level+3);
											}
										}
									}
								}
								break;

							case ITEM_NEW_YEAR_GREETING_VNUM:
								{
									DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets;
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case ITEM_VALENTINE_ROSE:
							case ITEM_VALENTINE_CHOCOLATE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;


									if (item->GetVnum() == ITEM_VALENTINE_ROSE && SEX_MALE==GET_SEX(this) ||
										item->GetVnum() == ITEM_VALENTINE_CHOCOLATE && SEX_FEMALE==GET_SEX(this))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can be opened by the other gender only."));
										return false;
									}


									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WHITEDAY_CANDY:
							case ITEM_WHITEDAY_ROSE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;


									if (item->GetVnum() == ITEM_WHITEDAY_CANDY && SEX_MALE==GET_SEX(this) ||
										item->GetVnum() == ITEM_WHITEDAY_ROSE && SEX_FEMALE==GET_SEX(this))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can be opened by the other gender only."));
										return false;
									}


									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case 50011:
								{
									DWORD dwBoxVnum = 50011;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("You received nothing."));
										return false;
									}
								}
								break;

							case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
								{
									PointChange(POINT_STAT_RESET_COUNT, 1);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50107:
								{
									EffectPacket(SE_CHINA_FIREWORK);
									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50108:
								{
									EffectPacket(SE_SPIN_TOP);
									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WONSO_BEAN_VNUM:
								PointChange(POINT_HP, GetMaxHP() - GetHP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_SUGAR_VNUM:
								PointChange(POINT_SP, GetMaxSP() - GetSP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_FRUIT_VNUM:
								PointChange(POINT_STAMINA, GetMaxStamina()-GetStamina());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_ELK_VNUM:
								{
									long long	lldGold = item->GetSocket(0);
									ITEM_MANAGER::instance().RemoveItem(item);
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You received %d Yang."), lldGold);
									PointChange(POINT_GOLD, lldGold);
								}
								break;

							case 27995:
								{
								}
								break;

							case 71092 :
								{
									if (m_pkChrTarget != NULL)
									{
										if (m_pkChrTarget->IsPolymorphed())
										{
											m_pkChrTarget->SetPolymorph(0);
											m_pkChrTarget->RemoveAffect(AFFECT_POLYMORPH);
										}
									}
									else
									{
										if (IsPolymorphed())
										{
											SetPolymorph(0);
											RemoveAffect(AFFECT_POLYMORPH);
										}
									}
								}
								break;

							case ITEM_AUTO_HP_RECOVERY_S:
							case ITEM_AUTO_HP_RECOVERY_M:
							case ITEM_AUTO_HP_RECOVERY_L:
							case ITEM_AUTO_HP_RECOVERY_X:
							case ITEM_AUTO_SP_RECOVERY_S:
							case ITEM_AUTO_SP_RECOVERY_M:
							case ITEM_AUTO_SP_RECOVERY_L:
							case ITEM_AUTO_SP_RECOVERY_X:
							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS: 
							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S: 
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS: 
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
							case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
							case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
								{
									EAffectTypes type = AFFECT_NONE;
									bool isSpecialPotion = false;

									switch (item->GetVnum())
									{
										case ITEM_AUTO_HP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_HP_RECOVERY_S:
										case ITEM_AUTO_HP_RECOVERY_M:
										case ITEM_AUTO_HP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
										case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
											type = AFFECT_AUTO_HP_RECOVERY;
											break;

										case ITEM_AUTO_SP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_SP_RECOVERY_S:
										case ITEM_AUTO_SP_RECOVERY_M:
										case ITEM_AUTO_SP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
										case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
											type = AFFECT_AUTO_SP_RECOVERY;
											break;
									}

									if (AFFECT_NONE == type)
										break;

									if (item->GetCount() > 1)
									{
#ifdef ENABLE_SPECIAL_INVENTORY
										int pos = GetEmptyInventory(item);
#else
										int pos = GetEmptyInventory(item->GetSize());
#endif

										if (-1 == pos)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in the inventory."));
											break;
										}

										item->SetCount( item->GetCount() - 1 );

										LPITEM item2 = ITEM_MANAGER::instance().CreateItem( item->GetVnum(), 1 );
										item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

										if (item->GetSocket(1) != 0)
										{
											item2->SetSocket(1, item->GetSocket(1));
										}

										item = item2;
									}

									CAffect* pAffect = FindAffect( type );

									if (NULL == pAffect)
									{
										EPointTypes bonus = POINT_NONE;

										if (true == isSpecialPotion)
										{
											if (type == AFFECT_AUTO_HP_RECOVERY)
											{
												bonus = POINT_MAX_HP_PCT;
											}
											else if (type == AFFECT_AUTO_SP_RECOVERY)
											{
												bonus = POINT_MAX_SP_PCT;
											}
										}

										AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

										item->Lock(true);
										item->SetSocket(0, true);

										AutoRecoveryItemProcess( type );
									}
									else
									{
										if (item->GetID() == pAffect->dwFlag)
										{
											RemoveAffect( pAffect );

											item->Lock(false);
											item->SetSocket(0, false);
										}
										else
										{
											LPITEM old = FindItemByID( pAffect->dwFlag );

											if (NULL != old)
											{
												old->Lock(false);
												old->SetSocket(0, false);
											}

											RemoveAffect( pAffect );

											EPointTypes bonus = POINT_NONE;

											if (true == isSpecialPotion)
											{
												if (type == AFFECT_AUTO_HP_RECOVERY)
												{
													bonus = POINT_MAX_HP_PCT;
												}
												else if (type == AFFECT_AUTO_SP_RECOVERY)
												{
													bonus = POINT_MAX_SP_PCT;
												}
											}

											AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

											item->Lock(true);
											item->SetSocket(0, true);

											AutoRecoveryItemProcess( type );
										}
									}
								}
								break;
						}
						break;

					case USE_CLEAR:
					{
						switch (item->GetVnum())
						{
			#ifdef ENABLE_WOLFMAN
						case 27124:
							RemoveBleeding();
							break;
			#endif
						case 27874:
						default:
							RemoveBadAffect();
							break;
						}
						item->SetCount(item->GetCount() - 1);
					}
					break;

					case USE_INVISIBILITY:
						{
							if (item->GetVnum() == 70026)
							{
								quest::CQuestManager& q = quest::CQuestManager::instance();
								quest::PC* pPC = q.GetPC(GetPlayerID());

								if (pPC != NULL)
								{
									int last_use_time = pPC->GetFlag("mirror_of_disapper.last_use_time");

									if (get_global_time() - last_use_time < 10*60)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use it now."));
										return false;
									}

									pPC->SetFlag("mirror_of_disapper.last_use_time", get_global_time());
								}
							}

							AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_POTION_NODELAY:
						{
							bool used = false;

							if (item->GetValue(0) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (GetDungeon())
									GetDungeon()->UsePotion(this);

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_POTION:
						if (item->GetValue(1) != 0)
						{
							if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
							{
								return false;
							}

							PointChange(POINT_SP_RECOVERY, item->GetValue(1) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_SPUP_BLUE);
						}

						if (item->GetValue(0) != 0)
						{
							if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
							{
								return false;
							}

							PointChange(POINT_HP_RECOVERY, item->GetValue(0) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_HPUP_RED);
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						// item->SetCount(item->GetCount() - 1);
						break;

					case USE_POTION_CONTINUE:
						{
							if (item->GetValue(0) != 0)
							{
								AddAffect(AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue(0), 0, item->GetValue(2), 0, true);
							}
							else if (item->GetValue(1) != 0)
							{
								AddAffect(AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue(1), 0, item->GetValue(2), 0, true);
							}
							else
								return false;
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_ABILITY_UP:
						{
							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
									break;

								case APPLY_ATT_SPEED:
									AddAffect(AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true);
									break;

								case APPLY_STR:
									AddAffect(AFFECT_STR, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_DEX, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_CON, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_INT, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_CAST_SPEED, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_ATT_GRADE, POINT_ATT_GRADE_BONUS, 
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_DEF_GRADE, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;
							}
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_TALISMAN:
						{
							const int TOWN_PORTAL	= 1;
							const int MEMORY_PORTAL = 2;

							if (GetMapIndex() == 200 || GetMapIndex() == 113)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this from your current position."));
								return false;
							}

							if (m_pkWarpEvent)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are ready to teleport so you cannot use the Return Scroll."));
								return false;
							}

							int consumeLife = CalculateConsume(this);

							if (consumeLife < 0)
								return false;

							if (item->GetValue(0) == TOWN_PORTAL)
							{
								if (item->GetSocket(0) == 0)
								{
									if (!GetDungeon())
										if (!GiveRecallItem(item))
											return false;

									PIXEL_POSITION posWarp;

									if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp))
									{
										PointChange(POINT_HP, -consumeLife, false);
										WarpSet(posWarp.x, posWarp.y);
									}
									else
									{
										sys_err("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
									}
								}
								else
								{
									if (test_server)
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are brought back to the place of origin."));	

									ProcessRecallItem(item);
								}
							}
							else if (item->GetValue(0) == MEMORY_PORTAL)
							{
								if (item->GetSocket(0) == 0)
								{
									if (GetDungeon())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%s cannot be used in a dungeon."), item->GetName(), "");
										return false;
									}

									if (!GiveRecallItem(item))
										return false;
								}
								else
								{
									PointChange(POINT_HP, -consumeLife, false);
									ProcessRecallItem(item);
								}
							}
						}
						break;

					case USE_TUNING:
					case USE_DETACHMENT:
						{
							LPITEM item2;

							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsExchanging())
								return false;
	
							if (item2->IsEquipped())
								return false;

							if (CItemVnumHelper::IsAcceItem(item2->GetVnum()))
							{
								return false;
							}

							if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("+3 spirit stones can not be upgraded by this item."));
								return false;
							}
							
							if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)
							{
								if (item->GetVnum() == 71056)
								{
									RefineItem(item, item2);
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The spirit stone can not be advanced by this item."));
								}
							}
							else
							{
								RefineItem(item, item2);
							}
						}
						break;

					case USE_PUT_INTO_BELT_SOCKET:
					case USE_PUT_INTO_RING_SOCKET:
					case USE_PUT_INTO_ACCESSORY_SOCKET:
					case USE_ADD_ACCESSORY_SOCKET:
					case USE_CLEAN_SOCKET:
					case USE_CHANGE_COSTUME_ATTR:
					case USE_RESET_COSTUME_ATTR:
					case USE_CHANGE_ATTRIBUTE:
					case USE_CHANGE_ATTRIBUTE2:
					case USE_ADD_ATTRIBUTE:
					case USE_ADD_ATTRIBUTE2:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

							if (ITEM_COSTUME == item2->GetType() && COSTUME_ACCE == item2->GetSubType())
							{
								return false;
							}

							if (ITEM_COSTUME == item2->GetType() && item->GetVnum() != 70063 && item->GetVnum() != 70064)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the bonus of this Item."));
								return false;
							}

							if (item2->IsExchanging())
								return false;

							if (item2->IsEquipped())
								return false;

							switch (item->GetSubType())
							{
								case USE_CLEAN_SOCKET:
									{
										int i;
#ifdef ENABLE_EXTENDED_SOCKETS
										for (i = 0; i < ITEM_STONES_MAX_NUM; ++i)
#else
										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
#endif
										{
											if (item2->GetSocket(i) == ITEM_BROKEN_METIN_VNUM)
												break;
										}

#ifdef ENABLE_EXTENDED_SOCKETS
										if (i == ITEM_STONES_MAX_NUM)
#else
										if (i == ITEM_SOCKET_MAX_NUM)
#endif
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No Stone there to be cleaned."));
											return false;
										}

										int j = 0;

#ifdef ENABLE_EXTENDED_SOCKETS
										for (i = 0; i < ITEM_STONES_MAX_NUM; ++i)
#else
										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
#endif
										{
											if (item2->GetSocket(i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket(i) != 0)
												item2->SetSocket(j++, item2->GetSocket(i));
										}

#ifdef ENABLE_EXTENDED_SOCKETS
										for (; j < ITEM_STONES_MAX_NUM; ++j)
#else
										for (; j < ITEM_SOCKET_MAX_NUM; ++j)
#endif
										{
											if (item2->GetSocket(j) > 0)
												item2->SetSocket(j, 1);
										}

										item->SetCount(item->GetCount() - 1);

									}
									break;

								case USE_CHANGE_COSTUME_ATTR:
									{
										if (item2->GetType() != ITEM_COSTUME)
											return false;

										if (item2->GetSubType() != COSTUME_BODY && item2->GetSubType() != COSTUME_HAIR && item2->GetSubType() != COSTUME_WEAPON)
											return false;

										if (item2->GetAttributeSetIndex() == -1)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CHANGE_ATTR_CANNOT_CHANGE_THIS_ITEM"));
											return false;
										}

										if (item2->GetAttributeCount() == 0)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CHANGE_ATTR_NO_CHANGE_POSSIBLE"));
											return false;
										}

										item2->ChangeAttribute();
										item->SetCount(item->GetCount() - 1);
									}
									break;

								case USE_RESET_COSTUME_ATTR:
									{
										if (item2->GetType() != ITEM_COSTUME)
											return false;

										if (item2->GetSubType() != COSTUME_BODY && item2->GetSubType() != COSTUME_HAIR && item2->GetSubType() != COSTUME_WEAPON)
											return false;

										if (item2->GetAttributeSetIndex() == -1)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CHANGE_ATTR_CANNOT_CHANGE_THIS_ITEM"));
											return false;
										}

										item2->ClearAttribute();
										item2->AlterToMagicItem(number(40, 50), number(10, 15));
										item->SetCount(item->GetCount() - 1);
									}
									break;

								case USE_CHANGE_ATTRIBUTE :
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the bonus of this Item."));
										return false;
									}

									if (item2->GetAttributeCount() == 0)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have no upgrade you could possibly change."));
										return false;
									}

									if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] = 
										{
											0, 0, 30, 40, 3
										};

										item2->ChangeAttribute(aiChangeProb);
									}
									else if (item->GetVnum() == 76014)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] = 
										{
											0, 10, 50, 39, 1
										};

										item2->ChangeAttribute(aiChangeProb);
									}
									else
									{
										if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
										{
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be used on weapons or armor up to level 40."));
													break;
												}
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be used on accesories."));
												break;
											}
										}

										if (item->GetVnum() == 71052)
										{
											if (item2->ChangeRareAttribute() == true)
											{
												item->SetCount(item->GetCount() - 1);
											}
										}
										else
										{
											item2->ChangeAttribute();
											// item->SetCount(item->GetCount() - 1);
										}
									}

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You changed the item bonus."));
									break;

								case USE_ADD_ATTRIBUTE :
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the bonus of this Item."));
										return false;
									}

									if (item->GetVnum() == 71051)
									{
										if (item2->AddRareAttribute() == true)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Enhancement successfully added."));
											item->SetCount(item->GetCount() - 1);
										}
									}
									else if (item2->GetAttributeCount() < 5)
									{
										if (item->GetVnum() == 71152 || item->GetVnum() == 76024)
										{
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be used on weapons or armor up to level 40."));
													break;
												}
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be used on accesories."));
												break;
											}
										}

#ifdef ENABLE_INSTANT_5_BONUS
										for (int i = 0; i < MAX_NORM_ATTR_NUM; i++)
											item2->AddAttribute();
#else
										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										{
											item2->AddAttribute();
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Enhancement successfully added."));

											int iAddedIdx = item2->GetAttributeCount() - 1;
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Enhancement could not be added."));
										}
#endif
										// item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot add any more attributes by using this item."));
									}
									break;

								case USE_ADD_ATTRIBUTE2 :
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the bonus of this Item."));
										return false;
									}

									if (item2->GetAttributeCount() == 4)
									{
										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										{
											item2->AddAttribute();
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Enhancement successfully added."));

											int iAddedIdx = item2->GetAttributeCount() - 1;
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Enhancement could not be added."));
										}

										item->SetCount(item->GetCount() - 1);
									}
									else if (item2->GetAttributeCount() == 5)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this Item for other Upgrades."));
									}
									else if (item2->GetAttributeCount() < 4)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please use the Advancement Scroll to add another Upgrade."));
									}
									else
									{
										sys_err("ADD_ATTRIBUTE2 : Item has wrong AttributeCount(%d)", item2->GetAttributeCount());
									}
									break;

								case USE_ADD_ACCESSORY_SOCKET:
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (item2->IsAccessoryForSocket())
										{
											if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
												if (number(1, 100) <= 50)
												{
													item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Socket successfully added."));
												}
												else
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Socket could not be added."));
												}

												item->SetCount(item->GetCount() - 1);
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot add more Sockets here."));
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot attach a socket into this Item."));
										}
									}
									break;

								case USE_PUT_INTO_BELT_SOCKET:
								case USE_PUT_INTO_ACCESSORY_SOCKET:
									if (item2->IsAccessoryForSocket() && item->CanPutInto(item2))
									{

										if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
										{
											if (number(1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
											{
												item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1);
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Arming successful."));
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Arming failed."));
											}

											item->SetCount(item->GetCount() - 1);
										}
										else
										{
											if (item2->GetAccessorySocketMaxGrade() == 0)
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to use a Diamond before you can add a Socket."));
											else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Here is no Socket with Gems."));
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to add a Socket if you want to use a Diamond."));
											}
											else
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Here no more Gems can be added."));
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item cannot be equipped."));
									}
									break;
							}
							if (item2->IsEquipped())
							{
								BuffOnAttr_AddBuffsFromItem(item2);
							}
						}
						break;

					case USE_BAIT:
						{

							if (m_pkFishingEvent)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the Bait while fishing."));
								return false;
							}

							LPITEM weapon = GetWear(WEAR_WEAPON);

							if (!weapon || weapon->GetType() != ITEM_ROD)
								return false;

							if (weapon->GetSocket(2))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You exchange the current Bait with %s."), item->GetName());
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Stick the %s Bait to the Hook."), item->GetName());
							}

							weapon->SetSocket(2, item->GetValue(0));
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_MOVE:
					case USE_TREASURE_BOX:
					case USE_MONEYBAG:
						break;

					case USE_AFFECT :
						{
							if (!IsLoadedAffect())
							{
								return false;
							}

							if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The effect is already working."));
							}
							else
							{
								AddAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false);
								// item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_CREATE_STONE:
						AutoGiveItem(number(28000, 28013));
						item->SetCount(item->GetCount() - 1);
						break;

					case USE_RECIPE :
						{
							LPITEM pSource1 = FindSpecifyItem(item->GetValue(1));
							DWORD dwSourceCount1 = item->GetValue(2);

							LPITEM pSource2 = FindSpecifyItem(item->GetValue(3));
							DWORD dwSourceCount2 = item->GetValue(4);

							if (dwSourceCount1 != 0)
							{
								if (pSource1 == NULL)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough ingrediant to creat the potion."));
									return false;
								}
							}

							if (dwSourceCount2 != 0)
							{
								if (pSource2 == NULL)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough ingrediant to creat the potion."));
									return false;
								}
							}

							if (pSource1 != NULL)
							{
								if (pSource1->GetCount() < dwSourceCount1)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough ingredient(%s)."), pSource1->GetName());
									return false;
								}

								pSource1->SetCount(pSource1->GetCount() - dwSourceCount1);
							}

							if (pSource2 != NULL)
							{
								if (pSource2->GetCount() < dwSourceCount2)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough ingredient(%s)."), pSource2->GetName());
									return false;
								}

								pSource2->SetCount(pSource2->GetCount() - dwSourceCount2);
							}

							LPITEM pBottle = FindSpecifyItem(50901);

							if (!pBottle || pBottle->GetCount() < 1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough empty bottles."));
								return false;
							}

							pBottle->SetCount(pBottle->GetCount() - 1);

							if (number(1, 100) > item->GetValue(5))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Failed to create the potion."));
								return false;
							}

							AutoGiveItem(item->GetValue(0));
						}
						break;

					case USE_FLOWER:
					{
						DWORD item_give = item->GetValue(0);

						if (item->GetCount() >= 10)
						{
							if (item->GetCount() == 10)
							{
								item->RemoveFromCharacter();
							}
							else
							{
								item->SetCount(item->GetCount() - 10);
							}
							AutoGiveItem(item_give);
						}
						else
						{
							return false;
						}

					}
					break;
				}
			}
			break;

			case ITEM_METIN:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped())
					return false;

				if (item2->GetType() == ITEM_PICK) return false;
				if (item2->GetType() == ITEM_ROD) return false;

				int i;

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				{
					DWORD dwVnum;   

					if ((dwVnum = item2->GetSocket(i)) <= 2)
						continue;

					TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

					if (!p)
						continue;

					if (item->GetValue(5) == p->alValues[5])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot attach several Spirit Stones of the same kind."));
						return false;
					}
				}

				if (item2->GetType() == ITEM_ARMOR)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_BODY) || !IS_SET(item2->GetWearFlag(), WEARABLE_BODY))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot attach this Stone there."));
						return false;
					}
				}
				else if (item2->GetType() == ITEM_WEAPON)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_WEAPON))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot attach this Stone to a Weapon."));
						return false;
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No slot free."));
					return false;
				}

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					if (item2->GetSocket(i) >= 1 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
					{
						if (number(1, 100) <= 30)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You attached the Stone successfully."));
							item2->SetSocket(i, item->GetVnum());
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not attach the Stone."));
							item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
						}

						ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (METIN)");
						break;
					}

				if (i == ITEM_SOCKET_MAX_NUM)
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No slot free."));
			}
			break;

		case ITEM_AUTOUSE:
		case ITEM_MATERIAL:
		case ITEM_SPECIAL:
		case ITEM_TOOL:
		case ITEM_LOTTERY:
			break;

		case ITEM_TOTEM:
			{
				if (!item->IsEquipped())
					EquipItem(item);
			}
			break;

		case ITEM_BLEND:
			sys_log(0,"ITEM_BLEND!!");
			if (Blend_Item_find(item->GetVnum()))
			{
				int		affect_type		= AFFECT_BLEND;
				if (item->GetSocket(0) >= _countof(aApplyInfo))
				{
					sys_err ("INVALID BLEND ITEM(id : %d, vnum : %d). APPLY TYPE IS %d.", item->GetID(), item->GetVnum(), item->GetSocket(0));
					return false;
				}
				int		apply_type		= aApplyInfo[item->GetSocket(0)].bPointType;
				int		apply_value		= item->GetSocket(1);
				int		apply_duration	= item->GetSocket(2);
				
				if (FindAffect(affect_type, apply_type))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The effect is already working."));
				}
				else
				{
					if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The effect is already working."));
					}
					else
					{
						AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
						// item->SetCount(item->GetCount() - 1);
					}
				}
			}
			break;
		case ITEM_EXTRACT:
			{
				LPITEM pDestItem = GetItem(DestCell);
				if (NULL == pDestItem)
				{
					return false;
				}
				switch (item->GetSubType())
				{
				case EXTRACT_DRAGON_SOUL:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().PullOut(this, NPOS, pDestItem, item);
					}
					return false;
				case EXTRACT_DRAGON_HEART:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().ExtractDragonHeart(this, pDestItem, item);
					}
					return false;
				default:
					return false;
				}
			}
			break;

		case ITEM_GACHA:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				switch (item->GetSubType())
				{
					case USE_GACHA:
						if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
						{
							if (item->GetSocket(0) > 1)
							{
								item->SetSocket(0, item->GetSocket(0) - 1);
							}
							else
							{
								ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (ITEM_GACHA)");
							}
						}
						return false;
					case GEM_LUCKY_BOX_GACHA:
						ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (ITEM_GACHA)");
						return false;
					case SPECIAL_LUCKY_BOX_GACHA:
						ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (ITEM_GACHA)");
						return false;
					default:
						return false;
				}

			}
			break;

		case ITEM_NONE:
			sys_err("Item type NONE %s", item->GetName());
			break;

		default:
			sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
			return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
			return false;

	sys_log(0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
		return false;

#ifdef ENABLE_SWITCHBOT
	if (Cell.IsSwitchbotPosition())
	{
		CSwitchbot* pkSwitchbot = CSwitchbotManager::Instance().FindSwitchbot(GetPlayerID());
		if (pkSwitchbot && pkSwitchbot->IsActive(Cell.cell))
		{
			return false;
		}

#ifdef ENABLE_SPECIAL_INVENTORY
		int iEmptyCell = GetEmptyInventory(item);
#else
		int iEmptyCell = GetEmptyInventory(item->GetSize());
#endif
		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot remove item from switchbot. Inventory is full."));
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif

	if (!item->CanUsedBy(this))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your character class can not use this item."));
		return false;
	}

	if (IsStun())
		return false;

	if (false == FN_check_item_sex(this, item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can be used by the other gender only."));
		return false;
	}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if(Cell.IsBuffEquipmentPosition())
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		int iEmptyCell = GetEmptyInventory(item);
#else
		int iEmptyCell = GetEmptyInventory(item->GetSize());
#endif

		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Inventory is full."));
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif

	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot be used"));
			return false;
		}

		int iPulse = thecore_pulse();

		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use for %d seconds after opening a Warehouse any Return Scroll."), g_nPortalLimitTime);
			return false; 
		}

		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen() || IsAcceWindowOpen())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use the Return Scroll as long as you have opened another window."));
			return false;
		}

		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport for %d seconds after a trade."), g_nPortalLimitTime);
				return false;
			}
		}

		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use a Return Scroll %d seconds after opening a Warehouse."), g_nPortalLimitTime);
				return false;
			}
			
		}

		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;

			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket(0) - GetX();
				y = item->GetSocket(1) - GetY();
			}
			else if (item->GetVnum() == 22000) 
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket(0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket(0) - GetX();
					y = item->GetSocket(1) - GetY();
				}
			}

			nDist = sqrt(pow((float)x,2) + pow((float)y,2));

			if (nDistant > nDist)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use the Return Scroll because the distance is too small."));				
				if (test_server)
					ChatPacket(CHAT_TYPE_INFO, "PossibleDistant %f nNowDist %f", nDistant,nDist); 
				return false;
			}
		}

		if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade you cannot use a Return Scroll for %d seconds."), g_nPortalLimitTime);
			return false;
		}
	}

	if (item->GetVnum() == 50200 || item->GetVnum() == 71049)
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen() || IsAcceWindowOpen())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a storage if another Window is already open."));
			return false;
		}

	}

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item if you're using quests"));
		return false;
	}

	if (IS_SET(item->GetFlag(), ITEM_FLAG_LOG))
	{
		DWORD vid = item->GetVID();
		DWORD oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 32 + 1];
		int len = snprintf(hint, sizeof(hint) - 32, "%s", item->GetName());

		if (len < 0 || len >= (int) sizeof(hint) - 32)
			len = (sizeof(hint) - 32) - 1;

		bool ret = UseItemEx(item, DestCell);

		return (ret);
	}
	else
		return UseItemEx(item, DestCell);
}

bool CHARACTER::DestroyItem(TItemPos Cell)
{
	if (m_iLastItemDestroyPulse > thecore_pulse())
	{
		int deltaInSeconds = ((m_iLastItemDestroyPulse / PASSES_PER_SEC(1)) - (thecore_pulse() / PASSES_PER_SEC(1)));

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can destroy another item in: %d seconds!"), deltaInSeconds);
		return false;
	}

	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		}
		return false;
	}

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (item->GetCount() <= 0)
		return false;

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have destroyed: %s"), item->GetName());

	ITEM_MANAGER::instance().RemoveItem(item);
	m_iLastItemDestroyPulse = thecore_pulse() + PASSES_PER_SEC(1);

	return true;
}

bool CHARACTER::DropItem(TItemPos Cell, WORD wCount)
{
	LPITEM item = NULL; 

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		return false;
	}

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot drop this Item."));
		return false;
	}

	if (wCount == 0 || wCount > item->GetCount())
		wCount = item->GetCount();

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	LPITEM pkItemToDrop;

	if (wCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (wCount == 0)
		{
			if (test_server)
				sys_log(0, "[DROP_ITEM] drop item count == 0");
			return false;
		}

		item->SetCount(item->GetCount() - wCount);
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), wCount);

		FN_copy_item_socket(pkItemToDrop, item);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (pkItemToDrop->AddToGround(GetMapIndex(), pxPos))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The dropped Item will vanish in 3 minutes."));
		pkItemToDrop->StartDestroyEvent(5);

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemToDrop);
	}

	return true;
}

bool CHARACTER::DropGold(long long gold)
{
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't drop gold."));
	return false;
}

bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, WORD count)
{
	LPITEM item = NULL;

	if (!IsValidItemPosition(Cell))
		return false;

	if (!(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (item->GetCount() < count)
		return false;

	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	if (true == item->isLocked())
		return false;

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if(DestCell.IsBuffEquipmentPosition())
	{
		if(DestCell.IsBuffEquipmentPosition() && !Cell.IsEquipPosition()) {
			if(!EquipBuffItem(DestCell.cell,item)) {
				return false;
			}
		}
		else if (Cell.IsBuffEquipmentPosition() && !DestCell.IsEquipPosition()) {
			if(!UnequipBuffItem(Cell.cell,item)) {
				return false;
			}
		}
	}
#endif

	if (!IsValidItemPosition(DestCell))
	{
		return false;
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	if (item->GetSpecialInventoryType() != DestCell.GetSpecialInventoryType())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SPECIAL_INVENTORY_CANNOT_PLACE_HERE"));
		return false;
	}
#endif

	if (Cell.cell == DestCell.cell && Cell.window_type == DestCell.window_type)
	{
		return false;
	}

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		return false;
	}

	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip this item in your belt inventory."));			
		return false;
	}

#ifdef ENABLE_SWITCHBOT
	if (Cell.IsSwitchbotPosition() && CSwitchbotManager::Instance().IsActive(GetPlayerID(), Cell.cell))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move active switchbot item."));
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && !SwitchbotHelper::IsValidItem(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Invalid item type for switchbot."));
		return false;
	}
#endif

	if (Cell.IsEquipPosition() && !CanUnequipNow(item))
		return false;

	if (DestCell.IsEquipPosition())
	{
		if (GetItem(DestCell))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already equipped this kind of Dragon Stone."));
			
			return false;
		}

		EquipItem(item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut(this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}

				if (!DSManager::instance().IsValidCellForThisItem(item, DestCell))
					return false;
			}
		}

		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
			return false;

		LPITEM item2;

		if ((item2 = GetItem(DestCell)) && item != item2 && item2->IsStackable() &&
				!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
				item2->GetVnum() == item->GetVnum())
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket(i) != item->GetSocket(i))
					return false;

			if (count == 0)
				count = item->GetCount();

			sys_log(0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell, 
				DestCell.window_type, DestCell.cell, count);

			count = MIN(MAX_ITEM_STACK - item2->GetCount(), count);

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);
			return true;
		}

#ifdef ENABLE_SPECIAL_INVENTORY
		if (item->GetSpecialInventoryType() != -1)
		{
			if (!IsEmptySpecialItemGrid(DestCell, item->GetSize(), Cell.cell))
			{
				return false;
			}
		}
		else
		{
			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			{
				return false;
			}
		}
#else
		if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
		{
			return false;
		}
#endif

		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log(0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell, 
				DestCell.window_type, DestCell.cell, count);
			
			item->RemoveFromCharacter();
#ifdef HIGHLIGHT_SLOT
			SetItem(DestCell, item, true);
#else
			SetItem(DestCell, item);
#endif

			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
		}
		else if (count < item->GetCount())
		{
			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell, 
				DestCell.window_type, DestCell.cell, count);

			item->SetCount(item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), count);

			FN_copy_item_socket(item2, item);

			item2->AddToCharacter(this, DestCell);
		}
	}

	if (DestCell.IsDefaultInventoryPosition() && Cell.IsEquipPosition())
	{

		if (item->IsCostumeMountItem() && GetHorse())
		{
			HorseSummon(false);
		}

		if (item->IsCostumePetItem() && GetPetSystem())
		{
			GetPetSystem()->HandlePetCostumeItem();
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
	struct FFindOwnership
	{
		LPITEM item;
		LPCHARACTER owner;

		FFindOwnership(LPITEM item) 
			: item(item), owner(NULL)
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (item->IsOwnership(ch))
				owner = ch;
		}
	};

	struct FCountNearMember
	{
		int		total;
		int		x, y;

		FCountNearMember(LPCHARACTER center )
			: total(0), x(center->GetX()), y(center->GetY())
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				total += 1;
		}
	};

	struct FMoneyDistributor
	{
		long long	total;
		LPCHARACTER	c;
		int		x, y;
		long long	lldMoney;

		FMoneyDistributor(LPCHARACTER center, long long lldMoney) 
			: total(0), c(center), x(center->GetX()), y(center->GetY()), lldMoney(lldMoney)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch!=c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_GOLD, lldMoney, true);
				}
		}
	};
}

void CHARACTER::GiveGold(long long lldAmount)
{
	if (lldAmount <= 0)
	{
		return;
	}

	sys_log(0, "GIVE_GOLD: %s %d", GetName(), lldAmount);

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		long long lldTotal = lldAmount;
		long long lldMyAmount = lldTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			long long lldShare = lldTotal / funcCountNearMember.total;
			lldMyAmount -= lldShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, lldShare);

			pParty->ForEachOnlineMember(funcMoneyDist);
		}

		PointChange(POINT_GOLD, lldMyAmount, true);
	}
	else
	{
		PointChange(POINT_GOLD, lldAmount, true);
	}
}

bool CHARACTER::PickupItem(LPITEM item, DWORD dwVID)
{
	if (!item && dwVID)
	{
		item = ITEM_MANAGER::instance().FindByVID(dwVID);
	}

	if (IsObserverMode())
		return false;

	if (!item || !item->GetSectree())
		return false;

	if (item->DistanceValid(this))
	{
		if (item->GetType() == ITEM_QUEST)
		{
			if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot pickup this item if you're using quests"));
				return false;
			}
		}

		if (!dwVID && IsPickupBlockedItem(item->GetVnum()))
		{
			return false;
		}

		if (item->IsOwnership(this))
		{
			if (item->GetType() == ITEM_ELK)
			{
				GiveGold(item->GetCount());
				item->RemoveFromGround();

				M2_DESTROY_ITEM(item);

				Save();
			}
			else
			{
				if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				{
					WORD wCount = item->GetCount();

					for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
					{
						LPITEM item2 = GetInventoryItem(i);

						if (!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
								if (item2->GetSocket(j) != item->GetSocket(j))
									break;

							if (j != ITEM_SOCKET_MAX_NUM)
								continue;

							WORD wCount2 = MIN(MAX_ITEM_STACK - item2->GetCount(), wCount);
							wCount -= wCount2;

							item2->SetCount(item2->GetCount() + wCount2);

							if (wCount == 0)
							{
								if (item->GetCount() > 1)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dx %s received"), item->GetCount(), item2->GetName());
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item2->GetName());
								}

								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
									quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);
								return true;
							}
						}
					}

#ifdef ENABLE_SPECIAL_INVENTORY
					for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
					{
						LPITEM item2 = GetInventoryItem(i);

						if (!item2)
						{
							continue;
						}

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
							{
								if (item2->GetSocket(j) != item->GetSocket(j))
								{
									break;
								}
							}

							if (j != ITEM_SOCKET_MAX_NUM)
							{
								continue;
							}

							WORD wCount2 = MIN(MAX_ITEM_STACK - item2->GetCount(), wCount);
							wCount -= wCount2;

							item2->SetCount(item2->GetCount() + wCount2);

							if (wCount == 0)
							{
								ChatPacket(CHAT_TYPE_INFO, ": %s", item2->GetName());
								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
								{
									quest::CQuestManager::instance().PickupItem(GetPlayerID(), item2);
								}
								return true;
							}
						}
					}
#endif

					item->SetCount(wCount);
				}

				int iEmptyCell;
				if (item->IsDragonSoul())
				{
					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry too many Items."));
						return false;
					}
				}
				else
				{
#ifdef ENABLE_SPECIAL_INVENTORY
					if ((iEmptyCell = GetEmptyInventory(item)) == -1)
#else
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
#endif
					{
						sys_log(0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry too many Items."));
						return false;
					}
				}

				item->RemoveFromGround();
				
				if (item->IsDragonSoul())
					item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
				else
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));

				if (item->GetCount() > 1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dx %s received"), item->GetCount(), item->GetName());
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetName());
				}

				if (item->GetType() == ITEM_QUEST)
					quest::CQuestManager::instance().PickupItem (GetPlayerID(), item);
			}
			return true;
		}
		else if (!IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			NPartyPickupDistribute::FFindOwnership funcFindOwnership(item);
			GetParty()->ForEachOnlineMember(funcFindOwnership);
			LPCHARACTER owner = funcFindOwnership.owner;

			if (!owner)
			{
				return false;
			}

			if (item->IsStackable())
			{
				int iCount = item->GetCount();
				for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
				{
					LPITEM item2 = owner->GetInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == item->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
							if (item2->GetSocket(j) != item->GetSocket(j))
							{
								break;
							}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

						WORD wCount2 = MIN(MAX_ITEM_STACK - item2->GetCount(), iCount);
						iCount -= wCount2;

						item2->SetCount(item2->GetCount() + wCount2);

						if (iCount == 0)
						{
							if (owner == this)
							{
								if (item->GetCount() > 1)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dx %s received"), item->GetCount(), item2->GetName());
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item2->GetName());
								}
							}
							else
							{
								if (item->GetCount() > 1)
								{
									owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Received Item: %s, %dx %s "), GetName(), item->GetCount(), item2->GetName());
								}
								else
								{
									owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Received Item: %s, %s "), GetName(), item2->GetName());
								}
							}
							M2_DESTROY_ITEM(item);
							return true;
						}
					}
				}
				item->SetCount(iCount);
			}

			int iEmptyCell;

			if (item->IsDragonSoul())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyDragonSoulInventory(item)) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry too many Items."));
						return false;
					}
				}
			}
			else
			{
#ifdef ENABLE_SPECIAL_INVENTORY
				if (!(owner && (iEmptyCell = owner->GetEmptyInventory(item)) != -1))
#else
				if (!(owner && (iEmptyCell = owner->GetEmptyInventory(item->GetSize())) != -1))
#endif
				{
					owner = this;

#ifdef ENABLE_SPECIAL_INVENTORY
					if ((iEmptyCell = GetEmptyInventory(item)) == -1)
#else
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
#endif
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry too many Items."));
						return false;
					}
				}
			}

			item->RemoveFromGround();

			if (item->IsDragonSoul())
				item->AddToCharacter(owner, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
			else
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));

			if (owner == this)
			{
				if (item->GetCount() > 1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dx %s received"), item->GetCount(), item->GetName());
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetName());
				}
			}
			else
			{
				if (item->GetCount() > 1)
				{
					owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Received Item: %s, %dx %s "), GetName(), item->GetCount(), item->GetName());
				}
				else
				{
					owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Received Item: %s, %s "), GetName(), item->GetName());
				}
			}

			if (item->GetType() == ITEM_QUEST)
				quest::CQuestManager::instance().PickupItem (owner->GetPlayerID(), item);

			return true;
		}
	}

	return false;
}

bool CHARACTER::SwapItem(WORD wCell, WORD bDestCell)
{
	if (!CanHandleItem())
		return false;

	TItemPos srcCell(INVENTORY, wCell), destCell(INVENTORY, bDestCell);

	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
		return false;

	if (wCell == bDestCell)
		return false;

	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
		return false;

	LPITEM item1, item2;

	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem(bDestCell);
		item2 = GetInventoryItem(wCell);
	}
	else
	{
		item1 = GetInventoryItem(wCell);
		item2 = GetInventoryItem(bDestCell);
	}

	if (!item1 || !item2)
		return false;
	
	if (item1 == item2)
	{
	    sys_log(0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), wCell, bDestCell);
	    return false;
	}

	if (!IsEmptyItemGrid(TItemPos (INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
		return false;

	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		if (item1->GetType() == ITEM_WEAPON && item1->GetSubType() != WEAPON_ARROW && item1->GetSubType() != WEAPON_QUIVER)
		{
			LPITEM pkItem = GetWear(WEAR_COSTUME_WEAPON);
			if (pkItem && item1->GetSubType() != pkItem->GetValue(3))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't wear a costume weapon who has different type of your weapon."));
				return false;
			}
		}

		WORD wEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		WORD wInvenCell = item1->GetCell();

		if (item2->IsDragonSoul() || item2->GetType() == ITEM_BELT)
		{
			if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
				return false;
		}

		if (false == CanUnequipNow(item2, NPOS, NPOS, false) || false == CanEquipNow(item1))
			return false;

		if (wEquipCell != item1->FindEquipCell(this))
			return false;

		item2->RemoveFromCharacter();

		if (item1->EquipTo(this, wEquipCell))
			item2->AddToCharacter(this, TItemPos(INVENTORY, wInvenCell));
		else
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
	}
	else
	{
		WORD wCell1 = item1->GetCell();
		WORD wCell2 = item2->GetCell();
		
		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

		item1->AddToCharacter(this, TItemPos(INVENTORY, wCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, wCell1));
	}

	return true;
}

bool CHARACTER::UnequipItem(LPITEM item)
{
	int pos;

	if (false == CanUnequipNow(item))
		return false;

	if (item->IsDragonSoul())
		pos = GetEmptyDragonSoulInventory(item);
	else
#ifdef ENABLE_SPECIAL_INVENTORY
		pos = GetEmptyInventory(item);
#else
		pos = GetEmptyInventory(item->GetSize());
#endif

	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		ShowAlignment(true);

	item->RemoveFromCharacter();
	if (item->IsDragonSoul())
	{
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos));
	}
	else
	{
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
	}

	if (item->IsCostumeMountItem())
	{
		if (GetHorse())
		{
			HorseSummon(false);
		}
	}

	if (item->IsCostumePetItem())
	{
		CPetSystem* petSystem = GetPetSystem();
		if (petSystem)
		{
			petSystem->HandlePetCostumeItem();
		}
	}

	CheckMaximumPoints();

	return true;
}

bool CHARACTER::EquipItem(LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
		return false;

	if (false == item->IsEquipable())
		return false;

	if (false == CanEquipNow(item))
		return false;

	int iWearCell = item->FindEquipCell(this, iCandidateCell);

	if (iWearCell < 0)
		return false;

	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot wear Tuxido on a horse."));
		return false;
	}

	if (iWearCell != WEAR_ARROW && IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the equipped Items as long as you are transformed."));
		return false;
	}

	if (FN_check_item_sex(this, item) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can be used by the other gender only."));
		return false;
	}

	if(item->IsRideItem() && IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already riding a mount."));
		return false;
	}

	DWORD dwCurTime = get_dword_time();

	if (iWearCell == WEAR_WEAPON && (dwCurTime - GetLastAttackTime() <= 1500 || dwCurTime - m_dwLastSkillTime <= 1500))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to be motionless to equip the Item."));
		return false;
	}

	if (item->IsDragonSoul())
	{
		if(GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
			ChatPacket(CHAT_TYPE_INFO, "이미 같은 종류의 용혼석을 착용하고 있습니다.");
			return false;
		}
		
		if (!item->EquipTo(this, iWearCell))
		{
			return false;
		}
	}
	else if (iWearCell != WEAR_ARROW && !item->IsEquipped() && item->GetCount() > 1)
	{
		item->SetCount(item->GetCount() - 1);
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);
		return item2->EquipTo(this, iWearCell);
	}
	else
	{
		if (GetWear(iWearCell) && !IS_SET(GetWear(iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{

			if (false == SwapItem(item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo(this, iWearCell))
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{
		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{
			if (0 == item->GetSocket(1))
			{
				long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[item->GetProto()->cLimitRealTimeFirstUseIndex].lValue;

				if (0 == duration)
					duration = 60 * 60 * 24 * 7;

				item->SetSocket(0, time(0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket(1, item->GetSocket(1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
			ShowAlignment(false);

		const DWORD& dwVnum = item->GetVnum();

		if (true == CItemVnumHelper::IsRamadanMoonRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_RAMADAN_RING);
		}
		else if (true == CItemVnumHelper::IsHalloweenCandy(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HALLOWEEN_CANDY);
		}
		else if (true == CItemVnumHelper::IsHappinessRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HAPPINESS_RING);
		}
		else if (true == CItemVnumHelper::IsLovePendant(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_LOVE_PENDANT);
		}
		else if (true == CItemVnumHelper::IsAcceItem(dwVnum))
		{
			this->EffectPacket(SE_ACCE_EQUIP);
		}
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(pGroup->GetAttrVnum(item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket(std.c_str());
				}
			}
		}

		if (UNIQUE_SPECIAL_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE))
		{
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
		}
		if (item->IsCostumeMountItem())
		{
			if (GetHorse())
			{
				HorseSummon(false);
			}

			HorseSummon(true);
		}

		if (item->IsCostumePetItem())
		{
			CPetSystem* petSystem = GetPetSystem();
			if (petSystem)
			{
				petSystem->HandlePetCostumeItem();
			}
		}
	}

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem(LPITEM pItem)
{
	for (int i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		auto it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem)
{
	for (int i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		auto it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (auto it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	auto it = m_map_buff_on_attrs.find(bType);

	if (0 == bNewValue)
	{
		if (m_map_buff_on_attrs.end() == it)
			return;
		else
			it->second->Off();
	}
	else if(0 == bOldValue)
	{
		CBuffOnAttributes* pBuff;
		if (m_map_buff_on_attrs.end() == it)
		{
			switch (bType)
			{
			case POINT_ENERGY:
				{
					static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;
			case POINT_COSTUME_ATTR_BONUS:
				{
					static BYTE abSlot[] = { WEAR_COSTUME_BODY, WEAR_COSTUME_HAIR, WEAR_COSTUME_WEAPON };
					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;
			default:
				break;
			}
			m_map_buff_on_attrs.insert(TMapBuffOnAttrs::value_type(bType, pBuff));

		}
		else
			pBuff = it->second;
			
		pBuff->On(bNewValue);
	}
	else
	{
		if (m_map_buff_on_attrs.end() == it)
			return;
		else
			it->second->ChangeBuffValue(bNewValue);
	}
}


LPITEM CHARACTER::FindSpecifyItem(DWORD vnum) const
{
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			return GetInventoryItem(i);

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			return GetInventoryItem(i);
#endif

	return NULL;
}

LPITEM CHARACTER::FindItemByID(DWORD id) const
{
	for (int i=0 ; i < INVENTORY_MAX_NUM ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	for (int i=BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}
#endif

	return NULL;
}

int CHARACTER::CountSpecifyItem(DWORD vnum, int iExceptionCell) const
{
	int	count = 0;
	LPITEM item;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (i == iExceptionCell)
			continue;

		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		item = GetInventoryItem(i);
		if (item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyItem(DWORD vnum, DWORD count, int iExceptionCell)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (i == iExceptionCell)
			continue;

		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		LPITEM item = GetInventoryItem(i);
		if (item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}

//			if (vnum >= 80003 && vnum <= 80007)
//			{
//				LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");
//			}

			if (count >= GetInventoryItem(i)->GetCount())
			{
				count -= GetInventoryItem(i)->GetCount();
				GetInventoryItem(i)->SetCount(0);

				if (0 == count)
				{
					return;
				}
			}
			else
			{
				GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
				return;
			}
		}
	}
#endif

	if (count)
		sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
}

int CHARACTER::CountSpecifyTypeItem(BYTE type) const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem(BYTE type, DWORD count)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetType() != type)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
	{
		if (NULL == GetInventoryItem(i))
		{
			continue;
		}

		if (GetInventoryItem(i)->GetType() != type)
		{
			continue;
		}

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
			{
				continue;
			}
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
			{
				return;
			}
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
#endif
}

void CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip)
{
	if (NULL == item)
	{
		sys_err ("NULL point.");
		return;
	}
	if (item->GetOwner())
	{
		sys_err ("item %d 's owner exists!",item->GetID());
		return;
	}
	
	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory(item);
	}
	else
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		cell = GetEmptyInventory(item);
#else
		cell = GetEmptyInventory(item->GetSize());
#endif
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround (GetMapIndex(), GetXYZ());
		item->StartDestroyEvent();

		if (longOwnerShip)
			item->SetOwnership (this, 300);
		else
			item->SetOwnership (this, 60);
	}
}

LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, WORD wCount, int iRarePct, bool bMsg)
{
	TItemTable * p = ITEM_MANAGER::instance().GetTable(dwItemVnum);

	if (!p)
		return NULL;

	if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND) 
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (wCount < p->alValues[1])
						wCount = p->alValues[1];
				}

				WORD wCount2 = MIN(MAX_ITEM_STACK - item->GetCount(), wCount);
				wCount -= wCount2;

				item->SetCount(item->GetCount() + wCount2);

				if (wCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetName());

					return item;
				}
			}
		}
#ifdef ENABLE_SPECIAL_INVENTORY
		for (int i = SPECIAL_INVENTORY_SLOT_START; i < SPECIAL_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
			{
				continue;
			}

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (wCount < p->alValues[1])
					{
						wCount = p->alValues[1];
					}
				}

				WORD wCount2 = MIN(MAX_ITEM_STACK - item->GetCount(), wCount);
				wCount -= wCount2;
				item->SetCount(item->GetCount() + wCount2);

				if (wCount == 0)
				{
					if (bMsg)
					{
						ChatPacket(CHAT_TYPE_INFO, ": %s", item->GetName());
					}
					return item;
				}
			}
		}
#endif	
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(dwItemVnum, wCount, 0, true);

	if (!item)
	{
		sys_err("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}

	if (item->GetType() == ITEM_BLEND)
	{
		for (int i=0; i < INVENTORY_MAX_NUM; i++)
		{
			LPITEM inv_item = GetInventoryItem(i);

			if (inv_item == NULL) continue;

			if (inv_item->GetType() == ITEM_BLEND)
			{
				if (inv_item->GetVnum() == item->GetVnum())
				{
					if (inv_item->GetSocket(0) == item->GetSocket(0) &&
							inv_item->GetSocket(1) == item->GetSocket(1) &&
							inv_item->GetSocket(2) == item->GetSocket(2) &&
							inv_item->GetCount() + item->GetCount() <= MAX_ITEM_STACK)
					{
						inv_item->SetCount(inv_item->GetCount() + item->GetCount());
						M2_DESTROY_ITEM(item);
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		iEmptyCell = GetEmptyDragonSoulInventory(item);
	}
	else
#ifdef ENABLE_SPECIAL_INVENTORY
		iEmptyCell = GetEmptyInventory(item);
#else
		iEmptyCell = GetEmptyInventory(item->GetSize());
#endif

	if (iEmptyCell != -1)
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));

		if (bMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
		item->StartDestroyEvent();

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
	}

	sys_log(0, 
		"7: %d %d", dwItemVnum, wCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot take this item if you're using quests"));
		return false;
	}

	LPITEM item = GetItem(Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem(this, item))
		{
			victim->ReceiveItem(this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item) const
{
	if (IsPC())
		return false;

	if (DISTANCE_APPROX(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
		return false;

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH && 
					(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				return true;
			break;

		case fishing::FISHER_MOB:
			if (item->GetType() == ITEM_ROD)
				return true;
			break;

		case BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
			if (item->GetType() == ITEM_WEAPON && 
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
			if (item->GetType() == ITEM_ARMOR && 
					(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_ACCESSORY_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetType() == ITEM_ARMOR &&
					!(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_MOB:
			if (item->GetRefinedVnum() && item->GetRefineSet() < 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case GOLDEN_BLACKSMITH_MOB:
			if (item->GetRefinedVnum() && item->GetRefineSet() < 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case BLACKSMITH2_MOB:
			if (item->GetRefineSet() >= 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case ALCHEMIST_MOB:
			if (item->GetRefinedVnum())
				return true;
			break;

		case 20101:
		case 20102:
		case 20103:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a living Horse with Herbs."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20104:
		case 20105:
		case 20106:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a living Horse with Herbs."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20107:
		case 20108:
		case 20109:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a living Horse with Herbs."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				return false;
			}
			break;
	}

	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
	if (IsPC())
		return;

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				fishing::Grill(from, item);
			else
			{
				from->SetQuestNPCID(GetVID());
				quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			}
			break;
 
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum() != 0 && item->GetRefineSet() != 0 && item->GetRefineSet() < 500)
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_MONEY_ONLY);
			}
			else
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can't be made better."));
			}
			break;

		case BLACKSMITH_MOB:
		case GOLDEN_BLACKSMITH_MOB:
		case BLACKSMITH2_MOB:
		case BLACKSMITH_WEAPON_MOB:
		case BLACKSMITH_ARMOR_MOB:
		case BLACKSMITH_ACCESSORY_MOB:
			if (CItemVnumHelper::IsAcceItem(item->GetVnum()))
			{
				return;
			}

			if (item->GetRefinedVnum())
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_NORMAL);
			}
			else
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Item can't be made better."));
			}
			break;

		case 20101:
		case 20102:
		case 20103:
		case 20104:
		case 20105:
		case 20106:
		case 20107:
		case 20108:
		case 20109:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				from->ReviveHorse();
				item->SetCount(item->GetCount()-1);
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You fed the Horse with Herbs."));
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
					item->GetVnum() == ITEM_HORSE_FOOD_2 ||
					item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				from->FeedHorse();
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You fed the Horse."));
				item->SetCount(item->GetCount()-1);
				EffectPacket(SE_HPUP_RED);
			}
			break;

		default:
			sys_log(0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
			from->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			break;
	}
}

bool CHARACTER::IsEquipUniqueItem(DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
		return IsEquipUniqueItem(UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);

	return false;
}

bool CHARACTER::IsEquipUniqueGroup(DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	return false;
}

void CHARACTER::SetRefineMode(int iAdditionalCell)
{
	m_iRefineAdditionalCell = iAdditionalCell;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC( NULL );
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums, 
											std::vector<DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess = false;

	for (int i = 0; i < n; i++)
	{
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
		DWORD dwCount = pGroup->GetCount(idx);
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
			case CSpecialItemGroup::GOLD:
				PointChange(POINT_GOLD, dwCount);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You received %d Yang."), dwCount);
				bSuccess = true;
				break;
			case CSpecialItemGroup::EXP:
				{
					PointChange(POINT_EXP, dwCount);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Mysterious light comes out of this box."));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You received %d Experience Points."), dwCount);
					bSuccess = true;
				}
				break;

			case CSpecialItemGroup::MOB:
				{
					sys_log(0, "CSpecialItemGroup::MOB %d", dwCount);
					int x = GetX() + number(-500, 500);
					int y = GetY() + number(-500, 500);

					LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), x, y, 0, true, -1);
					if (ch)
						ch->SetAggressive();

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A monster came out of the box!"));
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::SLOW:
				{
					sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)dwCount);
					AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the red smoke coming out of the box, you will move faster!"));
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::DRAIN_HP:
				{
					int iDropHP = GetMaxHP()*dwCount/100;
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					iDropHP = MIN(iDropHP, GetHP()-1);
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					PointChange(POINT_HP, -iDropHP);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box exploded suddenly! Your HP are lower."));
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::POISON:
				{
					AttackedByPoison(NULL);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke coming from the box, poison will spread in your body!"));
					bSuccess = true;
				}
				break;

#ifdef ENABLE_WOLFMAN
			case CSpecialItemGroup::BLEEDING:
				{
					AttackedByBleeding(NULL);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke coming from the box, poison will spread in your body!"));
					bSuccess = true;
				}
				break;
#endif

			case CSpecialItemGroup::MOB_GROUP:
				{
					int sx = GetX() - number(300, 500);
					int sy = GetY() - number(300, 500);
					int ex = GetX() + number(300, 500);
					int ey = GetY() + number(300, 500);
					CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A monster came out of the box!"));
					bSuccess = true;
				}
				break;
			default:
				{
					item_get = AutoGiveItem(dwVnum, dwCount, iRarePct, false);

					if (item_get)
					{
						if (dwCount > 1)
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There are %s of %d in the box."), item_get->GetName(), dwCount);
						else
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is %s in the box."), item_get->GetName());
						bSuccess = true;
					}
				}
				break;
		}
	
		if (bSuccess)
		{
			dwItemVnums.push_back(dwVnum);
			dwItemCounts.push_back(dwCount);
			item_gets.push_back(item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You Level is too low to wear this Hairstyle."));
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
		case JOB_WARRIOR :
			hair -= 72000;
			break;

		case JOB_ASSASSIN :
			hair -= 71250;
			break;

		case JOB_SURA :
			hair -= 70500;
			break;

		case JOB_SHAMAN :
			hair -= 69750;
			break;

#ifdef ENABLE_WOLFMAN
		case JOB_WOLFMAN:
			break;
#endif

		default :
			return false;
			break;
	}

	if (hair == GetPart(PART_HAIR))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already wear this Hairstyle."));
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{
	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already transformed."));
		return false;
	}

	if (true == IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not available."));
		return false;
	}

	DWORD dwVnum = item->GetSocket(0);

	if (dwVnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That's the wrong trading item."));
		item->SetCount(item->GetCount()-1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That's the wrong trading item."));
		item->SetCount(item->GetCount()-1);
		return false;
	}

	switch (item->GetVnum())
	{
		case 70104 :
		case 70105 :
		case 70106 :
		case 70107 :
		case 71093 :
			{
				sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);
				int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
				if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't transform into another player who has a higher Level than you."));
					return false;
				}

				int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID)/40 * 25));
				iDuration *= 60;

				DWORD dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40) * 100;

				AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
				AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);
				
				item->SetCount(item->GetCount()-1);
			}
			break;

		case 50322:
			{
				sys_log(0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

				if (CPolymorphUtils::instance().PolymorphCharacter(this, item, pMob) == true)
				{
					CPolymorphUtils::instance().UpdateBookPracticeGrade(this, item);
				}
				else
				{
				}
			}
			break;

		default :
			sys_err("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
			return false;
	}

	return true;
}

bool CHARACTER::CanDoCube() const
{
	if (m_bIsObserver)	return false;
	if (GetShop())		return false;
	if (GetMyShop())	return false;
	if (m_bUnderRefine)	return false;
	if (IsWarping())	return false;
	if (IsAcceWindowOpen())	return false;

	return true;
}

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear(WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear(WEAR_UNIQUE2);

	if( NULL != Unique1 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup() )
		{
			return UnequipItem(Unique1);
		}
	}

	if( NULL != Unique2 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup() )
		{
			return UnequipItem(Unique2);
		}
	}

	return true;
}

void CHARACTER::AutoRecoveryItemProcess(const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
		return;

	if (false == IsPC())
		return;

	if (AFFECT_AUTO_HP_RECOVERY != type && AFFECT_AUTO_SP_RECOVERY != type)
		return;

	if (NULL != FindAffect(AFFECT_STUN))
		return;

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i=0 ; i < sizeof(stunSkills)/sizeof(DWORD) ; ++i)
		{
			const CAffect* p = FindAffect(stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
				return;
		}
	}

	const CAffect* pAffect = FindAffect(type);
	const size_t idx_of_amount_of_used = 1;
	const size_t idx_of_amount_of_full = 2;

	if (NULL != pAffect)
	{
		LPITEM pItem = FindItemByID(pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket(0))
		{
			const long amount_of_used = pItem->GetSocket(idx_of_amount_of_used);
			const long amount_of_full = pItem->GetSocket(idx_of_amount_of_full);
				
			const int32_t avail = amount_of_full - amount_of_used;

			int32_t amount = 0;

			if (AFFECT_AUTO_HP_RECOVERY == type)
			{
				amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
			}
			else if (AFFECT_AUTO_SP_RECOVERY == type)
			{
				amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
			}

			if (amount > 0)
			{
				if (avail > amount)
				{
					const int pct_of_used = amount_of_used * 100 / amount_of_full;
					const int pct_of_will_used = (amount_of_used + amount) * 100 / amount_of_full;

					bool bLog = false;

					if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
						bLog = true;
					pItem->SetSocket(idx_of_amount_of_used, amount_of_used + amount, bLog);
				}
				else
				{
					amount = avail;

					ITEM_MANAGER::instance().RemoveItem( pItem );
				}

				if (AFFECT_AUTO_HP_RECOVERY == type)
				{
					PointChange( POINT_HP_RECOVERY, amount );
					EffectPacket( SE_AUTO_HPUP );
				}
				else if (AFFECT_AUTO_SP_RECOVERY == type)
				{
					PointChange( POINT_SP_RECOVERY, amount );
					EffectPacket( SE_AUTO_SPUP );
				}
			}
		}
		else
		{
			RemoveAffect( const_cast<CAffect*>(pAffect) );
		}
	}
}

bool CHARACTER::IsValidItemPosition(TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;
	
	switch (window_type)
	{
	case RESERVED_WINDOW:
		return false;

	case INVENTORY:
	case EQUIPMENT:
		return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

	case DRAGON_SOUL_INVENTORY:
		return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

	case SAFEBOX:
		if (NULL != m_pkSafebox)
			return m_pkSafebox->IsValidPosition(cell);
		else
			return false;

	case MALL:
		if (NULL != m_pkMall)
			return m_pkMall->IsValidPosition(cell);
		else
			return false;

#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		return cell < SWITCHBOT_SLOT_COUNT;
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		return cell < (BUFF_WINDOW_SLOT_MAX_NUM);
#endif
	default:
		return false;
	}
}

#define VERIFY_MSG(exp, msg)  \
	if (true == (exp)) { \
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT(msg)); \
			return false; \
	}
	
bool CHARACTER::CanEquipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell)
{
	const TItemTable* itemTable = item->GetProto();
	BYTE itemType = item->GetType();
	BYTE itemSubType = item->GetSubType();

	switch (GetJob())
	{
		case JOB_WARRIOR:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;

#ifdef ENABLE_WOLFMAN
		case JOB_WOLFMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
			{
				return false;
			}
			break;
#endif
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Level is too low to equip this."));
					return false;
				}
				break;

			case LIMIT_STR:
				if (GetPoint(POINT_ST) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not strong enough to equip this."));
					return false;
				}
				break;

			case LIMIT_INT:
				if (GetPoint(POINT_IQ) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Intelligence it too low to equip this."));
					return false;
				}
				break;

			case LIMIT_DEX:
				if (GetPoint(POINT_DX) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Agility is too low to equip this."));
					return false;
				}
				break;

			case LIMIT_CON:
				if (GetPoint(POINT_HT) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You Vitality is too low to equip this."));
					return false;
				}
				break;
		}
	}

	if (item->GetType() == ITEM_RING)
	{
		if ((GetWear(WEAR_RING1) && GetWear(WEAR_RING1)->GetVnum() == item->GetVnum()) ||
			(GetWear(WEAR_RING2) && GetWear(WEAR_RING2)->GetVnum() == item->GetVnum()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip this Item twice."));
			return false;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		if ((GetWear(WEAR_UNIQUE1) && GetWear(WEAR_UNIQUE1)->IsSameSpecialGroup(item)) ||
			(GetWear(WEAR_UNIQUE2) && GetWear(WEAR_UNIQUE2)->IsSameSpecialGroup(item)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip this Item twice."));
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) && 
			!marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this Item if you are not married."));
			return false;
		}

	}

	if (item->GetType() == ITEM_WEAPON && item->GetSubType() != WEAPON_ARROW && item->GetSubType() != WEAPON_QUIVER)
	{
		LPITEM pkItem = GetWear(WEAR_COSTUME_WEAPON);
		if (!pkItem)
		{
			return true;
		}
		else if (item->GetSubType() != pkItem->GetValue(3))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't wear a weapon costume which has a different type than your weapon."));
			return false;
		}
	}
	else if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
	{
		LPITEM pkItem = GetWear(WEAR_WEAPON);
		if (!pkItem)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cant wear a weapon costume without wearing a weapon"));
			return false;
		}
		else if (item->GetValue(3) != pkItem->GetSubType())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't wear a weapon costume which has a different type than your weapon."));
			return false;
		}
	}

	if (item->IsCostumeMountItem())
	{
		if (IsRiding())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't equip a mount skin while riding."));
			return false;
		}
	}

	return true;
}

bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell, bool checkCostume)
{
	if (ITEM_BELT == item->GetType())
		VERIFY_MSG(CBeltInventoryHelper::IsExistItemInBeltInventory(this), "You can only discard the belt when there are no longer any items in its inventory.");

	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	if (item->IsCostumeMountItem() && IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't unequip a mount skin while riding."));
		return false;
	}

	{
		int pos = -1;

		if (item->IsDragonSoul())
			pos = GetEmptyDragonSoulInventory(item);
		else
#ifdef ENABLE_SPECIAL_INVENTORY
			pos = GetEmptyInventory(item);
#else
			pos = GetEmptyInventory(item->GetSize());
#endif

		VERIFY_MSG( -1 == pos, "There isn't enough space in the inventory." );
	}

	if (checkCostume && item->GetType() == ITEM_WEAPON && item->GetSubType() != WEAPON_ARROW && item->GetSubType() != WEAPON_QUIVER)
	{
		LPITEM pkItem = GetWear(WEAR_COSTUME_WEAPON);
		if (pkItem)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cant do this until you unwear your weapon costume."));
			return false;
		}
	}

	return true;
}

int GetAcceRefineGrade(DWORD vnum)
{
	if (vnum >= 85001 && vnum <= 85004)
	{
		return vnum - 85000;
	}
	else if (vnum >= 85005 && vnum <= 85008)
	{
		return vnum - 85004;
	}
	else if (vnum >= 85011 && vnum <= 85014)
	{
		return vnum - 85010;
	}
	else if (vnum >= 85015 && vnum <= 85018)
	{
		return vnum - 85014;
	}
	else if (vnum >= 85021 && vnum <= 85024)
	{
		return vnum - 85020;
	}
	else if (vnum >= 86001 && vnum <= 86004)
	{
		return vnum - 86000;
	}
	else if (vnum >= 86005 && vnum <= 86008)
	{
		return vnum - 86004;
	}
	else if (vnum >= 86011 && vnum <= 86014)
	{
		return vnum - 86010;
	}
	else if (vnum >= 86015 && vnum <= 86018)
	{
		return vnum - 86014;
	}
	else if (vnum >= 86021 && vnum <= 86024)
	{
		return vnum - 86020;
	}
	else if (vnum >= 86025 && vnum <= 86028)
	{
		return vnum - 86024;
	}
	else if (vnum >= 86031 && vnum <= 86034)
	{
		return vnum - 86030;
	}
	else if (vnum >= 86035 && vnum <= 86038)
	{
		return vnum - 86034;
	}
	else if (vnum >= 86041 && vnum <= 86044)
	{
		return vnum - 86040;
	}
	else if (vnum >= 86045 && vnum <= 86048)
	{
		return vnum - 86044;
	}
	else if (vnum >= 86051 && vnum <= 86054)
	{
		return vnum - 86050;
	}
	else if (vnum >= 86055 && vnum <= 86058)
	{
		return vnum - 86054;
	}
	else if (vnum >= 86061 && vnum <= 86064)
	{
		return vnum - 86060;
	}
	else
	{
		return 0;
	}
}

void CHARACTER::AcceRefineCheckin(BYTE acceSlot, TItemPos currentCell)
{
	if (GetAcceWindowType() >= 3)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Wrong sash window, detected hacker!"));
		return;
	}

	for (int i = 0; i < ACCE_SLOT_MAX_NUM; i++)
	{
		if (m_pointsInstant.pAcceSlots[i] == currentCell.cell)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This sash is already selected."));
			return;
		}
	}

	if (IsOpenSafebox() || IsCubeOpen())
		return;

	if (!IsValidItemPosition(currentCell))
		return;

	if (IsOpenSafebox())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't do this while using safebox."));
		return;
	}

	if (m_pointsInstant.pAcceSlots[acceSlot] != WORD_MAX)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This slot is already in use."));
		return;
	}

	LPITEM pkItem = GetItem(currentCell);

	if (pkItem == NULL)
		return;

	if (ABSORB == GetAcceWindowType())
	{
		if (pkItem->IsEquipped())
			return;

		if (acceSlot == ACCE_SLOT_RIGHT)
		{
			if (pkItem->GetType() == ITEM_COSTUME)
				return;

			if (pkItem->GetType() != ITEM_WEAPON && pkItem->GetType() != ITEM_ARMOR)
			{
				return;
			}

			if (pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() != ARMOR_BODY)
			{
				return;
			}

			if (pkItem->GetType() == ITEM_WEAPON && pkItem->GetSubType() == WEAPON_ARROW || pkItem->GetSubType() == WEAPON_QUIVER)
				return;
		}
		else if (acceSlot == ACCE_SLOT_LEFT)
		{
			if (pkItem->GetType() != ITEM_COSTUME)
				return;

			if (pkItem->GetSubType() != COSTUME_ACCE)
				return;

			if (!CItemVnumHelper::IsAcceItem(pkItem->GetVnum()))
			{
				return;
			}

			if (pkItem->GetSocket(1) != 0)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This sash already absorbed an item."));
				return;
			}
		}
	}
	else if (COMBINE == GetAcceWindowType())
	{
		if (pkItem->IsEquipped())
			return;

		if (pkItem->GetType() != ITEM_COSTUME && pkItem->GetSubType() != COSTUME_ACCE)
			return;

		if (acceSlot == ACCE_SLOT_LEFT && pkItem->GetSocket(0) == 25)
		{
			ChatPacket(CHAT_TYPE_INFO, "This sash already got the maximum degree, you cant combine it anymore.");
			return;
		}
	}

	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storage> Cannot move items in safebox."));
		return;
	}

	if (true == pkItem->isLocked())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Storages> The Item cannot be stored."));
		return;
	}

	if (COMBINE == GetAcceWindowType())
	{
		if (pkItem->GetType() != ITEM_COSTUME || pkItem->GetSubType() != COSTUME_ACCE || !CItemVnumHelper::IsAcceItem(pkItem->GetVnum()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item is not a sash."));
			return;
		}

		LPITEM pkAcceChosen = NULL;
		if (acceSlot == ACCE_SLOT_LEFT)
		{
			pkAcceChosen = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT]));
		}
		else if (acceSlot == ACCE_SLOT_RIGHT)
		{
			pkAcceChosen = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT]));
		}

		DWORD gradeOfChosen = 0;
		DWORD gradeOfNew = GetAcceRefineGrade(pkItem->GetVnum());

		if (pkAcceChosen != NULL)
		{
			gradeOfChosen = GetAcceRefineGrade(pkAcceChosen->GetVnum());

			if (gradeOfChosen != 0 && gradeOfNew != 0)
			{
				if (gradeOfNew != gradeOfChosen)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't combine sashes with a diffrent grade."));
					return;
				}
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't combine sashes with a diffrent grade."));
				return;
			}

		}
	}

	m_pointsInstant.pAcceSlots[acceSlot] = currentCell.cell;

	if (ABSORB == GetAcceWindowType())
	{
		if (m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT] != WORD_MAX && m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT] != WORD_MAX)
		{
			LPITEM pkAcce = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT]));
			LPITEM pkWeaponToAbsorb = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT]));
			LPITEM pShowItem = ITEM_MANAGER::instance().CreateItem(pkAcce->GetVnum(), 1, 0, false, -1, true);

			pkWeaponToAbsorb->CopyAttributeTo(pShowItem);
			pShowItem->SetSocket(0, pkAcce->GetSocket(0));
			pShowItem->SetSocket(1, pkWeaponToAbsorb->GetVnum());

			TPacketGCAcce pack;
			pack.header = HEADER_GC_ACCE;
			pack.subheader = ACCE_SUBHEADER_GC_SET_ITEM;
			pack.size = sizeof(TPacketGCAcce) + sizeof(TPacketGCItemSet);

			TPacketGCItemSet pack_sub;

			pack_sub.header = 0;
			pack_sub.Cell = TItemPos(ACCE_REFINE, (WORD)ACCE_SLOT_RESULT);
			pack_sub.vnum = pShowItem->GetVnum();
			pack_sub.count = pShowItem->GetCount();
			pack_sub.flags = pShowItem->GetFlag();
			pack_sub.anti_flags = pShowItem->GetAntiFlag();

			thecore_memcpy(pack_sub.alSockets, pShowItem->GetSockets(), sizeof(pack_sub.alSockets));
			thecore_memcpy(pack_sub.aAttr, pShowItem->GetAttributes(), sizeof(pack_sub.aAttr));

			GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCAcce));
			GetDesc()->BufferedPacket(&NPOS, sizeof(TItemPos));
			GetDesc()->Packet(&pack_sub, sizeof(TPacketGCItemSet));

			M2_DESTROY_ITEM(pShowItem);
		}

	}

	if (COMBINE == GetAcceWindowType())
	{
		if (m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT] != WORD_MAX)
		{
			LPITEM pkAcce = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT]));

			TPacketGCAcce pack;
			pack.header = HEADER_GC_ACCE;
			pack.subheader = ACCE_SUBHEADER_GC_SET_ITEM;
			pack.size = sizeof(TPacketGCAcce) + sizeof(TPacketGCItemSet);

			TPacketGCItemSet pack_sub;

			pack_sub.header = 0;
			pack_sub.Cell = TItemPos(ACCE_REFINE, (WORD)ACCE_SLOT_RESULT);
			pack_sub.vnum = pkAcce->GetRefinedVnum() != 0 ? pkAcce->GetRefinedVnum() : pkAcce->GetVnum();
			pack_sub.count = pkAcce->GetCount();
			pack_sub.flags = pkAcce->GetFlag();
			pack_sub.anti_flags = pkAcce->GetAntiFlag();		

			LPITEM pShowItem = ITEM_MANAGER::instance().CreateItem(pkAcce->GetRefinedVnum() != 0 ? pkAcce->GetRefinedVnum() : pkAcce->GetVnum(), 1, 0, false, -1, true);

			pkAcce->CopyAttributeTo(pShowItem);
			pShowItem->SetSocket(1, pkAcce->GetSocket(1));		

			thecore_memcpy(pack_sub.alSockets, pShowItem->GetSockets(), sizeof(pack_sub.alSockets));
			thecore_memcpy(pack_sub.aAttr, pShowItem->GetAttributes(), sizeof(pack_sub.aAttr));

			GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCAcce));
			GetDesc()->BufferedPacket(&NPOS, sizeof(TItemPos));
			GetDesc()->Packet(&pack_sub, sizeof(TPacketGCItemSet));
			M2_DESTROY_ITEM(pShowItem);
		}
	}


	TPacketGCAcce pack;
	pack.header = HEADER_GC_ACCE;
	pack.subheader = ACCE_SUBHEADER_GC_SET_ITEM;
	pack.size = sizeof(TPacketGCAcce) + sizeof(TPacketGCItemSet) + sizeof(TItemPos);

	TPacketGCItemSet pack_sub;

	pack_sub.header = 0;
	pack_sub.Cell = TItemPos(ACCE_REFINE, (WORD)acceSlot);
	pack_sub.vnum = pkItem->GetVnum();
	pack_sub.count = pkItem->GetCount();
	pack_sub.flags = pkItem->GetFlag();
	pack_sub.anti_flags = pkItem->GetAntiFlag();
	thecore_memcpy(pack_sub.alSockets, pkItem->GetSockets(), sizeof(pack_sub.alSockets));
	thecore_memcpy(pack_sub.aAttr, pkItem->GetAttributes(), sizeof(pack_sub.aAttr));

	GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCAcce));
	GetDesc()->BufferedPacket(&currentCell, sizeof(TItemPos));
	GetDesc()->Packet(&pack_sub, sizeof(TPacketGCItemSet));
}

void CHARACTER::AcceRefineCheckout(BYTE acceSlot)
{

	if (m_pointsInstant.pAcceSlots[acceSlot] == WORD_MAX)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This slot is already empty."));
		return;
	}

	LPDESC d = GetDesc();
	if (!d)
		return;

	TPacketGCAcce p;
	p.header = HEADER_GC_ACCE;
	p.size = sizeof(TPacketGCAcce) + sizeof(WORD) + sizeof(TItemPos);
	p.subheader = ACCE_SUBHEADER_GC_CLEAR_SLOT;

	TItemPos src = TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[acceSlot]);

	d->BufferedPacket(&p, sizeof(p));
	d->BufferedPacket(&acceSlot, sizeof(WORD));
	d->Packet(&src, sizeof(TItemPos));

	m_pointsInstant.pAcceSlots[acceSlot] = WORD_MAX;
}

void CHARACTER::AcceRefineAccept(int windowType)
{
	if (ABSORB == windowType)
	{

		if (m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT] == WORD_MAX)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to insert an sash."));
			return;
		}


		if (m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT] == WORD_MAX)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need an item to absorb from."));
			return;
		}

		LPITEM pAcce = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT]));
		LPITEM pAbsorbItem = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT]));

		if (pAcce && pAbsorbItem)
		{
			if (!CItemVnumHelper::IsAcceItem(pAcce->GetVnum()))
			{
				AcceRefineCheckout(ACCE_SLOT_LEFT);
				return;
			}

			WORD TargetCell = pAcce->GetCell();
			pAcce->RemoveFromCharacter();

			pAbsorbItem->CopyAttributeTo(pAcce);
			pAcce->SetSocket(1, pAbsorbItem->GetVnum());

			pAcce->AddToCharacter(this, TItemPos(INVENTORY, TargetCell));
			pAbsorbItem->RemoveFromCharacter();
			ITEM_MANAGER::instance().RemoveItem(pAbsorbItem, "ACCE_ABSORBED");
			AcceRefineCancel();

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The absorb was successful."));
		}

	}
	else if (COMBINE == windowType)
	{

		if (m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT] == WORD_MAX)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to insert an sash."));
			return;
		}
		if (m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT] == WORD_MAX)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need an item to absorb from."));
			return;
		}

		LPITEM pBaseItem = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT]));
		LPITEM pMaterialItem = GetItem(TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT]));

		if (pBaseItem && pMaterialItem)
		{

			if (!CItemVnumHelper::IsAcceItem(pBaseItem->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use non sash items for combine."));
				return;
			}

			if (!CItemVnumHelper::IsAcceItem(pMaterialItem->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use non sash items for combine."));
				return;
			}

			if (GetAcceRefineGrade(pBaseItem->GetVnum()) != GetAcceRefineGrade(pMaterialItem->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't combine sashes with a diffrent grade."));
				return;
			}


			WORD wRefineSet = 0;

			if (GetAcceRefineGrade(pBaseItem->GetVnum()) == 4 && pBaseItem->GetRefineSet() == 0)
			{
				TItemTable* lowerAcce = ITEM_MANAGER::instance().GetTable(pBaseItem->GetVnum() - 1);
				if (lowerAcce)
				{
					wRefineSet = lowerAcce->wRefineSet;
				}
			}
			else
			{
				wRefineSet = pBaseItem->GetRefineSet();
			}

			const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe(wRefineSet);

			if (!prt)
				return;

			DWORD result_vnum = pBaseItem->GetRefinedVnum() != 0 ? pBaseItem->GetRefinedVnum() : pBaseItem->GetVnum();
			long long cost = pBaseItem->GetProto()->lldGold;

			if (GetGold() < cost)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang for an advancement."));
				return;
			}

			int prob = number(1, 100);

			if (prob <= prt->prob)
			{

				LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);
				if (pkNewItem)
				{
					if (GetAcceRefineGrade(pkNewItem->GetVnum()) == 4 && pBaseItem->GetSocket(0) != 25)
					{
						int from = 0;
						int to = 0;
						BYTE newDrain = 0;

						if (GetAcceRefineGrade(pBaseItem->GetVnum()) == 3)
						{
							from = pBaseItem->GetProto()->aApplies[0].lValue + 1;
							to = 20;

							newDrain = number(from, to);

							if (newDrain >= 19)
							{
								ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("The Player %s created a sash with %d%% drain!"), GetName(), newDrain);
							}
						}
						else if (GetAcceRefineGrade(pBaseItem->GetVnum()) == 4)
						{
							from = pBaseItem->GetSocket(0) + 1;
							to = from + 4;

							newDrain = number(from, MIN(25, to));
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New drain %d"), newDrain);
						}

						pkNewItem->SetSocket(0, newDrain);
						if (GetAcceRefineGrade(pBaseItem->GetVnum()) != 4)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The combine of your sash was successful."));
						}
					}

					if (pBaseItem->GetSocket(1) > 0)
					{
						pkNewItem->SetSocket(1, pBaseItem->GetSocket(1));
					}

					if (pBaseItem->GetAttributeCount() > 0)
					{
						pBaseItem->CopyAttributeTo(pkNewItem);
					}

					WORD Cell = pBaseItem->GetCell();

					ITEM_MANAGER::instance().RemoveItem(pMaterialItem, "ACCE_MAT");
					ITEM_MANAGER::instance().RemoveItem(pBaseItem, "ACCE_BASE");

					pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, Cell));
					ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

					PayRefineFee(cost);
					AcceRefineCancel();

					EffectPacket(SE_ACCE_SUCESS_ABSORB);
				}
				else
				{
					sys_err("cannot create item %u", result_vnum);
					AcceRefineCancel();
				}
			}
			else
			{

				ITEM_MANAGER::instance().RemoveItem(pMaterialItem, "ACCE_FAIL");
				AcceRefineCancel();

				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your attempt of combining was a failure."));
			}
		}
	}
}

void CHARACTER::AcceRefineCancel()
{
	TPacketGCAcce pack;
	pack.header = HEADER_GC_ACCE;
	pack.subheader = ACCE_SUBHEADER_GC_CLEAR_ALL;
	pack.size = sizeof(TPacketGCAcce);

	TItemPos srcLeft = NPOS;
	TItemPos srcRight = NPOS;

	if (m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT] != WORD_MAX)
	{
		srcLeft = TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT]);
	}

	if (m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT] != WORD_MAX)
	{
		srcRight = TItemPos(INVENTORY, m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT]);
	}

	GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCAcce));
	GetDesc()->BufferedPacket(&srcLeft, sizeof(TItemPos));
	GetDesc()->Packet(&srcRight, sizeof(TItemPos));
	m_pointsInstant.pAcceSlots[ACCE_SLOT_LEFT] = WORD_MAX;
	m_pointsInstant.pAcceSlots[ACCE_SLOT_RIGHT] = WORD_MAX;
	m_pointsInstant.pAcceSlots[ACCE_SLOT_RESULT] = WORD_MAX;
}

void CHARACTER::AcceClose()
{
	ChatPacket(CHAT_TYPE_COMMAND, "acce close");
}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
static bool FN_check_item_sex_buff(DWORD dwSex, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE)) {
		if (0==dwSex)
			return false;
	}
	
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)) {
		if (1==dwSex)
			return false;
	}

	return true;
}

static bool FN_check_item_level_buff(DWORD dwLevel, LPITEM item)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (dwLevel < limitValue)
					return false;
				break;
		}
	}
	return true;
}

LPITEM CHARACTER::GetBuffWear(UINT bCell) const
{
	if (bCell >= BUFF_WINDOW_SLOT_MAX_NUM)
	{
		sys_err("CHARACTER::GetBuffWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_pointsInstant.pBuffEquipmentItem[bCell];
}

bool CHARACTER::IsBuffEquipEmpty()
{
	for (int i = 0; i < 3; ++i) {
		if (m_pointsInstant.pBuffEquipmentItem[i])
			return false;
	}
	return true;
}

bool CHARACTER::CheckBuffEquipmentPositionAvailable(int iWearCell)
{
	if(iWearCell < 0){
		return false;
	}
	return false;
}

LPITEM CHARACTER::GetBuffEquipmentItem(WORD wCell) const
{
	return GetItem(TItemPos(BUFF_EQUIPMENT,wCell));
}

bool CHARACTER::IsBuffEquipUniqueItem(DWORD dwItemVnum) const
{
	LPITEM item = GetBuffWear(BUFF_WEAR_UNIQUE);

	if (item && item->GetVnum() == dwItemVnum)
		return true;

	return false;
}

bool CHARACTER::EquipBuffItem(BYTE cell, LPITEM item)
{
	if (item->IsExchanging())
		return false;

	int iWearCell = item->FindBuffEquipCell(this);

	if(iWearCell != cell) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_PUT_ITEM_HERE"));
		return false;
	}
	
	if (!GetBuffNPCSystem()->IsActive()) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN"));
		return false;
	}

	if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_THIS_ITEM_IS_NOT_FOR_SHAMAN"));
		return false;
	}

	if (false == FN_check_item_sex_buff(GetBuffNPCSystem()->GetSex(), item)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_THIS_ITEM_IS_NOT_FOR_YOU_GENDER"));
		return false;
	}
	
	if (false == FN_check_item_level_buff(GetBuffNPCSystem()->GetLevel(), item)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_TO_LOW_LEVEL"));
		return false;
	}
	
	if (GetBuffNPCSystem() != NULL) {
		if (GetBuffNPCSystem()->IsSummoned()) {
			item->StartUniqueExpireEvent();	
		}
	}

	return true;
}

bool CHARACTER::UnequipBuffItem(BYTE cell, LPITEM item)
{
	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_REMOVE_ITEM_FROM_SLOT"));
		return false;
	}

	item->StopUniqueExpireEvent();
	
	return true;
}
#endif

