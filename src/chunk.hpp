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
    u16 tile_pack = 0;
    rgb color = RGB15(31, 31, 31);
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
	void Draw(Camera const& cam, ChunkPosition pos);
	static entt::entity MakeChunk(ChunkLookup& lookup, entt::registry& registry, ChunkPosition pos);
	void FillSurrounding(ChunkLookup& lookup, entt::registry& registry, ChunkPosition pos);
	// Searches top down for an entity within the layers provided
	entt::entity FindEntityByLayer(
		entt::registry const& r,
		GridTransform const& tr,
		u8 begin_layer = 0,
		u8 end_layer = 255,
		// If nullptr, ignored. If valid and search failed, will be set to true and entity returned is the closest match
		bool* get_last = nullptr
	) const;

	ChunkSprite cached_sprites[64];
    entt::entity top_entity_ids[64]; // The topmost object's entity id, if it has an entity on it
    entt::entity surrounding_chunks[8]; // In clock order
};

struct FactoryLayer {
    entt::entity above = entt::null;
    entt::entity below = entt::null;

	// Count the number of things that need this structure to auto-remove when required
	int components_dependent_on = 0;
};


class ChunkLookup {
	// The u32 is composed of 2 positions
    std::unordered_map<ChunkPosition, entt::entity, HashForHelper<ChunkPosition>> m_chunks;

public:
	entt::entity GetChunk(ChunkPosition transform) const;
	entt::entity GetChunk(GridTransform transform) const;
	Chunk& GetChunkObj(entt::registry& r, ChunkPosition transform);
	Chunk& GetChunkObj(entt::registry& r, GridTransform transform);
	Chunk const& GetChunkObj(entt::registry const& r, ChunkPosition transform) const;
	Chunk const& GetChunkObj(entt::registry const& r, GridTransform transform) const;
	// Search for an entity between the layers provided (inclusive)
	entt::entity ChunkSearchEntity
		(entt::registry const& r, GridTransform transform, u8 begin_layer = 0, u8 end_layer = 255);

	friend Chunk;
};

void SetupChunkCallbacks(entt::registry& r);

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
