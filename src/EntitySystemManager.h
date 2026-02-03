#pragma once

#include "System.h"
#include "entt.hpp"

class EntitySystemManager : public core::System
{
public:
    EntitySystemManager(entt::registry& registry);
    ~EntitySystemManager();

    void Update(entt::registry& registry) override;
};