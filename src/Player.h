// Sangbeom Kim
// 01/20/2026

#pragma once

#include <chunk.hpp>
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
	fixed speed = .16666f;
};

void CreatePlayerComponent(entt::registry& registry);
void UpdatePlayerComponent(entt::registry& registry, ChunkLookup& chl);
