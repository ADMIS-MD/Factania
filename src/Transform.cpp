//
// Created by willb on 1/20/2026.
//

#include "Transform.h"

#include <RenderSystem.h>

GridTransform::GridTransform(int32 x, int32 y): x(x),
                                                y(y)
{
}

GridTransform::GridTransform(): x(0), y(0)
{}

GridTransform::GridTransform(Vec2& vec) : x((vec.X().GetInt() + 16) / TILE_SIZE), y((vec.Y().GetInt() + 16) / TILE_SIZE)
{
    if (vec.X() < fixed(0.f))
    {
        x = vec.X().GetInt() / TILE_SIZE - 1;
    }
    else
    {
        x = vec.X().GetInt() / TILE_SIZE;
    }

    if (vec.Y() < fixed(0.f))
    {
        y = vec.Y().GetInt() / TILE_SIZE - 1;
    }
    else
    {
        y = vec.Y().GetInt() / TILE_SIZE;
    }
}

GridTransform::GridTransform(const GridTransform& other): x(other.x),
                                                          y(other.y)
{
}

GridTransform::GridTransform(GridTransform&& other) noexcept: x(other.x),
                                                              y(other.y)
{
}

GridTransform& GridTransform::operator=(const GridTransform& other)
{
    x = other.x;
    y = other.y;
    return *this;
}

GridTransform& GridTransform::operator=(GridTransform&& other) noexcept
{
    x = other.x;
    y = other.y;
    return *this;
}

GridTransform::GridTransform(Transform const& transform)
{
    x = transform.pos.X().GetInt();
    y = transform.pos.Y().GetInt();
}

Transform::Transform(Vec2 pos, u8 layer) : pos(pos), layer(layer)
{
}

Transform::Transform() : pos({}), layer(0)
{
}

Transform::Transform(const Transform& other) : pos(other.pos), layer(other.layer)
{
}

Transform::Transform(Transform&& other) noexcept : pos(other.pos), layer(other.layer)
{
}

Transform& Transform::operator=(const Transform& other)
{
    pos = other.pos;
    layer = other.layer;
    return *this;
}

Transform& Transform::operator=(Transform&& other) noexcept
{
    pos = other.pos;
    layer = other.layer;
    return *this;
}

Transform::Transform(GridTransform const& transform)
{
    pos.X() = transform.x;
    pos.Y() = transform.y;
}
