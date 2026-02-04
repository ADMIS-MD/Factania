#pragma once

#include "Item.h"

struct Inventory
{
	static constexpr int MAX_ITEMS = (int)ItemID::Count;
	int quantities[MAX_ITEMS] = { 0 };

	bool AddItem(ItemID id, int amount = 1)
	{
		quantities[(int)id] += amount;
		return true;
	}

	bool RemoveItem(ItemID id, int amount = 1)
	{
		int& quant = quantities[(int)id];
		if (quant < amount)
		{
			return false;
		}

		quant -= amount;
		return true;
	}

	int GetItem(ItemID id) const
	{
		return quantities[(int)id];
	}
};