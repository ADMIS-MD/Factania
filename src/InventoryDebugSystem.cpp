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
            if (inv.RemoveItem(ItemType::Iron, 1))
            {
                consoleClear();
                printf("Removed 1 Iron\n");
                printf("Inventory:\n");
                for (int i = 0; i < (int)ItemType::Count; ++i)
                {
                    int q = inv.quantities[i];
                    if (q > 0)
                    {
                        printf("  %s x%d\n", ItemName((ItemType)i), q);
                    }
                }
            }
            else
                printf("No Iron to remove\n");
        }
        else if (down & KEY_A)
        {
            inv.AddItem(ItemType::Iron, 1);
            consoleClear();
            printf("Added 1 Iron\n");
            printf("Inventory:\n");
            for (int i = 0; i < (int)ItemType::Count; ++i)
            {
                int q = inv.quantities[i];
                if (q > 0)
                {
                    printf("  %s x%d\n", ItemName((ItemType)i), q);
                }
            }
        }

        // --- COPPER ---
        if ((down & KEY_B) && (held & KEY_R))
        {
            if (inv.RemoveItem(ItemType::Copper, 1))
            {
                consoleClear();
                printf("Removed 1 Copper\n");
                printf("Inventory:\n");
                for (int i = 0; i < (int)ItemType::Count; ++i)
                {
                    int q = inv.quantities[i];
                    if (q > 0)
                    {
                        printf("  %s x%d\n", ItemName((ItemType)i), q);
                    }
                }
            }
            else
                printf("No Copper to remove\n");
        }
        else if (down & KEY_B)
        {
            inv.AddItem(ItemType::Copper, 1);
            consoleClear();
            printf("Added 1 Copper\n");
            printf("Inventory:\n");
            for (int i = 0; i < (int)ItemType::Count; ++i)
            {
                int q = inv.quantities[i];
                if (q > 0)
                {
                    printf("  %s x%d\n", ItemName((ItemType)i), q);
                }
            }
        }

        // --- COAL ---
        if ((down & KEY_X) && (held & KEY_R))
        {
            if (inv.RemoveItem(ItemType::Coal, 1))
            {
                consoleClear();
                printf("Removed 1 Coal\n");
                printf("Inventory:\n");
                for (int i = 0; i < (int)ItemType::Count; ++i)
                {
                    int q = inv.quantities[i];
                    if (q > 0)
                    {
                        printf("  %s x%d\n", ItemName((ItemType)i), q);
                    }
                }
            }
            else
                printf("No Coal to remove\n");
        }
        else if (down & KEY_X)
        {
            inv.AddItem(ItemType::Coal, 1);
            consoleClear();
            printf("Added 1 Coal\n");
            printf("Inventory:\n");
            for (int i = 0; i < (int)ItemType::Count; ++i)
            {
                int q = inv.quantities[i];
                if (q > 0)
                {
                    printf("  %s x%d\n", ItemName((ItemType)i), q);
                }
            }
        }
    });
}
