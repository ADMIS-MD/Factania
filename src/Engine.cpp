//-----------------------------------------------------------------------------
//
//	File:   Engine.cpp
//  Author: Nicholas Brennan
//  Date:   01/15/2026
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Includes
//-----------------------------------------------------------------------------

#include "Engine.h"

#include <errno.h>
#include <dlfcn.h>
#include <stdio.h>

#include <nds.h>
#include <sys/time.h>

#include "RenderSystem.h"

//-----------------------------------------------------------------------------
//	Method Declarations
//-----------------------------------------------------------------------------

namespace core {

  Engine::Engine()
	{
    m_systems[SYSTEM_RENDER] = new render::RenderSystem();
    // Setup sub screen for the text console
    consoleDemoInit();

    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    // Setup perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    // Switch to model view matrix
    glMatrixMode(GL_MODELVIEW);

    shouldQuit = false;

    // Print some text in the demo console
    consoleClear();

    // Print some controls
    printf("PAD:     Move\n");
    printf("A,B,X,Y: Rotate\n");
    printf("\n");
    printf("START:   Exit to loader\n");
    printf("Printing from Engine");
	}

  Engine::~Engine()
	{

	}

  void Engine::Update()
  {

  }

  void Engine::Draw()
  {

  }

   System* Engine::GetSystem(E_SYSTEM_TYPE type)
   {
     return m_systems[type];
   }

	void Engine::Run()
	{
    while (1)
    {
      swiWaitForVBlank();

      Update();

      render::BeginFrame();
      Draw();
      render::EndFrame();


      if (shouldQuit)
        break;
    }
	}

}