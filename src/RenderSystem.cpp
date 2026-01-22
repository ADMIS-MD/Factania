//-----------------------------------------------------------------------------
//
//	File:   RenderSystem.cpp
//  Author: Nicholas Brennan
//  Date:   01/20/2026
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Includes
//-----------------------------------------------------------------------------

#include "RenderSystem.h"

#include <chunk.hpp>
#include <gl2d.h>
#include "planet_tiles.h"
#include <nds.h>

//-----------------------------------------------------------------------------
//	Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Method Implementations
//-----------------------------------------------------------------------------

namespace core {

    // Adapted from https://codeberg.org/blocksds/sdk/src/branch/master/examples/gl2d/tileset_background/source/main.c

    RenderSystem::RenderSystem()
    {
        glScreen2D();

        videoSetMode(MODE_0_3D);

        // Setup some memory to be used for textures and for texture palettes
        vramSetBankA(VRAM_A_TEXTURE_SLOT0);
        vramSetBankB(VRAM_B_TEXTURE_SLOT1);
        vramSetBankE(VRAM_E_TEX_PALETTE);

        tileset_texture_id = glLoadTileSet(
            g_tileset,                                         // glImage array
            TILE_SIZE, TILE_SIZE,                            // tile size
            TILE_SIZE * TILE_COLUMNS, TILE_SIZE * TILE_ROWS, // bitmap area that contains tiles (2 rows only)
            GL_RGB256,                                       // texture type
            TILE_SIZE * TILE_COLUMNS, TILE_SIZE,             // full VRAM texture size
            TEXGEN_TEXCOORD,                                 // texture params
            256,                                             // palette entries
            planet_tilesPal,                                 // palette
            planet_tilesBitmap                               // bitmap data
        );

        if (tileset_texture_id < 0)
            printf("Failed to load texture: %d\n", tileset_texture_id);

    }

    RenderSystem::~RenderSystem()
    {
        glDeleteTextures(1, &tileset_texture_id);
    }

    void RenderSystem::Update()
    {
        const int scrollSpeed = 2;
        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_UP)
            scroll_y += scrollSpeed;
        if (keys & KEY_DOWN)
            scroll_y -= scrollSpeed;

        if (keys & KEY_LEFT)
            scroll_x += scrollSpeed;
        if (keys & KEY_RIGHT)
            scroll_x -= scrollSpeed;

    }

    void RenderSystem::Draw()
    {
        Chunk c;
        c.Draw();
    }

    void BeginFrame()
    {
        glBegin2D();
    }

    void EndFrame()
    {
        glEnd2D();
        glFlush(0);
    }

}