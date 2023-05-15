#include "stdafx.h"
#include "itemshop.h"

#include "char.h"
#include "db.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include "config.h"
#include "item_manager.h"
#include "item.h"
#include "desc.h"

#ifdef ENABLE_ITEMSHOP
CItemshopManager::CItemshopManager()
{
}

CItemshopManager::~CItemshopManager()
{
}

void CItemshopManager::InitializeItems(TItemshopItemTable * table, WORD size)
{
	m_ItemshopItems.clear();

	for (int i = 0; i < size; ++i, ++table)
	{
		const TItemTable* item_table = ITEM_MANAGER::instance().GetTable(table->dwVnum);
		if (item_table)
		{
			if (table->wCount > 1 && ((IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_STACK)) || !(IS_SET(item_table->dwFlags, ITEM_FLAG_STACKABLE))))
			{
				sys_err("<Itemshop / Are you stupid?> Vnum %lld Count %d but item is not stackable. Adjust this shit!", table->dwVnum, table->wCount);
				continue;
			}

			m_ItemshopItems[table->byCategory].push_back(*table);
		}
	}
}

void CItemshopManager::InitializeCategories(TItemshopCategoryTable* table, WORD size)
{
	m_ItemshopCategories.clear();

	for (int i = 0; i < size; ++i, ++table)
	{
		m_ItemshopCategories.insert(std::pair(table->index, table->info));
	}
}

void CItemshopManager::RefreshSingleItem(TItemshopItemTable* item)
{
	for (auto& category : m_ItemshopItems)
	{
		for (auto& cur_item : category.second)
		{
			if (!strcmp(cur_item.hash, item->hash))
			{
				thecore_memcpy(&cur_item, item, sizeof(TItemshopItemTable));
				CHARACTER_MANAGER::instance().SendItemshopSingleItemRefresh(item);
				return;
			}
		}
	}
}

void CItemshopManager::RemoveSingleItem(TItemshopItemTable* item)
{
	for (auto& category : m_ItemshopItems)
	{
		for(int i = 0; i < category.second.size(); i++)
		{
			if (!strcmp(category.second[i].hash, item->hash))
			{
				category.second.erase(category.second.begin() + i);
				CHARACTER_MANAGER::instance().SendItemshopSingleRemoveItem(item);
				return;
			}
		}
	}
}

void CItemshopManager::AddSingleItem(TItemshopItemTable* item)
{
	const TItemTable* item_table = ITEM_MANAGER::instance().GetTable(item->dwVnum);
	if (item_table)
	{
		if (item->wCount > 1 && ((IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_STACK)) || !(IS_SET(item_table->dwFlags, ITEM_FLAG_STACKABLE))))
		{
			sys_err("<Itemshop / Are you stupid?> Vnum %ld Count %d but item is not stackable. Adjust this shit!", item->dwVnum, item->wCount);
			return;
		}

		m_ItemshopItems[item->byCategory].push_back(*item);
		CHARACTER_MANAGER::instance().SendItemshopSingleAddItem(item);
	}
}

bool CItemshopManager::IsValidHash(const char* hash)
{
	for (const auto& category : m_ItemshopItems)
	{
		for (const auto& item : category.second) {
			if (!strcmp(hash, item.hash))
				return true;
		}
	}

	return false;
}

bool CItemshopManager::IsValidPurchase(TItemshopItemTable itemInfo, WORD wCount, unsigned long long &ullPrice)
{
	const TItemTable* item_table = ITEM_MANAGER::instance().GetTable(itemInfo.dwVnum);
	if (item_table)
	{
		// NOTE: Log user reason custom packet
		if (wCount > 1 && ((IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_STACK)) || !(IS_SET(item_table->dwFlags, ITEM_FLAG_STACKABLE))))
			return false;
	}

	if (wCount <= 0
		|| check_mul_error(itemInfo.wCount, wCount)
		|| itemInfo.wCount * wCount > MAX_ITEM_STACK)
	{
		return false;
	}

	BYTE discountPercent = MINMAX(0, itemInfo.byDiscountPercent, 100);
	if (discountPercent > 0) {
		ullPrice = ullPrice - (ullPrice * discountPercent / 100);
	}

	if (check_mul_error<unsigned long long>(ullPrice, wCount)) {
		return false;
	}

	ullPrice *= wCount;

	return true;
}

bool CItemshopManager::HasEnoughCoins(DWORD accID, unsigned long long ullItemPrice)
{
	std::unique_ptr<SQLMsg> pkMsg(DBManager::instance().DirectQuery("SELECT coins from account.account WHERE id = %ld", accID));
	SQLResult* pRes = pkMsg->Get();
	if (pRes->uiNumRows == 0)
	{
		return false;
	}

	MYSQL_ROW data = mysql_fetch_row(pRes->pSQLResult);
	unsigned long long ullTotalCoins;
	str_to_number(ullTotalCoins, data[0]);
	return ullTotalCoins >= ullItemPrice;
}

bool CItemshopManager::CanBuyItem(LPCHARACTER ch, const char* hash, WORD wCount)
{
	if (!ch->CanUseItemshop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait 10 seconds after an invalid itemshop action."));
		return false;
	}

	if (!IsValidHash(hash))
	{
		ch->SetItemshopCooldown(thecore_pulse() + PASSES_PER_SEC(10));
		return false;
	}

	TItemshopItemTable itemInfo{ 0 };

	for (const auto& category : m_ItemshopItems)
	{
		for (const auto& item : category.second)
		{
			if (!strcmp(hash, item.hash))
			{
				itemInfo = item;
				break;
			}
		}
	}

	if (itemInfo.llLimitCount == 0)
		return false;

	unsigned long long ullPrice = itemInfo.ullPrice;

	if (!IsValidPurchase(itemInfo, wCount, ullPrice))
	{
		sys_err("<Itemshop> OVERFLOW check failed. PID: %d - BuyCount %d - Hash %s - Vnum %lld - Count %d", ch->GetPlayerID(), wCount, itemInfo.hash, itemInfo.dwVnum, itemInfo.wCount);
		return false;
	}

	const TAccountTable& accTable = ch->GetDesc()->GetAccountTable();
	if (!HasEnoughCoins(accTable.id, ullPrice))
	{
		ch->SetItemshopCooldown(thecore_pulse() + PASSES_PER_SEC(10));
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You dont have enough coins."));
		return false;
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(itemInfo.dwVnum, itemInfo.wCount * wCount);

	if (!item)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Invalid item -> Write your Serveradmin to get some free coins :)"));
		return false;
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		if ((iEmptyCell = ch->GetEmptyDragonSoulInventory(item)) == -1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry too many Items."));
			M2_DESTROY_ITEM(item);
			return false;
		}
	}
	else
	{
		// NOTE: Adjust for special inventory by sanii
		if (iEmptyCell = ch->GetEmptyInventory(item) == -1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry too many Items."));
			M2_DESTROY_ITEM(item);
			return false;
		}
	}

	M2_DESTROY_ITEM(item);
	return true;
}

// NOTE: CALL THIS ONLY FROM DB_MANAGER
bool CItemshopManager::BuyItem(LPCHARACTER ch, const char* hash, WORD wCount)
{
	if (!IsValidHash(hash))
	{
		ch->SendItemshopItemsPacket(GetItemshopItems(),GetItemshopCategories());
		return false;
	}

	TItemshopItemTable itemInfo{0};

	for (const auto& category : m_ItemshopItems)
	{
		for (const auto & item : category.second)
		{
			if (!strcmp(hash, item.hash))
			{
				itemInfo = item;
				break;
			}
		}
	}

	unsigned long long ullPrice = itemInfo.ullPrice;

	if (!IsValidPurchase(itemInfo, wCount, ullPrice))
	{
		sys_err("<Itemshop> OVERFLOW check failed. PID: %d - BuyCount %d - Hash %s - Vnum %lld - Count %d", ch->GetPlayerID(), wCount, itemInfo.hash, itemInfo.dwVnum, itemInfo.wCount);
		return false;
	}

	const TAccountTable& accTable = ch->GetDesc()->GetAccountTable();
	if (!HasEnoughCoins(accTable.id, ullPrice))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You dont have enough coins."));
		return false;
	}


	LPITEM item = ITEM_MANAGER::instance().CreateItem(itemInfo.dwVnum, itemInfo.wCount * wCount);

	if (!item)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Invalid item -> Write your Serveradmin to get some free coins :)"));
		return false;
	}

	// NOTE: adjust as needed

	bool forcedSockets = false;

	if (itemInfo.alSockets[0])
	{
		for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
		{
			if (LIMIT_REAL_TIME == item->GetLimitType(i))
			{
				// NOTE: We set socket0 as time value, but only if socket is set so we still can use default times in createitem
				item->SetSocket(0, time(0) + itemInfo.alSockets[0]);
				// NOTE: We dont have to start realtime event because createitem already does
				forcedSockets = true;

			}
			else if (LIMIT_TIMER_BASED_ON_WEAR == item->GetLimitType(i))
			{
				item->SetSocket(0, itemInfo.alSockets[0]);
				forcedSockets = true;
			}
		}
	}

	// NOTE: I dont have systems where i need to set multiple sockets in createitem but you can still do:
	// for(const auto& socket : itemInfo.alSockets)
	if(!forcedSockets)
		item->SetSockets(itemInfo.alSockets);

	// NOTE: You can adjust this if you want to use bAlterToMagicItemPct
	item->SetAttributes(itemInfo.aAttr);

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		if ((iEmptyCell = ch->GetEmptyDragonSoulInventory(item)) == -1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry too many Items."));
			M2_DESTROY_ITEM(item);
			return false;
		}
	}
	else
	{
		// NOTE: Adjust for special inventory by sanii
		if (iEmptyCell = ch->GetEmptyInventory(item) == -1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry too many Items."));
			M2_DESTROY_ITEM(item);
			return false;
		}
	}
	DBManager::instance().DirectQuery("UPDATE account.account SET coins = coins - %lld WHERE id = %ld", ullPrice, accTable.id);

	if (item->IsDragonSoul())
		item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
	else
		item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyCell));


	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The purchase of the item was successful."));

	ch->SendItemshopCoinPacket();
	return true;
}

const std::unordered_map <BYTE, std::vector<TItemshopItemTable>> CItemshopManager::GetItemshopItems()
{
	return m_ItemshopItems;
}

const std::unordered_map < BYTE, TItemshopCategoryInfo> CItemshopManager::GetItemshopCategories()
{
	return m_ItemshopCategories;
}
#endif