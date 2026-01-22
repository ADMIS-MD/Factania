#pragma once

#include <nds/ndstypes.h>
#include <stdlib.h>
#include <entt.hpp>
#include <nds/arm9/videoGL.h>

#include "Transform.h"

#define CHUNK_SIZE 8

#define CHUNK_WIDTH CHUNK_SIZE
#define CHUNK_HEIGHT CHUNK_SIZE

/* Ores are rendered as entities over the base tile-sheet
 *
 */

struct ChunkSprite {
    u16 tile_pack; // (12 bytes tile_id, 4 bytes layer);
    rgb color;
};

// Chunks in their most beefy state
class Chunk
{
public:
    void Draw();

    ChunkSprite cached_sprites[64];
    entt::entity top_entity_ids[64]; // The topmost object's entity id, if it has an entity on it
    entt::entity surrounding_chunks[8];
};

struct FactoryLayer {
    u8 layer;
    entt::entity above {};
    entt::entity below {};
};

class ChunkLookup {
    std::unordered_map<u32, entt::entity> chunks;
};

u32 reinterpret_array_as_u32(u16 arr[2]);

struct OreType {
	u16 weight;
	u16 item_id;
	u32 base_amount;
	u32 amount_variance;
    rgb color;
    u8 splotch_gen_chance;
};

// Sohuld be moved
struct OreContext {
    u16 ore_count;
    u16 ore_chance;
    OreType* ores;
    // Binary search this from 0 to u16-max
	u16* generated_probabilities;
};

// T = type, ST = size type
template<typename T, typename ST>
T bsearch_T(T value, T const* list, ST size) {
    if(size == 1)
        return *list;
    if (size == 0)
        return -1;

	ST half = size/2;
	T const* mid = list + size/2;
	T vmid = *mid;



	if(value > vmid) {
		return half + bsearch_T<T, ST>(value, mid, size - half);
	} else if (value < vmid) {
		return bsearch_T<T, ST>(value, list, half);
	} else {
		return half;
	}
}

entt::entity make_chunk(u32 local_seed, GridTransform* chunk_position, OreContext& context, entt::registry& registry);
FactoryLayer chunk_push_entity(Chunk &storage, u8 position, ChunkSprite sprite, entt::entity e, entt::registry& registry);
void chunk_pop_entity(Chunk &storage, u8 position, FactoryLayer& layer, entt::registry &registry);
void chunk_update_entity(Chunk &storage, u8 position, entt::registry &registry);