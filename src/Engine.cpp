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
#include "building.h"

#include "Conveyer.h"

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
        //std::vector<Conveyer*> convTest = InitTest();
        Recipe tempRecipie;
        // Build input/output using ItemQuantity (Item(itemID, name), quantity)
        tempRecipie.inputItems.push_back(ItemQuantity(Item((int)ItemType::Coal, "Coal"), 1));
        tempRecipie.outputItems.push_back(ItemQuantity(Item((int)ItemType::IronPlate, "Iron Plate"), 1));
        tempRecipie.recipeDuration = 1.0f;
        tempRecipie.powerDraw = 0.0f;

        std::vector<Recipe> buildingRecipes;
        buildingRecipes.push_back(tempRecipie);

        // Create entity and emplace FIRST
        const entt::entity buildingEntity = m_registry.create();
        auto& building = m_registry.emplace<FactoryBuilding>(buildingEntity, buildingRecipes, 0);
        
        // Now modify the component that's stored in the registry
        building.InputItems(ItemType::Coal, 20);
        building.status = BuildingStatus::Idle;
        
        // Create entities for conveyers FIRST, before setting up links
        std::vector<entt::entity> conveyorEntities;
        std::vector<entt::entity> createdEntities;
        for (int i = 0; i <= 2; i++)
        {
            const entt::entity convEntity = m_registry.create();
            auto& conv1 = m_registry.emplace<Conveyer>(convEntity);
            conv1.id = i + 1;
            conveyorEntities.push_back(convEntity);
        }

        // Now wire them using entities -> get components from registry
        for (size_t i = 1; i < conveyorEntities.size(); ++i)
        {
            auto& prev = m_registry.get<Conveyer>(conveyorEntities[i - 1]);
            auto& cur  = m_registry.get<Conveyer>(conveyorEntities[i]);

            // cur.inputs <- prev
            cur.inputs.push_back(&prev);
            // prev.outputs <- cur
            prev.outputs.push_back(&cur);
        }
        
        // NOW set up the pointer to building on the REGISTRY version
        auto& conv0 = m_registry.get<Conveyer>(conveyorEntities[0]);
        conv0.inputs.push_back(&building);
        
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


        if ((up & KEY_L) || (up & KEY_R)) {
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
            check_debug_menu();

            Update();

            BeginFrame();

            Draw();

            EndFrame();
        }
    }
}