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
#include "entt.hpp"
#include "Sprite.h"
#include "Player.h"


//-----------------------------------------------------------------------------
//	Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Method Implementations
//-----------------------------------------------------------------------------

namespace core {

    // Adapted from https://codeberg.org/blocksds/sdk/src/branch/master/examples/gl2d/tileset_background/source/main.c

    RenderSystem::RenderSystem() : m_activeCam(Camera())
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

    void RenderSystem::Update(entt::registry& registry)
    {
        const int boxW = SCREENW / 2;
        const int boxH = SCREENH / 2;

        const int left = (SCREENW - boxW) / 2;
        const int right = left + boxW;
        const int top = (SCREENH - boxH) / 2;
        const int bottom = top + boxH;

        auto view = registry.view<PlayerState, Transform>();
        for (auto e : view) {
            auto& tr = view.get<Transform>(e);

            Vec2 camPos = m_activeCam.GetPos();
            Vec2 screenPos = tr.pos - camPos;

            if (screenPos.X().GetInt() < left) {
                camPos.X() = tr.pos.X() - fixed(static_cast<int32>(left));
            }
            else if (screenPos.X().GetInt() > right) {
                camPos.X() = tr.pos.X() - fixed(static_cast<int32>(right));
            }

            if (screenPos.Y().GetInt() < top) {
                camPos.Y() = tr.pos.Y() - fixed(static_cast<int32>(top));
            }
            else if (screenPos.Y().GetInt() > bottom) {
                camPos.Y() = tr.pos.Y() - fixed(static_cast<int32>(bottom));
            }

            m_activeCam.SetPos(camPos);
            break;
        }
    }

    void RenderSystem::Draw(entt::registry& registry)
    {
        Chunk c;
        c.Draw(m_activeCam);

        auto view = registry.view<Sprite, Transform>();
        for (auto spriteEntts : view) {
            auto& sp = view.get<Sprite>(spriteEntts);
            auto& tr = view.get<Transform>(spriteEntts);
            fixed x = m_activeCam.WorldToCamera().X();
            fixed y = m_activeCam.WorldToCamera().Y();
            int flip = sp.xFlip ? GL_FLIP_H : GL_FLIP_NONE;
            int drawX = (tr.pos.X() + x).GetInt() - PLAYER_SPR / 2;
            int drawY = (tr.pos.Y() + y).GetInt() - PLAYER_SPR;

            glSprite(drawX, drawY, flip, &sp.sprite[sp.spriteID]);
        }
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