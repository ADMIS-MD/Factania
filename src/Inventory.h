#pragma once

#include "Item.h"

// probably a better way to do this, but a tag for each inventory
// add as needed
enum class InventoryTag : uint8_t
{
	Generic = 0,
	Player,
	Building
};

inline const char* InventoryTagName(InventoryTag tag)
{
	switch (tag)
	{
	case InventoryTag::Player:
		return "Player";
	case InventoryTag::Building:
		return "Building";
	default: 
		return "Generic";
	}
}

struct Inventory
{
	static constexpr int MAX_ITEMS = (int)ItemType::Count;
	static constexpr int UNLIMITED = -1;

	int quantities[MAX_ITEMS] = { 0 };
	int capacity = UNLIMITED; // total item cap across all types
	InventoryTag tag = InventoryTag::Generic;

	Inventory() = default;
	explicit Inventory(InventoryTag tag_, int capacity_ = UNLIMITED) : capacity(capacity_), tag(tag_) {}

	int TotalCount() const
	{
		int n = 0;
		for (int i = 0; i < MAX_ITEMS; ++i) n += quantities[i];
		return n;
	}

	bool HasItem(ItemType type, int amount = 1) const
	{
		return quantities[(int)type] >= amount;
	}

	bool CanAdd(int amount = 1) const
	{
		if (capacity == UNLIMITED) return true;
		return TotalCount() + amount <= capacity;
	}

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

	// moves up to an amount of a type from this to a target, respects target capacity
	// returns how much was actually moved
	int TransferTo(Inventory& target, ItemType type, int amount = 1)
	{
		int available = GetItem(type);
		int toMove = (amount < available) ? amount : available;
		
		if (!target.CanAdd(toMove))
		{
			int space = (target.capacity == UNLIMITED) ? toMove : (target.capacity - target.TotalCount());
			toMove = (space < toMove) ? space : toMove;

			if (toMove <= 0)
			{
				return 0;
			}
		}

		RemoveItem(type, toMove);
		target.AddItem(type, toMove);
		return toMove;
	}
};