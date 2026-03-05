#pragma once
#include <vector>
#include <string>
#include <cstdint>

#include "ItemType.h"
#include "Inventory.h"

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