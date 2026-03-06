#pragma once
#include "building.h"

class Conveyer : public ItemBuilding
{
public:
	int id = 0;
	bool TakeItems();
	bool InputItems(ItemType items, int count) override;
	void UpdateBuilding(float dt) override;
	Conveyer()
	{
		type = BuildingType::Conveyer;
	}
};

// Print the conveyor chain safely (by iterating the vector)
void printTest(std::vector<Conveyer*>& testVec);
