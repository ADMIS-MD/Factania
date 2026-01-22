#include "chunk.hpp"
#include "entt.hpp"

void Chunk::Draw(Camera cam)
{
    glColor(RGB15(31, 31, 31));
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

    // This code could be made more intelligent by only drawing the
    // tiles that are actually shown on the screen. That would reduce
    // the number of polygons that are sent to the GPU and improve
    // performance.
    for (int j = 0; j < CHUNK_HEIGHT; j++)
    {
        for (int i = 0; i < CHUNK_WIDTH; i++)
        {
            int tile_id = cached_sprites[j * CHUNK_WIDTH + i].tile_pack;

            int x = cam.WorldToCamera().X().GetInt() + i * TILE_SIZE;
            int y = cam.WorldToCamera().Y().GetInt() + j * TILE_SIZE;

            glSprite(x, y, GL_FLIP_NONE, &core::g_tileset[tile_id]);
        }
    }
}

void orecontext_generate_probabilities(OreType const* ores, u8 count, u16* out) {
    u32 sum = 0;
    for(int i = 0; i < count; ++i) {
        sum += ores[i].spawn_chance;
    }

    u32 multiplier = UINT32_MAX / sum;
    u32 acc = 0;
    for(int i = 0; i < count - 1; ++i) {
        acc += ores[i].spawn_chance;
        u32 outv = acc * multiplier;
        out[i] = static_cast<u16>(outv << 16);
    }
}

u32 next_seed_a(u32 seed) {
    srand(seed);
    return rand();
}

u32 scale_to(u32 max, u32 value) {
    u32 scalar = UINT32_MAX / max;
    return value / scalar;
}

#define ORE_TEXTURE_INDEX 1

ChunkPosition ChunkPosition::FromGridTransform(const GridTransform& transform)
{
    return {
        static_cast<int16>(transform.x >> 3),
        static_cast<int16>(transform.y >> 3)
    };
}

entt::entity ChunkLookup::GetChunk(GridTransform transform)
{
    auto pos = ChunkPosition::FromGridTransform(transform);
    return GetChunk(pos);
}

entt::entity ChunkLookup::GetChunk(ChunkPosition transform)
{
    auto it = m_chunks.find(transform);
    if (it == m_chunks.end()) // TODO: Make chunk
        return {};

    return it->second;
}

std::size_t hash_value(const ChunkPosition& obj)
{
    return static_cast<uint32_t>(obj.x << 16) + obj.y;
}

bool operator==(ChunkPosition const& a, ChunkPosition const& b)
{
    return a.x == b.x && a.y == b.y;
}

// entt::entity make_chunk(u32 local_seed, GridTransform* chunk_position, OreContext& context, entt::registry& registry) {
//     //  8 bits for ore spawn chance
//     u8 ore_chance = static_cast<u8>(local_seed);
//     u16 ore_type = static_cast<u16>(local_seed >> 8);
//     u8 ore_spawn_amount = static_cast<u8>(local_seed >> 24) / 4;
//
//     entt::entity chunk_entity = registry.create();
//     Chunk& chunk = registry.emplace<Chunk>(chunk_entity);
//
//     // Spawn ore?
//     // TODO: Wave function collapse ? Perlin noise
//     if(ore_chance > context.ore_chance) {
//         // TODO: don't think this works properly yet
//         u8 ore_idx = bsearch_T<u16, u8>(ore_type, context.generated_probabilities, context.ore_count - 1);
//         if (ore_idx != 255) {
//             u8 probability_v = context.generated_probabilities[ore_idx];
//             if (ore_type < probability_v)
//                 ore_idx = 0;
//         } else
//             ore_idx = 0;
//
//         OreType* ore = context.ores + ore_idx;
//
//         u32 ore_seed = local_seed;
//         printf("osa%d\n", ore_spawn_amount);
//         for (int i = 0; i < ore_spawn_amount; ++i) {
//             ore_seed = next_seed_a(ore_seed);
//             u8 pos = static_cast<u8>(ore_seed) / 4;
//             u32 ore_amount = scale_to(ore->amount_variance, ore_seed);
//             ore_amount = ore->base_amount + ore_amount - ore_amount / 2;
//
//             GridTransform world_pos = *chunk_position;
//             world_pos.x += pos % 8;
//             world_pos.y += pos / 8;
//
//             ChunkSprite s = {
//                 ORE_TEXTURE_INDEX, ore->color
//             };
//
//             entt::entity ore_entity = registry.create();
//             registry.emplace<GridTransform>(ore_entity, world_pos);
//             registry.emplace<ChunkSprite>(ore_entity, s);
//             registry.emplace<FactoryLayer>(ore_entity, chunk_push_entity(
//                 chunk, pos, s, ore_entity, registry
//             ));
//         }
//         printf("done\n");
//     }
//
//     return chunk_entity;
// }

FactoryLayer chunk_push_entity(Chunk &storage, u8 position, ChunkSprite sprite, entt::entity e, entt::registry& registry) {
    auto current_entity = storage.top_entity_ids[position];
    u8 next_layer = 0;
    if (static_cast<u32>(current_entity) != 0) {
        auto& prev_layer = registry.get<FactoryLayer>(current_entity);
        prev_layer.above = e;
        next_layer = prev_layer.layer + 1;
    }

    storage.cached_sprites[position] = sprite;
    storage.top_entity_ids[position] = e;
    return {
        {}, current_entity
    };
}

void chunk_pop_entity(Chunk &storage, u8 position, FactoryLayer& layer, entt::registry &registry) {
    auto underneath = layer.below;
    if (static_cast<u32>(underneath) != 0) {
        registry.get<FactoryLayer>(underneath).above = {};
        storage.cached_sprites[position] = registry.get<ChunkSprite>(underneath);
        storage.top_entity_ids[position] = underneath;
    } else {
        storage.cached_sprites[position] = ChunkSprite {0, RGB15(31,31,31)};
        storage.top_entity_ids[position] = {};
    }
}

void chunk_update_entity(Chunk &storage, u8 position, entt::registry &registry) {
    storage.cached_sprites[position] = registry.get<ChunkSprite>(storage.top_entity_ids[position]);
}
