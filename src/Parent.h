//
// Created by willb on 2/19/2026.
//

#ifndef FACTANIA_PARENT_H
#define FACTANIA_PARENT_H

#include <entt.hpp>

const int MAX_CHILD_COUNT = 9;

class Parent
{
    Parent(entt::entity e);
    entt::entity parent;

public:
    static void AttachEntities(entt::registry& registry, entt::entity parent, entt::entity child);
    entt::entity GetParent();

    bool follow_destroy = true; // If true, destroyed when parent is
    friend void setup_parents(entt::registry& registry);
};

class Children
{
    Children(entt::entity e);

    int m_child_count = 0;
    std::array<entt::entity, MAX_CHILD_COUNT> m_children = {};

public:
    const std::array<entt::entity, 9>& GetChildren();
    int GetChildCount() const;

    bool follow_destroy = true; // If true, destroys parent when destroyed

    friend Parent;
    friend void setup_parents(entt::registry& registry);
};

void setup_parents(entt::registry& registry);


#endif //FACTANIA_PARENT_H
