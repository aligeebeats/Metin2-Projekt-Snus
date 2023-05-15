#include "../../common/singleton.h"
#include "../../common/service.h"
#include <unordered_map>
#include "constants.h"

#ifdef ENABLE_ITEMSHOP
class CItemshopManager : public singleton<CItemshopManager>
{
public:
	CItemshopManager();
	~CItemshopManager();

	void InitializeItems(TItemshopItemTable* table, WORD size);
	void InitializeCategories(TItemshopCategoryTable* table, WORD size);

	void RefreshSingleItem(TItemshopItemTable* item);
	void RemoveSingleItem(TItemshopItemTable* item);
	void AddSingleItem(TItemshopItemTable* item);

	bool IsValidHash(const char* hash);
	bool HasEnoughCoins(DWORD accID, unsigned long long ullItemPrice);
	bool IsValidPurchase(TItemshopItemTable itemInfo, WORD wCount, unsigned long long &ullPrice);
	bool CanBuyItem(LPCHARACTER ch, const char* hash, WORD wCount);
	bool BuyItem(LPCHARACTER ch, const char* hash, WORD wCount);

	const std::unordered_map <BYTE, TItemshopCategoryInfo> GetItemshopCategories();
	const std::unordered_map <BYTE, std::vector<TItemshopItemTable>> GetItemshopItems();

	std::unordered_map <BYTE, TItemshopCategoryInfo> m_ItemshopCategories;
	std::unordered_map <BYTE, std::vector<TItemshopItemTable>> m_ItemshopItems;
};
#endif