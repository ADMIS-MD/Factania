#pragma once

#include <list>
#include <vector>

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

class Item
{
public:
    int itemID;
};

struct ItemQuantity
{
    Item item;
    int quantity;
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
    std::vector<Building> inputs;
    std::vector<Building> outputs;
    virtual void UpdateBuilding(float dt) = 0;
    virtual void InputItems(ItemQuantity items) = 0;
private:

};

class FactoryBuilding : Building
{
public:
    BuildingStatus status = BuildingStatus::Unpowered;
    std::vector<ItemQuantity> inputInventory;
    std::vector<ItemQuantity> outputInventory;
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