#include "chunk.hpp"

#include <Engine.h>
#include <Sprite.h>
#include <nds/arm9/input.h>

#include "RenderSystem.h"

Chunk::Chunk()
{
    std::fill(std::begin(top_entity_ids), std::end(top_entity_ids), entt::null);
    std::fill(std::begin(surrounding_chunks), std::end(surrounding_chunks), entt::null);
}

void Chunk::Draw(Camera const& cam, ChunkPosition pos)
{
    glColor(RGB15(31, 31, 31));
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

    // This code could be made more intelligent by only drawing the
    // tiles that are actually shown on the screen. That would reduce
    // the number of polygons that are sent to the GPU and improve
    // performance.
    fixed scale = cam.GetScale();
    fixed fx = FINT(pos.x * 8), fy = FINT(pos.y * 8);
    for (int j = 0; j < CHUNK_HEIGHT; j++)
    {
        for (int i = 0; i < CHUNK_WIDTH; i++)
        {
            int tile_id = cached_sprites[j * CHUNK_WIDTH + i].tile_pack;

            Vec2 world_pos = {FINT(i) + fx, FINT(j) + fy};
            Vec2 cam_pos = cam.WorldToCamera(world_pos);

            // Early outs to skip renderings
            // TODO: Fix
            // if (cam_pos.X() <= -scale)
            //     continue;
            // if (cam_pos.X() > FINT( CHUNK_WIDTH ))
            //     break;
            //
            // if (cam_pos.Y() <= -scale)
            //     break;
            // if (cam_pos.Y() > FINT(CHUNK_HEIGHT))
            //     return;

            glSprite(cam_pos.X().GetInt(), cam_pos.Y().GetInt(), GL_FLIP_NONE, &core::g_tileset[tile_id]);
        }
    }
}

entt::entity Chunk::MakeChunk(ChunkLookup& lookup, entt::registry& registry, ChunkPosition pos)
{
    auto entity = lookup.GetChunk(pos);
    assert(!registry.valid(entity));

    entity = registry.create();
    Chunk c{};
    c.surrounding_chunks[0] = lookup.GetChunk(ChunkPosition{
        static_cast<int16>(pos.x + 0), static_cast<int16>(pos.y + 1)
    });
    c.surrounding_chunks[1] = lookup.GetChunk(ChunkPosition{
        static_cast<int16>(pos.x + 1), static_cast<int16>(pos.y + 1)
    });
    c.surrounding_chunks[2] = lookup.GetChunk(ChunkPosition{
        static_cast<int16>(pos.x + 1), static_cast<int16>(pos.y + 0)
    });
    c.surrounding_chunks[3] = lookup.GetChunk(ChunkPosition{
        static_cast<int16>(pos.x + 1), static_cast<int16>(pos.y - 1)
    });
    c.surrounding_chunks[4] = lookup.GetChunk(ChunkPosition{
        static_cast<int16>(pos.x + 0), static_cast<int16>(pos.y - 1)
    });
    c.surrounding_chunks[5] = lookup.GetChunk(ChunkPosition{
        static_cast<int16>(pos.x - 1), static_cast<int16>(pos.y - 1)
    });
    c.surrounding_chunks[6] = lookup.GetChunk(ChunkPosition{
        static_cast<int16>(pos.x - 1), static_cast<int16>(pos.y + 0)
    });
    c.surrounding_chunks[7] = lookup.GetChunk(ChunkPosition{
        static_cast<int16>(pos.x - 1), static_cast<int16>(pos.y + 1)
    });

    for (int i = 0; i < 8; ++i)
    {
        if (registry.valid(c.surrounding_chunks[i]))
        {
            // & 0b0111 is mod 4 (x % 4)
            registry.get<Chunk>(c.surrounding_chunks[i]).surrounding_chunks[(i + 4) & 0b0111] = entity;
        }
    }

    registry.emplace<Chunk>(entity, std::forward<Chunk>(c));
    lookup.m_chunks.insert({pos, entity});

    return entity;
}

void Chunk::FillSurrounding(ChunkLookup& lookup, entt::registry& registry, ChunkPosition pos)
{
    int xc = pos.x, yc = pos.y;
    const ChunkPosition transforms[8] = {
        {static_cast<int16>(xc + 0), static_cast<int16>(yc + 1)},
        {static_cast<int16>(xc + 1), static_cast<int16>(yc + 1)},
        {static_cast<int16>(xc + 1), static_cast<int16>(yc + 0)},
        {static_cast<int16>(xc + 1), static_cast<int16>(yc - 1)},
        {static_cast<int16>(xc + 0), static_cast<int16>(yc - 1)},
        {static_cast<int16>(xc - 1), static_cast<int16>(yc - 1)},
        {static_cast<int16>(xc - 1), static_cast<int16>(yc + 0)},
        {static_cast<int16>(xc - 1), static_cast<int16>(yc + 1)},
    };

    for (int i = 0; i < 8; ++i)
    {
        if (!registry.valid(surrounding_chunks[i]))
        {
            MakeChunk(lookup, registry, transforms[i]);
        }
    }
}

entt::entity Chunk::FindEntityByLayer
    (entt::registry const& r, GridTransform const& tr, u8 begin_layer, u8 end_layer, bool* get_last) const
{
    u8 ch_pos = tr.CropTo8x8Grid();
    entt::entity it = top_entity_ids[ch_pos];
    entt::entity last = entt::null;
    while (r.valid(it))
    {
        GridTransform const& grid = r.get<GridTransform>(it);
        FactoryLayer const& fl = r.get<FactoryLayer>(it);

        if (grid.layer >= begin_layer && grid.layer <= end_layer)
        {
            return it;
        }

        last = it;
        it = fl.below;
    }

    if (get_last)
    {
        *get_last = last != entt::null;
        return last;
    }
    return entt::null;
}

ChunkPosition ChunkPosition::FromGridTransform(const GridTransform& transform)
{
    return {
        static_cast<int16>(transform.x >> 3),
        static_cast<int16>(transform.y >> 3)
    };
}
;

entt::entity ChunkLookup::GetChunk(GridTransform transform) const
{
    auto pos = ChunkPosition::FromGridTransform(transform);
    return GetChunk(pos);
}

Chunk& ChunkLookup::GetChunkObj(entt::registry& r, ChunkPosition transform)
{
    auto ent = GetChunk(transform);
    return r.get<Chunk>(ent);
}

Chunk& ChunkLookup::GetChunkObj(entt::registry& r, GridTransform transform)
{
    auto ent = GetChunk(transform);
    return r.get<Chunk>(ent);
}

Chunk const& ChunkLookup::GetChunkObj(entt::registry const& r, ChunkPosition transform) const
{
    auto ent = GetChunk(transform);
    return r.get<Chunk>(ent);
}

Chunk const& ChunkLookup::GetChunkObj(entt::registry const& r, GridTransform transform) const
{
    auto ent = GetChunk(transform);
    return r.get<Chunk>(ent);
}

entt::entity ChunkLookup::ChunkSearchEntity(entt::registry const& r, GridTransform transform, u8 top_search_layer,
                                            u8 bottom_search_layer)
{
    return GetChunkObj(r, transform).FindEntityByLayer(r, transform, top_search_layer, bottom_search_layer);
}

entt::entity ChunkLookup::GetChunk(ChunkPosition transform) const
{
    auto it = m_chunks.find(transform);
    if (it == m_chunks.end())
        return entt::null;

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
//
// static void ChunkUpdateEntityHelper(Chunk& storage, u8 position, entt::registry& registry)
// {
//     entt::entity search = storage.top_entity_ids[position];
//     while (registry.valid(search))
//     {
//         FactoryLayer& layer = registry.get<FactoryLayer>(search);
//         if (ChunkSprite* sp = registry.try_get<ChunkSprite>(search); sp)
//         {
//             storage.cached_sprites[position] = *sp;
//             return;
//         }
//
//         search = layer.below;
//     }
//
//     storage.cached_sprites[position] = {};
// }
//
// static void ChunkRemoveLayerHelper1(entt::registry& r, entt::entity entity)
// {
//     GridTransform& grid = r.get<GridTransform>(entity);
//     FactoryLayer& layer = r.get<FactoryLayer>(entity);
//
//     if (r.valid(layer.above))
//     {
//         r.get<FactoryLayer>(layer.above).below = layer.below;
//     }
//     else
//     {
//         // If was topmost, get rid of self
//         entt::entity chunk_e = chunk_lookup.GetChunk(layer.last);
//         Chunk& chunk = r.get<Chunk>(chunk_e);
//         u8 chunk_grid_pos = layer.last.CropTo8x8Grid();
//
//         chunk.top_entity_ids[chunk_grid_pos] = layer.below;
//         ChunkUpdateEntityHelper(chunk, chunk_grid_pos, r);
//     }
//
//     if (r.valid(layer.below))
//     {
//         r.get<FactoryLayer>(layer.below).above = layer.above;
//     }
// }
//
// static void ChunkSpriteModificationHelper(entt::registry& r, entt::entity entity);
//
// static void ChunkRemoveLayerHelper(entt::registry& r, entt::entity entity)
// {
//     ChunkRemoveLayerHelper1(r, entity);
//     ChunkSpriteModificationHelper(r, entity);
//     r.erase<FactoryLayer>(entity);
// }
//
// static void ChunkRemoveLayerHelperWithoutFL(entt::registry& r, entt::entity entity)
// {
//     ChunkRemoveLayerHelper1(r, entity);
//     if (r.try_get<ChunkSprite>(entity))
//         r.erase<ChunkSprite>(entity);
// }

static void ChunkAddLayerHelper(entt::registry& r, entt::entity entity)
{
    GridTransform& grid = r.get<GridTransform>(entity);
    FactoryLayer* p_layer = r.try_get<FactoryLayer>(entity);

    // Clear if entity was already referring to something
    FactoryLayer layer;
    layer.components_dependent_on = 1;
    if (p_layer)
    {
        layer.components_dependent_on = p_layer->components_dependent_on;
        ChunkRemoveLayerHelper1(r, entity);
    }

    entt::entity chunk_e = chunk_lookup.GetChunk(grid);
    Chunk& chunk = r.get<Chunk>(chunk_e);
    u8 chunk_pos = grid.CropTo8x8Grid();

    layer.last = grid;

    bool get_last;
    entt::entity find = chunk.FindEntityByLayer(r, grid, grid.layer, 255, &get_last);
    if (r.valid(find))
    {
        GridTransform find_tr = r.get<GridTransform>(find);
        FactoryLayer find_fl = r.get<FactoryLayer>(find);
        // If the search failed, we are given the closest match (above us)
        if (get_last)
        {
            layer.above = find;
            find_fl.below = entity;
        } else
        {
            assert(find_tr.layer != grid.layer && "Did not check if layer was occupied before placing at layer");

            layer.above = find_fl.above;
            layer.below = find;
            find_fl.above = entity;

            if (!r.valid(layer.above))
            {
                chunk.top_entity_ids[chunk_pos] = entity;
            }
        }
    }
    else
    {
        chunk.top_entity_ids[chunk_pos] = entity;
    }
    r.emplace<FactoryLayer>(entity);
    ChunkUpdateEntityHelper(chunk, chunk_pos, r);
}

static void ModfiySpriteLayer(entt::registry r, entt::entity entity)
{
}

static void GridTransformRemoveDependents(entt::registry& r, entt::entity entity)
{
    if (r.try_get<ChunkSprite>(entity)) r.remove<ChunkSprite>(entity);
}

static void FactoryLayerRemoveDependents(entt::registry& r, entt::entity entity)
{
    if (r.try_get<GridTransform>(entity)) r.remove<GridTransform>(entity);
}

void SetupChunkCallbacks(entt::registry& r)
{
    r.on_construct<GridTransform>().connect<ChunkAddLayerHelper>();
    r.on_update<GridTransform>().connect<ChunkAddLayerHelper>();
    r.on_destroy<GridTransform>().connect<GridTransformRemoveDependents>();

    r.on_construct<ChunkSprite>().connect<ChunkSpriteModificationHelper>();
    r.on_update<ChunkSprite>().connect<ChunkSpriteModificationHelper>();
    r.on_destroy<ChunkSprite>().connect<ChunkSpriteModificationHelper>();

    r.on_destroy<FactoryLayer>().connect<FactoryLayerRemoveDependents>();
}
