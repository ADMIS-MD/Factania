#pragma once

#include <nds/ndstypes.h>
#include <stdlib.h>
#include <entt.hpp>
#include <nds/arm9/videoGL.h>

#include "Transform.h"
#include "Camera.h"

#define CHUNK_SIZE 8

#define CHUNK_WIDTH CHUNK_SIZE
#define CHUNK_HEIGHT CHUNK_SIZE

/* Ores are rendered as entities over the base tile-sheet
 *
 */

class ChunkLookup;

struct ChunkSprite {
    u16 tile_pack;
    rgb color;
};

struct ChunkPosition
{
	int16 x;
	int16 y;

	// Fake hashing function that just packs into a u32
	friend std::size_t hash_value(const ChunkPosition& obj);
	friend bool operator==(ChunkPosition const& a, ChunkPosition const& b);
	static ChunkPosition FromGridTransform(const GridTransform& transform);
};

// Chunks in their most beefy state
class Chunk
{
	Chunk();

public:
	void Draw(Camera const& cam, ChunkPosition pos) const;
	static entt::entity MakeChunk(ChunkLookup& lookup, entt::registry& registry, ChunkPosition pos);
	void FillSurrounding(ChunkLookup& lookup, entt::registry& registry, ChunkPosition pos);

	ChunkSprite cached_sprites[64] = { 0 };
    entt::entity top_entity_ids[64] = { entt::null }; // The topmost object's entity id, if it has an entity on it
    entt::entity surrounding_chunks[8] = { entt::null }; // In clock order
};

struct FactoryLayer {
    u8 layer;
    entt::entity above {};
    entt::entity below {};
};


class ChunkLookup {
	// The u32 is composed of 2 positions
    std::unordered_map<ChunkPosition, entt::entity, HashForHelper<ChunkPosition>> m_chunks;

public:
	entt::entity GetChunk(ChunkPosition transform);
	entt::entity GetChunk(GridTransform transform);

	friend Chunk;
};

u32 reinterpret_array_as_u32(u16 arr[2]);

struct OreType {
	u16 spawn_chance;
	u16 item_id;
	u8 splotch_size;
	u8 splotch_size_variance;
    rgb color;
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
