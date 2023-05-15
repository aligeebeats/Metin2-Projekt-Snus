#include "stdafx.h"
#include "Cache.h"
#include "QID.h"
#include "ClientManager.h"
#include "Main.h"

extern CPacketInfo g_item_info;
extern int g_iPlayerCacheFlushSeconds;
extern int g_iItemCacheFlushSeconds;
extern int g_test_server;
extern int g_iItemPriceListTableCacheFlushSeconds;
extern int g_item_count;

CItemCache::CItemCache()
{
	m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CItemCache::~CItemCache()
{
}

void CItemCache::Delete()
{
	if (m_data.vnum == 0)
		return;

	if (g_test_server)
		sys_log(0, "ItemCache::Delete : DELETE %u", m_data.id);

	m_data.vnum = 0;
	m_bNeedQuery = true;
	m_lastUpdateTime = time(0);
	OnFlush();
}

void CItemCache::OnFlush()
{
	if (m_data.vnum == 0)
	{
		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM item%s WHERE id=%u", GetTablePostfix(), m_data.id);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_DESTROY, 0, NULL);

		if (g_test_server)
			sys_log(0, "ItemCache::Flush : DELETE %u %s", m_data.id, szQuery);
	}
	else
	{
		long alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
		bool isSocket = false, isAttr = false;

		memset(&alSockets, 0, sizeof(long) * ITEM_SOCKET_MAX_NUM);
		memset(&aAttr, 0, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM);

		TPlayerItem * p = &m_data;

		if (memcmp(alSockets, p->alSockets, sizeof(long) * ITEM_SOCKET_MAX_NUM))
			isSocket = true;

		if (memcmp(aAttr, p->aAttr, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM))
			isAttr = true;

		char szColumns[QUERY_MAX_LEN];
		char szValues[QUERY_MAX_LEN];
		char szUpdate[QUERY_MAX_LEN];

		int iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, window, pos, count, vnum");

		int iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, %d, %d, %u, %u",
				p->id, p->owner, p->window, p->pos, p->count, p->vnum);

		int iUpdateLen = snprintf(szUpdate, sizeof(szUpdate), "owner_id=%u, window=%d, pos=%d, count=%u, vnum=%u",
				p->owner, p->window, p->pos, p->count, p->vnum);

		// if (isSocket)
		// {
			// iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", socket0, socket1, socket2, socket3, socket4, socket5");
			// iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
					// ", %lu, %lu, %lu, %lu, %lu, %lu", p->alSockets[0], p->alSockets[1], p->alSockets[2], p->alSockets[3], p->alSockets[4], p->alSockets[5]);
			// iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
					// ", socket0=%lu, socket1=%lu, socket2=%lu, socket3=%lu, socket4=%lu, socket5=%lu", p->alSockets[0], p->alSockets[1], p->alSockets[2], p->alSockets[3], p->alSockets[4], p->alSockets[5]);
		// }

#ifndef ENABLE_EXTENDED_SOCKETS
		if (isSocket)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", socket0, socket1, socket2");
			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
					", %lu, %lu, %lu", p->alSockets[0], p->alSockets[1], p->alSockets[2]);
			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
					", socket0=%lu, socket1=%lu, socket2=%lu", p->alSockets[0], p->alSockets[1], p->alSockets[2]);
		}
#else
		if (isSocket)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", socket0, socket1, socket2, socket3, socket4, socket5");
			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
					", %lu, %lu, %lu, %lu, %lu, %lu", p->alSockets[0], p->alSockets[1], p->alSockets[2], p->alSockets[3], p->alSockets[4], p->alSockets[5]);
			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
					", socket0=%lu, socket1=%lu, socket2=%lu, socket3=%lu, socket4=%lu, socket5=%lu", p->alSockets[0], p->alSockets[1], p->alSockets[2], p->alSockets[3], p->alSockets[4], p->alSockets[5]);
		}
#endif

		if (isAttr)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, 
					", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3"
					", attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6");

			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
					", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
					p->aAttr[0].bType, p->aAttr[0].sValue,
					p->aAttr[1].bType, p->aAttr[1].sValue,
					p->aAttr[2].bType, p->aAttr[2].sValue,
					p->aAttr[3].bType, p->aAttr[3].sValue,
					p->aAttr[4].bType, p->aAttr[4].sValue,
					p->aAttr[5].bType, p->aAttr[5].sValue,
					p->aAttr[6].bType, p->aAttr[6].sValue);

			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
					", attrtype0=%d, attrvalue0=%d"
					", attrtype1=%d, attrvalue1=%d"
					", attrtype2=%d, attrvalue2=%d"
					", attrtype3=%d, attrvalue3=%d"
					", attrtype4=%d, attrvalue4=%d"
					", attrtype5=%d, attrvalue5=%d"
					", attrtype6=%d, attrvalue6=%d",
					p->aAttr[0].bType, p->aAttr[0].sValue,
					p->aAttr[1].bType, p->aAttr[1].sValue,
					p->aAttr[2].bType, p->aAttr[2].sValue,
					p->aAttr[3].bType, p->aAttr[3].sValue,
					p->aAttr[4].bType, p->aAttr[4].sValue,
					p->aAttr[5].bType, p->aAttr[5].sValue,
					p->aAttr[6].bType, p->aAttr[6].sValue);
		}

		char szItemQuery[QUERY_MAX_LEN + QUERY_MAX_LEN];
		snprintf(szItemQuery, sizeof(szItemQuery), "REPLACE INTO item%s (%s) VALUES(%s)", GetTablePostfix(), szColumns, szValues);

		if (g_test_server)	
			sys_log(0, "ItemCache::Flush :REPLACE  (%s)", szItemQuery);

		CDBManager::instance().ReturnQuery(szItemQuery, QID_ITEM_SAVE, 0, NULL);
		++g_item_count;
	}

	m_bNeedQuery = false;
}

CPlayerTableCache::CPlayerTableCache()
{
	m_expireTime = MIN(1800, g_iPlayerCacheFlushSeconds);
}

CPlayerTableCache::~CPlayerTableCache()
{
}

void CPlayerTableCache::OnFlush()
{
	if (g_test_server)
		sys_log(0, "PlayerTableCache::Flush : %s", m_data.name);

	char szQuery[QUERY_MAX_LEN];
	CreatePlayerSaveQuery(szQuery, sizeof(szQuery), &m_data);
	CDBManager::instance().ReturnQuery(szQuery, QID_PLAYER_SAVE, 0, NULL);
}

const int CItemPriceListTableCache::s_nMinFlushSec = 1800;

CItemPriceListTableCache::CItemPriceListTableCache()
{
	m_expireTime = MIN(s_nMinFlushSec, g_iItemPriceListTableCacheFlushSeconds);
}

void CItemPriceListTableCache::UpdateList(const TItemPriceListTable* pUpdateList)
{
	if (pUpdateList->wCount > SHOP_PRICELIST_MAX_NUM)
	{
		sys_err("Count overflow!");
		return;
	}

	m_data.wCount = pUpdateList->wCount;
	thecore_memcpy(m_data.aPriceInfo, pUpdateList->aPriceInfo, sizeof(TItemPriceInfo) * pUpdateList->wCount);
	m_bNeedQuery = true;
	sys_log(0, "ItemPriceListTableCache::UpdateList : OwnerID[%u] Update [%u] Items, Total [%u] Items", m_data.dwOwnerID, pUpdateList->wCount, m_data.wCount);
}

void CItemPriceListTableCache::OnFlush()
{
	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery), "DELETE FROM myshop_pricelist%s WHERE owner_id = %u", GetTablePostfix(), m_data.dwOwnerID);
	CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_DESTROY, 0, NULL);

	for (int idx = 0; idx < m_data.wCount; ++idx)
	{
		snprintf(szQuery, sizeof(szQuery),
				"INSERT INTO myshop_pricelist%s(owner_id, item_vnum, price) VALUES(%u, %u, %lld)", 
				GetTablePostfix(), m_data.dwOwnerID, m_data.aPriceInfo[idx].dwVnum, m_data.aPriceInfo[idx].lldPrice);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_SAVE, 0, NULL);
	}

	sys_log(0, "ItemPriceListTableCache::Flush : OwnerID[%u] Update [%u]Items", m_data.dwOwnerID, m_data.wCount);
	
	m_bNeedQuery = false;
}
