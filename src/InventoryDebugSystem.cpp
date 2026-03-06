#include "InventoryDebugSystem.h"

#include "Inventory.h"
#include "Conveyer.h"
#include "building.h"
#include <nds.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>

#define INVENTORY_TEST 0
#define BUILDING_TEST 1

static void PrintInventory(const Inventory& inv)
{
    bool any = false;
    for (int i = 0; i < (int)ItemType::Count; ++i)
    {
        int q = inv.quantities[i];
        if (q > 0)
        {
            if (!any) { printf("  "); any = true; }
            printf("%s x%d  ", ItemName((ItemType)i), q);
        }
    }
    if (!any) printf("  (empty)");
    printf("\n");
}

// Try to resolve a raw Building* to a readable label and print details (conveyer or factory)
static bool DescribeAndPrintBuilding(entt::registry& registry, Building* b)
{
    if (!b) { printf("[null]\n"); return true; }

    // Try Conveyers
    auto convView = registry.view<Conveyer>();
    for (auto ent : convView)
    {
        Conveyer& c = convView.get<Conveyer>(ent);
        if (&c == b)
        {
            printf("Conveyer id=%d \n", c.id, (void*)&c);
            printf("  inputInventory: "); PrintInventory(c.inputInventory);
            printf("  outputInventory: "); PrintInventory(c.outputInventory);
            printf("  input building: %d", c.inputs[0]);
            return true;
        }
    }

    // Try FactoryBuildings
    auto facView = registry.view<FactoryBuilding>();
    for (auto ent : facView)
    {
        FactoryBuilding& f = facView.get<FactoryBuilding>(ent);
        if (&f == b)
        {
            printf("Factory %d \n  selRecipe=%d status=%d\n", (void*)&f, f.selectedRecipe, (int)f.status);
            printf("  inputInventory: "); PrintInventory(f.inputInventory);
            printf("  outputInventory: "); PrintInventory(f.outputInventory);
            return true;
        }
    }

    // Unknown pointer (not a tracked registry component)
    printf("Unknown Building addr=%p\n", (void*)b);
    return false;
}

// Depth-first print following outputs. Handles branching by listing each output on its own subtree.
// visited prevents infinite loops on cycles.
static void PrintRecursive(entt::registry& registry, Building* node, std::unordered_set<Building*>& visited, int depth = 0)
{
    for (int i = 0; i < depth; ++i) printf("  ");
    if (!node) { printf("[null]\n"); return; }

    // Print the node summary
    // Try to match and print details
    DescribeAndPrintBuilding(registry, node);

    if (visited.find(node) != visited.end())
    {
        for (int i = 0; i <= depth; ++i) printf("  ");
        printf("(visited)\n");
        return;
    }

    visited.insert(node);

    // iterate outputs and recurse
    ItemBuilding* ib = dynamic_cast<ItemBuilding*>(node);
    if (!ib) return;

    for (size_t i = 0; i < ib->outputs.size(); ++i)
    {
        Building* out = ib->outputs[i];
        for (int j = 0; j < depth + 1; ++j) printf("  ");
        printf("-> ");
        PrintRecursive(registry, out, visited, depth + 1);
    }
}

void UpdateInventoryDebug(entt::registry& registry)
{
    const uint16 keys = keysDown();

    if(INVENTORY_TEST)
    { 
        auto view = registry.view<Inventory>();
        view.each([&](entt::entity e, Inventory& inv)
        {
            if (keys & KEY_A)
            {
                inv.AddItem(ItemType::IronPlate, 1);
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
                if (inv.RemoveItem(ItemType::IronPlate, 1))
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
                inv.AddItem(ItemType::CopperPlate, 1);
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

    if (BUILDING_TEST)
    {
        // Continuous frame-by-frame dump of network ordered by outputs.
        // This runs every Update call; it clears the console then prints the network.
        consoleClear();
        printf("=== Building -> outputs network dump ===\n\n");

        // Collect all nodes (conveyers and factories) that are in the registry
        std::vector<Building*> allNodes;
        std::unordered_set<Building*> outputsReferenced;

        auto convView = registry.view<Conveyer>();
        for (auto ent : convView)
        {
            Conveyer& c = convView.get<Conveyer>(ent);
            allNodes.push_back(&c);
            for (Building* o : c.outputs) if (o) outputsReferenced.insert(o);
        }

        auto facView = registry.view<FactoryBuilding>();
        for (auto ent : facView)
        {
            FactoryBuilding& f = facView.get<FactoryBuilding>(ent);
            allNodes.push_back(&f);
            for (Building* o : f.outputs) if (o) outputsReferenced.insert(o);
        }

        // Roots: nodes that are not referenced as someone's output
        std::vector<Building*> roots;
        for (Building* n : allNodes)
        {
            if (outputsReferenced.find(n) == outputsReferenced.end())
            {
                roots.push_back(n);
            }
        }

        // If no clear root found, fall back to printing all nodes as starting points.
        if (roots.empty()) roots = allNodes;

        std::unordered_set<Building*> visited;
        for (Building* r : roots)
        {
            PrintRecursive(registry, r, visited, 0);
            printf("\n");
        }

        printf("=== end dump ===\n");
    }
}
