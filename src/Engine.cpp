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
#include "RenderSystem.h"

#include "debug_menu/debug_menu.h"
#include <nds.h>

//-----------------------------------------------------------------------------
//	Method Declarations
//-----------------------------------------------------------------------------

namespace core {

  Engine::Engine()
  {
    m_systems[SYSTEM_RENDER] = new render::RenderSystem();
    consoleDemoInit();
  }

  Engine::~Engine()
  {
    for (uint8 i = 0; i < NUM_SYSTEMS; ++i)
    {
      delete m_systems[i];
    }
  }

  void Engine::Update()
  {
    for (uint8 i = 0; i < NUM_SYSTEMS; ++i)
    {
      m_systems[i]->Update();
    }
  }

  void Engine::Draw()
  {
    for (uint8 i = 0; i < NUM_SYSTEMS; ++i)
    {
      m_systems[i]->Draw();
    }
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


      if (m_shouldQuit)
        break;
    }
  }
}