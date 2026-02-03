#pragma once
#include "building.h"

class Conveyer : public ItemBuilding
{
public:
	int id = 0;
	bool TakeItems() override;
	bool InputItems(ItemQuantity items) override;
	void UpdateBuilding(float dt) override;
};

// Return reference to the shared container
std::vector<Conveyer*> InitTest();

// Print the conveyor chain safely (by iterating the vector)
void printTest(std::vector<Conveyer*>& testVec);
