#pragma once

#include "entt.hpp"

namespace core {

    class System
    {
    public:
        virtual ~System() = default;

        virtual void Update(entt::registry& registry) {}
        virtual void Draw(entt::registry& registry) {}
    };

}