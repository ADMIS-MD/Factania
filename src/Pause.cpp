// Sangbeom Kim
// 02/25/2026

#include <nds.h>
#include <gl2d.h>

#include "Pause.h"

#include "Pause_UI.h"

#define SCREENW 256
#define SCREENH 192

namespace PauseUI {
    // gl2d image
    static glImage s_pauseImg[1];
    static const uint16_t s_pauseTexcoords[] = { 0, 0, 256, 64 };

    // obj image
    static u16* s_pauseObjGfx[4] = { nullptr, nullptr, nullptr, nullptr };
    static bool s_pauseObjUploaded = false;

    constexpr int kPauseOamBase = 112;
    constexpr int kPauseY = (SCREENH - 48) / 2;

    void PauseImageInit() 
    {
        // gl2d
        glLoadSpriteSet(
            s_pauseImg,
            1,
            s_pauseTexcoords,
            GL_RGB256,
            256, 64,
            TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
            256,
            Pause_UIPal,
            Pause_UIBitmap
        );

        // obj
        for (int i = 0; i < 4; ++i) {
            s_pauseObjGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_Bmp);
            memset(s_pauseObjGfx[i], 0, 64 * 64 * 2);
        }

        s_pauseObjUploaded = false;
    }

    void DrawPauseSpr() {
        glSprite(0, kPauseY, GL_FLIP_NONE, &s_pauseImg[0]);
    }

    static inline u16 PalIdxToBmp16(u8 idx) 
    {
        if (idx == 0) return 0;
        return (Pause_UIPal[idx] | BIT(15));
    }

    void DrawPauseObj() {
        if (!s_pauseObjUploaded) {
            const u8* src = reinterpret_cast<const u8*>(Pause_UIBitmap);

            for (int y = 0; y < 48; ++y) {
                const int row = y * 256;
                for (int x = 0; x < 64; ++x) s_pauseObjGfx[0][y * 64 + x] = PalIdxToBmp16(src[row + (0 + x)]);
                for (int x = 0; x < 64; ++x) s_pauseObjGfx[1][y * 64 + x] = PalIdxToBmp16(src[row + (64 + x)]);
                for (int x = 0; x < 64; ++x) s_pauseObjGfx[2][y * 64 + x] = PalIdxToBmp16(src[row + (128 + x)]);
                for (int x = 0; x < 64; ++x) s_pauseObjGfx[3][y * 64 + x] = PalIdxToBmp16(src[row + (192 + x)]);
            }

            for (int i = 0; i < 4; ++i) {
                memset(&s_pauseObjGfx[i][48 * 64], 0, 16 * 64 * 2);
            }

            s_pauseObjUploaded = true;
        }

        constexpr int kPriority = 0;
        constexpr int kAlpha = 15;

        for (int i = 0; i < 4; ++i) {
            oamSet(&oamMain,
                kPauseOamBase + i,
                i * 64, kPauseY,
                kPriority,
                kAlpha,
                SpriteSize_64x64,
                SpriteColorFormat_Bmp,
                s_pauseObjGfx[i],
                -1, false,
                false,
                false, false, false);
        }
    }

    void HidePauseObj() 
    {
        s_pauseObjUploaded = false;

        for (int i = 0; i < 4; ++i) {
            oamSet(&oamMain,
                kPauseOamBase + i,
                0, 0,
                0, 0,
                SpriteSize_64x64,
                SpriteColorFormat_Bmp,
                s_pauseObjGfx[i],
                -1, false,
                true,
                false, false, false);
        }
    }
}