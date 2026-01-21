//
// Created by willb on 1/20/2026.
//

#pragma once

#include "nds/ndstypes.h"

using fixed = float;

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

