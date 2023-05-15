#include "stdafx.h"
#include "../../libgame/src/grid.h"
#include "utils.h"
#include "desc.h"
#include "desc_client.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "locale_service.h"
#include "../../common/length.h"
#include "exchange.h"
#include "DragonSoul.h"
#include "questmanager.h"

void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, long long arg1, TItemPos arg2, DWORD arg3, void * pvData = NULL);
void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, long long arg1, TItemPos arg2, DWORD arg3, void * pvData)
{
	if (!ch->GetDesc())
		return;

	struct packet_exchange pack_exchg;

	pack_exchg.header 		= HEADER_GC_EXCHANGE;
	pack_exchg.sub_header 	= sub_header;
	pack_exchg.is_me		= is_me;
	pack_exchg.arg1		= arg1;
	pack_exchg.arg2		= arg2;
	pack_exchg.arg3		= arg3;

	if (sub_header == EXCHANGE_SUBHEADER_GC_ITEM_ADD && pvData)
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(((LPITEM)pvData)->GetWindow(), ((LPITEM)pvData)->GetCell());
#endif
		thecore_memcpy(&pack_exchg.alSockets, ((LPITEM) pvData)->GetSockets(), sizeof(pack_exchg.alSockets));
		thecore_memcpy(&pack_exchg.aAttr, ((LPITEM) pvData)->GetAttributes(), sizeof(pack_exchg.aAttr));
	}
	else
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(RESERVED_WINDOW, 0);
#endif
		memset(&pack_exchg.alSockets, 0, sizeof(pack_exchg.alSockets));
		memset(&pack_exchg.aAttr, 0, sizeof(pack_exchg.aAttr));
	}

	ch->GetDesc()->Packet(&pack_exchg, sizeof(pack_exchg));
}

bool CHARACTER::ExchangeStart(LPCHARACTER victim)
{
	if (this == victim)
		return false;

	if (IsObserverMode())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade while observing."));
		return false;
	}

	if (victim->IsNPC())
		return false;

	if ( IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || IsAcceWindowOpen())
	{
		ChatPacket( CHAT_TYPE_INFO, LC_TEXT("If the Trade Window is open, you cannot trade with others." ) );
		return false;
	}

	if ( victim->IsOpenSafebox() || victim->GetShopOwner() || victim->GetMyShop() || victim->IsCubeOpen() || victim->IsAcceWindowOpen())
	{
		ChatPacket( CHAT_TYPE_INFO, LC_TEXT("The other person is already trading." ) );
		return false;
	}

	int iDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());

	if (iDist >= EXCHANGE_MAX_DISTANCE)
		return false;

	if (GetExchange())
		return false;

	if (victim->GetExchange())
	{
		exchange_packet(this, EXCHANGE_SUBHEADER_GC_ALREADY, 0, 0, NPOS, 0);
		return false;
	}

	if (victim->IsBlockMode(BLOCK_EXCHANGE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person cancelled the Trade."));
		return false;
	}

	SetExchange(M2_NEW CExchange(this));
	victim->SetExchange(M2_NEW CExchange(victim));

	victim->GetExchange()->SetCompany(GetExchange());
	GetExchange()->SetCompany(victim->GetExchange());

	SetExchangeTime();
	victim->SetExchangeTime();

	exchange_packet(victim, EXCHANGE_SUBHEADER_GC_START, 0, GetVID(), NPOS, 0);
	exchange_packet(this, EXCHANGE_SUBHEADER_GC_START, 0, victim->GetVID(), NPOS, 0);

	return true;
}

CExchange::CExchange(LPCHARACTER pOwner)
{
	m_pCompany = NULL;

	m_bAccept = false;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		m_apItems[i] = NULL;
		m_aItemPos[i] = NPOS;
		m_abItemDisplayPos[i] = 0;
	}

	m_lGold = 0;

	m_pOwner = pOwner;
	pOwner->SetExchange(this);

	m_pGrid = M2_NEW CGrid(4,3);
}

CExchange::~CExchange()
{
	M2_DELETE(m_pGrid);
}

#if defined(ITEM_CHECKINOUT_UPDATE)
int CExchange::GetEmptyExchange(BYTE size)
{
	for (unsigned int i = 0; m_pGrid && i < m_pGrid->GetSize(); i++)
		if (m_pGrid->IsEmpty(i, 1, size))
			return i;

	return -1;
}

bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos, bool SelectPosAuto)
#else
bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos)
#endif
{
	assert(m_pOwner != NULL && GetCompany());

	if (!item_pos.IsValidItemPosition())
		return false;

	if (item_pos.IsEquipPosition())
		return false;

	LPITEM item;

	if (!(item = m_pOwner->GetItem(item_pos)))
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
	{
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade this Item."));
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (item->IsExchanging())
	{
		sys_log(0, "EXCHANGE under exchanging");
		return false;
	}

#if defined(ITEM_CHECKINOUT_UPDATE)
	if (SelectPosAuto)
	{
		int AutoPos = GetEmptyExchange(item->GetSize());
		if (AutoPos == -1)
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "<EXCHANGE> You don't have enough space.");
			return false;
		}
		display_pos = AutoPos;
	}
#endif

	if (!m_pGrid->IsEmpty(display_pos, 1, item->GetSize()))
	{
		sys_log(0, "EXCHANGE not empty item_pos %d %d %d", display_pos, 1, item->GetSize());
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			continue;

		m_apItems[i]		= item;
		m_aItemPos[i]		= item_pos;
		m_abItemDisplayPos[i]	= display_pos;
		m_pGrid->Put(display_pos, 1, item->GetSize());

		item->SetExchanging(true);

		exchange_packet(m_pOwner, 
				EXCHANGE_SUBHEADER_GC_ITEM_ADD,
				true,
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		exchange_packet(GetCompany()->GetOwner(),
				EXCHANGE_SUBHEADER_GC_ITEM_ADD, 
				false, 
				item->GetVnum(),
				TItemPos(RESERVED_WINDOW, display_pos),
				item->GetCount(),
				item);

		sys_log(0, "EXCHANGE AddItem success %s pos(%d, %d) %d", item->GetName(), item_pos.window_type, item_pos.cell, display_pos);

		return true;
	}

	return false;
}

bool CExchange::RemoveItem(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
		return false;

	if (!m_apItems[pos])
		return false;

	TItemPos PosOfInventory = m_aItemPos[pos];
	m_apItems[pos]->SetExchanging(false);

	m_pGrid->Get(m_abItemDisplayPos[pos], 1, m_apItems[pos]->GetSize());

	exchange_packet(GetOwner(),	EXCHANGE_SUBHEADER_GC_ITEM_DEL, true, pos, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ITEM_DEL, false, pos, PosOfInventory, 0);

	Accept(false);
	GetCompany()->Accept(false);

	m_apItems[pos]	    = NULL;
	m_aItemPos[pos]	    = NPOS;
	m_abItemDisplayPos[pos] = 0;
	return true;
}

bool CExchange::AddGold(long long gold)
{
	if (gold <= 0)
	{
		return false;
	}

	if (GetOwner()->GetGold() < gold)
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
		return false;
	}

	if ( m_lGold > 0 )
	{
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	m_lGold = gold;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, true, m_lGold, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, false, m_lGold, NPOS, 0);
	return true;
}

bool CExchange::Check(int * piItemCount)
{
	if (GetOwner()->GetGold() < m_lGold)
		return false;

	int item_count = 0;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
			continue;

		if (!m_aItemPos[i].IsValidItemPosition())
			return false;

		if (m_apItems[i] != GetOwner()->GetItem(m_aItemPos[i]))
			return false;

		++item_count;
	}

	*piItemCount = item_count;
	return true;
}

bool CExchange::CheckSpace()
{
	static CGrid s_grid1(INVENTORY_WIDTH, INVENTORY_MAX_NUM / INVENTORY_WIDTH / INVENTORY_PAGE_NUM);
	static CGrid s_grid2(INVENTORY_WIDTH, INVENTORY_MAX_NUM / INVENTORY_WIDTH / INVENTORY_PAGE_NUM);
	static CGrid s_grid3(INVENTORY_WIDTH, INVENTORY_MAX_NUM / INVENTORY_WIDTH / INVENTORY_PAGE_NUM);
	static CGrid s_grid4(INVENTORY_WIDTH, INVENTORY_MAX_NUM / INVENTORY_WIDTH / INVENTORY_PAGE_NUM);

	s_grid1.Clear();
	s_grid2.Clear();
	s_grid3.Clear();
	s_grid4.Clear();

	LPCHARACTER	victim = GetCompany()->GetOwner();
	LPITEM item;

	int i;

	const int perPageSlotCount = INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM;

	for (i = 0; i < INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		s_grid1.Put(i, 1, item->GetSize());
	}
	for (i = (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) * 1; i < (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) * 2; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		s_grid2.Put(i - (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) * 1, 1, item->GetSize());
	}
	for (i = (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) * 2; i < (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) * 3; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		s_grid3.Put(i - (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) * 2, 1, item->GetSize());
	}
	for (i = (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) * 3; i < INVENTORY_MAX_NUM; ++i)
	{
		if (!(item = victim->GetInventoryItem(i)))
			continue;

		s_grid4.Put(i - (INVENTORY_MAX_NUM / INVENTORY_PAGE_NUM) * 3, 1, item->GetSize());
	}

	static std::vector <WORD> s_vDSGrid(DRAGON_SOUL_INVENTORY_MAX_NUM);
	bool bDSInitialized = false;
	
	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

		if (item->IsDragonSoul())
		{
			if (!victim->DragonSoul_IsQualified())
			{
				return false;
			}

			if (!bDSInitialized)
			{
				bDSInitialized = true;
				victim->CopyDragonSoulItemGrid(s_vDSGrid);
			}

			bool bExistEmptySpace = false;
			WORD wBasePos = DSManager::instance().GetBasePosition(item);
			if (wBasePos >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				return false;
			
			for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; i++)
			{
				WORD wPos = wBasePos + i;
				if (0 == s_vDSGrid[wPos])
				{
					bool bEmpty = true;
					for (int j = 1; j < item->GetSize(); j++)
					{
						if (s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM])
						{
							bEmpty = false;
							break;
						}
					}
					if (bEmpty)
					{
						for (int j = 0; j < item->GetSize(); j++)
						{
							s_vDSGrid[wPos + j * DRAGON_SOUL_BOX_COLUMN_NUM] =  wPos + 1;
						}
						bExistEmptySpace = true;
						break;
					}
				}
				if (bExistEmptySpace)
					break;
			}
			if (!bExistEmptySpace)
				return false;
}
		else
		{
			int iPos = s_grid1.FindBlank(1, item->GetSize());

			if (iPos >= 0)
			{
				s_grid1.Put(iPos, 1, item->GetSize());
			}
			else
			{
				iPos = s_grid2.FindBlank(1, item->GetSize());

				if (iPos >= 0)
				{
					s_grid2.Put(iPos, 1, item->GetSize());
				}
				else
				{
					iPos = s_grid3.FindBlank(1, item->GetSize());

					if (iPos >= 0)
					{
						s_grid3.Put(iPos, 1, item->GetSize());
					}
					else
					{
						iPos = s_grid4.FindBlank(1, item->GetSize());

						if (iPos >= 0)
						{
							s_grid4.Put(iPos, 1, item->GetSize());
						}
						else
						{
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

bool CExchange::Done()
{
	int		empty_pos, i;
	LPITEM	item;

	LPCHARACTER	victim = GetCompany()->GetOwner();

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
			continue;

		if (item->IsDragonSoul())
			empty_pos = victim->GetEmptyDragonSoulInventory(item);
		else
#ifdef ENABLE_SPECIAL_INVENTORY
			empty_pos = victim->GetEmptyInventory(item);
#else
			empty_pos = victim->GetEmptyInventory(item->GetSize());
#endif

		if (empty_pos < 0)
		{
			sys_err("Exchange::Done : Cannot find blank position in inventory %s <-> %s item %s", 
					m_pOwner->GetName(), victim->GetName(), item->GetName());
			continue;
		}

		assert(empty_pos >= 0);

		m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 255);

		item->RemoveFromCharacter();
		if (item->IsDragonSoul())
			item->AddToCharacter(victim, TItemPos(DRAGON_SOUL_INVENTORY, empty_pos));
		else
			item->AddToCharacter(victim, TItemPos(INVENTORY, empty_pos));
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		item->SetExchanging(false);
		{
			LogManager::instance().ExchangeLogItems(GetOwner()->GetName(), victim->GetName(), item->GetName(), item->GetID(), item->GetCount(), GetOwner()->GetAID(), victim->GetAID());
		}

		m_apItems[i] = NULL;
	}

	if (m_lGold)
	{
		GetOwner()->PointChange(POINT_GOLD, -m_lGold, true);
		victim->PointChange(POINT_GOLD, m_lGold, true);

		if (m_lGold >= 10000000)
		{
			LogManager::instance().ExchangeLogGold(GetOwner()->GetName(), victim->GetName(), m_lGold, GetOwner()->GetAID(), victim->GetAID());
		}
	}

	m_pGrid->Clear();
	return true;
}

bool CExchange::Accept(bool bAccept)
{
	if (m_bAccept == bAccept)
		return true;

	m_bAccept = bAccept;

	if (m_bAccept && GetCompany()->m_bAccept)
	{
		int	iItemCount;

		LPCHARACTER victim = GetCompany()->GetOwner();

		GetOwner()->SetExchangeTime();
		victim->SetExchangeTime();		

		if (quest::CQuestManager::instance().GetPCForce(GetOwner()->GetPlayerID())->IsRunning() == true)
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade if you're using quests"));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade if the other part using quests"));
			goto EXCHANGE_END;
		}
		else if (quest::CQuestManager::instance().GetPCForce(victim->GetPlayerID())->IsRunning() == true)
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade if you're using quests"));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade if the other part using quests"));
			goto EXCHANGE_END;
		}

		if (!Check(&iItemCount))
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang or no space in the inventory."));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person does not have enough Yang or no space in the inventory left."));
			goto EXCHANGE_END;
		}

		if (!CheckSpace())
		{
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person has no space in the inventory left."));
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in the inventory."));
			goto EXCHANGE_END;
		}

		if (!GetCompany()->Check(&iItemCount))
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang or no space in the inventory."));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person does not have enough Yang or no space in the inventory left."));
			goto EXCHANGE_END;
		}

		if (!GetCompany()->CheckSpace())
		{
			victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person has no space in the inventory left."));
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in the inventory."));
			goto EXCHANGE_END;
		}

		if (db_clientdesc->GetSocket() == INVALID_SOCKET)
		{
			sys_err("Cannot use exchange feature while DB cache connection is dead.");
			victim->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, "Unknown error");
			goto EXCHANGE_END;
		}

		if (Done())
		{
			if (m_lGold)
				GetOwner()->Save();

			if (GetCompany()->Done())
			{
				if (GetCompany()->m_lGold)
					victim->Save();

				GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The trade with %s was successful."), victim->GetName());
				victim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The trade with %s was successful."), GetOwner()->GetName());
			}
		}

EXCHANGE_END:
		Cancel();
		return false;
	}
	else
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, true, m_bAccept, NPOS, 0);
		exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, false, m_bAccept, NPOS, 0);
		return true;
	}
}

void CExchange::Cancel()
{
	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_END, 0, 0, NPOS, 0);
	GetOwner()->SetExchange(NULL);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
			m_apItems[i]->SetExchanging(false);
	}

	if (GetCompany())
	{
		GetCompany()->SetCompany(NULL);
		GetCompany()->Cancel();
	}

	M2_DELETE(this);
}
