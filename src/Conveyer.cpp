#include "Item.h"
#include "building.h"
#include "Conveyer.h"
#include <cstdio>
#include <nds.h>


// Simple, safe printer — iterate the vector and print each conveyor's last quantity (if any)
void printTest(std::vector<Conveyer*> &testVec)
{
    consoleClear();
    for (size_t i = 0; i < testVec.size(); ++i) {
        Conveyer* c = testVec[i];
        if (!c) { printf("[   ]"); }
        //else if (!c->outputInventory.RemoveItem()) { printf("[ %d ]", c->outputInventory.back().quantity); }
        else { printf("[   ]"); }
        if (i + 1 < testVec.size()) printf("->");
    }
    printf("\n");
}

bool Conveyer::InputItems(ItemType items, int count)
{
    outputInventory.AddItem(items, count);
    return true;
}

// safe TakeItems: move a single item and RETURN a value
bool Conveyer::TakeItems()
{
    if (inputs.empty()) return false;

    // Expect first input to be an ItemBuilding (could be a Conveyer or Factory)
    ItemBuilding* upstream = dynamic_cast<ItemBuilding*>(inputs[0]);
    if (!upstream) return false;

    // Prevent attempting to transfer to self (same inventory)
    if (&upstream->outputInventory == &outputInventory) return false;

    // If we have an empty input buffer and upstream has outputs, grab up to one item
    if (outputInventory.IsEmpty() && !upstream->outputInventory.IsEmpty())
    {;
        //Move at most one item — safe, non-blocking, deterministic
        outputInventory = upstream->outputInventory;
        upstream->outputInventory = Inventory();
        
        if (upstream->type == BuildingType::Conveyer)
        {
            dynamic_cast<Conveyer*>(upstream)->TakeItems();
        }
        return true;
    }

    // no-op if we already have items or nothing upstream
    return false;
}

void Conveyer::UpdateBuilding(float dt)
{
    // Debug: show Update being called for conveyors (can be noisy)
    //printf("Conveyer::UpdateBuilding id=%d\n", id);
    //ItemBuilding* upstream = dynamic_cast<Conveyer*>(outputs[0]);
    //if (upstream)
    //{
    //    return;
    //}
    TakeItems();
    return;
}