#pragma once

enum class ItemType
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