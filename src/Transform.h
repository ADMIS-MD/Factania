//
// Created by willb on 1/20/2026.
//

#pragma once

#include "Math.h"
#include "nds/ndstypes.h"

// Grid positions
struct GridTransform {
    int32 x;
    int32 y;
};

// Positions for anything else
struct Transform {
    Vec2 pos;
    u8 layer;
};

