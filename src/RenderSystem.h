#pragma once

#include <chunk.hpp>

#include "System.h"
#include "gl2d.h"

#include "Camera.h"
#include "chunk.hpp"
#include "Math.h"

#include "BuildMode.h"

#define TILE_ROWS    2
#define TILE_COLUMNS 7
#define TILE_SIZE    16

namespace core {

    inline glImage g_tileset[TILE_ROWS * TILE_COLUMNS];

    class RenderSystem : public System
    {
    public:
        RenderSystem();
        ~RenderSystem();

        void Update(entt::registry& registry) override;
        void Draw(entt::registry& registry) override;

        bool IsTransitioning() const;

    private:
        Camera m_activeCam;

        BuildMode m_buildMode;

        int m_tileset_texture_id;
        int scroll_x = 0;
        int scroll_y = 0;

        int frameCount = 0;
        int ticksPerFrame = 6;

    };

    void BeginFrame();
    void EndFrame();

}