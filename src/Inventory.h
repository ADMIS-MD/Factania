#pragma once

#include "Item.h"

struct Inventory
{
	static constexpr int MAX_ITEMS = (int)ItemType::Count;
	int quantities[MAX_ITEMS] = { 0 };
	int discoveryOrder[MAX_ITEMS];
	int nextDiscoveryOrder = 0;

	Inventory()
	{
		for (int i = 0; i < MAX_ITEMS; ++i) {
			discoveryOrder[i] = -1;
		}
	}

	bool AddItem(ItemType type, int amount = 1)
	{
		if (amount <= 0) {
			return true;
		}

		const int index = (int)type;
		if (quantities[index] <= 0 && discoveryOrder[index] < 0) {
			discoveryOrder[index] = nextDiscoveryOrder++;
		}

		quantities[index] += amount;
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

	int GetDiscoveryOrder(ItemType type) const
	{
		return discoveryOrder[(int)type];
	}
};
