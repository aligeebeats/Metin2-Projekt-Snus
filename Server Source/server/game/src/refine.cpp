#include "stdafx.h"
#include "refine.h"

CRefineManager::CRefineManager()
{
}

CRefineManager::~CRefineManager()
{
}

bool CRefineManager::Initialize(TRefineTable * table, int size)
{
	m_map_RefineRecipe.clear();
	for (int i = 0; i < size; ++i, ++table)
	{
		m_map_RefineRecipe.insert(std::make_pair(table->id, *table));
	}

	return true;
}

const TRefineTable* CRefineManager::GetRefineRecipe(DWORD vnum)
{
	if (vnum == 0)
		return NULL;

	auto it = m_map_RefineRecipe.find(vnum);

	if (it == m_map_RefineRecipe.end())
	{
		return NULL;
	}

	return &it->second;
}
