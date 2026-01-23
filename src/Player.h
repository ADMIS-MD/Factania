// Sangbeom Kim
// 01/20/2026

#pragma once

#include <entt.hpp>
#include "Math.h"

const int PLAYER_SPR = 32;

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

struct PlayerMove 
{
	fixed speed = 2.f;
};

entt::entity CreatePlayer(entt::registry& registry);

void UpdatePlayer(entt::registry& registry);