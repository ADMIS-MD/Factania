// Sangbeom Kim
// 02/19/2026

#include <nds.h>
#include <string.h>
 
#include "BuildMode.h"
#include "chunk.hpp"
#include "Console.h"

#include "Build.h"
#include "BG.h"

#define SCREENW 256
#define SCREENH 192

// Capture size in ram
alignas(32) static u16 s_captureRam[SCREENW * SCREENH];
static constexpr u32 kCapBytes = sizeof(s_captureRam);

// VRAM Bank B Toggle
static constexpr VRAM_B_TYPE kVramBCapture = VRAM_B_LCD;
static constexpr VRAM_B_TYPE kVramBMainObj = VRAM_B_MAIN_SPRITE_0x06400000;

//------------------------------------------------------------------------------
// Capture Effects
//------------------------------------------------------------------------------

static inline u16 BrightenPixel(u16 px, fixed wlevel)
{
    int r = px & 31;
    int g = (px >> 5) & 31;
    int b = (px >> 10) & 31;

    int dr = (FINT(31 - r) * wlevel).GetInt();
    int dg = (FINT(31 - g) * wlevel).GetInt();
    int db = (FINT(31 - b) * wlevel).GetInt();

    r += dr; 
    g += dg;
    b += db; 

    if (r > 31) r = 31;
    if (g > 31) g = 31;
    if (b > 31) b = 31;
    
    return (u16)(RGB15(r, g, b) | BIT(15));
}

static void SetCaptureOutline(u16* img)
{
    // Border Thickness
    constexpr int border = 16;

    fixed wTable[border + 1];
    for (int i = 0; i <= border; ++i) {
        fixed t = FINT(border - i) / FINT(border);
        t = t * t;
        wTable[i] = t;
    }

    for (int x = 0; x < SCREENW; ++x) {
        const int dx = (x < (SCREENW - 1 - x)) ? x : (SCREENW - 1 - x);
        for (int y = 0; y < SCREENH; ++y) {
            const int dy = (y < (SCREENH - 1 - y)) ? y : (SCREENH - 1 - y);
            const int d = (dx < dy) ? dx : dy;

            if (d >= border) {
                img[y * SCREENW + x] |= BIT(15);
                continue;
            }

            img[y * SCREENW + x] = BrightenPixel(img[y * SCREENW + x], wTable[d]);
        }
    }
}

static inline u16 DarkenPixel(u16 px, int scale)
{
    int r = px & 31;
    int g = (px >> 5) & 31;
    int b = (px >> 10) & 31;

    r = (r * scale + 8) >> 4;
    g = (g * scale + 8) >> 4;
    b = (b * scale + 8) >> 4;

    return (u16)(RGB15(r, g, b) | BIT(15));
}

static void DarkenCapture(u16* img, int scale)
{
    const int count = SCREENW * SCREENH;
    for (int i = 0; i < count; ++i) {
        img[i] = DarkenPixel(img[i], scale);
    }
}

//------------------------------------------------------------------------------
// BuildMode Functions
//------------------------------------------------------------------------------

void BuildMode::Init()
{
    // Sub BG3 default (bmp8) background.
    m_subBg3Id = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
    bgSetPriority(m_subBg3Id, 3);
    m_subBg3Gfx = bgGetGfxPtr(m_subBg3Id);

    dmaCopy(BGBitmap, m_subBg3Gfx, BGBitmapLen);
    dmaCopy(BGPal, BG_PALETTE_SUB, BGPalLen);

    // Allocate Sprite memory
    vramSetBankB(kVramBMainObj);
    for (int i = 0; i < kOverlayCount; ++i) {
        m_mainOverlayGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_Bmp);
        memset(m_mainOverlayGfx[i], 0, 64 * 64 * 2);
    }
    vramSetBankB(kVramBCapture);

    for (int i = 0; i < kOverlayCount; ++i) {
        m_subOverlayGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_Bmp);
        memset(m_subOverlayGfx[i], 0, 64 * 64 * 2);
    }

    setBrightness(1, 0);

    m_slidePos = FINT(0);
    m_slideLerpFactor = FINT(30) / FINT(256);

    OverlayInit();
    ToggleOverlay(false);

    REG_DISPCAPCNT = 0;

    m_slideState = SlideState::Idle;
}

void BuildMode::Update(entt::registry& registry, const Camera& camera)
{
    HandleToggle();
    HandleTouchBuild(registry, camera);

    // Transition Requests
    if (m_request == Request::Enter) {
        if (m_slideState == SlideState::Idle) {
            m_flashActive = true;
            m_flashLevel = 16;
            setBrightness(1, m_flashLevel);

            StartScreenCapture();
            m_slideState = SlideState::WaitingCapture;
        }
        m_request = Request::None;
    }
    else if (m_request == Request::Exit) {
        if (m_slideState == SlideState::Finished && m_mirrorInitialized) {
            m_mirrorActive = false;
            m_mirrorInitialized = false;

            REG_DISPCAPCNT = 0;

            StartScreenCapture();
            m_slideState = SlideState::ExitWaitingCapture;
        }
        m_request = Request::None;
    }

    // Flash
    if (m_flashActive) {
        m_flashLevel -= m_flashDecay;
        if (m_flashLevel <= 0) {
            m_flashLevel = 0;
            m_flashActive = false;
        }
        setBrightness(1, m_flashLevel);
    }

    // Transition State Machine
    if (m_slideState == SlideState::WaitingCapture)
    {
        if ((REG_DISPCAPCNT & DCAP_ENABLE) == 0) {
            vramSetBankB(kVramBCapture);
            // Copy Capture to Ram
            DC_InvalidateRange(s_captureRam, kCapBytes);
            dmaCopyHalfWords(0, VRAM_B, s_captureRam, SCREENW * SCREENH * 2);
            DC_InvalidateRange(s_captureRam, kCapBytes);

            SetCaptureOutline(s_captureRam);
            DC_FlushRange(s_captureRam, kCapBytes);

            vramSetBankB(kVramBMainObj);
            CopyRamToMainOverlay();
            CopyRamToSubOverlay();

            m_slidePos = FINT(0);
            m_mainOverlayY = 0;
            m_subOverlayY = -SCREENH;

            m_exitDelay = 1;
            m_slideState = SlideState::Sliding;
        }
    }
    else if (m_slideState == SlideState::Sliding)
    {
        if (m_exitDelay > 0) {
            --m_exitDelay;
            if (m_exitDelay == 0) {
                ToggleOverlay(true);
            }
        }

        if (m_exitDelay == 0) {
            fixed target = FINT(SCREENH);
            fixed delta = target - m_slidePos;
            fixed step = delta * m_slideLerpFactor;

            // Max Speed Cap
            fixed maxStep = FINT(6);
            if (step > maxStep) step = maxStep;
            if (step < -maxStep) step = -maxStep;

            m_slidePos += step;

            if (fixed::Abs(target - m_slidePos) <= FINT(1)) {
                m_slidePos = target;
            }

            int y = m_slidePos.GetInt();
            m_mainOverlayY = y;
            m_subOverlayY = y - SCREENH;

            if (m_slidePos == target) {
                vramSetBankB(kVramBCapture);
                m_slideState = SlideState::Finished;
                if (ConsoleVisible()) {
                    ToggleConsole(false);
                }
            }
        }
    }
    else if (m_slideState == SlideState::ExitWaitingCapture)
    {
        if ((REG_DISPCAPCNT & DCAP_ENABLE) == 0) {
            vramSetBankB(kVramBCapture);
            // Copy Capture to Ram
            DC_InvalidateRange(s_captureRam, kCapBytes);
            dmaCopyHalfWords(0, VRAM_B, s_captureRam, SCREENW* SCREENH * 2);
            DC_InvalidateRange(s_captureRam, kCapBytes);

            DarkenCapture(s_captureRam, 10);
            DC_FlushRange(s_captureRam, kCapBytes);

            vramSetBankB(kVramBMainObj);
            CopyRamToMainOverlay();
            CopyRamToSubOverlay();

            m_slidePos = FINT(SCREENH);
            m_mainOverlayY = SCREENH;
            m_subOverlayY = 0;

            m_startDelay = 2;
            m_slideState = SlideState::ExitSliding;
        }
    }
    else if (m_slideState == SlideState::ExitSliding)
    {
        if (m_startDelay > 0) {
            --m_startDelay;
            if (m_startDelay == 0) {
                if (ConsoleVisible()) {
                    ToggleConsole(false);
                }
                SubBg3ToBmp8();
            }
        }

        if (m_startDelay == 0) {
            fixed target = FINT(0);
            fixed delta = target - m_slidePos;
            fixed step = delta * m_slideLerpFactor;

            // Max Speed Cap
            fixed maxStep = FINT(6);
            if (step > maxStep) step = maxStep;
            if (step < -maxStep) step = -maxStep;

            m_slidePos += step;

            if (fixed::Abs(target - m_slidePos) <= FINT(1)) {
                m_slidePos = target;
            }

            int y = m_slidePos.GetInt();
            m_mainOverlayY = y;
            m_subOverlayY = y - SCREENH;

            if (m_slidePos == target) {
                HideMainOverlaySprites();
                HideSubOverlaySprites();

                ToggleOverlay(false);

                vramSetBankB(kVramBCapture);
                m_slideState = SlideState::Idle;
            }
        }
    }

    // Mirror only when transition done.
    if (m_slideState != SlideState::Finished) {
        m_mirrorActive = false;
        m_mirrorInitialized = false;
        return;
    }

    if (!m_mirrorActive) {
        m_mirrorActive = true;
        m_mirrorInitialized = false;
        SubBg3ToBmp16();
        StartScreenCapture();
        return;
    }

    if ((REG_DISPCNT_SUB & DISPLAY_BG0_ACTIVE) && !(REG_DISPCNT_SUB & DISPLAY_BG3_ACTIVE)) {
        return;
    }

    if (REG_DISPCAPCNT & DCAP_ENABLE) {
        return;
    }

    vramSetBankB(kVramBCapture);

    if (m_skipCapture) {
        m_skipCapture = false;
        return;
    }

    if (m_subBg3Gfx) {
        dmaCopyHalfWords(0, VRAM_B, m_subBg3Gfx, SCREENW * SCREENH * 2);
    }

    m_mirrorInitialized = true;
    StartScreenCapture();
}

void BuildMode::DrawMainOverlay()
{
    if (m_slideState != SlideState::Sliding &&
        m_slideState != SlideState::ExitSliding) {
        HideMainOverlaySprites();
        return;
    }

    constexpr int kPriority = 1;
    constexpr int kAlpha = 15;
    
    for (int bx = 0; bx < kOverlayCols; ++bx) {
        for (int by = 0; by < kOverlayRows; ++by) {
            const int idx = by * kOverlayCols + bx;
            const int x = bx * 64;
            const int y = by * 64 + m_mainOverlayY;

            bool hide = false;
            int yOam = y;

            if (y < -64) { 
                hide = true; 
                yOam = 0; 
            }
            else if (y < 0) { 
                yOam = 256 + y; 
            }
            else if (y >= SCREENH) {
                hide = true;
            }

            oamSet(&oamMain,
                kMainOverlayOamBase + idx,
                x, yOam,
                kPriority,
                kAlpha,
                SpriteSize_64x64,
                SpriteColorFormat_Bmp,
                m_mainOverlayGfx[idx],
                -1, false,
                hide,
                false, false, false);
        }
    }
}

void BuildMode::DrawSubOverlay()
{
    if (m_mirrorActive && m_mirrorInitialized) {
        HideSubOverlaySprites();
        return;
    }

    if (m_slideState == SlideState::Idle ||
        m_slideState == SlideState::WaitingCapture ||
        m_slideState == SlideState::ExitWaitingCapture) {
        HideSubOverlaySprites();
        return;
    }

    constexpr int kPriority = 1;
    constexpr int kAlpha = 15;

    for (int bx = 0; bx < kOverlayCols; ++bx) {
        for (int by = 0; by < kOverlayRows; ++by) {
            const int idx = by * kOverlayCols + bx;
            const int x = bx * 64;
            const int y = by * 64 + m_subOverlayY;

            bool hide = false;
            int yOam = y;

            if (y < -64) { 
                hide = true; 
                yOam = 0; 
            }
            else if (y < 0) {
                yOam = 256 + y;
            }
            else if (y >= SCREENH) {
                hide = true;
            }

            oamSet(&oamSub,
                kSubOverlayOamBase + idx,
                x, yOam,
                kPriority,
                kAlpha,
                SpriteSize_64x64,
                SpriteColorFormat_Bmp,
                m_subOverlayGfx[idx],
                -1, false,
                hide,
                false, false, false);
        }
    }
}

bool BuildMode::IsTransitioning() const
{
    return (m_slideState == SlideState::WaitingCapture) ||
        (m_slideState == SlideState::Sliding) ||
        (m_slideState == SlideState::ExitWaitingCapture) ||
        (m_slideState == SlideState::ExitSliding);
}

bool BuildMode::IsBankBCapture() const
{
    return !(m_slideState == SlideState::Sliding ||
        m_slideState == SlideState::ExitSliding);
}

int BuildMode::GetFlashLevel() const
{
    return m_flashLevel;
}

void BuildMode::SkipCapture()
{
    m_skipCapture = true;
}

void BuildMode::HandleToggle()
{
    if ((keysDown() & KEY_X) == 0) {
        return;
    }

    if (IsTransitioning()) {
        return;
    }

    if (m_slideState == SlideState::Idle) {
        m_request = Request::Enter;
    }
    else if (m_slideState == SlideState::Finished) {
        if (m_mirrorInitialized) {
            m_request = Request::Exit;
        }
    }
}

void BuildMode::HandleTouchBuild(entt::registry& registry, const Camera& camera)
{
    if (m_slideState != SlideState::Finished || !m_mirrorInitialized) {
        m_dragActive = false;
        m_dragMode = DragMode::None;
        m_lastGridX = 0x7fffffff;
        m_lastGridY = 0x7fffffff;
        return;
    }

    const uint16_t held = keysHeld();
    if ((held & KEY_TOUCH) == 0) {
        m_dragActive = false;
        m_dragMode = DragMode::None;
        m_lastGridX = 0x7fffffff;
        m_lastGridY = 0x7fffffff;
        return;
    }

    touchPosition tp;
    touchRead(&tp);

    Vec2 screenPx{ FINT(tp.px), FINT(tp.py) };
    const Vec2 world = camera.GetPos() + camera.ScreenSpaceFactor() * screenPx;
    const int32_t gx = fixed::Floor(world.X());
    const int32_t gy = fixed::Floor(world.Y());

    if (gx == m_lastGridX && gy == m_lastGridY) {
        return;
    }

    m_lastGridX = gx;
    m_lastGridY = gy;

    GridTransform grid;
    grid.x = gx;
    grid.y = gy;
    grid.layer = 0;

    // Ensure chunk exists.
    const ChunkPosition chp = ChunkPosition::FromGridTransform(grid);
    auto& lookup = registry.ctx().get<ChunkLookup>();
    entt::entity chunkEnt = lookup.GetChunk(chp);
    if (!registry.valid(chunkEnt)) {
        chunkEnt = Chunk::MakeChunk(lookup, registry, chp);
    }

    Chunk& chunk = registry.get<Chunk>(chunkEnt);
    const u8 cell = grid.CropTo8x8Grid();
    const entt::entity existing = chunk.top_entity_ids[cell];

    // First touch decides mode.
    if (!m_dragActive) {
        m_dragActive = true;
        m_dragMode = (existing == entt::null) ? DragMode::Place : DragMode::Erase;
    }

    if (m_dragMode == DragMode::Place) {
        if (existing != entt::null) {
            return;
        }

        const entt::entity e = registry.create();
        registry.emplace<ChunkSprite>(e, ChunkSprite{ 1, RGB15(15, 15, 0) });
        registry.emplace<GridTransform>(e, grid);
        return;
    }

    if (m_dragMode == DragMode::Erase) {
        if (existing == entt::null) {
            return;
        }

        registry.destroy(existing);
        return;
    }
}

void BuildMode::StartScreenCapture()
{
    vramSetBankB(kVramBCapture);

    REG_DISPCAPCNT =
        DCAP_ENABLE |
        DCAP_MODE(0) |
        DCAP_SRC_ADDR(0) |
        DCAP_SRC(1) |
        DCAP_SIZE(3) |
        DCAP_OFFSET(0) |
        DCAP_BANK(1) |
        DCAP_A(16) | DCAP_B(0);
}

void BuildMode::CopyRamToMainOverlay()
{
    const u16* src = s_captureRam;
    
    for (int bx = 0; bx < kOverlayCols; ++bx) {
        for (int by = 0; by < kOverlayRows; ++by) {
            const int idx = by * kOverlayCols + bx;
            u16* dst = m_mainOverlayGfx[idx];

            const int srcBaseY = by * 64;
            const int srcBaseX = bx * 64;

            for (int row = 0; row < 64; ++row) {
                const u16* srcRow = src + (srcBaseY + row) * SCREENW + srcBaseX;
                u16* dstRow = dst + row * 64;

                dmaCopyHalfWords(0, srcRow, dstRow, 64 * 2);
            }
        }
    }
}

void BuildMode::CopyRamToSubOverlay()
{
    const u16* src = s_captureRam;

    for (int bx = 0; bx < kOverlayCols; ++bx) {
        for (int by = 0; by < kOverlayRows; ++by) {
            const int idx = by * kOverlayCols + bx;
            u16* dst = m_subOverlayGfx[idx];

            const int srcBaseY = by * 64;
            const int srcBaseX = bx * 64;

            for (int row = 0; row < 64; ++row) {
                const u16* srcRow = src + (srcBaseY + row) * SCREENW + srcBaseX;
                u16* dstRow = dst + row * 64;

                dmaCopyHalfWords(0, srcRow, dstRow, 64 * 2);
            }
        }
    }
}

void BuildMode::HideSubOverlaySprites()
{
    for (int i = 0; i < kOverlayCount; ++i) {
        oamSet(&oamSub, kSubOverlayOamBase + i,
            0, 0, 0, 0,
            SpriteSize_64x64, SpriteColorFormat_Bmp,
            m_subOverlayGfx[i],
            -1, false,
            true, false, false, false);
    }
}

void BuildMode::HideMainOverlaySprites()
{
    for (int i = 0; i < kOverlayCount; ++i) {
        oamSet(&oamMain, kMainOverlayOamBase + i,
            0, 0, 0, 0,
            SpriteSize_64x64, SpriteColorFormat_Bmp,
            m_mainOverlayGfx[i],
            -1, false,
            true, false, false, false);
    }
}

void BuildMode::SubBg3ToBmp16()
{
    m_subBg3Id = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 4, 0);
    bgSetPriority(m_subBg3Id, 3);
    m_subBg3Gfx = bgGetGfxPtr(m_subBg3Id);

    dmaFillHalfWords(0, m_subBg3Gfx, 256 * 256 * 2);
}

void BuildMode::SubBg3ToBmp8()
{
    m_subBg3Id = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
    bgSetPriority(m_subBg3Id, 3);
    m_subBg3Gfx = bgGetGfxPtr(m_subBg3Id);

    dmaCopy(BGBitmap, m_subBg3Gfx, BGBitmapLen);
    dmaCopy(BGPal, BG_PALETTE_SUB, BGPalLen);
}

//------------------------------------------------------------------------------
// Main Screen Overlay
//------------------------------------------------------------------------------

void BuildMode::OverlayInit()
{
    // Bank G used for overlay
    vramSetBankG(VRAM_G_MAIN_BG_0x06004000);

    // Overlay image
    m_overlayId = bgInit(2, BgType_Text4bpp, BgSize_T_256x256, 15, 1);

    bgSetPriority(m_overlayId, 2);

    // Copy overlay image
    dmaCopy(BuildTiles, bgGetGfxPtr(m_overlayId), BuildTilesLen);
    dmaCopy(BuildMap, bgGetMapPtr(m_overlayId), BuildMapLen);
    dmaCopy(BuildPal, &BG_PALETTE[1 * 16], 16 * sizeof(u16));

    // palbank 1
    u16* m = (u16*)bgGetMapPtr(m_overlayId);
    for (int i = 0; i < 32 * 32; ++i) {
        m[i] = (m[i] & 0x0FFF) | (1 << 12);
    }

    bgHide(m_overlayId);

    // Overlay darkening layer
    m_darkenId = bgInit(1, BgType_Text4bpp, BgSize_T_256x256, 14, 1);

    bgSetPriority(m_darkenId, 1);

    const int usedTiles = BuildTilesLen / 32;
    const int blackTile = usedTiles;

    u8* gfx8 = (u8*)bgGetGfxPtr(m_darkenId);
    dmaFillHalfWords(0x1111, gfx8 + blackTile * 32, 32);

    // palbank 2
    BG_PALETTE[2 * 16 + 0] = 0;
    BG_PALETTE[2 * 16 + 1] = RGB15(0, 0, 0);

    u16* map = (u16*)bgGetMapPtr(m_darkenId);
    for (int i = 0; i < 32 * 32; ++i) {
        map[i] = (blackTile) | (2 << 12);
    }

    bgHide(m_darkenId);

    // Make sure 3D BG0 is behind
    REG_BG0CNT = (REG_BG0CNT & ~3) | 3;
}

void BuildMode::ToggleOverlay(bool on)
{
    if (!on) {
        bgHide(m_overlayId);
        bgHide(m_darkenId);
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        return;
    }

    bgShow(m_overlayId);
    bgShow(m_darkenId);

    // Blend mode (Global)
    REG_BLDCNT = BLEND_ALPHA
        | BLEND_SRC_BG1
        | BLEND_DST_BG0
        | BLEND_DST_BG2
        | BLEND_DST_SPRITE;

    // Blend amount
    REG_BLDALPHA = BLDALPHA_EVA(16) | BLDALPHA_EVB(10);
}