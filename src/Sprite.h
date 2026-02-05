// Sangbeom Kim
// 01/20/2026

#pragma once

#include <gl2d.h>
#include <nds.h>

#define SCREENW 256
#define SCREENH 192

struct Sprite 
{
	glImage* sprite;
	int spriteID = 0;
	int spriteSize = 0;

	bool hide = false;
	bool xFlip = false;

	Vec2 camDrawOffset = {FINT(-16), FINT(-16)};
};

struct Animation
{
	int start = 0;
	int end = 0;
};

struct SubSprite
{
	u16* gfx = nullptr;
	int oamId = 0;
	SpriteSize size = SpriteSize_16x16;
	int spriteSize = 16;

	bool hide = false;
	bool xFlip = false;
};