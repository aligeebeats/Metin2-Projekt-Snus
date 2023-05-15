#include "stdafx.h"

#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "../../common/building.h"
#include "../../common/vnum_helper.h"
#include "Main.h"
#include "Config.h"
#include "DBManager.h"
#include "QID.h"
#include "Peer.h"
#include "ClientManager.h"
#include "OfflineshopCache.h"

namespace offlineshop
{
	std::string CreateShopCacheInsertItemQuery(DWORD dwOwner, const CShopCache::TShopCacheItemInfo& rItem);
	std::string CreateShopCacheUpdateItemQuery(DWORD dwItemID, const TPriceInfo& rItemPrice);
	std::string CreateShopCacheDeleteShopQuery(DWORD dwOwner);
	std::string CreateShopCacheDeleteShopItemQuery(DWORD dwOwner);
#ifdef ENABLE_IRA_REWORK
	std::string CreateShopCacheInsertShopQuery(DWORD dwOwnerID, DWORD dwDuration, const char* name, TShopPosition pos);
#else
	std::string CreateShopCacheInsertShopQuery(DWORD dwOwnerID, DWORD dwDuration, const char* name);
#endif
	std::string CreateShopCacheUpdateShopNameQuery(DWORD dwOwnerID, const char* name);
	std::string CreateShopCacheUpdateDurationQuery(DWORD dwOwnerID, DWORD dwDuration);
	std::string CreateShopCacheDeleteItemQuery(DWORD dwOwnerID, DWORD dwItemID);
	std::string CreateShopCacheUpdateSoldItemQuery(DWORD dwOwnerID, DWORD dwItemID);
	std::string CreateSafeboxCacheDeleteItemQuery(DWORD dwItem);
	std::string CreateSafeboxCacheInsertItemQuery(DWORD dwOwner, const TItemInfoEx& item);
	std::string CreateSafeboxCacheUpdateValutes(DWORD dwOwner, const TValutesInfo& val);
	std::string CreateSafeboxCacheInsertSafeboxValutesQuery(DWORD dwOwnerID);
	std::string CreateSafeboxCacheUpdateValutesByAdding(DWORD dwOwner, const TValutesInfo& val);
	std::string CreateSafeboxCacheLoadItemsQuery(DWORD dwOwnerID);
	std::string CreateSafeboxCacheLoadValutesQuery(DWORD dwOwnerID);
	std::string CreateOfferCacheInsertOfferQuery(const COfferCache::TOfferCacheInfo& rOffer);
	std::string CreateOfferCacheUpdateNotifiedQuery(DWORD dwOfferID);
	std::string CreateOfferCacheUpdateAcceptedQuery(DWORD dwOfferID);
	std::string CreateOfferCacheRemoveOfferByShopOwner(DWORD dwShopOwner);
	std::string CreateOfferCacheDeleteOfferQuery(DWORD dwOfferID);
	std::string CreateAuctionCacheAddAuctionQuery(const TAuctionInfo& auction);
	std::string CreateAuctionCacheAddOfferQuery(const TAuctionOfferInfo& auctionOffer);
	std::string CreateAuctionCacheDeleteAuction(DWORD dwOwnerID);
	std::string CreateAuctionCacheDeleteAuctionOffers(DWORD dwOwnerID);
	std::string CreateAuctionCacheUpdateDurationQuery(DWORD dwOwnerID, DWORD dwDuration);

	CShopCache::CShopCache()
	{
	}

	CShopCache::~CShopCache()
	{
	}

	bool CShopCache::Get(DWORD dwOwnerID, TShopCacheInfo** ppCache) const
	{
		CONST_CACHEITER it = m_shopsMap.find(dwOwnerID);
		if (it == m_shopsMap.end())
			return false;

		*ppCache = (TShopCacheInfo*)&(it->second);
		return true;
	}

	bool CShopCache::AddItem(DWORD dwOwnerID, const TShopCacheItemInfo& rItem)
	{
		TShopCacheInfo* pCache;
		if (!Get(dwOwnerID, &pCache))
			return false;

		SQueryInfoAddItem* qi = new SQueryInfoAddItem;
		qi->dwOwnerID = dwOwnerID;
		CopyObject(qi->item, rItem);

		std::string query = CreateShopCacheInsertItemQuery(dwOwnerID, rItem);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_ADD_ITEM, 0, qi);
		return true;
	}
	
	bool CShopCache::RemoveItem(DWORD dwOwnerID, DWORD dwItemID)
	{
		OFFSHOP_DEBUG("owner %u , item id %u ", dwOwnerID, dwItemID);

		TShopCacheInfo* pCache;
		if (!Get(dwOwnerID, &pCache))
			return false;

		OFFSHOP_DEBUG("found successful (shop %u)",dwOwnerID);

		std::map<DWORD, TShopCacheItemInfo>::iterator it = pCache->itemsmap.find(dwItemID);
		if (it == pCache->itemsmap.end())
			return false;

		OFFSHOP_DEBUG("found successful (item %u)",dwItemID);

		if(it->second.bLock)
			return false;

		OFFSHOP_DEBUG("is not locked (item %u)",dwItemID);

		pCache->itemsmap.erase(it);

		std::string query = CreateShopCacheDeleteItemQuery(dwOwnerID, dwItemID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_REMOVE_ITEM, 0, NULL);

		if(pCache->itemsmap.empty())
			CClientManager::instance().OfflineshopExpiredShop(dwOwnerID);

		return true;
	}

	bool CShopCache::SellItem(DWORD dwOwnerID, DWORD dwItemID)
	{
		TShopCacheInfo* pCache;
		if (!Get(dwOwnerID, &pCache))
			return false;

		std::map<DWORD, TShopCacheItemInfo>::iterator it = pCache->itemsmap.find(dwItemID);
		if (it == pCache->itemsmap.end())
			return false;

		OFFSHOP_DEBUG("found item %u ",dwItemID);

		if(!it->second.bLock)
			return false;

		OFFSHOP_DEBUG("inserted into soldsmap %u ",dwItemID);

		pCache->soldsmap.insert(std::make_pair(it->first, it->second));
		pCache->itemsmap.erase(it);

		std::string query = CreateShopCacheUpdateSoldItemQuery(dwOwnerID, dwItemID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_UPDATE_SOLD_ITEM, 0, NULL);

		if(pCache->itemsmap.empty())
			CClientManager::instance().OfflineshopExpiredShop(dwOwnerID);

		return true;
	}

	bool CShopCache::LockSellItem(DWORD dwOwnerID, DWORD dwItemID)
	{
		TShopCacheInfo* pCache;
		if (!Get(dwOwnerID, &pCache))
			return false;

		std::map<DWORD, TShopCacheItemInfo>::iterator it = pCache->itemsmap.find(dwItemID);
		if (it == pCache->itemsmap.end())
			return false;

		if(it->second.bLock)
			return false;

		OFFSHOP_DEBUG("locked success %u ",dwItemID);
		it->second.bLock = true;
		return true;
	}

	bool CShopCache::UnlockSellItem(DWORD dwOwnerID, DWORD dwItemID)
	{
		TShopCacheInfo* pCache;
		if (!Get(dwOwnerID, &pCache))
			return false;

		std::map<DWORD, TShopCacheItemInfo>::iterator it = pCache->itemsmap.find(dwItemID);
		if (it == pCache->itemsmap.end())
			return false;

		if (!it->second.bLock)
			return false;

		OFFSHOP_DEBUG("Unlocked success %u ", dwItemID);
		it->second.bLock = false;
		return true;
	}

	bool CShopCache::EditItem(DWORD dwOwnerID, DWORD dwItemID, const TPriceInfo& rItemPrice)
	{
		TShopCacheInfo* pCache;
		if (!Get(dwOwnerID, &pCache))
			return false;

		std::map<DWORD, TShopCacheItemInfo>::iterator it = pCache->itemsmap.find(dwItemID);
		if (it == pCache->itemsmap.end())
			return false;

		if(it->second.bLock)
			return false;

		TShopCacheItemInfo& rItem = it->second;
		CopyObject(rItem.price, rItemPrice);

		std::string query = CreateShopCacheUpdateItemQuery(dwItemID, rItemPrice);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_EDIT_ITEM, 0, NULL);
		return true;
	}

	bool CShopCache::CloseShop(DWORD dwOwnerID)
	{
		CACHEITER it = m_shopsMap.find(dwOwnerID);
		if (it == m_shopsMap.end())
			return false;

		m_shopsMap.erase(it);

		std::string query = CreateShopCacheDeleteShopQuery(dwOwnerID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_DELETE_SHOP, 0, NULL);

		query = CreateShopCacheDeleteShopItemQuery(dwOwnerID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_DELETE_SHOP_ITEM, 0, NULL);
		return true;
	}

#ifdef ENABLE_IRA_REWORK
	bool CShopCache::CreateShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName, std::vector<TShopCacheItemInfo>& items, TShopPosition pos)
#else
	bool CShopCache::CreateShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName, std::vector<TShopCacheItemInfo>& items)
#endif
	{
		CACHEITER it = m_shopsMap.find(dwOwnerID);
		if (it != m_shopsMap.end())
			return false;

		SQueryInfoCreateShop* qi = new SQueryInfoCreateShop;
		qi->dwOwnerID = dwOwnerID;
		qi->dwDuration = dwDuration;

		strncpy(qi->szName, szName, sizeof(qi->szName));
		CopyContainer(qi->items, items);
#ifdef ENABLE_IRA_REWORK
		CopyContainer(qi->pos, pos);
		OFFSHOP_DEBUG("CreateShop! map_index %d (%d, %d)", pos.lMapIndex, pos.x, pos.y);

		std::string query = CreateShopCacheInsertShopQuery(dwOwnerID, dwDuration, szName, pos);
#else
		std::string query = CreateShopCacheInsertShopQuery(dwOwnerID, dwDuration, szName);
#endif
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_CREATE_SHOP, 0, qi);
		
		OFFSHOP_DEBUG("Sent query %s", query.c_str());
		return true;
	}

	bool CShopCache::CreateShopAddItem(SQueryInfoCreateShop* qi, const TShopCacheItemInfo& rItem)
	{
		CACHEITER it = m_shopsMap.find(qi->dwOwnerID);
		if (it == m_shopsMap.end())
			return false;

		std::string query = CreateShopCacheInsertItemQuery(qi->dwOwnerID, rItem);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_CREATE_SHOP_ADD_ITEM, 0, qi);
		OFFSHOP_DEBUG("Sent query %s", query.c_str());
		return true;
	}

	bool CShopCache::ChangeShopName(DWORD dwOwnerID, const char* szName)
	{
		CACHEITER it = m_shopsMap.find(dwOwnerID);
		if (it == m_shopsMap.end())
			return false;

		TShopCacheInfo& rShop = it->second;
		strncpy(rShop.szName, szName, sizeof(rShop.szName));

		std::string query = CreateShopCacheUpdateShopNameQuery(dwOwnerID, szName);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SHOP_CHANGE_NAME, 0, NULL);
		return true;
	}

	bool CShopCache::PutItem(DWORD dwOwnerID, DWORD dwItemID, const TShopCacheItemInfo& rItem, bool isSold)
	{
		CACHEITER it = m_shopsMap.find(dwOwnerID);
		if (it == m_shopsMap.end())
			return false;

		TShopCacheInfo& rShop	= it->second;
		SHOPCACHE_MAP& rMap		= isSold ? rShop.soldsmap : rShop.itemsmap;

		if (rMap.find(dwItemID) != rMap.end())
			return false;

		rMap.insert(std::make_pair(dwItemID, rItem));
		return true;
	}

#ifdef ENABLE_IRA_REWORK
	bool CShopCache::PutShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName, TShopPosition pos)
#else
	bool CShopCache::PutShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName)
#endif
	{
		CACHEITER it = m_shopsMap.find(dwOwnerID);
		if (it != m_shopsMap.end())
			return false;

		TShopCacheInfo sShop;
		sShop.dwDuration = dwDuration;
		strncpy(sShop.szName, szName, sizeof(sShop.szName));
#ifdef ENABLE_IRA_REWORK
		CopyContainer(sShop.posDetails, pos);
#endif
		m_shopsMap.insert(std::make_pair(dwOwnerID, sShop));
		return true;
	}

	void CShopCache::EncodeCache(CPeer* peer) const
	{
		TShopInfo shopInfo;
#ifdef ENABLE_IRA_REWORK
		TShopPosition shopPosInfo;
#endif
		itertype(m_shopsMap) it=m_shopsMap.begin();

		while (it != m_shopsMap.end())
		{
			DWORD dwOwnerID				= it->first;
			const TShopCacheInfo& rShop	= it->second;
			
			strncpy(shopInfo.szName, rShop.szName, sizeof(shopInfo.szName));
			shopInfo.dwDuration = rShop.dwDuration;
			shopInfo.dwOwnerID	= dwOwnerID;
			shopInfo.dwCount	= rShop.itemsmap.size();
#ifdef ENABLE_IRA_REWORK
			shopPosInfo.x		= rShop.posDetails.x;
			shopPosInfo.y		= rShop.posDetails.y;
			shopPosInfo.lMapIndex= rShop.posDetails.lMapIndex;
			shopPosInfo.bChannel= rShop.posDetails.bChannel;
#endif
			peer->Encode(&shopInfo, sizeof(shopInfo));
#ifdef ENABLE_IRA_REWORK
			peer->Encode(&shopPosInfo, sizeof(shopPosInfo));
#endif
			peer->EncodeDWORD(rShop.soldsmap.size());

			OFFSHOP_DEBUG("encoding shop %u %s (solds %u) ", shopInfo.dwOwnerID, shopInfo.szName);

			itertype(rShop.itemsmap) itItem= rShop.itemsmap.begin();
			TItemInfo itemInfo;

			for (; itItem != rShop.itemsmap.end(); itItem++)
			{

				DWORD dwItemID					= itItem->first;			
				const TShopCacheItemInfo& rItem = itItem->second;

				
				itemInfo.dwOwnerID = dwOwnerID;
				itemInfo.dwItemID  = dwItemID;
				
				CopyObject(itemInfo.item , rItem.item);
				CopyObject(itemInfo.price, rItem.price);

				OFFSHOP_DEBUG("encoding for sale item %u ",itemInfo.dwItemID);
				peer->Encode(&itemInfo, sizeof(itemInfo));
			}

			for (itItem = rShop.soldsmap.begin(); itItem != rShop.soldsmap.end(); itItem++)
			{

				DWORD dwItemID					= itItem->first;			
				const TShopCacheItemInfo& rItem = itItem->second;


				itemInfo.dwOwnerID = dwOwnerID;
				itemInfo.dwItemID  = dwItemID;

				CopyObject(itemInfo.item , rItem.item);
				CopyObject(itemInfo.price, rItem.price);

				OFFSHOP_DEBUG("encoding sold item %u ",itemInfo.dwItemID);
				peer->Encode(&itemInfo, sizeof(itemInfo));
			}

			it++;
		}
	}

	DWORD CShopCache::GetItemCount() const
	{
		DWORD dwItemCount=0;
		CONST_CACHEITER it = m_shopsMap.begin();
		for (; it != m_shopsMap.end(); it++)
		{
			dwItemCount += it->second.itemsmap.size();
			dwItemCount += it->second.soldsmap.size();
		}

		return dwItemCount;
	}

	void CShopCache::ShopDurationProcess()
	{
		CACHEITER it = m_shopsMap.begin();
		for (; it != m_shopsMap.end(); it++)
			if(--it->second.dwDuration!=0 && it->second.dwDuration % 5 == 0)
				UpdateDurationQuery(it->first, it->second);

		//expired check
		std::vector<DWORD> vec;

		//item expired check
		std::vector<std::pair<DWORD,DWORD>> item_vec;
		const time_t now = time(0);

		it = m_shopsMap.begin();
		for (; it != m_shopsMap.end(); it++)
		{
			CShopCache::TShopCacheInfo& shop = it->second;

			if (shop.dwDuration == 0) {
				vec.push_back(it->first);
				continue;
			}

			itertype(shop.itemsmap) it_item  = shop.itemsmap.begin();
			itertype(shop.itemsmap) end_item = shop.itemsmap.end();

			for (; it_item != end_item; it_item++) {
				TItemInfoEx& item_info = it_item->second.item;
				if (item_info.expiration == offlineshop::ExpirationType::EXPIRE_REAL_TIME) {
					if (now > item_info.alSockets[0])
						item_vec.push_back(std::make_pair(it->first, it_item->first));
				}
				else if (item_info.expiration == offlineshop::ExpirationType::EXPIRE_REAL_TIME_FIRST_USE) {
					if (item_info.alSockets[1] != 0 && item_info.alSockets[0] < now)
						item_vec.push_back(std::make_pair(it->first, it_item->first));
				}
			}
		}

		for(DWORD i=0; i < vec.size(); i++)
			CClientManager::instance().OfflineshopExpiredShop(vec[i]);

		itertype(item_vec) item_it  = item_vec.begin();
		itertype(item_vec) item_end = item_vec.end();
		for (; item_it != item_end; item_it++) {
			CClientManager::Instance().SendOfflineShopRemoveItemPacket(item_it->first, item_it->second);
			RemoveItem(item_it->first, item_it->second);
		}
	}

	void CShopCache::UpdateDurationQuery(DWORD dwOwnerID, const TShopCacheInfo& rShop)
	{
		std::string query = CreateShopCacheUpdateDurationQuery(dwOwnerID, rShop.dwDuration);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SHOP_UPDATE_DURATION, 0 , NULL);
	}

	CSafeboxCache::CSafeboxCache()
	{
	}

	CSafeboxCache::~CSafeboxCache()
	{
	}

	bool CSafeboxCache::Get(DWORD dwOwnerID, TSafeboxCacheInfo** ppSafebox) const
	{
		CHACHECONSTITER it = m_safeboxMap.find(dwOwnerID);
		if (it == m_safeboxMap.end())
			return false;

		*ppSafebox = (TSafeboxCacheInfo*)&(it->second);
		return true;
	}

	bool CSafeboxCache::PutSafebox(DWORD dwOwnerID, const TSafeboxCacheInfo& rSafebox)
	{
		CHACHECONSTITER it = m_safeboxMap.find(dwOwnerID);
		if (it != m_safeboxMap.end())
			return false;
		
		m_safeboxMap.insert(std::make_pair(dwOwnerID, rSafebox));
		return true;
	}

	bool CSafeboxCache::PutItem(DWORD dwOwnerID, DWORD dwItem, const TItemInfoEx& item)
	{
		TSafeboxCacheInfo* pSafebox = nullptr;
		if (!Get(dwOwnerID, &pSafebox))
			return false;

		std::map<DWORD, TItemInfoEx>::iterator it = pSafebox->itemsmap.find(dwItem);
		if (it != pSafebox->itemsmap.end())
			return false;

		pSafebox->itemsmap.insert(std::make_pair(dwItem, item));
		return true;
	}

	bool CSafeboxCache::RemoveItem(DWORD dwOwnerID, DWORD dwItemID)
	{
		TSafeboxCacheInfo* pSafebox = nullptr;
		if (!Get(dwOwnerID, &pSafebox))
			return false;

		std::map<DWORD, TItemInfoEx>::iterator it = pSafebox->itemsmap.find(dwItemID);
		if (it == pSafebox->itemsmap.end())
			return false;

		pSafebox->itemsmap.erase(it);

		std::string query = CreateSafeboxCacheDeleteItemQuery(dwItemID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_DELETE_ITEM, 0, NULL);
		return true;
	}

	bool CSafeboxCache::AddItem(DWORD dwOwnerID, const TItemInfoEx& item)
	{
		SQueryInfoSafeboxAddItem* qi = new SQueryInfoSafeboxAddItem;
		qi->dwOwnerID = dwOwnerID;
		CopyObject(qi->item, item);

		std::string query = CreateSafeboxCacheInsertItemQuery(dwOwnerID, item);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_ADD_ITEM, 0, qi);
		return true;
	}

	bool CSafeboxCache::AddValutes(DWORD dwOwnerID, const TValutesInfo& val)
	{
		TSafeboxCacheInfo* pSafebox = nullptr;
		if (!Get(dwOwnerID, &pSafebox))
		{
			sys_err("CANNOT FIND SAFEBOX BY ID %u !! ADDING VALUTES BY QUERY", dwOwnerID);
			AddValutesAsQuery(dwOwnerID, val);
			return true;
		}

		pSafebox->valutes += val;

		std::string query = CreateSafeboxCacheUpdateValutes(dwOwnerID, pSafebox->valutes);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES, 0, NULL);
		return true;
	}

	void CSafeboxCache::AddValutesAsQuery(DWORD dwOwnerID, const TValutesInfo& val)
	{
		std::string query=CreateSafeboxCacheUpdateValutesByAdding(dwOwnerID,val);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES_ADDING, 0, NULL);
	}

	bool CSafeboxCache::RemoveValutes(DWORD dwOwnerID, const TValutesInfo& val)
	{
		TSafeboxCacheInfo* pSafebox = nullptr;
		if (!Get(dwOwnerID, &pSafebox))
			return false;

		pSafebox->valutes -= val;

		std::string query = CreateSafeboxCacheUpdateValutes(dwOwnerID, pSafebox->valutes);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_UPDATE_VALUTES, 0, NULL);
		return true;
	}

	CSafeboxCache::TSafeboxCacheInfo* CSafeboxCache::CreateSafebox(DWORD dwOwnerID)
	{
		if (!PutSafebox(dwOwnerID, TSafeboxCacheInfo()))
			return nullptr;

		std::string query = CreateSafeboxCacheInsertSafeboxValutesQuery(dwOwnerID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_SAFEBOX_INSERT_VALUTES, 0, NULL);

		CSafeboxCache::TSafeboxCacheInfo* pInfo=nullptr;
		Get(dwOwnerID, &pInfo);

		return pInfo;
	}

	CSafeboxCache::TSafeboxCacheInfo* CSafeboxCache::LoadSafebox(DWORD dwPID)
	{
		TSafeboxCacheInfo* pSafebox = nullptr;
		if (Get(dwPID, &pSafebox))
			return pSafebox;

		TSafeboxCacheInfo safebox;
		MYSQL_ROW row;

		{
			std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(CreateSafeboxCacheLoadValutesQuery(dwPID).c_str()));
			if(pMsg->Get()->uiAffectedRows == 0)
				return CreateSafebox(dwPID);

			if (pMsg->Get()->uiAffectedRows != 1)
			{
				sys_err("multiple safebox rows for id %d ",dwPID);
				return nullptr;
			}

			if ((row = mysql_fetch_row(pMsg->Get()->pSQLResult))) {
				str_to_number(safebox.valutes.illYang, row[0]);
#ifdef __ENABLE_CHEQUE_SYSTEM__
				str_to_number(safebox.valutes.iCheque , row[1]);
#endif
			}
			
			else
			{
				sys_err("cannot fetch safebox row for id %d ",dwPID);
				return nullptr;
			}
		}
		
		{
			std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(CreateSafeboxCacheLoadItemsQuery(dwPID).c_str()));
			DWORD dwItemID =0;
			TItemInfoEx item;

			while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				int col=0;

				str_to_number(dwItemID ,		row[col++]);
				str_to_number(item.dwVnum,		row[col++]);
				str_to_number(item.dwCount,		row[col++]);
				
				for(int i=0; i < ITEM_SOCKET_MAX_NUM; i++)
					str_to_number(item.alSockets[i] , row[col++]);

				for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
				{
					str_to_number(item.aAttr[i].bType , row[col++]);
					str_to_number(item.aAttr[i].sValue, row[col++]);
				}
				
#ifdef __ENABLE_CHANGELOOK_SYSTEM__
				str_to_number(item.dwTransmutation, row[col++]);
#endif
				BYTE exp = 0;
				str_to_number(exp, row[col++]);
				item.expiration = offlineshop::ExpirationType(exp);
	
				safebox.itemsmap.insert(std::make_pair(dwItemID, item));
			}
		}

		CACHEMAP::iterator it= m_safeboxMap.insert(std::make_pair(dwPID , safebox)).first;
		return &it->second;
	}

	DWORD CSafeboxCache::GetItemCount() const
	{
		DWORD dwItemCount=0;
		CACHEMAP::const_iterator it = m_safeboxMap.begin();
		for (; it != m_safeboxMap.end(); it++)
			dwItemCount+= it->second.itemsmap.size();
		
		return dwItemCount;
	}

	void CSafeboxCache::ItemExpirationProcess() {
		std::vector<std::pair<DWORD, DWORD>> vec;

		const auto now = time(0);
		for (auto& iter : m_safeboxMap) {
			auto& info = iter.second;
			
			for (auto& item : info.itemsmap) {
				auto& item_info = item.second;
				if (item_info.expiration == offlineshop::ExpirationType::EXPIRE_REAL_TIME) {
					if (now > item_info.alSockets[0])
						vec.emplace_back(std::make_pair(iter.first , item.first));
				} else if(item_info.expiration == offlineshop::ExpirationType::EXPIRE_REAL_TIME_FIRST_USE){
					if (item_info.alSockets[1] != 0 && item_info.alSockets[0] < now)
						vec.emplace_back(std::make_pair(iter.first, item.first));
				}
			}
		}

		for (const auto& data : vec) {
			RemoveItem(data.first, data.second);
			CClientManager::instance().SendOfflineshopSafeboxExpiredItem(data.first, data.second);
		}
	}

	COfferCache::COfferCache()
	{
	}

	COfferCache::~COfferCache()
	{
	}

	bool COfferCache::Puts(DWORD dwOfferID, const TOfferCacheInfo& rOffer)
	{
		CONST_CACHEITER it = m_mapOffer.find(dwOfferID);
		if (it != m_mapOffer.end())
			return false;

		m_mapOffer.insert(std::make_pair(dwOfferID, rOffer));

		OFFERIDVEC& vec = m_findOffersByShop[rOffer.dwOwnerID];
		vec.push_back(dwOfferID);
		
		return true;
	}

	bool COfferCache::Get(DWORD dwOfferID, TOfferCacheInfo** ppOffer) const
	{
		CONST_CACHEITER it = m_mapOffer.find(dwOfferID);
		if (it == m_mapOffer.end())
			return false;

		*ppOffer = (TOfferCacheInfo*)&(it->second);
		return true;
	}

	bool COfferCache::AddOffer(const TOfferCacheInfo& rOffer)
	{
		SQueryInfoOfferAdd* qi = new SQueryInfoOfferAdd;
		CopyObject(qi->offer, rOffer);

		std::string query = CreateOfferCacheInsertOfferQuery(rOffer);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_OFFER_ADD, 0, qi);
		return true;
	}

	bool COfferCache::NoticedOffer(DWORD dwOfferID)
	{
		TOfferCacheInfo* pOffer = nullptr;
		if (!Get(dwOfferID, &pOffer))
			return false;

		pOffer->bNoticed = true;
		std::string query = CreateOfferCacheUpdateNotifiedQuery(dwOfferID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_OFFER_UPDATE_NOTIFIED, 0, NULL);
		return true;
	}

	bool COfferCache::CancelOffer(DWORD dwOfferID, DWORD dwOwnerID)
	{
		TOfferCacheInfo* pOffer = nullptr;
		if (!Get(dwOfferID, &pOffer))
			return false;

		FINDOFFERBYSHOP::iterator it=m_findOffersByShop.find(dwOwnerID);
		if(it==m_findOffersByShop.end())
			return false;

		OFFERIDVEC& vec= it->second;
		for(OFFERIDVEC::iterator itID = vec.begin() ; itID!= vec.end(); itID++)
		{
			if (*itID == dwOfferID)
			{
				vec.erase(itID);
				break;
			}
		}

		if(vec.empty())
			m_findOffersByShop.erase(it);
		m_mapOffer.erase(m_mapOffer.find(dwOfferID));

		std::string query = CreateOfferCacheDeleteOfferQuery(dwOfferID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_OFFER_DELETE, 0, NULL);
		return true;
	}

	bool COfferCache::AcceptOffer(DWORD dwOfferID)
	{
		TOfferCacheInfo* pOffer = nullptr;
		if (!Get(dwOfferID, &pOffer))
			return false;

		pOffer->bAccepted = true;
		std::string query = CreateOfferCacheUpdateAcceptedQuery(dwOfferID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_OFFER_UPDATE_ACCEPTED, 0, NULL);
		return true;
	}

	bool COfferCache::RemoveOffersByShopOwner(DWORD dwShopOwnerID)
	{
		FINDOFFERBYSHOP::iterator it=m_findOffersByShop.find(dwShopOwnerID);
		if(it==m_findOffersByShop.end())
			return false;

		OFFERIDVEC& vec= it->second;
		for(OFFERIDVEC::iterator itID = vec.begin() ; itID!= vec.end(); itID++)
		{
			CACHEITER itCache = m_mapOffer.find(*itID);
			if(itCache!=m_mapOffer.end())
				m_mapOffer.erase(itCache);
		}


		m_findOffersByShop.erase(it);
		std::string query = CreateOfferCacheRemoveOfferByShopOwner(dwShopOwnerID);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_OFFER_DELETE_BY_SHOP, 0, NULL);
		return true;
	}

	void COfferCache::EncodeCache(CPeer* peer) const
	{
		CONST_CACHEITER it = m_mapOffer.begin();
		TOfferInfo offer;

		for (; it != m_mapOffer.end(); it++)
		{
			DWORD dwOfferID					= it->first;
			const TOfferCacheInfo &	rOffer	= it->second;

			CopyObject(offer.price, rOffer.price);
			
			offer.dwOfferID		= dwOfferID;
			offer.dwOffererID	= rOffer.dwOffererID;
			offer.dwOwnerID		= rOffer.dwOwnerID;
			offer.bNoticed		= rOffer.bNoticed;
			offer.dwItemID		= rOffer.dwItemID;
			offer.bAccepted		= rOffer.bAccepted;
			offer.bNoticed		= rOffer.bNoticed;

			strncpy(offer.szBuyerName, rOffer.szBuyerName, sizeof(offer.szBuyerName));

			peer->Encode(&offer, sizeof(offer));
		}
	}

	bool COfferCache::GetOffersByShopOwner(DWORD dwOwnerID , COfferCache::OFFERIDVEC** ppVec)
	{
		FINDOFFERBYSHOP::iterator it =  m_findOffersByShop.find(dwOwnerID);
		if(it == m_findOffersByShop.end())
			return false;

		*ppVec = &(it->second);
		return true;
	}

	bool COfferCache::GetOffersByItemID(DWORD dwItemID, std::vector<DWORD>& offerIDs)
	{
		offerIDs.clear();
		itertype(m_mapOffer) it = m_mapOffer.begin();
		for (; it != m_mapOffer.end(); it++)
		{
			if(it->second.dwItemID == dwItemID)
				offerIDs.push_back(it->first);
		}

		return !offerIDs.empty();
	}

	CAuctionCache::CAuctionCache() 
	{
	}

	CAuctionCache::~CAuctionCache()
	{
	}

	bool CAuctionCache::PutsAuction(const TAuctionInfo& auction)
	{
		m_mapAuction[auction.dwOwnerID]			= auction;
		m_mapAuctionOffer[auction.dwOwnerID]	= std::vector<TAuctionOfferInfo>();

		return true;
	}

	bool CAuctionCache::PutsAuctionOffer(const TAuctionOfferInfo& auctionOffer)
	{
		AUCTIONOFFERVEC& vec= m_mapAuctionOffer[auctionOffer.dwOwnerID];
		vec.push_back(auctionOffer);
		return true;
	}

	bool CAuctionCache::AddAuction(const TAuctionInfo& auction)
	{
		OFFSHOP_DEBUG("auction %u , %u min",auction.dwOwnerID, auction.dwDuration);

		std::string query = CreateAuctionCacheAddAuctionQuery(auction);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_AUCTION_INSERT, 0, NULL);

		OFFSHOP_DEBUG("query:%s", query.c_str());

		PutsAuction(auction);
		return true;
	}

	bool CAuctionCache::AddOffer(const TAuctionOfferInfo& auctionOffer, bool quering)
	{
		if (quering)
		{
			std::string query = CreateAuctionCacheAddOfferQuery(auctionOffer);
			CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_AUCTION_INSERT_OFFER, 0 , NULL);
			if (m_mapAuction[auctionOffer.dwOwnerID].dwDuration == 0) // +
				m_mapAuction[auctionOffer.dwOwnerID].dwDuration = 1; // +
		}

		PutsAuctionOffer(auctionOffer);
		return true;
	}

	bool CAuctionCache::ExpiredAuction(DWORD dwOwnerID)
	{
		
		itertype(m_mapAuction)		it		= m_mapAuction.find(dwOwnerID);
		itertype(m_mapAuctionOffer)	itOff	= m_mapAuctionOffer.find(dwOwnerID);

		if(it !=m_mapAuction.end())
			m_mapAuction.erase(it);

		if(itOff !=m_mapAuctionOffer.end())
			m_mapAuctionOffer.erase(itOff);
		
		CDBManager::instance().ReturnQuery(CreateAuctionCacheDeleteAuction(dwOwnerID).c_str(), QID_OFFLINESHOP_AUCTION_DELETE, 0, NULL);
		CDBManager::instance().ReturnQuery(CreateAuctionCacheDeleteAuctionOffers(dwOwnerID).c_str(), QID_OFFLINESHOP_AUCTION_DELETE_OFFERS, 0 , NULL);
		return true;
	}

	bool CAuctionCache::AuctionDurationProcess()
	{
		if(m_mapAuction.empty())
			return false;

		std::vector<DWORD> owners;

		//item expiration
		std::vector<DWORD> items_expired;
		const time_t now = time(0);

		itertype(m_mapAuction) it = m_mapAuction.begin();
		for (; it != m_mapAuction.end(); it++)
		{
			TAuctionInfo& auction = it->second;

			//check about the item expiration
			TItemInfoEx& itemInfo = auction.item;
			if (itemInfo.expiration == offlineshop::ExpirationType::EXPIRE_REAL_TIME) {
				if (now > itemInfo.alSockets[0]) {
					items_expired.push_back(it->first);
					continue;
				}
			}
			else if (itemInfo.expiration == offlineshop::ExpirationType::EXPIRE_REAL_TIME_FIRST_USE) {
				if (itemInfo.alSockets[1] != 0 && itemInfo.alSockets[0] < now) {
					items_expired.push_back(it->first);
					continue;
				}
			}



			if (auction.dwDuration != 0)
			{
				if(--auction.dwDuration%5==0 && auction.dwDuration!=0)
					__UpdateDuration(auction);
			}


			else
			{
				owners.push_back(it->first);
			}
		}


		for(DWORD i=0; i < owners.size(); i++)
			CClientManager::instance().OfflineshopExpiredAuction(owners[i]);

		itertype(items_expired) exp_it = items_expired.begin();
		itertype(items_expired) end_it = items_expired.end();

		for (; exp_it != end_it; exp_it++) {
			CClientManager::instance().OfflineshopExpiredAuctionItem(*exp_it);
		}


		return true;
	}

	void CAuctionCache::__UpdateDuration(const TAuctionInfo & auction)
	{	
		std::string query = CreateAuctionCacheUpdateDurationQuery(auction.dwOwnerID, auction.dwDuration);
		CDBManager::instance().ReturnQuery(query.c_str(), QID_OFFLINESHOP_AUCTION_UPDATE_DURATION, 0 , NULL);
	}

	DWORD CAuctionCache::GetBestBuyer(DWORD dwOwnerID, TAuctionOfferInfo** ppOffer)
	{
		*ppOffer = nullptr;
		AUCTIONOFFERVEC& vec = m_mapAuctionOffer[dwOwnerID];

		if(vec.empty())
			return 0;

		DWORD dwBest=0;
		TPriceInfo* pBestPrice=nullptr;
		
		for (itertype(vec) it = vec.begin(); it != vec.end(); it++)
		{
			if (!pBestPrice || *pBestPrice < it->price)
			{
				pBestPrice	= &it->price;
				dwBest		= it->dwBuyerID;
				*ppOffer	= &(*it);
				continue;
			}
		}

		return dwBest;
	}

	bool CAuctionCache::Get(DWORD dwOwnerID, TAuctionInfo** ppInfo)
	{
		*ppInfo=nullptr;
		itertype(m_mapAuction) it;

		if((it=m_mapAuction.find(dwOwnerID))!=m_mapAuction.end())
			*ppInfo = &(it->second);

		return *ppInfo != nullptr;
	}

	bool CAuctionCache::GetOffers(DWORD dwOwnerID, std::vector<TAuctionOfferInfo>** ppVec)
	{
		*ppVec=nullptr;
		itertype(m_mapAuctionOffer) it;

		if((it=m_mapAuctionOffer.find(dwOwnerID))!=m_mapAuctionOffer.end())
			*ppVec = &(it->second);

		return *ppVec != nullptr;
	}

	DWORD CAuctionCache::GetCount()
	{
		return m_mapAuction.size();
	}

	DWORD CAuctionCache::GetOffersCount()
	{
		DWORD dwCount =0;
		for(itertype(m_mapAuctionOffer) it = m_mapAuctionOffer.begin(); it != m_mapAuctionOffer.end(); it++)
			dwCount += it->second.size();

		return dwCount;
	}

	void CAuctionCache::EncodeCache(CPeer* peer)
	{
		for(itertype(m_mapAuction) it = m_mapAuction.begin(); it != m_mapAuction.end(); it++)
			peer->Encode(&it->second, sizeof(it->second));

		for(itertype(m_mapAuctionOffer) it = m_mapAuctionOffer.begin(); it != m_mapAuctionOffer.end(); it++)
			if(!it->second.empty())
				peer->Encode(&it->second[0], sizeof(it->second[0])* it->second.size());
	}

	std::string CreateShopCacheInsertItemQuery(DWORD dwOwner, const CShopCache::TShopCacheItemInfo& rItem)
	{
		char szQuery[1024] = "INSERT INTO `player`.`offlineshop_shop_items` (`item_id`, `owner_id`, `price_yang`, "
#ifdef __ENABLE_CHEQUE_SYSTEM__
			"`price_cheque`,"
#endif
			" `vnum`, `count` ";
		size_t len = strlen(szQuery);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`socket%d` ", i);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`attr%d` , `attrval%d` ", i, i);

		len += snprintf(szQuery + len, sizeof(szQuery) - len, "%s", " "
#ifdef __ENABLE_CHANGELOOK_SYSTEM__
		", `trans` "
#endif
		", expiration "
		") VALUES (");
		len += snprintf(szQuery + len, sizeof(szQuery) - len, "0, %u, %lld,"
#ifdef __ENABLE_CHEQUE_SYSTEM__
			" %d, "
#endif
			" %u, %u ",
			dwOwner, rItem.price.illYang,
#ifdef __ENABLE_CHEQUE_SYSTEM__
			rItem.price.iCheque,
#endif
			rItem.item.dwVnum, rItem.item.dwCount

		);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",%ld ", rItem.item.alSockets[i]);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				", %d , %d ", rItem.item.aAttr[i].bType, rItem.item.aAttr[i].sValue);

#ifdef __ENABLE_CHANGELOOK_SYSTEM__
		len += snprintf(szQuery + len, sizeof(szQuery) - len, " , %u ", rItem.item.dwTransmutation );
#endif
		len += snprintf(szQuery + len, sizeof(szQuery) - len, ", %u ", (BYTE)rItem.item.expiration);
		std::string query = szQuery;
		query += ");";

		return query;
	}

	std::string CreateShopCacheUpdateItemQuery(DWORD dwItemID, const TPriceInfo& rItemPrice)
	{
		static char szQuery[256];
		snprintf(szQuery, sizeof(szQuery), "UPDATE `player`.`offlineshop_shop_items` SET `price_yang` = %lld "
#ifdef __ENABLE_CHEQUE_SYSTEM__
			", `price_cheque` = %d "
#endif
			" WHERE `item_id` = %u;",
			rItemPrice.illYang,
#ifdef __ENABLE_CHEQUE_SYSTEM__
			rItemPrice.iCheque,
#endif

			dwItemID);

		return szQuery;
	}

	std::string CreateShopCacheDeleteShopQuery(DWORD dwOwner)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery), "DELETE from `player`.`offlineshop_shops` WHERE `owner_id` = %d;", dwOwner);
		return szQuery;
	}

	std::string CreateShopCacheDeleteShopItemQuery(DWORD dwOwner)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery), "DELETE from `player`.`offlineshop_shop_items` WHERE `owner_id` = %d;", dwOwner);
		return szQuery;
	}

#ifdef ENABLE_IRA_REWORK
	std::string CreateShopCacheInsertShopQuery(DWORD dwOwnerID, DWORD dwDuration, const char* name, TShopPosition pos)
#else
	std::string CreateShopCacheInsertShopQuery(DWORD dwOwnerID, DWORD dwDuration, const char* name)
#endif
	{
		static char szQuery[256];
		char szEscapeString[OFFLINE_SHOP_NAME_MAX_LEN + 32];

		CDBManager::instance().EscapeString(szEscapeString, name, strlen(name));

#ifdef ENABLE_IRA_REWORK
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO `player`.`offlineshop_shops` (`owner_id`, `duration`, `name`, `pos_x`, `pos_y`, `map_index`, `channel`) VALUES(%u, %u, '%s', %u, %u, %d, %d);",
			dwOwnerID, dwDuration, szEscapeString, pos.x, pos.y, pos.lMapIndex, pos.bChannel 
#else
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO `player`.`offlineshop_shops` (`owner_id`, `duration`, `name`) VALUES(%u, %u, '%s');",
			dwOwnerID, dwDuration, szEscapeString
#endif
		);

		return szQuery;
	}

	std::string CreateShopCacheUpdateShopNameQuery(DWORD dwOwnerID, const char* name)
	{
		static char szQuery[256];
		static char szEscapeString[OFFLINE_SHOP_NAME_MAX_LEN + 32];
		CDBManager::instance().EscapeString(szEscapeString, name, strlen(name));

		snprintf(szQuery, sizeof(szQuery), "UPDATE `player`.`offlineshop_shops` SET `name` = '%s' WHERE `owner_id` = %u;", szEscapeString, dwOwnerID);
		return szQuery;
	}

	std::string CreateShopCacheUpdateDurationQuery(DWORD dwOwnerID, DWORD dwDuration)
	{
		static char szQuery[256];
		snprintf(szQuery, sizeof(szQuery), "UPDATE `player`.`offlineshop_shops` SET `duration` = '%d' WHERE `owner_id` = %u;", dwDuration, dwOwnerID);
		return szQuery;
	}

	std::string CreateSafeboxCacheDeleteItemQuery(DWORD dwItem)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery), "DELETE from `player`.`offlineshop_safebox_items` WHERE `item_id` = %d;", dwItem);
		return szQuery;
	}

	std::string CreateSafeboxCacheInsertItemQuery(DWORD dwOwner, const TItemInfoEx& item)
	{
		char szQuery[1024] = "INSERT INTO `player`.`offlineshop_safebox_items` (`item_id`, `owner_id`, `vnum`, `count` ";
		size_t len = strlen(szQuery);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`socket%d` ", i);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`attr%d` , `attrval%d` ", i, i);

		len += snprintf(szQuery + len, sizeof(szQuery) - len, "%s", "  "
#ifdef __ENABLE_CHANGELOOK_SYSTEM__
		" , `trans` "
#endif
		", expiration "
		") VALUES (");
		len += snprintf(szQuery + len, sizeof(szQuery) - len, "0, %u, %u, %u ",
			dwOwner, item.dwVnum, item.dwCount
		);


		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",%ld ", item.alSockets[i]);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				", %d , %d ", item.aAttr[i].bType, item.aAttr[i].sValue);

#ifdef __ENABLE_CHANGELOOK_SYSTEM__
		len += snprintf(szQuery + len, sizeof(szQuery) - len,
			", %u ", item.dwTransmutation );
#endif
		len += snprintf(szQuery + len, sizeof(szQuery) - len, ", %u ", (BYTE)item.expiration);
		std::string query = szQuery;
		query += ");";

		return query;
	}

	std::string CreateSafeboxCacheUpdateValutes(DWORD dwOwner, const TValutesInfo& val)
	{
		static char szQuery[256];
		snprintf(szQuery, sizeof(szQuery), "UPDATE `player`.`offlineshop_safebox_valutes` SET `yang` = %lld "
#ifdef __ENABLE_CHEQUE_SYSTEM__
			", `cheque` = %d "
#endif
			" WHERE `owner_id` = %u ;", 
			val.illYang,
#ifdef __ENABLE_CHEQUE_SYSTEM__
			val.iCheque,
#endif
			dwOwner);

		return szQuery;
	}

	std::string CreateSafeboxCacheUpdateValutesByAdding(DWORD dwOwner, const TValutesInfo& val)
	{
		static char szQuery[256];
		snprintf(szQuery, sizeof(szQuery), "UPDATE `player`.`offlineshop_safebox_valutes` SET `yang` = `yang`+%lld "
#ifdef __ENABLE_CHEQUE_SYSTEM__
			", `cheque` = `cheque`+%d "
#endif
			" WHERE `owner_id` = %u ;", val.illYang,
#ifdef __ENABLE_CHEQUE_SYSTEM__
			val.iCheque,
#endif
			dwOwner);

		return szQuery;
	}

	std::string CreateSafeboxCacheInsertSafeboxValutesQuery(DWORD dwOwnerID)
	{
		static char szQuery[256];
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO `player`.`offlineshop_safebox_valutes` (`owner_id`,`yang`,`cheque`) VALUES( %u , 0,0);", dwOwnerID);
		return szQuery;
	}

	std::string CreateSafeboxCacheLoadValutesQuery(DWORD dwOwnerID)
	{
		static char szQuery[256];
		snprintf(szQuery, sizeof(szQuery), "SELECT `yang` "
#ifdef __ENABLE_CHEQUE_SYSTEM__
			", `cheque` "
#endif
			" from `player`.`offlineshop_safebox_valutes` WHERE `owner_id` = %d;", dwOwnerID);
		return szQuery;
	}

	std::string CreateSafeboxCacheLoadItemsQuery(DWORD dwOwnerID)
	{
		char szQuery[1024] = "SELECT `item_id`, `vnum`, `count` ";
		size_t len = strlen(szQuery);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`socket%d` ", i);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`attr%d` , `attrval%d` ", i, i);

		len += snprintf(szQuery + len, sizeof(szQuery) - len, "%s%u;", " "

#ifdef __ENABLE_CHANGELOOK_SYSTEM__
		" , `trans` "
#endif
		", expiration "
		" from `player`.`offlineshop_safebox_items` WHERE `owner_id`=", dwOwnerID);
		
		return szQuery;
	}

	std::string CreateOfferCacheInsertOfferQuery(const COfferCache::TOfferCacheInfo& rOffer)
	{
		static char szEscapeString[CHARACTER_NAME_MAX_LEN + 32];
		CDBManager::instance().EscapeString(szEscapeString, rOffer.szBuyerName, strnlen(rOffer.szBuyerName, sizeof(rOffer.szBuyerName)));

		static char szQuery[256];
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO `player`.`offlineshop_offers` (`offer_id`,`owner_id`,`offerer_id`, `item_id`, `price_yang`,"/*`price_cheque`,*/"`is_notified`,`is_accept`, `buyer_name`) VALUES"
											"(0, %u, %u, %u, %lld, %u, %u, '%s')",
											rOffer.dwOwnerID, rOffer.dwOffererID, rOffer.dwItemID, rOffer.price.illYang, rOffer.bNoticed?1:0, rOffer.bAccepted?1:0, szEscapeString
		);

		return szQuery;
	}

	std::string CreateOfferCacheUpdateNotifiedQuery(DWORD dwOfferID)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery),"UPDATE `player`.`offlineshop_offers` SET `is_notified` = 1 WHERE `offer_id` = %u ;",dwOfferID);
		return szQuery;
	}

	std::string CreateOfferCacheUpdateAcceptedQuery(DWORD dwOfferID)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery),"UPDATE `player`.`offlineshop_offers` SET `is_accept` = 1 WHERE `offer_id` = %u ;",dwOfferID);
		return szQuery;
	}

	std::string CreateOfferCacheRemoveOfferByShopOwner(DWORD dwShopOwner)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery),"DELETE from `player`.`offlineshop_offers`  WHERE `owner_id` = %u ;",dwShopOwner);
		return szQuery;
	}

	std::string CreateOfferCacheDeleteOfferQuery(DWORD dwOfferID)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery),"DELETE from `player`.`offlineshop_offers`  WHERE `offer_id` = %u ;",dwOfferID);
		return szQuery;
	}

	std::string CreateShopCacheDeleteItemQuery(DWORD dwOwnerID, DWORD dwItemID)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery),"DELETE from `player`.`offlineshop_shop_items`  WHERE `owner_id` = %u AND item_id = '%u' ;",dwOwnerID, dwItemID);
		return szQuery;
	}

	std::string CreateShopCacheUpdateSoldItemQuery(DWORD dwOwnerID, DWORD dwItemID)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery),"UPDATE `player`.`offlineshop_shop_items` SET `is_sold` = 1  WHERE `owner_id` = %u AND item_id = '%u' ;",dwOwnerID, dwItemID);
		return szQuery;
	}

	std::string CreateAuctionCacheAddAuctionQuery(const TAuctionInfo& auction)
	{
		static char szQuery[1024];
		size_t len = snprintf(szQuery, sizeof(szQuery), "%s" , "INSERT INTO `player`.`offlineshop_auctions` (`owner_id`, `duration`, `name` , `vnum`, `count` , `init_yang` ");

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`socket%d` ", i);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`attr%d` , `attrval%d` ", i, i);

		static char szEscapeName[CHARACTER_NAME_MAX_LEN+32];
		CDBManager::instance().EscapeString(szEscapeName, auction.szOwnerName, strnlen(auction.szOwnerName , sizeof(auction.szOwnerName)));

		len += snprintf(szQuery + len, sizeof(szQuery)-len, "  "
#ifdef __ENABLE_CHANGELOOK_SYSTEM__
			" , `trans` "
#endif
			", expiration "
			") VALUES( %u, %u, '%s', %u, %u ,%lld ",
			auction.dwOwnerID, auction.dwDuration, szEscapeName, auction.item.dwVnum, auction.item.dwCount, auction.init_price.illYang

		);

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",%ld ", auction.item.alSockets[i]);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",%u , %d ", auction.item.aAttr[i].bType, auction.item.aAttr[i].sValue);
		
#ifdef __ENABLE_CHANGELOOK_SYSTEM__
		len += snprintf(szQuery + len, sizeof(szQuery)-len, " , %u ",auction.item.dwTransmutation);
#endif
		len += snprintf(szQuery + len, sizeof(szQuery) - len, ", %u ", (BYTE)auction.item.expiration);
		len += snprintf(szQuery + len, sizeof(szQuery)-len, "%s",");");
		return szQuery;
	}

	std::string CreateAuctionCacheAddOfferQuery(const TAuctionOfferInfo& auctionOffer)
	{
		static char szEscapeName[CHARACTER_NAME_MAX_LEN+32];
		CDBManager::instance().EscapeString(szEscapeName, auctionOffer.szBuyerName, strnlen(auctionOffer.szBuyerName , sizeof(auctionOffer.szBuyerName)));

		static char szQuery[512];
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO `player`.`offlineshop_auction_offers` (`owner_id`, `buyer_id`, `buyer_name` , `yang`) "
																"VALUES( %u, %u, '%s' , %lld );",
																auctionOffer.dwOwnerID, auctionOffer.dwBuyerID, szEscapeName, auctionOffer.price.illYang
		);
		
		return szQuery;
	}

	std::string CreateAuctionCacheDeleteAuction(DWORD dwOwnerID)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM `player`.`offlineshop_auctions` WHERE `owner_id` = %u;",dwOwnerID);
		return szQuery;
	}

	std::string CreateAuctionCacheDeleteAuctionOffers(DWORD dwOwnerID)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM `player`.`offlineshop_auction_offers` WHERE `owner_id` = %u;",dwOwnerID);
		return szQuery;
	}

	std::string CreateAuctionCacheUpdateDurationQuery(DWORD dwOwnerID, DWORD dwDuration)
	{
		static char szQuery[128];
		snprintf(szQuery, sizeof(szQuery), "UPDATE `player`.`offlineshop_auctions` SET `duration` = %u WHERE `owner_id` = %u;",dwDuration, dwOwnerID);
		return szQuery;
	}
}
#endif