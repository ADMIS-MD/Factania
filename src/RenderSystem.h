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
    private:
        // TEMP!!
        float x, y, z;
        int angle_x, angle_z;
    };

    void BeginFrame();
    void EndFrame();

}