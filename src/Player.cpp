// Sangbeom Kim
// 01/20/2026

#include "Player.h"
#include "nds.h"

entt::entity CreatePlayer(entt::registry registry) 
{
	const entt::entity entity = registry.create();

	registry.emplace<Transform>(entity);
	registry.emplace<PlayerState>(entity);
	registry.emplace<PlayerSprite>(entity);
	registry.emplace<PlayerMove>(entity);

	return entity;
}

void UpdatePlayer(entt::registry &registry)
{
    scanKeys();
    const uint16_t held = keysHeld();

    fixed dx = 0;
    fixed dy = 0;
    if (held & KEY_LEFT)  dx -= 1;
    if (held & KEY_RIGHT) dx += 1;
    if (held & KEY_UP)    dy -= 1;
    if (held & KEY_DOWN)  dy += 1;

    auto view = registry.view<Transform, PlayerState, PlayerMove>();
    for (auto e : view)
    {
        auto& tr = view.get<Transform>(e);
        auto& st = view.get<PlayerState>(e);
        auto& mv = view.get<PlayerMove>(e);

        if (!st.inputEnabled) {
            st.mode = PlayerMode::IDLE;
            continue;
        }

        if (dx == 0 && dy == 0) {
            st.mode = PlayerMode::IDLE;
            continue;
        }

        st.mode = PlayerMode::MOVING;
        tr.x += (fixed)dx * mv.speed;
        tr.y += (fixed)dy * mv.speed;
    }
}