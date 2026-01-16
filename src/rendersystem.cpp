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

#include "rendersystem.h"

#include <nds.h>
#include <gl2d.h>

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

	void RenderSystem::BeginFrame()
	{
		glBegin2D();
	}

	void RenderSystem::EndFrame()
	{
		glEnd2D();
		glFlush(0);
	}

}