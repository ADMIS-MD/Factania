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

GridTransform::GridTransform(Vec2 const& vec)
{
    x = vec.X().GetInt();
    y = vec.Y().GetInt();
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

GridTransform::GridTransform(Transform const& transform) : GridTransform(transform.pos)
{
}

u8 GridTransform::CropTo8x8Grid()
{
    return (x & 0b0111) + ((y & 0b0111) * 8);
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
