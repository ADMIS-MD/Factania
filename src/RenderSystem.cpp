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
#include "nds.h"
#include "FactaniaSpriteSheet.h"
#include "entt.hpp"
#include "Sprite.h"
#include "Player.h"
#include "BG.h"
#include "Console.h"


//-----------------------------------------------------------------------------
//	Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Method Implementations
//-----------------------------------------------------------------------------

ChunkLookup chunk_lookup;

namespace core {

    // Adapted from https://codeberg.org/blocksds/sdk/src/branch/master/examples/gl2d/tileset_background/source/main.c

    RenderSystem::RenderSystem() : m_activeCam(Camera())
    {
        glScreen2D();

        videoSetMode(MODE_0_3D);

        // Setup some memory to be used for textures and for texture palettes
        vramSetBankA(VRAM_A_TEXTURE);
        vramSetBankB(VRAM_B_TEXTURE);
        vramSetBankC(VRAM_C_TEXTURE);
        vramSetBankE(VRAM_E_TEX_PALETTE);

        m_tileset_texture_id = glLoadTileSet(
            g_tileset,                                       // glImage array
            TILE_SIZE, TILE_SIZE,                            // tile size
            TILE_SIZE * TILE_COLUMNS, TILE_SIZE * TILE_ROWS, // bitmap area that contains tiles
            GL_RGB256,                                       // texture type
            128, 32,                                         // full VRAM texture size
            TEXGEN_TEXCOORD,                                 // texture params
            256,                                             // palette entries
            FactaniaSpriteSheetPal,                          // palette
            FactaniaSpriteSheetBitmap                        // bitmap data
        );

        if (m_tileset_texture_id < 0)
            printf("Failed to load texture: %d\n", m_tileset_texture_id);


        // Initialize Debug Console (BG0)
        ConsoleInit();

        // Bottom Screen Init
        videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT | DISPLAY_SPR_1D_SIZE_64);
        vramSetBankC(VRAM_C_SUB_BG);
        vramSetBankD(VRAM_D_SUB_SPRITE);
        oamInit(&oamSub, SpriteMapping_1D_64, false);

        // BG3 bitmap
        int bg3 = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
        u16* bgGfx = bgGetGfxPtr(bg3);
        dmaCopy(BGBitmap, bgGfx, BGBitmapLen);
        dmaCopy(BGPal, BG_PALETTE_SUB, BGPalLen);
    }

    RenderSystem::~RenderSystem()
    {
        glDeleteTextures(1, &m_tileset_texture_id);
    }

    void RenderSystem::Update(entt::registry& registry)
    {
        frameCount++;
        if (frameCount >= ticksPerFrame) {
            frameCount = 0;
            auto view = registry.view<Sprite, Animation>();
            for (auto e : view) {
                auto& sp = view.get<Sprite>(e);
                auto& an = view.get<Animation>(e);
                sp.spriteID++;
                if (sp.spriteID > an.end) sp.spriteID = an.start;
            }
        }

        const Vec2 screenCenterOffset = m_activeCam.ScreenSpaceExtent() * FFLOAT(.5f);
        Vec2 camPos = m_activeCam.GetPos();

        // Techincally the same lol
        const Vec2& boxSize = screenCenterOffset;
        const Vec2 boxHalfSize = boxSize * FFLOAT(.5f);


        const fixed left = boxHalfSize.X() + camPos.X();
        const fixed right = boxSize.X() + boxHalfSize.X() + camPos.X();
        const fixed top = boxHalfSize.Y() + camPos.Y();
        const fixed bottom = boxSize.Y() + boxHalfSize.Y() + camPos.Y();

        auto view = registry.view<PlayerState, Transform>();
        for (auto e : view) {
            auto& tr = view.get<Transform>(e);

            if (tr.pos.X() < left) {
                camPos.X() += tr.pos.X() - left;
            }
            else if (tr.pos.X() > right) {
                camPos.X() += tr.pos.X() - right;
            }

            if (tr.pos.Y() < top) {
                camPos.Y() += tr.pos.Y() - top;
            }
            else if (tr.pos.Y() > bottom) {
                camPos.Y() += tr.pos.Y() - bottom;
            }

            m_activeCam.SetPos(camPos);
            break;
        }

        bgUpdate();
    }

    void RenderSystem::Draw(entt::registry& registry)
    {
        Vec2 world = m_activeCam.GetPos();
        fixed x = world.X();
        fixed y = world.Y();

        GridTransform grid{ world };
        ChunkPosition pos = ChunkPosition::FromGridTransform(grid);
        entt::entity center = chunk_lookup.GetChunk(pos);
        if (!registry.valid(center))
        {
            center = Chunk::MakeChunk(chunk_lookup, registry, pos);
        }
        Chunk& center_chunk = registry.get<Chunk>(center);
        center_chunk.FillSurrounding(chunk_lookup, registry, pos);
        center_chunk.Draw(m_activeCam, pos);
        int xc = pos.x;
        int yc = pos.y;
        const ChunkPosition transforms[8] = {
            {static_cast<int16>(xc + 0), static_cast<int16>(yc + 1)},
            {static_cast<int16>(xc + 1), static_cast<int16>(yc + 1)},
            {static_cast<int16>(xc + 1), static_cast<int16>(yc + 0)},
            {static_cast<int16>(xc + 1), static_cast<int16>(yc - 1)},
            {static_cast<int16>(xc + 0), static_cast<int16>(yc - 1)},
            {static_cast<int16>(xc - 1), static_cast<int16>(yc - 1)},
            {static_cast<int16>(xc - 1), static_cast<int16>(yc + 0)},
            {static_cast<int16>(xc - 1), static_cast<int16>(yc + 1)},
        };
        for (int i = 0; i < 8; ++i)
        {
            registry.get<Chunk>(center_chunk.surrounding_chunks[i]).Draw(m_activeCam, transforms[i]);
        }

        // Draw every sprite in Mainscreen
        auto view = registry.view<Sprite, Transform>();
        for (auto spriteEntts : view) {
            auto& sp = view.get<Sprite>(spriteEntts);
            if (sp.hide == true) continue;

            auto& tr = view.get<Transform>(spriteEntts);
            Vec2 wtc = m_activeCam.WorldToCamera(tr.pos);
            wtc += sp.camDrawOffset;

            int flip = sp.xFlip ? GL_FLIP_H : GL_FLIP_NONE;

            glSprite(wtc.X().GetInt(), wtc.Y().GetInt(), flip, &sp.sprite[sp.spriteID]);
        }

        // Draw every sprite in Subscreen
        auto viewSub = registry.view<SubSprite, Transform>();
        for (auto e : viewSub)
        {
            auto& ss = viewSub.get<SubSprite>(e);
            auto& tr = viewSub.get<Transform>(e);

            int sx = tr.pos.X().GetInt();
            int sy = tr.pos.Y().GetInt();

            oamSet(&oamSub,
                ss.oamId,
                sx, sy,
                0, 0,
                ss.size,
                SpriteColorFormat_256Color,
                ss.gfx,
                -1,
                false,
                ss.hide,
                ss.xFlip,
                false,
                false
            );
        }
        oamUpdate(&oamSub);
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