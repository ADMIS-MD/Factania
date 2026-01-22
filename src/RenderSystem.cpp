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
        vramSetBankE(VRAM_E_TEX_PALETTE);

        // A tile set is formed by several images of the same size that start at the
        // top left corner. It increses to the right in the top row until the end of
        // the texture is reached, then it continues to the second row.
        //
        // When all the images are put together they form a bitmap with some
        // dimensions. The dimensions can be whatever is required for that specific
        // sprite, with no restrictions.
        //
        // However, the GPU of the DS requires textures to have sizes that are power
        // of two. When you have a bitmap with dimensions that aren't a power of
        // two, padding needs to be added to the bottom and to the right to fill the
        // image up to a valid size.
        //
        // Note that if you leave enough space on the right of the texture for a new
        // image, even if there aren't graphics there, it will count.
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