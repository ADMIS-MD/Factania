// Sangbeom Kim
// 01/30/2026

#include "Console.h"

PrintConsole g_console;

void ConsoleInit() {
    consoleInit(&g_console, 0, BgType_Text4bpp, BgSize_T_256x256, 30, 0, false, true);
    consoleSelect(&g_console);
    consoleClear();

    printf("START: Exit to loader\n");
    printf("SELECT: Open Debug Menu\n");
    printf("\nPrinting from Engine\n");
}

void ToggleConsole(bool drawConsole) {
    if (drawConsole) {
        REG_DISPCNT_SUB &= ~(DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE);
        REG_DISPCNT_SUB |= (DISPLAY_BG0_ACTIVE);

        BG_PALETTE_SUB[255] = RGB15(31, 31, 31);
    }
    else {
        REG_DISPCNT_SUB &= ~(DISPLAY_BG0_ACTIVE);
        REG_DISPCNT_SUB |= (DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE);
    }
}