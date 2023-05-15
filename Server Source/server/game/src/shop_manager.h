#pragma once

class CShop;
typedef class CShop * LPSHOP;

class CShopManager : public singleton<CShopManager>
{
public:
	typedef std::map<DWORD, CShop *> TShopMap;

public:
	CShopManager();
	virtual ~CShopManager();

	bool	Initialize(TShopTable * table, int size);
	void	Destroy();
	LPSHOP	Get(DWORD dwVnum);
	LPSHOP	GetByNPCVnum(DWORD dwVnum);
	bool	StartShopping(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);
	void	StopShopping(LPCHARACTER ch);
	void	Buy(LPCHARACTER ch, BYTE pos);
	void	Sell(LPCHARACTER ch, WORD wCell, WORD wCount=0);
	LPSHOP	CreatePCShop(LPCHARACTER ch, TShopItemTable * pTable, WORD wItemCount);
	LPSHOP	FindPCShop(DWORD dwVID);
	void	DestroyPCShop(LPCHARACTER ch);
	bool	ReadShopTableEx(const char* stFileName);

private:
	TShopMap	m_map_pkShop;
	TShopMap	m_map_pkShopByNPCVnum;
	TShopMap	m_map_pkShopByPC;
};
