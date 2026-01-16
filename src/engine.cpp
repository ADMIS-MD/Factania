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
#include <debug_menu/debug_menu.h>
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

    glEnable(GL_ANTIALIAS);
    glEnable(GL_BLEND);

    // The background must be fully opaque and have a unique polygon ID
    // (different from the polygons that are going to be drawn) so that
    // alpha blending works.
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

    angle_x = 45;
    angle_z = 45;

    x = 0.0;
    y = 0.0;
    z = 0.0;

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
    scanKeys();

    uint16_t keys = keysHeld();

    if (keys & KEY_LEFT)
      x -= 0.05;
    if (keys & KEY_RIGHT)
      x += 0.05;

    if (keys & KEY_UP)
      y += 0.05;
    if (keys & KEY_DOWN)
      y -= 0.05;

    if (keys & KEY_A)
      angle_x += 3;
    if (keys & KEY_Y)
      angle_x -= 3;

    if (keys & KEY_X)
      angle_z += 3;
    if (keys & KEY_B)
      angle_z -= 3;

    if (keys & KEY_START)
      shouldQuit = true;
  }

  void Engine::Draw()
  {
    // Synchronize game loop to the screen refresh

    // Setup camera
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 4.0,  // Position
      0.0, 0.0, 0.0,  // Look at
      0.0, 1.0, 0.0); // Up

    // Move and rotate the view before drawing a box
    glTranslatef(x, y, z);

    glRotateY(angle_z);
    glRotateX(angle_x);

    // Use a different polygon ID for front-facing polygons and back-facing
    // polygons. Draw the back-facing polygons first, then the front-facing
    // ones.
    //
    // We don't know which polygons are front-facing or back-facing, so we
    // use culling to select them for us (but we need to send the polygons
    // to the GPU twice.
  }

   System* Engine::GetSystem(E_SYSTEM_TYPE type)
   {
     return m_systems[type];
   }

	void Engine::Run()
	{
    while (1)
    {
      check_debug_menu();
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