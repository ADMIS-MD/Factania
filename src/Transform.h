//
// Created by willb on 1/20/2026.
//

#pragma once

#include "Math.h"
#include "nds/ndstypes.h"

// Grid positions
struct Transform;

struct GridTransform {
    GridTransform(int32 x, int32 y);
    GridTransform();;
    GridTransform(const GridTransform& other);
    GridTransform(GridTransform&& other) noexcept;
    GridTransform& operator=(const GridTransform& other);
    GridTransform& operator=(GridTransform&& other) noexcept;
    GridTransform(Transform const& transform);

    int32 x;
    int32 y;
};

// Positions for anything else
struct Transform {
    Transform(Vec2 pos, u8 layer);
    Transform();
    Transform(const Transform& other);
    Transform(Transform&& other) noexcept;
    Transform& operator=(const Transform& other);
    Transform& operator=(Transform&& other) noexcept;
    Transform(GridTransform const& transform);

    Vec2 pos;
    u8 layer;
};

