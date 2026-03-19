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

//-----------------------------------------------------------------------------
//	Methods
//-----------------------------------------------------------------------------

EntitySystemManager::EntitySystemManager(entt::registry& registry)
{
	CreatePlayerComponent(registry);

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
}