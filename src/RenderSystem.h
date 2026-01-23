#pragma once

#include "System.h"
#include "gl2d.h"

#include "Camera.h"

#define TILE_ROWS    1
#define TILE_COLUMNS 2

#define TILE_SIZE 32

namespace core {

    inline glImage g_tileset[TILE_ROWS * TILE_COLUMNS];

    class RenderSystem : public System
    {
    public:
        RenderSystem();
        ~RenderSystem();

        void Update(entt::registry& registry) override;
        void Draw(entt::registry& registry) override;
    private:
        Camera m_activeCam;
        
        int tileset_texture_id;
        int scroll_x = 0;
        int scroll_y = 0;

    };

    void BeginFrame();
    void EndFrame();

}