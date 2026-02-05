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
        name = other.name;
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
    Iron = 0,
    Copper,
    Coal,
    Count
};

inline const char* ItemName(ItemType id)
{
    switch (id)
    {
    case ItemType::Iron:
        return "Iron";
    case ItemType::Copper:
        return "Copper";
    case ItemType::Coal:
        return "Coal";
    default: 
        return "Unknown";
    }
}