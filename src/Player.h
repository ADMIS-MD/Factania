// Sangbeom Kim
// 01/20/2026

#pragma once

#include <entt.hpp>
#include "Math.h"

enum class PlayerMode 
{
	IDLE,
	MOVING,
	MINING,
	BUILDING,
};

struct PlayerState 
{
	PlayerMode mode = PlayerMode::IDLE;
	bool inputEnabled = true;
};

struct PlayerSprite 
{
	int spriteID = 0;
	int anim = 0;
	bool xFlip = false;
};

struct PlayerMove 
{
	fixed speed = 0.2f;
};

entt::entity CreatePlayer(entt::registry& registry);

void UpdatePlayer(entt::registry& registry);