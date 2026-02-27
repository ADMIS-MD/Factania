#pragma once

#include <chunk.hpp>

#include "System.h"
#include "gl2d.h"

#include "Camera.h"
#include "chunk.hpp"
#include "Math.h"

#include "BuildMode.h"

#define TILE_ROWS    2
#define TILE_COLUMNS 8
#define TILE_SIZE    16

namespace core {

    struct Tile
    {
        Tile(glImage image, bool collision, bool transparent);
        glImage image;
        bool collision;
        bool transparent;
    };
    inline Tile g_tileset[TILE_ROWS * TILE_COLUMNS] = {
        Tile({}, false, false),      // Grass
        Tile({}, true, true),        // Crafter 0
        Tile({}, true, true),        // Crafter 1
        Tile({}, true, true),        // Mine 0
        Tile({}, true, true),        // Shuttle 0
        Tile({}, true, true),        // Shuttle 1
        Tile({}, true, true),        // Shuttle 2
        Tile({}, false, true),       // Conveyer 0
        Tile({}, false, false),      // Ore
        Tile({}, true, true),        // Crafter 2
        Tile({}, true, true),        // Crafter 3
        Tile({}, true, true),        // Mine 1
        Tile({}, true, true),        // Shuttle 3
        Tile({}, true, true),        // Shuttle 4
        Tile({}, true, true),        // Shuttle 5
        Tile({}, false, true),       // Conveyer 1
    };

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