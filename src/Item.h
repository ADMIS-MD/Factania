#pragma once
#include <vector>

class Item
{
public:
    int itemID;

    bool operator==(const Item& other) const 
    {
        return itemID == other.itemID;
    }
    bool operator=(const Item& other) 
    {
        return itemID = other.itemID;
    }
    bool operator=(const int other) 
    {
        return itemID = other;
    }
};

class ItemQuantity
{

public:
    Item item;
    int quantity;

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

    Recipe operator=(const Recipe& other) 
    {
        inputItems = other.inputItems;
        outputItems = other.outputItems;
        return *this;
    }
};