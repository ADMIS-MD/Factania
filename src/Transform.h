//
// Created by willb on 1/20/2026.
//

#pragma once

#include "Math.h"
#include "nds/ndstypes.h"

template<typename T>
struct HashForHelper
{
    size_t operator()(const T& v) const
    {
        return hash_value(v);
    }
};

// Grid positions
struct GridTransform {
    int32 x;
    int32 y;
};

// Positions for anything else
struct Transform {
    fixed x;
    fixed y;
    u8 layer;
};

