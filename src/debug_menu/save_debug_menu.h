// Sangbeom Kim
// 01/15/2026

#pragma once

#include <vector>
#include <functional>
#include <string>
#include "debug_menu.h"

void register_save_debug_actions(
    std::function<std::string()> save_cb,
    std::function<std::string()> load_cb,
    std::function<std::string()> delete_cb
);

extern std::vector<DebugNode*> save_game_debug_page;
extern std::vector<DebugNode*> load_save_debug_page;
extern std::vector<DebugNode*> delete_save_debug_page;