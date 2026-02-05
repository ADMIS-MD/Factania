#include "InventoryDebugSystem.h"

#include "inventory.h"
#include <nds.h>

void UpdateInventoryDebug(entt::registry& registry)
{
    const uint16 keys = keysDown();

    auto view = registry.view<Inventory>();
    view.each([&](entt::entity e, Inventory& inv)
    {
        if (keys & KEY_A)
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

        if (keys & KEY_B)
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
            {
                consoleClear();
                printf("No Iron to remove\n");
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
        }

        if (keys & KEY_X)
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

        if (keys & KEY_Y)
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

        if (keys & KEY_L)
        {
            consoleClear();
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
