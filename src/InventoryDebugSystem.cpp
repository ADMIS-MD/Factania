#include "InventoryDebugSystem.h"

#include "Inventory.h"
#include <nds.h>

void UpdateInventoryDebug(entt::registry& registry)
{
    const uint16 down = keysDown();
    const uint16 held = keysHeld();

    auto view = registry.view<Inventory>();
    view.each([&](entt::entity e, Inventory& inv)
    {
        // --- IRON ---
        if ((down & KEY_A) && (held & KEY_R))
        {
            if (inv.RemoveItem(ItemType::IronOre, 1))
            {
                consoleClear();
                printf("Removed 1 Iron Ore\n");
                inv.PrintInventory(inv);
            }
            else
                printf("No Iron Ore to remove\n");
        }
        else if (down & KEY_A)
        {
            inv.AddItem(ItemType::IronOre, 1);
            consoleClear();
            printf("Added 1 Iron Ore\n");
            inv.PrintInventory(inv);
        }

        // --- COPPER ---
        if ((down & KEY_B) && (held & KEY_R))
        {
            if (inv.RemoveItem(ItemType::CopperOre, 1))
            {
                consoleClear();
                printf("Removed 1 Copper Ore\n");
                inv.PrintInventory(inv);
            }
            else
                printf("No Copper Ore to remove\n");
        }
        else if (down & KEY_B)
        {
            inv.AddItem(ItemType::CopperOre, 1);
            consoleClear();
            printf("Added 1 Copper Ore\n");
            inv.PrintInventory(inv);
        }

        // --- COAL ---
        if ((down & KEY_X) && (held & KEY_R))
        {
            if (inv.RemoveItem(ItemType::Coal, 1))
            {
                consoleClear();
                printf("Removed 1 Coal\n");
                inv.PrintInventory(inv);
            }
            else
                printf("No Coal to remove\n");
        }
        else if (down & KEY_X)
        {
            inv.AddItem(ItemType::Coal, 1);
            consoleClear();
            printf("Added 1 Coal\n");
            inv.PrintInventory(inv);
        }

        // --- ADMIS... ---
        if ((down & KEY_Y) && (held & KEY_R))
        {
            if (inv.RemoveItem(ItemType::AdmisiumOre, 1))
            {
                consoleClear();
                printf("Removed 1 Admisium Ore\n");
                inv.PrintInventory(inv);
            }
            else
                printf("No Admisium Ore to remove\n");
        }
        else if (down & KEY_Y)
        {
            inv.AddItem(ItemType::AdmisiumOre, 1);
            consoleClear();
            printf("Added 1 Admisium Ore\n");
            inv.PrintInventory(inv);
        }

        // --- CLEAR ALL ---
        if ((held & KEY_R) && (down & KEY_SELECT))
        {
            consoleClear();
            printf("Clearing inventory...\n");
            printf("Note: if you are seeing this, \ndebug menu is disabled, \ncheck engine.cpp\n");
            inv.ClearInventory();
            inv.PrintInventory(inv);
        }
    });
}
