//-----------------------------------------------------------------------------
//
//	File:   RenderSystem.cpp
//  Author: Nicholas Brennan
//  Date:   01/15/2026
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Includes
//-----------------------------------------------------------------------------

#include "RenderSystem.h"

#include <gl2d.h>
#include <nds.h>
#include <iostream>

//-----------------------------------------------------------------------------
//	Method Implementations
//-----------------------------------------------------------------------------

namespace render {

	RenderSystem::RenderSystem()
	{
		glScreen2D();
		videoSetMode(MODE_0_3D);
	}

	RenderSystem::~RenderSystem()
	{

	}

	void RenderSystem::Draw()
	{
		// test box
		glBoxFilledGradient(0, 0,
			TOP_SCREEN_WIDTH / 2 - 1, TOP_SCREEN_HEIGHT / 2- 1,
			RGB15(31, 0, 0),
			RGB15(31, 31, 0),
			RGB15(31, 0, 31),
			RGB15(0, 31, 31));
	}

	void BeginFrame()
	{
		glBegin2D();
	}

	void EndFrame()
	{
		glEnd2D();
		glFlush(0);
	}

	void SetClearColor(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		glClearColor(r, g, b, a);
	}

}