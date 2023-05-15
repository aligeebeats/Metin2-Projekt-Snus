#pragma once

namespace mining
{
	LPEVENT	CreateMiningEvent(LPCHARACTER ch, LPCHARACTER load, int count);
	DWORD	GetRawOreFromLoad(DWORD dwLoadVnum);
	bool	OreRefine(LPCHARACTER ch, LPCHARACTER npc, LPITEM item, long long cost, int pct, LPITEM metinstone_item);
	int		GetFractionCount();
	int		RealRefinePick(LPCHARACTER ch, LPITEM item);
	void	CHEAT_MAX_PICK(LPCHARACTER ch, LPITEM item);
	bool	IsVeinOfOre (DWORD vnum);
}
