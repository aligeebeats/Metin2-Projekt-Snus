#include "stdafx.h"
#include "../../libgame/src/grid.h"
#include "constants.h"
#include "safebox.h"
#include "packet.h"
#include "char.h"
#include "desc_client.h"
#include "item.h"
#include "item_manager.h"

CSafebox::CSafebox(LPCHARACTER pkChrOwner, int iSize, long long lldGold) : m_pkChrOwner(pkChrOwner), m_iSize(iSize), m_lldGold(lldGold)
{
	assert(m_pkChrOwner);
	memset(m_pkItems, 0, sizeof(m_pkItems));

	for (BYTE i = 0; i < m_iSize / 9; i++)
		v_Grid.emplace_back(std::make_shared<CGrid>(5, 9));

	m_bWindowMode = SAFEBOX;
}

CSafebox::~CSafebox()
{
	__Destroy();
}

static BYTE PosToPage(DWORD dwPos)
{
	return static_cast<BYTE>(dwPos / 45);
}

static BYTE PosToLocal(DWORD dwPos, BYTE Page)
{
	return static_cast<BYTE>(dwPos - (45 * Page));
}

void CSafebox::SetWindowMode(BYTE bMode)
{
	m_bWindowMode = bMode;
}

void CSafebox::__Destroy()
{
	for (int i = 0; i < SAFEBOX_MAX_NUM; ++i) {
		if (m_pkItems[i]) {
			m_pkItems[i]->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(m_pkItems[i]);
			M2_DESTROY_ITEM(m_pkItems[i]->RemoveFromCharacter());
			m_pkItems[i] = NULL;
		}
	}
	v_Grid.clear();
}

bool CSafebox::Add(DWORD dwPos, LPITEM pkItem)
{
	if (!IsValidPosition(dwPos))
	{
		sys_err("SAFEBOX: item on wrong position at %d (size of grid = %d)", dwPos, GetGridTotalSize());
		return false;
	}

	pkItem->SetWindow(m_bWindowMode);
	pkItem->SetCell(m_pkChrOwner, static_cast<WORD>(dwPos));
	pkItem->Save();
	ITEM_MANAGER::instance().FlushDelayedSave(pkItem);

	const BYTE Page = PosToPage(dwPos);
	if (Page >= v_Grid.size())
		return false;

	v_Grid.at(Page)->Put(PosToLocal(dwPos, Page), 1, pkItem->GetSize());

	m_pkItems[dwPos] = pkItem;

	TPacketGCItemSet pack;

	pack.header = m_bWindowMode == SAFEBOX ? HEADER_GC_SAFEBOX_SET : HEADER_GC_MALL_SET;
	pack.Cell = TItemPos(m_bWindowMode, static_cast<WORD>(dwPos));
	pack.vnum = pkItem->GetVnum();
	pack.count = static_cast<WORD>(pkItem->GetCount());
	pack.flags = pkItem->GetFlag();
	pack.anti_flags = pkItem->GetAntiFlag();
	thecore_memcpy(pack.alSockets, pkItem->GetSockets(), sizeof(pack.alSockets));
	thecore_memcpy(pack.aAttr, pkItem->GetAttributes(), sizeof(pack.aAttr));

	m_pkChrOwner->GetDesc()->Packet(&pack, sizeof(pack));
	sys_log(1, "SAFEBOX: ADD %s %s count %d", m_pkChrOwner->GetName(), pkItem->GetName(), pkItem->GetCount());
	return true;
}

LPITEM CSafebox::Get(DWORD dwPos)
{
	if (dwPos >= GetGridTotalSize())
		return nullptr;
	return m_pkItems[dwPos];
}

LPITEM CSafebox::Remove(DWORD dwPos)
{
	LPITEM pkItem = Get(dwPos);

	if (!pkItem)
		return nullptr;

	const BYTE Page = PosToPage(dwPos);
	if (!v_Grid.empty() && Page < v_Grid.size())
		v_Grid.at(Page)->Get(PosToLocal(dwPos, Page), 1, pkItem->GetSize());

	pkItem->RemoveFromCharacter();

	m_pkItems[dwPos] = nullptr;

	TPacketGCItemDel pack;

	pack.header = m_bWindowMode == SAFEBOX ? HEADER_GC_SAFEBOX_DEL : HEADER_GC_MALL_DEL;
	pack.pos = static_cast<BYTE>(dwPos);

	m_pkChrOwner->GetDesc()->Packet(&pack, sizeof(pack));
	sys_log(1, "SAFEBOX: REMOVE %s %s count %d", m_pkChrOwner->GetName(), pkItem->GetName(), pkItem->GetCount());
	return pkItem;
}

void CSafebox::Save()
{
	TSafeboxTable t;

	memset(&t, 0, sizeof(TSafeboxTable));

	t.dwID = m_pkChrOwner->GetDesc()->GetAccountTable().id;
	t.lldGold = m_lldGold;

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_SAVE, 0, &t, sizeof(TSafeboxTable));
	sys_log(1, "SAFEBOX: SAVE %s", m_pkChrOwner->GetName());
}

bool CSafebox::IsEmpty(DWORD dwPos, BYTE bSize)
{
	if (v_Grid.empty())
		return false;

	const BYTE Page = PosToPage(dwPos);
	if (Page >= v_Grid.size())
		return false;

	return v_Grid.at(Page)->IsEmpty(PosToLocal(dwPos, Page), 1, bSize);
}

#if defined(ITEM_CHECKINOUT_UPDATE)
int CSafebox::GetEmptySafebox(BYTE size)
{
	if (v_Grid.empty())
		return -1;

	for (unsigned int i = 0; i < GetGridTotalSize(); i++)
		if (IsEmpty(i, size))
			return i;

	return -1;
}
#endif

void CSafebox::ChangeSize(int iSize)
{
	return;
}

LPITEM CSafebox::GetItem(BYTE bCell)
{
	if (bCell >= 5 * m_iSize)
	{
		sys_err("CHARACTER::GetItem: invalid item cell %d", bCell);
		return NULL;
	}

	return m_pkItems[bCell];
}

bool CSafebox::MoveItem(BYTE bCell, BYTE bDestCell, WORD count)
{
	LPITEM item;

	int max_position = 5 * m_iSize;

	if (bCell >= max_position || bDestCell >= max_position)
		return false;

	if (!(item = GetItem(bCell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (item->GetCount() < count)
		return false;

	{
		LPITEM item2;

		if ((item2 = GetItem(bDestCell)) && item != item2 && item2->IsStackable() &&
			!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
			item2->GetVnum() == item->GetVnum())
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket(i) != item->GetSocket(i))
					return false;

			if (count == 0)
				count = static_cast<WORD>(item->GetCount());

			count = MIN(MAX_ITEM_STACK - item2->GetCount(), count);

			if (item->GetCount() >= count)
				Remove(bCell);

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);

			sys_log(1, "SAFEBOX: STACK %s %d -> %d %s count %d", m_pkChrOwner->GetName(), bCell, bDestCell, item2->GetName(), item2->GetCount());
			return true;
		}

		if (!IsEmpty(bDestCell, item->GetSize()))
			return false;

		const BYTE Page1 = PosToPage(bCell), Page2 = PosToPage(bDestCell);
		if (Page1 >= v_Grid.size() || Page2 >= v_Grid.size())
			return false;

		v_Grid.at(Page1)->Get(PosToLocal(bCell, Page1), 1, item->GetSize());

		if (!v_Grid.at(Page2)->Put(PosToLocal(bDestCell, Page2), 1, item->GetSize()))
		{
			v_Grid.at(Page1)->Put(PosToLocal(bCell, Page1), 1, item->GetSize());
			return false;
		}
		else
		{
			v_Grid.at(Page2)->Get(PosToLocal(bDestCell, Page2), 1, item->GetSize());
			v_Grid.at(Page1)->Put(PosToLocal(bCell, Page1), 1, item->GetSize());
		}

		sys_log(1, "SAFEBOX: MOVE %s %d -> %d %s count %d", m_pkChrOwner->GetName(), bCell, bDestCell, item->GetName(), item->GetCount());

		Remove(bCell);
		Add(bDestCell, item);
	}

	return true;
}

unsigned int CSafebox::GetGridTotalSize() const
{
	return v_Grid.size() * 45;
}

bool CSafebox::IsValidPosition(DWORD dwPos)
{
	if (v_Grid.empty())
		return false;

	if (dwPos >= GetGridTotalSize())
		return false;

	return true;
}
