// Sangbeom Kim
// 01/20/2026

#include "nds.h"
#include "Player.h"

#include <chunk.hpp>

#include "Transform.h"
#include "Sprite.h"
#include "player_sprite.h"

static glImage g_playerImages[16];
static int g_playerTexId = -1;
const uint16_t Player_texcoords[] = {
      0,   0, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR,   0, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR * 2,   0, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR * 3,   0, PLAYER_SPR, PLAYER_SPR,

      0,  PLAYER_SPR, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR,  PLAYER_SPR, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR * 2,  PLAYER_SPR, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR * 3,  PLAYER_SPR, PLAYER_SPR, PLAYER_SPR,

      0,  PLAYER_SPR * 2, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR,  PLAYER_SPR * 2, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR * 2,  PLAYER_SPR * 2, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR * 3,  PLAYER_SPR * 2, PLAYER_SPR, PLAYER_SPR,

     0,   PLAYER_SPR * 3, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR,  PLAYER_SPR * 3, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR * 2,  PLAYER_SPR * 3, PLAYER_SPR, PLAYER_SPR,
     PLAYER_SPR * 3,  PLAYER_SPR * 3, PLAYER_SPR, PLAYER_SPR,
};

static uint8 miningTimer = 0;

static void LoadPlayerSprite() {
    if (g_playerTexId >= 0) return;

    g_playerTexId = glLoadSpriteSet(
        g_playerImages,
        16,
        Player_texcoords,
        GL_RGB256,
        128, 128,
        TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
        128,
        player_spritePal,
        player_spriteBitmap
    );

    if (g_playerTexId < 0) {
        printf("Failed to load texture: %d\n", g_playerTexId);
    }
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
    return false;

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

static inline void SetAnim(Sprite& sp, Animation& an, int start, int end)
{
    an.start = start;
    an.end = end;
    sp.spriteID = start;
}

static inline void SetMode(PlayerState& st, Sprite& sp, Animation& an, PlayerMode newMode)
{
    if (st.mode == newMode) return;
    st.mode = newMode;

    switch (newMode)
    {
    case PlayerMode::IDLE:
        SetAnim(sp, an, 0, 5);
        break;
    case PlayerMode::MOVING:
        SetAnim(sp, an, 6, 15);
        break;
    case PlayerMode::MINING:
        SetAnim(sp, an, 0, 5); // until we get something better?
        break;
    default:
        SetAnim(sp, an, 0, 5); // just idle if we don't have something set
        break;
    }

}

void CreatePlayerComponent(entt::registry& registry)
{
    LoadPlayerSprite();

    const entt::entity entity = registry.create();

    registry.emplace<Transform>(entity, Vec2(FINT(0), FINT(0)), 1);
    registry.emplace<GridTransform>(entity);
    auto& st = registry.emplace<PlayerState>(entity);
    auto& sp = registry.emplace<Sprite>(entity, g_playerImages, 0, PLAYER_SPR, false, false);
    auto& an = registry.emplace<Animation>(entity);
    registry.emplace<PlayerMove>(entity);

    st.mode = PlayerMode::IDLE;
    SetAnim(sp, an, 0, 5);
}

void UpdatePlayerComponent(entt::registry& registry, ChunkLookup& chl)
{
    const uint16_t held = keysHeld();
    const uint16_t down = keysDown();

    Vec2 dir(0.f, 0.f);
    if (held & KEY_LEFT)  dir.X() -= 1.f;
    if (held & KEY_RIGHT) dir.X() += 1.f;
    if (held & KEY_UP)    dir.Y() -= 1.f;
    if (held & KEY_DOWN)  dir.Y() += 1.f;

    if (dir.X() != 0.f && dir.Y() != 0.f) {
        dir *= fixed(0.707f);
    }

    auto view = registry.view<Transform, GridTransform, PlayerState, Sprite, Animation, PlayerMove>();

    for (auto player : view) {
        auto& tr = view.get<Transform>(player);
        auto& gtr = view.get<GridTransform>(player);
        auto& st = view.get<PlayerState>(player);
        auto& sp = view.get<Sprite>(player);
        auto& an = view.get<Animation>(player);
        const auto& mv = view.get<PlayerMove>(player);

        if (!st.inputEnabled) {
            // skip update
            continue;
        }

        switch (st.mode) {
        case PlayerMode::IDLE:
        {
            // TODO: change to mine or build if key press or something

            if (dir.X() != 0.f || dir.Y() != 0.f)
            {
                SetMode(st, sp, an, PlayerMode::MOVING);
            }

            if (down & KEY_A)
            {
                SetMode(st, sp, an, PlayerMode::MINING);
            }

            break;
        }

        case PlayerMode::MOVING:
        {
            if (dir.X() == 0.f && dir.Y() == 0.f)
            {
                SetMode(st, sp, an, PlayerMode::IDLE);
                break;
            }

            // mining should take precedence when moving :3
            if (down & KEY_A)
            {
                SetMode(st, sp, an, PlayerMode::MINING);
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
            if (!(held & KEY_A))
            {
                SetMode(st, sp, an, PlayerMode::IDLE);
                miningTimer = 0;
                break;
            }

            miningTimer++; // we know our clock cycle

            if (miningTimer >= 60)
            {
                miningTimer = 0;
                printf("GET IRON.\n");
            }

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