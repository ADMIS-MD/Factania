// Sangbeom Kim
// 01/20/2026

#include "nds.h"
#include "Player.h"

#include <chunk.hpp>
#include <RenderSystem.h>

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

static inline bool CheckCollision(entt::registry const& r, const Vec2& pos)
{
    int32 playerTileX = pos.X().GetInt();
    int32 playerTileY = pos.Y().GetInt();

    consoleClear();
    printf("%d, %d\n", playerTileX, playerTileY);

    for (int indexX = -1; indexX <= 1; ++indexX) {
        for (int indexY = -1; indexY <= 1; ++indexY) {
            int checkX = playerTileX + indexX;
            int checkY = playerTileY + indexY;

            GridTransform t = {checkX, checkY};
            Chunk const& c = r.get<Chunk>(chunk_lookup.GetChunk(t));
            if (c.top_entity_ids[t.CropTo8x8Grid()] == entt::null)
                continue;

            fixed minX = fixed(static_cast<int32>(checkX));
            fixed minY = fixed(static_cast<int32>(checkY));
            fixed maxX = minX + FINT(1);
            fixed maxY = minY + FINT(1);

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

    if (newMode == PlayerMode::IDLE) {
        SetAnim(sp, an, 0, 5);
    }
    else if (newMode == PlayerMode::MOVING) {
        SetAnim(sp, an, 6, 15);
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

        // TODO: Remove
        if (down & KEY_A)
        {
            GridTransform grid = tr;
            ChunkPosition chp = ChunkPosition::FromGridTransform(grid);
            Chunk& chunk = chl.GetChunkObj(registry, chp);

            entt::entity new_obj = registry.create();
            registry.emplace<ChunkSprite>(new_obj, ChunkSprite { 1, RGB15(15, 15, 0) });
            registry.emplace<GridTransform>(new_obj, grid);
        }
        if (down & KEY_B)
        {
            GridTransform grid = tr;
            ChunkPosition chp = ChunkPosition::FromGridTransform(grid);
            Chunk& chunk = chl.GetChunkObj(registry, chp);
            if (chunk.top_entity_ids[grid.CropTo8x8Grid()] != entt::null)
                registry.destroy(chunk.top_entity_ids[grid.CropTo8x8Grid()]);
        }
        // TODO: End remove

        if (!st.inputEnabled) {
            // skip update
            continue;
        }

        switch (st.mode) {
        case PlayerMode::IDLE:
        {
            // TODO: change to mine or build if key press or something

            if (dir.X() != 0.f || dir.Y() != 0.f) {
                SetMode(st, sp, an, PlayerMode::MOVING);
            }
            break;
        }

        case PlayerMode::MOVING:
        {
            if (dir.X() == 0.f && dir.Y() == 0.f) {
                SetMode(st, sp, an, PlayerMode::IDLE);
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
            if (!CheckCollision(registry, next)) {
                tr.pos.X() = next.X();
            }

            next = tr.pos;
            next.Y() += delta.Y();
            if (!CheckCollision(registry, next)) {
                tr.pos.Y() = next.Y();
            }

            gtr = tr.pos;
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