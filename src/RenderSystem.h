#pragma once

#include "System.h"

namespace core {

    class RenderSystem : public System
    {
    public:
        RenderSystem();
        ~RenderSystem();

        void Update() override;
        void Draw() override;
    };

}