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

#include "Player.h"

//-----------------------------------------------------------------------------
//	Methods
//-----------------------------------------------------------------------------

EntitySystemManager::EntitySystemManager(entt::registry& registry)
{
	CreatePlayer(entt::registry & registry);
}

EntitySystemManager::~EntitySystemManager()
{

}

void EntitySystemManager::Update(entt::registry& registry)
{
	
}