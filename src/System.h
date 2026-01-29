#pragma once

#include "entt.hpp"

namespace core {

    class System
    {
    public:
        ~System() {}

        virtual void Update(entt::registry& registry) {}
        virtual void Draw(entt::registry& registry) {}
    };

}