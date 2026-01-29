// Sangbeom Kim
// 01/20/2026

#pragma once

#include <gl2d.h>

#define SCREENW 256
#define SCREENH 192

struct Sprite 
{
	glImage* sprite;
	int spriteID = 0;
	int spriteSize = 0;

	bool xFlip = false;
};

struct Animation
{
	int start = 0;
	int end = 0;
};