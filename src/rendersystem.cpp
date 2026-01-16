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

#include <nds.h>
#include <gl2d.h>

//-----------------------------------------------------------------------------
//	Method Implementations
//-----------------------------------------------------------------------------

namespace render {

	RenderSystem::RenderSystem()
	{
		videoSetMode(MODE_0_3D);
		glInit();
	}

	RenderSystem::~RenderSystem()
	{

	}

	void BeginFrame()
	{
		//glBegin();
	}

	void EndFrame()
	{
		glEnd();
		glFlush(GL_TRANS_MANUALSORT);
	}

	void SetClearColor(uint8 r, uint8 g, uint8 b, uint8 a)
	{
		glClearColor(r, g, b, a);
	}

}