#pragma once

#include <chunk.hpp>

#include "System.h"
#include "gl2d.h"

#include "Camera.h"
#include "chunk.hpp"

#define TILE_ROWS    2
#define TILE_COLUMNS 7

#define TILE_SIZE 16

extern ChunkLookup chunk_lookup;

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
        
        int m_tileset_texture_id;
        int scroll_x = 0;
        int scroll_y = 0;

        ChunkLookup cl;

        int frameCount = 0;
        int ticksPerFrame = 6;
    };

    void BeginFrame();
    void EndFrame();

}