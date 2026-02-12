#pragma once

#include "Item.h"

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

	void ClearInventory()
	{
		for (int i = 0; i < MAX_ITEMS; ++i)
		{
			quantities[i] = 0;
		}
	}

	void PrintInventory(Inventory inv)
	{
		printf("Inventory:\n");
		for (int i = 0; i < (int)ItemType::Count; ++i)
		{
			int q = inv.quantities[i];
			if (q > 0)
			{
				printf("  %s x%d\n", ItemName((ItemType)i), q);
			}
		}
	}
};