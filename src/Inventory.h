#pragma once

#include "ItemType.h"

struct Inventory
{
	static constexpr int MAX_ITEMS = (int)ItemType::Count;
	int quantities[MAX_ITEMS] = { 0 };

	bool AddItem(ItemType type, int amount = 1)
	{
		quantities[(int)type] += amount;
		return true;
	}

	bool RemoveItem(ItemType type, int amount = 1)
	{
		int& quant = quantities[(int)type];
		if (quant < amount)
		{
			return false;
		}

		quant -= amount;
		return true;
	}

	int GetItem(ItemType type) const
	{
		return quantities[(int)type];
	}
	
	bool IsEmpty() const
	{
		for (int i = 0; i < MAX_ITEMS; ++i)
		{
			if (quantities[i] > 0)
			{
				return false;
			}
		}
		return true;
	}
};