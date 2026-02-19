//
// Created by willb on 2/19/2026.
//

#ifndef FACTANIA_PARENT_H
#define FACTANIA_PARENT_H

#include <entt.hpp>

class Parent
{
    entt::entity parent;

public:
    static void AttachEntities(entt::registry& registry, entt::entity parent, entt::entity child);
    entt::entity GetParent();
};

class Children
{
    std::unordered_set<entt::entity> children;

public:
    const std::unordered_set<entt::entity> GetChildren();
};

void setup_parents(entt::registry& registry);


#endif //FACTANIA_PARENT_H
