#pragma once

#include <nds/ndstypes.h>
#include <stdlib.h>

/* Ores are rendered as entities over the base tile-sheet
 *
 */

// 512 byte chunk
struct Chunk {
    u16 pallete_indices[64];
    u16 top_entity_ids[64]; // The topmost object's entity id, if it has an entity
};

struct OreType {
	u16 weight;
	u16 item_id;
	u32 base_amount;
	u32 amount_variance;
    u8 splotch_gen_chance;
};

// Sohuld be moved
struct OreContext {
	OreType* ores;
    // Binary search this from 0 to u16-max
	u16* generated_probabilities;
	u8 ore_count;
	u8 ore_chance;
};

// T = type, ST = size type
template<typename T, typename ST>
T bsearch_T(T value, T const* list, ST size) {
	ST half = size/2;
	T const* mid = list + size/2;
	ST vmid = *mid;

	if(size == 1)
		return *list;

	if(value > vmid) {
		return half + bsearch_T<T, ST>(value, mid, size - half);
	} else if (value < vmid) {
		return bsearch_T<T, ST>(value, list, half);
	} else {
		return half;
	}
}

void orecontext_generate_probabilities(OreType const* ores, u8 count, u16* out) {
	u32 sum = 0;
	for(int i = 0; i < count; ++i) {
		sum += ores[i].weight;
	}

	u32 multiplier = UINT32_MAX / sum;
	u32 acc = 0;
	for(int i = 0; i < count - 1; ++i) {
		acc += ores[i].weight;
		u32 outv = acc * multiplier;
		out[i] = static_cast<u16>(outv << 16);
	}
}

u32 next_seed_a(u32 seed) {
    srand(seed);
    rand();
}

u32 scale_to(u32 max, u32 value) {
    u32 scalar = UINT32_MAX / max;
    return value / scalar;
}

void make_chunk(u32 local_seed, OreContext* context) {
	//  8 bits for ore spawn chance
	u8 ore_chance = static_cast<u8>(local_seed);
	u16 ore_type = static_cast<u16>(local_seed << 8);
    u8 ore_spawn_amount = static_cast<u8>(local_seed << 24);

	// Spawn ore?
    // TODO: Wave function collapse ? Perlin noise
	if(ore_chance > context->ore_chance) {
		u8 ore_idx = bsearch_T<u16, u8>(ore_type, context->generated_probabilities, context->ore_count)++;
		u8 probability_v = context->generated_probabilities[ore_idx];
	    if (ore_type < probability_v)
	        ore_idx = 0;

        OreType* ore = context->ores[ore_idx];

	    u32 ore_seed = local_seed;
	    for (int i = 0; i < ore_spawn_amount; ++i) {
            ore_seed = next_seed_a(ore_seed);
	        u8 pos = static_cast<u8>(ore_seed);
            u32 ore_amount = scale_to(ore->amount_variance, ore_seed);
	        ore_amount = ore->base_amount + ore_amount - ore_amount / 2;
	        // TODO: set ore at position `pos` to `ore_amount`
	    }
	}
}