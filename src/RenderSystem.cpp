//-----------------------------------------------------------------------------
//
//  File:   RenderSystem.cpp
//  Author: Nicholas Brennan
//  Date:   01/20/2026
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Includes
//-----------------------------------------------------------------------------

#include <chunk.hpp>
#include <gl2d.h>
#include <nds.h>
#include <entt.hpp>

#include "RenderSystem.h"
#include "Player.h"
#include "Sprite.h"
#include "Pause.h"
#include "Console.h"

#include "FactaniaSpriteSheet.h"

//-----------------------------------------------------------------------------
//	Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Method Implementations
//-----------------------------------------------------------------------------

namespace core {

    // Adapted from https://codeberg.org/blocksds/sdk/src/branch/master/examples/gl2d/tileset_background/source/main.c

    RenderSystem::RenderSystem(entt::registry& registry) : m_activeCam(Camera())
    {
        glScreen2D();

        // Main (top)
        videoSetMode(MODE_0_3D
            | DISPLAY_BG0_ACTIVE
            | DISPLAY_BG1_ACTIVE
            | DISPLAY_BG2_ACTIVE
            | DISPLAY_SPR_ACTIVE
            | DISPLAY_SPR_1D_BMP
            | DISPLAY_SPR_1D_SIZE_128);

        // 3D textures
        vramSetBankA(VRAM_A_TEXTURE);
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

        // Main OAM (used by build-mode captured overlay)
        oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

        // Debug console (BG0) under main screen
        ConsoleInit();

        // Sub (bottom)
        videoSetModeSub(MODE_5_2D
            | DISPLAY_BG1_ACTIVE
            | DISPLAY_BG3_ACTIVE
            | DISPLAY_SPR_ACTIVE
            | DISPLAY_SPR_1D_BMP
            | DISPLAY_SPR_1D_SIZE_128);

        // Pause image load
        vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);
        PauseUI::PauseImageInit();

        // Bank B is for captureing
        vramSetBankB(VRAM_B_LCD);
        vramSetBankC(VRAM_C_SUB_BG);
        vramSetBankD(VRAM_D_SUB_SPRITE);

        oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);

        oamClear(&oamMain, 0, 128);
        oamClear(&oamSub, 0, 128);

        // BuildMode initialization
        m_buildMode.Init();

        // Inventory initialization
        m_inventoryUI.Init(registry);
    }

    RenderSystem::~RenderSystem()
    {
        setBrightness(1, 0);
        glDeleteTextures(1, &m_tileset_texture_id);
    }

    bool RenderSystem::IsTransitioning() const
    {
        return m_buildMode.IsTransitioning();
    }

    void RenderSystem::Update(entt::registry& registry)
    {
        // OAM commits once per frame
        oamUpdate(&oamMain);
        oamUpdate(&oamSub);

        if (registry.ctx().get<PauseControl>().pause) {
            setBrightness(1, -4);
            setBrightness(2, -4);
            return;
        }
        else {
            setBrightness(1, m_buildMode.GetFlashLevel());
            setBrightness(2, 0);
        }

        // Build mode update
        m_buildMode.Update(registry, m_activeCam);
        m_inventoryUI.Update(registry, m_buildMode);

        if (m_buildMode.IsTransitioning()) {
            registry.ctx().get<PauseControl>().transition = true;
        }
        else {
            registry.ctx().get<PauseControl>().transition = false;
            // Previous update logic
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
        }

        ConsoleTick();
        bgUpdate();
    }

    void RenderSystem::Draw(entt::registry& registry)
    {
        Vec2 world = m_activeCam.GetPos();

        auto& lookup = registry.ctx().get<ChunkLookup>();

        GridTransform grid{ world };
        ChunkPosition pos = ChunkPosition::FromGridTransform(grid);
        entt::entity center = lookup.GetChunk(pos);

        // Don't touch chunk if paused
        if (registry.valid(center) || !registry.ctx().get<PauseControl>().PauseEntity()) {
            if (!registry.valid(center)) {
                center = Chunk::MakeChunk(lookup, registry, pos);
            }
            Chunk& center_chunk = registry.get<Chunk>(center);
            if (!registry.ctx().get<PauseControl>().PauseEntity()) {
                center_chunk.FillSurrounding(lookup, registry, pos);
            }

            for (int16 i = pos.x; i <= pos.x + 2; ++i) {
                for (int16 j = pos.y; j <= pos.y + 2; ++j) {
                    ChunkPosition p{ i, j };
                    entt::entity e = lookup.GetChunk(p);
                    if (!registry.valid(e)) continue;
                    registry.get<Chunk>(e).Draw(m_activeCam, p);
                }
            }
        }

        // Draw every sprite in Main screen
        auto view = registry.view<Sprite, Transform>();
        for (auto spriteEntts : view)
        {
            auto& sp = view.get<Sprite>(spriteEntts);
            if (sp.hide == true) continue;

            auto& tr = view.get<Transform>(spriteEntts);
            Vec2 wtc = m_activeCam.WorldToCamera(tr.pos);
            wtc += sp.camDrawOffset;

            int flip = sp.xFlip ? GL_FLIP_H : GL_FLIP_NONE;
            glSprite(wtc.X().GetInt(), wtc.Y().GetInt(), flip, &sp.sprite[sp.spriteID]);
        }

        // Build-mode captured overlay on Main screen
        m_buildMode.DrawMainOverlay();

        // Draw every sprite in Subscreen
        auto viewSub = registry.view<SubSprite, Transform>();
        for (auto e : viewSub)
        {
            auto& ss = viewSub.get<SubSprite>(e);
            auto& tr = viewSub.get<Transform>(e);

            int sx = tr.pos.X().GetInt();
            int sy = tr.pos.Y().GetInt();

            bool hide = ss.hide;
            int priority = 2;

            if (registry.any_of<InvIcon, InvArrow, InvDragIcon, InvOverlay>(e)) {
                if (!m_buildMode.ShouldInventoryUiVisible()) {
                    hide = true;
                }
            }

            oamSet(&oamSub,
                ss.oamId,
                sx, sy,
                priority, 0,
                ss.size,
                SpriteColorFormat_256Color,
                ss.gfx,
                -1,
                false,
                hide,
                ss.xFlip,
                false,
                false
            );
        }

        m_inventoryUI.Draw();

        // Build mode capture on Sub screen
        m_buildMode.DrawSubOverlay();

        // Pause UI
        if (!registry.ctx().get<PauseControl>().pause) {
            PauseUI::HidePauseObj();
            return;
        }

        if (m_buildMode.IsBankBCapture()) {
            PauseUI::HidePauseObj();
            PauseUI::DrawPauseSpr();
            m_buildMode.SkipCapture();
        }
        else {
            PauseUI::DrawPauseObj();
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
