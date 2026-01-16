

#include "chunk.hpp"
#include "test.h"

#include <entt.hpp>
#include <fstream>
#include <iostream>

inline entt::registry registry;
inline entt::entity chunk_entity;

extern "C" {
    int stupid_fn() {
        FactoryTransform ft {0, 0};
        OreType ot1 = {
            5, 0, 100, 100, RGB15(31, 31, 31),
        };
        OreContext ctx {};
        ctx.generated_probabilities = nullptr;
        ctx.ore_chance = 0;
        ctx.ore_count = 1;
        ctx.ores = &ot1;
        printf("s1\n");
        chunk_entity = make_chunk(38192816158327172834971298378912570259, &ft, ctx, registry);
        printf("success\n");

        Chunk const& chunk = registry.get<Chunk>(chunk_entity);
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (static_cast<uint32_t>(chunk.top_entity_ids[j + i * 8]) != 0) {
                    printf("E");
                } else
                    printf("_");
            }
            printf("\n");
        }

        return 0;
    }
}
