#ifndef __INCLUDE_HEADER_OFFLINESHOP_CACHE__
#define __INCLUDE_HEADER_OFFLINESHOP_CACHE__

#ifdef __ENABLE_NEW_OFFLINESHOP__
namespace offlineshop
{
	template <class T>
	void ZeroObject(T& obj)
	{
		memset(&obj, 0, sizeof(obj));
	}

	template <class T>
	void CopyObject(T& objDest, const T& objSrc)
	{
		memcpy(&objDest, &objSrc, sizeof(objDest));
	}

	template <class T>
	void CopyContainer(T& objDest, const T& objSrc)
	{
		objDest = objSrc;
	}

	template <class T>
	void DeletePointersContainer(T& obj)
	{
		typename T::iterator it = obj.begin();
		for (; it != obj.end(); it++)
			delete(*it);
	}

	struct SQueryInfoAddItem;
	struct SQueryInfoCreateShop;

	class CShopCache
	{
	public:
		typedef struct SShopCacheItemInfo
		{
			TPriceInfo		price;
			TItemInfoEx		item;
			bool			bLock;

			SShopCacheItemInfo()
			{
				ZeroObject(price);
				ZeroObject(item);

				bLock = false;
			}

		} TShopCacheItemInfo;

		typedef std::map<DWORD, TShopCacheItemInfo> SHOPCACHE_MAP;
		typedef struct SShopCacheInfo
		{
			DWORD	dwDuration;
			char	szName[OFFLINE_SHOP_NAME_MAX_LEN];

			SHOPCACHE_MAP 	itemsmap;
			SHOPCACHE_MAP	soldsmap;
#ifdef ENABLE_IRA_REWORK
			TShopPosition	posDetails;
#endif

		} TShopCacheInfo;

		typedef std::map<DWORD, TShopCacheItemInfo>::iterator ITEM_ITER;

	public:
		CShopCache();
		~CShopCache();

		bool		Get(DWORD dwOwnerID, TShopCacheInfo** ppCache) const;
		bool		AddItem(DWORD dwOwnerID, const TShopCacheItemInfo& rItem);
		bool		RemoveItem(DWORD dwOwnerID, DWORD dwItemID);
		bool		SellItem(DWORD dwOwnerID, DWORD dwItemID);
		bool		LockSellItem(DWORD dwOwnerID, DWORD dwItemID);
		bool		UnlockSellItem(DWORD dwOwnerID, DWORD dwItemID);
		bool		EditItem(DWORD dwOwnerID, DWORD dwItemID, const TPriceInfo& rItemPrice);
		bool		CloseShop(DWORD dwOwnerID);
#ifdef ENABLE_IRA_REWORK
		bool		CreateShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName, std::vector<TShopCacheItemInfo>& items, TShopPosition pos);
#else
		bool		CreateShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName, std::vector<TShopCacheItemInfo>& items);
#endif
		bool		CreateShopAddItem(SQueryInfoCreateShop* qi, const TShopCacheItemInfo& rItem);
		bool		ChangeShopName(DWORD dwOwnerID, const char* szName);
		bool		PutItem(DWORD dwOwnerID, DWORD dwItemID, const TShopCacheItemInfo& rItem, bool isSold=false);
#ifdef ENABLE_IRA_REWORK
		bool		PutShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName, TShopPosition pos);
#else
		bool		PutShop(DWORD dwOwnerID, DWORD dwDuration, const char* szName);
#endif
		DWORD		GetCount() const {return m_shopsMap.size();}
		void		EncodeCache(CPeer* peer) const;
		DWORD		GetItemCount() const;

		void		ShopDurationProcess();
		void		UpdateDurationQuery(DWORD dwOwnerID, const TShopCacheInfo& rShop );

	private:
		typedef std::map<DWORD, TShopCacheInfo>::iterator CACHEITER;
		typedef std::map<DWORD, TShopCacheInfo>::const_iterator CONST_CACHEITER;
		std::map<DWORD, TShopCacheInfo> m_shopsMap;
	};

	class CSafeboxCache
	{
	public:
		typedef struct SSafeboxCacheInfo
		{
			TValutesInfo	valutes;
			std::map<DWORD, TItemInfoEx>
				itemsmap;
		} TSafeboxCacheInfo;

		typedef std::map<DWORD, TSafeboxCacheInfo> CACHEMAP;
		typedef std::map<DWORD, TSafeboxCacheInfo>::iterator CHACHEITER;
		typedef std::map<DWORD, TSafeboxCacheInfo>::const_iterator CHACHECONSTITER;

	public:
		CSafeboxCache();
		~CSafeboxCache();

		bool				Get(DWORD dwOwnerID, TSafeboxCacheInfo** ppSafebox) const;
		bool				PutSafebox(DWORD dwOwnerID, const TSafeboxCacheInfo& rSafebox);
		bool				PutItem(DWORD dwOwnerID, DWORD dwItem, const TItemInfoEx& item);
		bool				RemoveItem(DWORD dwOwner, DWORD dwItemID);
		bool				AddItem(DWORD dwOwnerID, const TItemInfoEx& item);
		bool				AddValutes(DWORD dwOwnerID, const TValutesInfo& val);
		bool				RemoveValutes(DWORD dwOwnerID, const TValutesInfo& val);
		void				AddValutesAsQuery(DWORD dwOwnerID, const TValutesInfo& val);
		void				ItemExpirationProcess();

		TSafeboxCacheInfo*	CreateSafebox(DWORD dwOwnerID);
		DWORD				GetCount() const		{return m_safeboxMap.size();}
		DWORD				GetItemCount() const;

		TSafeboxCacheInfo*	LoadSafebox(DWORD dwPID);

	private:
		CACHEMAP	m_safeboxMap;

	};

	class COfferCache
	{
	public:
		typedef struct SOfferCacheInfo
		{
			DWORD		dwItemID,dwOwnerID, dwOffererID;
			TPriceInfo	price;
			bool		bNoticed,bAccepted;

			char		szBuyerName[CHARACTER_NAME_MAX_LEN+1];
		} TOfferCacheInfo;

		typedef std::map<DWORD, TOfferCacheInfo> CACHEMAP;
		typedef CACHEMAP::iterator CACHEITER;
		typedef CACHEMAP::const_iterator CONST_CACHEITER;
		typedef std::vector<DWORD> OFFERIDVEC;
		typedef std::map<DWORD, OFFERIDVEC>    FINDOFFERBYSHOP;

	public:
		COfferCache();
		~COfferCache();

		bool	Puts(DWORD dwOfferID, const TOfferCacheInfo& rOffer);
		bool	Get(DWORD dwOfferID, TOfferCacheInfo** ppOffer) const;
		bool	GetOffersByShopOwner(DWORD dwOwnerID ,  COfferCache::OFFERIDVEC** ppVec);
		bool	GetOffersByItemID(DWORD dwItemID, std::vector<DWORD>& offerIDs);
		bool	AddOffer(const TOfferCacheInfo& rOffer);
		bool	NoticedOffer(DWORD dwOfferID);
		bool	AcceptOffer(DWORD dwOfferID);
		bool	CancelOffer(DWORD dwOfferID,DWORD dwOwnerID);
		bool	RemoveOffersByShopOwner(DWORD dwShopOwnerID);

		DWORD	GetCount() {return m_mapOffer.size();}
		void	EncodeCache(CPeer* peer) const;

	private:
		CACHEMAP			m_mapOffer;
		FINDOFFERBYSHOP		m_findOffersByShop;

	};

	class CAuctionCache 
	{
	public:
		typedef std::vector<TAuctionOfferInfo>						AUCTIONOFFERVEC;
		typedef std::map<DWORD, TAuctionInfo>						AUCTIONMAP;
		typedef std::map<DWORD,AUCTIONOFFERVEC >	AUCTIONOFFERMAP;

	public:
		CAuctionCache();
		~CAuctionCache();

		bool	PutsAuction(const TAuctionInfo& auction);
		bool	PutsAuctionOffer(const TAuctionOfferInfo& auctionOffer);
		bool	AddAuction(const TAuctionInfo& auction);
		bool	AddOffer(const TAuctionOfferInfo& auctionOffer, bool quering = true);
		bool	ExpiredAuction(DWORD dwOwnerID);
		bool	AuctionDurationProcess();

		DWORD	GetBestBuyer(DWORD dwOwnerID, TAuctionOfferInfo** ppOffer);

		bool	Get(DWORD dwOwnerID, TAuctionInfo** ppInfo);
		bool	GetOffers(DWORD dwOwnerID, AUCTIONOFFERVEC** ppVec);

		DWORD	GetCount();
		DWORD	GetOffersCount();

		void	EncodeCache(CPeer* peer);

	private:
		void	__UpdateDuration(const TAuctionInfo& auction);

	private:
		AUCTIONMAP			m_mapAuction;
		AUCTIONOFFERMAP		m_mapAuctionOffer;

	};

	struct SQueryInfoAddItem
	{
		DWORD dwOwnerID;
		CShopCache::TShopCacheItemInfo item;
	};

	struct SQueryInfoCreateShop
	{
		DWORD dwOwnerID;
		DWORD dwDuration;
		char  szName[OFFLINE_SHOP_NAME_MAX_LEN];
		std::vector<CShopCache::TShopCacheItemInfo> items;
		std::vector<DWORD> ids;
		DWORD dwItemIndex;
#ifdef ENABLE_IRA_REWORK
		TShopPosition pos;
#endif
	};

	struct SQueryInfoSafeboxAddItem
	{
		DWORD		dwOwnerID;
		TItemInfoEx item;
	};

	struct SQueryInfoOfferAdd
	{
		COfferCache::TOfferCacheInfo offer;
	};
}

#endif
#endif