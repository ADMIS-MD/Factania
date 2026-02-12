#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Recipe
{
public:
    Inventory inputs;
    Inventory outputs;

    float recipeDuration;
    float powerDraw;

    Recipe& operator=(const Recipe& other) 
    {
        inputs = other.inputs;
        outputs = other.outputs;

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