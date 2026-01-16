//
// Created by willb on 1/15/2026.
//

#ifndef FACTANIA_TRANSFORM_H
#define FACTANIA_TRANSFORM_H

#include <nds/ndstypes.h>
#include <nds/arm9/videoGL.h>
#include <entt.hpp>

struct FactoryTransform {
    u16 position[2];
};

// TODO: BEGIN Move me
struct Sprite {
    u16 tile_pack; // (12 bytes tile_id, 4 bytes layer);
    rgb color;
};
// TODO: END Move me

struct FactoryLayer {
    u8 layer;
    entt::entity above {};
    entt::entity below {};
};

#endif //FACTANIA_TRANSFORM_H