// Sangbeom Kim
// 01/30/2026

#include "Console.h"

PrintConsole g_console;           // Main hidden console
static PrintConsole s_consoleSub; // Sub display console

static bool s_consoleVisible = false;

static u32 s_prevState = 0;
static u16 s_prevPal0 = 0;
static u16 s_prevPal255 = 0;

static constexpr int kTileBytes = 2 * 1024;
static constexpr int kFontBytes = 16 * 1024;

void ConsoleInit()
{
    // Main
    vramSetBankF(VRAM_F_MAIN_BG_0x06000000);

    consoleInit(&g_console,
        3,
        BgType_Text4bpp,
        BgSize_T_256x256,
        7,
        0,
        true,
        true);

    consoleSelect(&g_console);
    consoleClear();

    // Keep main console hidden
    REG_DISPCNT &= ~DISPLAY_BG3_ACTIVE;

    // Sub
    consoleInit(&s_consoleSub,
        0,
        BgType_Text4bpp,
        BgSize_T_256x256,
        30,
        0,
        false,
        false);
}

bool ConsoleVisible() {
    return s_consoleVisible;
}

void ToggleConsole(bool drawConsole)
{
    if (drawConsole) {
        if (s_consoleVisible) {
            return;
        }

        s_consoleVisible = true;

        // Save subscreen state + palette
        s_prevState = REG_DISPCNT_SUB;
        s_prevPal0 = BG_PALETTE_SUB[0];
        s_prevPal255 = BG_PALETTE_SUB[255];

        // Show only sub BG0 (console)
        REG_DISPCNT_SUB &= ~(DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE);
        REG_DISPCNT_SUB |= (DISPLAY_BG0_ACTIVE);

        // Black background + White text
        BG_PALETTE_SUB[0] = RGB15(0, 0, 0);
        BG_PALETTE_SUB[255] = RGB15(31, 31, 31);

        // Copy font
        dmaCopyHalfWords(0, g_console.fontBgGfx, s_consoleSub.fontBgGfx, kFontBytes);

        // Keep main console
        consoleSelect(&g_console);

        // Copy console tiles
        dmaCopyHalfWords(0, g_console.fontBgMap, s_consoleSub.fontBgMap, kTileBytes);
    }
    else {
        if (!s_consoleVisible) {
            return;
        }

        s_consoleVisible = false;

        // Restore subscreen state + palette
        REG_DISPCNT_SUB = s_prevState;
        BG_PALETTE_SUB[0] = s_prevPal0;
        BG_PALETTE_SUB[255] = s_prevPal255;

        // Keep main console
        consoleSelect(&g_console);
    }
}

void ConsoleTick()
{
    if (!s_consoleVisible) {
        return;
    }

    // Copy console tiles
    dmaCopyHalfWords(0, g_console.fontBgMap, s_consoleSub.fontBgMap, kTileBytes);
}