// Sangbeom Kim
// 01/20/2026

#include "nds.h"
#include "Player.h"
#include "Transform.h"

entt::entity CreatePlayer(entt::registry& registry)
{
    const entt::entity entity = registry.create();

    registry.emplace<Transform>(entity);
    registry.emplace<GridTransform>(entity);
    registry.emplace<PlayerState>(entity);
    registry.emplace<PlayerSprite>(entity);
    registry.emplace<PlayerMove>(entity);

    return entity;
}

static inline int32 WorldToTile(const fixed& p)
{
    // TODO: convert given world position to tilemap position
    return 0;
}

static inline bool CheckCollision(const Vec2& pos) 
{
    int32 playerTileX = WorldToTile(pos.X());
    int32 playerTileY = WorldToTile(pos.Y());

    // TODO: get tilemap array?

    for (int indexX = -1; indexX <= 1; ++indexX) {
        for (int indexY = -1; indexY <= 1; ++indexY) {
            int checkX = playerTileX + indexX;
            int checkY = playerTileY + indexY;

            // TODO: if (tile is empty) continue;

            fixed tileSize = 16.f;
            fixed minX = fixed(static_cast<int32>(checkX)) * tileSize;
            fixed minY = fixed(static_cast<int32>(checkY)) * tileSize;
            fixed maxX = minX + tileSize;
            fixed maxY = minY + tileSize;

            if (pos.X() >= minX && pos.X() < maxX && pos.Y() >= minY && pos.Y() < maxY) {
                return true;
            }
        }
    }
    return false;
}

static inline void SetMode(PlayerState& st, PlayerSprite& sp, PlayerMode newMode)
{
    if (st.mode == newMode) {
        return;
    }

    st.mode = newMode;

    // TODO: set animation
}

void UpdatePlayer(entt::registry& registry)
{
    // remove this if already exist 
    scanKeys();
    const uint16_t held = keysHeld();

    Vec2 dir(0.f, 0.f);
    if (held & KEY_LEFT)  dir.X() -= 1.f;
    if (held & KEY_RIGHT) dir.X() += 1.f;
    if (held & KEY_UP)    dir.Y() -= 1.f;
    if (held & KEY_DOWN)  dir.Y() += 1.f;

    if (dir.X() != 0.f && dir.Y() != 0.f) {
        dir *= fixed(0.707f);
    }

    auto player = registry.view<Transform, GridTransform, PlayerState, PlayerSprite, PlayerMove>();

    for (auto entity : player) {
        auto& tr = player.get<Transform>(entity);
        auto& gtr = player.get<GridTransform>(entity);
        auto& st = player.get<PlayerState>(entity);
        auto& sp = player.get<PlayerSprite>(entity);
        const auto& mv = player.get<PlayerMove>(entity);

        if (!st.inputEnabled) {
            // skip update
            continue;
        }

        switch (st.mode) {
        case PlayerMode::IDLE:
        {
            // TODO: change to mine or build if key press or something

            if (dir.X() != 0.f || dir.Y() != 0.f) {
                SetMode(st, sp, PlayerMode::MOVING);
            }
            break;
        }

        case PlayerMode::MOVING:
        {
            if (dir.X() == 0.f && dir.Y() == 0.f) {
                SetMode(st, sp, PlayerMode::IDLE);
                break;
            }

            // flip sprite
            if (dir.X() < 0.f) {
                sp.xFlip = true;
            }
            else if (dir.X() > 0.f) {
                sp.xFlip = false;
            }

            Vec2 delta = dir;
            delta *= mv.speed;

            Vec2 next = tr.pos;
            next.X() += delta.X();
            if (!CheckCollision(next)) {
                tr.pos.X() = next.X();
            }

            next = tr.pos;
            next.Y() += delta.Y();
            if (!CheckCollision(next)) {
                tr.pos.Y() = next.Y();
            }

            gtr.x = WorldToTile(tr.pos.X());
            gtr.y = WorldToTile(tr.pos.Y());

            break;
        }

        case PlayerMode::MINING:
        {
            // TODO
            break;
        }

        case PlayerMode::BUILDING:
        {
            // TODO
            break;
        }

        default:
            break;
        }
    }
}