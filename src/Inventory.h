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

struct InventorySlot
{
	ItemType type;
	int quantity;

	bool IsEmpty() const { return quantity == 0; }
	const char* Name() const { return ItemName(type); }
	int Index() const { return (int)type; }
};

struct Inventory
{
	static constexpr int MAX_ITEMS = (int)ItemType::Count;
	static constexpr int UNLIMITED = -1; // no cap 

	int quantities[MAX_ITEMS] = { 0 };
	int capacity = UNLIMITED; // total item cap across all types
	InventoryTag tag = InventoryTag::Generic;

	/*
	*	How to use:
	*	Inventory() -> generic, unlimited
	*   Inventory(InventoryTag::Player, 100) -> player inventory, capped at 100
	*   Inventory(InventoryTag::Building) -> building inventory, unlimited
	*/
	Inventory() = default;
	explicit Inventory(InventoryTag tag_, int capacity_ = UNLIMITED) : capacity(capacity_), tag(tag_) {}

	InventorySlot GetSLot(ItemType type) const
	{
		return { type, quantities[(int)type] };
	}

	InventorySlot GetSlotByIndex(int index) const
	{
		if (index < 0 || index >= MAX_ITEMS)
		{
			return { ItemType::Count, 0 };
		}

		return { (ItemType)index, quantities[index] };
	}

	// calls t(InventorySlot) for non-empty slot
	// pick this over a manyal loop if you care about *occupied* slots
	template<typename T>
	void ForEachOccupied(T t) const
	{
		for (int i = 0; i < MAX_ITEMS; ++i)
		{
			if (quantities[i] > 0)
			{
				t(GetSlotByIndex(i));
			}
		}
	}

	int OccupiedSlotCount() const
	{
		int n = 0;
		for (int i = 0; i < MAX_ITEMS; ++i)
		{
			if (quantities[i] > 0)
			{
				++n;
			}
		}

		return n;
	}

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
		if (amount <= 0)
		{
			return false;
		}
		if (!CanAdd(amount))
		{
			return false;
		}

		quantities[(int)type] += amount;
		return true;
	}

	bool RemoveItem(ItemType type, int amount = 1)
	{
		if (amount <= 0)
		{
			return false;
		}

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
		if (amount <= 0)
		{
			return 0;
		}

		int available = GetItem(type);
		// check what would be smaller, the amount we want, or the total amount of said item from other inventory
		int toMove = (amount < available) ? amount : available;

		if (toMove <= 0)
		{
			return 0;
		}

		
		// this clamps it, we check how much space is leftover, and only take what we can
		if (!target.CanAdd(toMove))
		{
			int space = (target.capacity == UNLIMITED) ? toMove : (target.capacity - target.TotalCount());
			toMove = (space < toMove) ? space : toMove;

			if (toMove <= 0)
			{
				return 0; // target is full
			}
		}

		target.AddItem(type, toMove);
		RemoveItem(type, toMove);
		return toMove;
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