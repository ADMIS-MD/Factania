//-----------------------------------------------------------------------------
//
//	File:   EntitySystemManager.cpp
//  Author: Nicholas Brennan
//  Date:   01/29/2026
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Includes
//-----------------------------------------------------------------------------

#include "EntitySystemManager.h"

#include <nds.h>

#include "Player.h"
#include "Transform.h"
#include "Sprite.h"
#include "InventoryDebugSystem.h"
#include "building.h"
#include "Pause.h"

#include "cursor.h"
#include "Conveyer.h"

//-----------------------------------------------------------------------------
//	Methods
//-----------------------------------------------------------------------------

EntitySystemManager::EntitySystemManager(entt::registry& registry)
{
	CreatePlayerComponent(registry);

    // Test Cursor Entity Init
    u16* cursorGfx = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
    dmaCopy(cursorPal, SPRITE_PALETTE_SUB, cursorPalLen);
    dmaCopy(cursorTiles, cursorGfx, cursorTilesLen);
    const entt::entity entity = registry.create();
    registry.emplace<Transform>(entity);
    registry.emplace<SubSprite>(entity, cursorGfx, 0, SpriteSize_32x32, 32, false, false);
    SetupChunkCallbacks(registry);
}

EntitySystemManager::~EntitySystemManager()
{

}

void EntitySystemManager::Update(entt::registry& registry)
{
    if (registry.ctx().get<PauseControl>().PauseEntity()) {
        return;
    }
    UpdateInventoryDebug(registry);
	UpdatePlayerComponent(registry, registry.ctx().get<ChunkLookup>());

    auto factoryBuildingView = registry.view<FactoryBuilding>();

    for (auto entity : factoryBuildingView)
    {
        auto& component = factoryBuildingView.get<FactoryBuilding>(entity);
        component.UpdateBuilding(0.0166f);
    }

    // Ensure conveyors also get updated each tick
    auto conveyerView = registry.view<Conveyer>();
    for (auto entity : conveyerView)
    {
        auto& conv = conveyerView.get<Conveyer>(entity);
        conv.UpdateBuilding(0.0166f);
    }

    // Test Cursor Entity Update
    touchPosition t;
    touchRead(&t);
    bool touching = (keysHeld() & KEY_TOUCH) != 0;
    auto view = registry.view<Transform, SubSprite>();
    for (auto e : view) {
        auto& tr = view.get<Transform>(e);
        auto& ss = view.get<SubSprite>(e);
        ss.hide = !touching;
        tr.pos.X() = fixed(static_cast<int32>(t.px - ss.spriteSize / 2));
        tr.pos.Y() = fixed(static_cast<int32>(t.py - ss.spriteSize / 2));
    }
}