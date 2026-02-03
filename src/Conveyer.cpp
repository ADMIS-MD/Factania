#include "Item.h"
#include "building.h"
#include "Conveyer.h"
#include <cstdio>
#include <nds.h>


// safe InitTest: seed the FIRST conveyor (convs[0]) and wire pointers
std::vector<Conveyer*> InitTest()
{
    static std::vector<Conveyer*> convs;
    convs.clear();

    Conveyer* conv1 = new Conveyer();
    conv1->id = 1;
    Conveyer* conv2 = new Conveyer();
    conv2->id = 2;
    Conveyer* conv3 = new Conveyer();
    conv3->id = 3;

    // wire: conv1 -> conv2 -> conv3 (conv2 inputs conv1, conv3 inputs conv2)
    conv2->inputs.push_back(conv1);
    conv3->inputs.push_back(conv2);

    convs.push_back(conv1);
    convs.push_back(conv2);
    convs.push_back(conv3);

    // seed first conveyor with an item (conv1)
    ItemQuantity itemA;
    itemA.item = Item(1);
    itemA.quantity = 5;
    conv1->outputInventory.push_back(itemA);

    return convs;
}

// Simple, safe printer — iterate the vector and print each conveyor's last quantity (if any)
void printTest(std::vector<Conveyer*> &testVec)
{
    consoleClear();
    for (size_t i = 0; i < testVec.size(); ++i) {
        Conveyer* c = testVec[i];
        if (!c) { printf("[   ]"); }
        else if (!c->outputInventory.empty()) { printf("[ %d ]", c->outputInventory.back().quantity); }
        else { printf("[   ]"); }
        if (i + 1 < testVec.size()) printf("->");
    }
    printf("\n");
}

bool Conveyer::InputItems(ItemQuantity items)
{
    outputInventory.push_back(items);
    return true;
}

// safe TakeItems: move a single item and RETURN a value
bool Conveyer::TakeItems()
{
    if (inputs.empty()) return false; 
    ItemBuilding* upstream = inputs[0];
    if (!upstream) return false;

    if (inputInventory.empty() && !upstream->outputInventory.empty())
    {
        ItemQuantity q = upstream->outputInventory.back();
        upstream->outputInventory.pop_back();
        outputInventory.push_back(q);
        upstream->TakeItems();
        return false;

    }
    else
    {
        upstream->TakeItems();
    }


    // move one item from upstream to this conveyor

    return true;
}

void Conveyer::UpdateBuilding(float dt)
{
    printf("updating");
    // If we have an item and a downstream, push one item

    // Otherwise try to pull one from upstream
    TakeItems();
}