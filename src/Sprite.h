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
	int frame = 0;

	int start = 0;
	int end = 0;

	int ticksPerFrame = 6;

	bool xFlip = false;
};