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
        int tileset_texture_id;
        int scroll_x = 0;
        int scroll_y = 0;

    };

    void BeginFrame();
    void EndFrame();

}