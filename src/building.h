#pragma once

#include <list>
#include <vector>
#include "Item.h"
#include <entt.hpp>

void TempUpdateBuildings(entt::registry& registry);

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
    std::vector<Building*> inputs;
    std::vector<Building*> outputs;
    std::vector<ItemQuantity> inputInventory;
    std::vector<ItemQuantity> outputInventory;
    virtual bool InputItems(ItemQuantity items) = 0;
    virtual bool TakeItems() = 0;

private:
};

class FactoryBuilding : public ItemBuilding
{
public:
    BuildingStatus status = BuildingStatus::Unpowered;
    std::vector<Recipe> recipes;
    int selectedRecipe = -1;

    void UpdateBuilding(float dt) override;
    void SelectRecipe(int recipeNum);
    bool InputItems(ItemQuantity items) override;
    bool TakeItems() override;
    FactoryBuilding(std::vector<Recipe> recipes_, int selectedRecipe_ = -1);
    FactoryBuilding(const FactoryBuilding& other);
    FactoryBuilding();

private:
    void ResolveRecipe(Recipe* recipe);
    float craftTimer = 0;
    bool inputInventoryChanged = true;
};

class PowerGrid : public Building
{
public:
    std::vector<FactoryBuilding> connectedSinks;
    std::vector<FactoryBuilding> connectedSources;
    void UpdateBuilding(float dt) override;
    void StartGrid();

private:
    float totalGenerated = 0;
    float totalUseage = 0;
};