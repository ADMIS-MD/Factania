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
            inv.AddItem(ItemID::Iron, 1);
            consoleClear();
            printf("Added 1 Iron\n");
        }

        if (keys & KEY_B)
        {
            if (inv.RemoveItem(ItemID::Iron, 1))
            {
                consoleClear();
                printf("Removed 1 Iron\n");
            }
            else
            {
                consoleClear();
                printf("No Iron to remove\n");
            }
        }

        if (keys & KEY_X)
        {
            inv.AddItem(ItemID::Copper, 1);
            consoleClear();
            printf("Added 1 Copper\n");
        }

        if (keys & KEY_Y)
        {
            inv.AddItem(ItemID::Coal, 1);
            consoleClear();
            printf("Added 1 Coal\n");
        }

        if (keys & KEY_L)
        {
            consoleClear();
            printf("Inventory:\n");
            for (int i = 0; i < (int)ItemID::Count; ++i)
            {
                int q = inv.quantities[i];
                if (q > 0)
                {
                    consoleClear();
                    printf("  %s x%d\n", ItemName((ItemID)i), q);
                }
            }
        }
    });
}
