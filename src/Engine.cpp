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
#include <fat.h> 

#include <debug_menu/debug_menu.h>
#include "RenderSystem.h"
#include "EntitySystemManager.h"
#include "Console.h"
#include "Pause.h"
#include "Player.h"
#include "Math.h"
#include "Conveyer.h"
#include "building.h"

//-----------------------------------------------------------------------------
//	Method Declarations
//-----------------------------------------------------------------------------

// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Ni�o D�az, 2024-2025

// This example shows how to draw a translucent 3D box in which you can see all
// faces. This is how you would see 3D objects that are translucent, but not
// fully solid. For example, you could have an empty cube where all the faces
// are translucent pieces of plastic.

// Test code? might have to remove later
std::vector<Conveyer*> convTest = InitTest();

namespace core {

    Engine::Engine()
    {
        // Initializing fat
        bool init_ok = fatInitDefault();

        if (!init_ok) {
            // Handle error
            consoleDemoInit();
            // Print error
            printf("fatInitDefault() failed: %d", errno);

            while (1)
                swiWaitForVBlank();
        }

        // Add Systems Here
        m_systems.push_back(new RenderSystem());
        m_systems.push_back(new EntitySystemManager(m_registry));

        m_registry.ctx().emplace<PauseControl>();
        m_registry.ctx().emplace<ChunkLookup>();

        //temp testing stuff
        ItemQuantity ironIngot = ItemQuantity(Item(100, "iron_ingot"), 1);
        ItemQuantity ironPlate = ItemQuantity(Item(101, "air"), 0);

        Recipe tempRecipie;
        tempRecipie.inputItems.push_back(ironPlate);
        tempRecipie.outputItems.push_back(ironIngot);
        tempRecipie.recipeDuration = 1;

        std::vector<Recipe> buildingRecipes;
        buildingRecipes.push_back(tempRecipie);

        FactoryBuilding building = FactoryBuilding(buildingRecipes, 0);
        ironIngot.quantity = 10;
        building.InputItems(ironIngot);

        building.status = BuildingStatus::Idle;

        const entt::entity entityLink = m_registry.create();
        m_registry.emplace<FactoryBuilding>(entityLink, std::forward<FactoryBuilding>(building));
        //end of testing stuffs
    }

    Engine::~Engine()
    {
        for (auto system : m_systems)
        {
            delete system;
        }
    }

    void Engine::Update()
    {
        // because i dont have a better place to put it for testing :)
        uint16_t up = keysUp();
		uint16_t down = keysDown();

        if (up & KEY_START) {
            m_registry.ctx().get<PauseControl>().pause = !m_registry.ctx().get<PauseControl>().pause;
        }

        if (down & KEY_A) {
            convTest[2]->UpdateBuilding(1.0f);
        }

        if ((up & KEY_L)) {
            if (ConsoleVisible()) {
                ToggleConsole(false);
            }
            else {
                ToggleConsole(true);
            }
        }

        for (auto system : m_systems)
        {
            system->Update(m_registry);
        }
    }

    void Engine::Draw()
    {
        for (auto system : m_systems)
        {
            system->Draw(m_registry);
        }
    }

    void Engine::Run()
    {
        while (1)
        {
            // As far as I'm aware, this is our "tick", so it should run
            // indepedent from any specific loop. Please correct me if wrong -Nick
            swiWaitForVBlank();

            scanKeys();
            //check_debug_menu(); // commenting out for now

            Update();

            BeginFrame();

            Draw();

            EndFrame();
        }
    }
}