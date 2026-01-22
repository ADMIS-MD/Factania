#pragma once

#include <list>
#include <vector>
#include "Item.h"

enum class BuildingType {
    None,
    Source,
    Transformer,
    Transporter,
    Battery,
    Sink
};

enum class BuildingStatus {
    None,
    Working,
    Idle,
    Unpowered
};

class Building
{
public:
    BuildingType type;
    virtual void UpdateBuilding(float dt) = 0;
};

class ItemBuilding : public Building
{
public:
    std::vector<Building> inputs;
    std::vector<Building> outputs;
    std::vector<ItemQuantity> inputInventory;
    std::vector<ItemQuantity> outputInventory;
    virtual bool InputItems(ItemQuantity items) = 0;
};

class FactoryBuilding : ItemBuilding
{
public:
    BuildingStatus status = BuildingStatus::Unpowered;
    std::vector<Recipe> recipes;   
    int selectedRecipe = -1;

    void UpdateBuilding(float dt);
    void SelectRecipe(int recipeNum);
    bool InputItems(ItemQuantity items);
    FactoryBuilding(std::vector<Recipe> recipes_, int selectedRecipe_ = -1);
    FactoryBuilding();



private:
    void ResolveRecipe(Recipe* recipe);
    float craftTimer = 0;
    bool inputInventoryChanged = true;
};

class PowerGrid : Building
{
public:
    std::vector<FactoryBuilding> connectedSinks;
    std::vector<FactoryBuilding> connectedSources;
    //std::vector<FactoryBuilding> connectedStorage;
    void UpdateBuilding(float dt);
    void StartGrid();

private:
    float totalGenerated = 0;
    float totalUseage = 0;
};