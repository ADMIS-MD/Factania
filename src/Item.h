#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Item
{
public:
    int itemID = 0;
    std::string name;

    Item(int itemID_ = -1, std::string name_ = "")
    {
        itemID = itemID_;
        name = name_;
    }

    bool operator==(const Item& other) const 
    {
        return itemID == other.itemID;
    }
    Item& operator=(const Item& other) 
    {
        itemID = other.itemID;
        return *this;

    }
    Item& operator=(const int other) 
    {
        itemID = other;
        return *this;
    }
};

class ItemQuantity
{

public:
    Item item;
    int quantity;

    ItemQuantity(Item item_, int quantity_)
    {
        item = item_;
        quantity = quantity_;
    }

    ItemQuantity operator-(const ItemQuantity& other) 
    {
        if (item == other.item && quantity - other.quantity >= 0)
        {
            quantity -= other.quantity;
        }
        return *this;
    }
    ItemQuantity operator+(const ItemQuantity& other) 
    {
        if (item == other.item && quantity + other.quantity >= 0)
        {
            quantity += other.quantity;
        }  
        return *this;
    }
    ItemQuantity operator=(const ItemQuantity& other) 
    {
        if (item == other.item)
        {
            quantity = other.quantity;
        }
        return *this;
    }
    bool operator==(const ItemQuantity& other) const 
    {
        return item == other.item;
    }
};

class Recipe
{
public:
    std::vector<ItemQuantity> inputItems;
    std::vector<ItemQuantity> outputItems;
    float recipeDuration;
    float powerDraw;

    Recipe& operator=(const Recipe& other) 
    {
        inputItems = other.inputItems;
        outputItems = other.outputItems;
        recipeDuration = other.recipeDuration;
        powerDraw = other.powerDraw;
        return *this;
    }
};

enum class ItemType : int
{
    IronOre = 0,
    CopperOre,
    AdmisiumOre,
    Coal,
    IronPlate,
    CopperPlate,
    Count
};

inline const char* ItemName(ItemType id)
{
    switch (id)
    {
    case ItemType::IronOre:
        return "Iron Ore";
    case ItemType::CopperOre:
        return "Copper Ore";
    case ItemType::Coal:
        return "Coal Ore";
    case ItemType::AdmisiumOre:
        return "Admisium Ore";
    case ItemType::IronPlate:
        return "Iron Plate";
    case ItemType::CopperPlate:
        return "Copper Ore";
    default: 
        return "Unknown";
    }
}