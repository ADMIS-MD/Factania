//
// Created by willb on 2/19/2026.
//

#include "Parent.h"

void Parent::AttachEntities(entt::registry& registry, entt::entity parent, entt::entity child)
{
    // res
}

entt::entity Parent::GetParent()
{
    return parent;
}

void setup_parents(entt::registry& registry)
{
    registry.on_destroy<Parent>().connect<[](entt::registry& r, entt::entity e)
    {
        entt::entity p = r.get<Parent>(e).GetParent();

    }>();
}
