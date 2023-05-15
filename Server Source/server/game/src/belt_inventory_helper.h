#pragma once
#include "char.h"
#include "item.h"

class CBeltInventoryHelper
{
public:
	typedef BYTE	TGradeUnit;

	static TGradeUnit GetBeltGradeByRefineLevel(int level)
	{
		static TGradeUnit beltGradeByLevelTable[] = 
		{
			0,
			1,
			1,
			2,
			2,
			3,
			4,
			5,
			6,
			7,
		};

		if (level >= _countof(beltGradeByLevelTable))
		{
			sys_err("CBeltInventoryHelper::GetBeltGradeByRefineLevel - Overflow level (%d", level);
			return 0;
		}

		return beltGradeByLevelTable[level];
	}

	static const TGradeUnit* GetAvailableRuleTableByGrade()
	{
		static TGradeUnit availableRuleByGrade[BELT_INVENTORY_SLOT_COUNT] = {
			1, 2, 4, 6,
			3, 3, 4, 6,
			5, 5, 5, 6,
			7, 7, 7, 7
		};

		return availableRuleByGrade;
	}

	static bool IsAvailableCell(WORD cell, int beltGrade)
	{	
		const TGradeUnit* ruleTable = GetAvailableRuleTableByGrade();

		return ruleTable[cell] <= beltGrade;
	}

	static bool IsExistItemInBeltInventory(LPCHARACTER pc)
	{
		for (WORD i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i)
		{
			LPITEM beltInventoryItem = pc->GetInventoryItem(i);

			if (NULL != beltInventoryItem)
				return true;
		}

		return false;
	}

	static bool CanMoveIntoBeltInventory(LPITEM item)
	{
		bool canMove = false;

		if (item->GetType() == ITEM_USE)
		{
			switch (item->GetSubType())
			{
				case USE_POTION:
				case USE_POTION_NODELAY:
				case USE_ABILITY_UP:
					canMove = true;
					break;
			}
		}

		return canMove;
	}

};
