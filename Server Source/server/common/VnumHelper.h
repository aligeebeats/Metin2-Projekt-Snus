#pragma once

class CItemVnumHelper
{
public:
	static	const bool	IsRamadanMoonRing(DWORD vnum)
	{ 
		return 71135 == vnum;
	}
	static	const bool	IsHalloweenCandy(DWORD vnum)
	{ 
		return 71136 == vnum;
	}
	static	const bool	IsHappinessRing(DWORD vnum)
	{
		return 71143 == vnum;
	}
	static	const bool	IsLovePendant(DWORD vnum)
	{
		return 71145 == vnum; 
	}
	static const bool IsAcceItem(DWORD vnum)
	{
		if ((vnum >= 85001 && vnum <= 85008) ||
			(vnum >= 85011 && vnum <= 85018) ||
			(vnum >= 85021 && vnum <= 85024) ||
			(vnum >= 86001 && vnum <= 86008) ||
			(vnum >= 86011 && vnum <= 86018) ||
			(vnum >= 86021 && vnum <= 86028) ||
			(vnum >= 86031 && vnum <= 86038) ||
			(vnum >= 86041 && vnum <= 86048) ||
			(vnum >= 86051 && vnum <= 86058) ||
			(vnum >= 86061 && vnum <= 86068))
		{
			return true;
		}
		return false;
	}
};

class CVnumHelper
{
};
