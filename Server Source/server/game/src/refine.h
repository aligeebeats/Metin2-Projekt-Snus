#pragma once

#include "constants.h"

enum
{
	BLACKSMITH_MOB = 20016,
	ALCHEMIST_MOB = 20001,
	GOLDEN_BLACKSMITH_MOB = 21900,
	BLACKSMITH_WEAPON_MOB = 20044,
	BLACKSMITH_ARMOR_MOB = 20045,
	BLACKSMITH_ACCESSORY_MOB = 20046,
	DEVILTOWER_BLACKSMITH_WEAPON_MOB = 20074,
	DEVILTOWER_BLACKSMITH_ARMOR_MOB = 20075,
	DEVILTOWER_BLACKSMITH_ACCESSORY_MOB = 20076,
	BLACKSMITH2_MOB	= 20091,
};

class CRefineManager : public singleton<CRefineManager>
{
	typedef std::map<DWORD, TRefineTable> TRefineRecipeMap;
	public:
	CRefineManager();
	virtual ~CRefineManager();

	bool	Initialize(TRefineTable * table, int size);
	const TRefineTable* GetRefineRecipe(DWORD id);

	private:
	TRefineRecipeMap    m_map_RefineRecipe;

};
