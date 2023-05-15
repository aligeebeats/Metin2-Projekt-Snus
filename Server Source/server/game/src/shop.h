#pragma once

enum
{
	SHOP_MAX_DISTANCE = 1000
};

class CGrid;

class CShop
{
	public:
		typedef struct shop_item
		{
			DWORD		vnum;
			long long	price;
			WORD		count;
			LPITEM		pkItem;
			int			itemid;
			long		alSockets[ITEM_SOCKET_MAX_NUM];
			TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
			DWORD		price_type;
			DWORD		price_vnum;

			shop_item()
			{
				vnum = 0;
				price = 0;
				count = 0;
				itemid = 0;
				pkItem = NULL;
				price_type = 1,
				price_vnum = 0,
				memset(&alSockets, 0, sizeof(alSockets));
				memset(&aAttr, 0, sizeof(aAttr));
			}
		} SHOP_ITEM;

		CShop();
		virtual ~CShop();

		bool			Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pItemTable);
		void			SetShopItems(TShopItemTable * pItemTable, WORD wItemCount);
		virtual void	SetPCShop(LPCHARACTER ch);
		virtual bool	IsPCShop()	{ return m_pkPC ? true : false; }
		virtual bool	IsShopEx() const { return false; };
		virtual bool	AddGuest(LPCHARACTER ch,DWORD owner_vid, bool bOtherEmpire);
		void			RemoveGuest(LPCHARACTER ch);
		void			RemoveAllGuests();
		virtual int		Buy(LPCHARACTER ch, BYTE pos);
		void			BroadcastUpdateItem(BYTE pos);
		int				GetNumberByVnum(DWORD dwVnum);
		virtual bool	IsSellingItem(DWORD itemID);
		DWORD			GetVnum() { return m_dwVnum; }
		DWORD			GetNPCVnum() { return m_dwNPCVnum; }

	protected:
		void			Broadcast(const void * data, int bytes);

	protected:
		DWORD		m_dwVnum;
		DWORD		m_dwNPCVnum;
		CGrid *		m_pGrid;
		typedef boost::unordered_map<LPCHARACTER, bool> GuestMapType;
		GuestMapType m_map_guest;
		std::vector<SHOP_ITEM>	m_itemVector;
		LPCHARACTER	m_pkPC;
};
