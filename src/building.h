#pragma once

#include <list>
#include <vector>
#include "Item.h"

enum BuildingType {
    None,
    Source,
    Transformer,
    Transporter,
    Sink
};

enum BuildingStatus {
    None,
    Working,
    Idle,
    Unpowered
};

class Recipe
{
public:
    std::vector<ItemQuantity> inputItems;
    std::vector<ItemQuantity> outputItems;
    float recipeDuration;
};

class Building
{
public:
    BuildingType type;
    std::vector<Building> outputs;
    virtual void UpdateBuilding(float dt) = 0;
    virtual void InputItems(ItemQuantity items) = 0;
private:
    std::vector<ItemQuantity> inputInventory;
    std::vector<ItemQuantity> outputInventory;
};

class FactoryBuilding : Building
{
public:
    BuildingStatus status = BuildingStatus::Unpowered;
    std::vector<Recipe> recipes;
    void UpdateBuilding(float dt);
    void SelectRecipe(int recipeNum);
    void InputItems(ItemQuantity items);
private:
    void ResolveRecipe(Recipe* recipe);
    int selectedRecipe = -1;
    float craftTimer = 0;
    bool invChanged = true;
};