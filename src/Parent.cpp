//
// Created by willb on 2/19/2026.
//

#include "Parent.h"

Parent::Parent(entt::entity e)
{
    parent = e;
}

void Parent::AttachEntities(entt::registry& registry, entt::entity parent, entt::entity child)
{
    // res
    auto* prev_parent_c = registry.try_get<Parent>(child);
    auto* children_c = registry.try_get<Children>(parent);

    if (prev_parent_c)
        registry.remove<Parent>(child);
    registry.emplace<Parent>(child, std::forward<Parent>(Parent {parent}));

    if (children_c == nullptr)
        registry.emplace<Children>(parent, std::forward<Children>(Children{child}));
    else
    {
        assert(children_c->GetChildCount() < MAX_CHILD_COUNT);
        children_c->m_children[children_c->m_child_count] = child;
        children_c->m_child_count += 1;
    }
}

entt::entity Parent::GetParent()
{
    return parent;
}

Children::Children(entt::entity e)
{
    m_children[0] = e;
    m_child_count = 1;
}

const std::array<entt::entity, 9>& Children::GetChildren()
{
    return m_children;
}

// Counter to prevent children `Parent` component destruction functions to be called while destroying parents
void setup_parents(entt::registry& registry)
{
    registry.on_destroy<Parent>().connect<[](entt::registry& r, entt::entity e)
    {
        entt::entity p = r.get<Parent>(e).GetParent();
        Children* children = r.try_get<Children>(p);
        auto& children_e= children->m_children;
        if (children == nullptr) return;
        for (int i = 0; i < children->m_child_count; ++i)
        {
            if (children_e[i] == e)
            {
                std::swap(children[i], children[children->m_child_count - 1]);
                children->m_child_count -= 1;
                return;
            }
        }
    }>();

    registry.on_destroy<Children>().connect<[](entt::registry& r, entt::entity e)
    {
        Children& children = r.get<Children>(e);
        // Preserve but set children to 0 to ensure nothing modifies our array while it is in use
        int child_count = children.m_child_count;
        children.m_child_count = 0;
        for (int i = 0; i < child_count; ++i)
        {
            entt::entity child = children.m_children[i];
            if (r.valid(child))
            {
                if (Parent* parent_c = r.try_get<Parent>(child); parent_c)
                {
                    if (parent_c->follow_destroy)
                        r.destroy(child);
                    else
                        r.remove<Parent>(child);
                }
            }
        }
    }>();
}
