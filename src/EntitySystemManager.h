#pragma once

#include "System.h"
#include "entt.hpp"

class EntitySystemManager : public core::System
{
public:
    EntitySystemManager();
    ~EntitySystemManager();

    void Update(entt::registry& registry) override;
};