#include "inventory_debug_menu.h"

#include <string>

#include "debug_menu.h"
#include "InventoryUI.h"

static std::string GetInventoryTempToggleLabel()
{
    return std::string("Toggle temp items: ") + (InventoryUI::IsTempItemsOn() ? "ON" : "OFF");
}

static void ToggleInventoryTempItems(bool& selected)
{
    InventoryUI::ToggleTempItems();
    selected = false;
}

DebugNode* GetInventoryTempToggleNode()
{
    static DebugNode* node = new FunctionResultNode {
        GetInventoryTempToggleLabel,
        ToggleInventoryTempItems
    };

    return node;
}