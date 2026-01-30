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
#include "EntitySystemManager.h"

#include <errno.h>
#include <dlfcn.h>
#include <Math.h>
#include <stdio.h>

#include <nds.h>
#include <debug_menu/debug_menu.h>
#include "Save.h"
#include "Player.h"

#include "building.h"

//-----------------------------------------------------------------------------
//	Method Declarations
//-----------------------------------------------------------------------------

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

        ItemQuantity ironIngot = ItemQuantity(Item(100, "iron_ingot"), 10);
        ItemQuantity ironPlate = ItemQuantity(Item(101, "iron_plate"), 1);

        Recipe tempRecipie;
        tempRecipie.inputItems.push_back(ironIngot);
        tempRecipie.outputItems.push_back(ironPlate);
        tempRecipie.recipeDuration = 10;

        std::vector<Recipe> buildingRecipes;
        buildingRecipes.push_back(tempRecipie);

        FactoryBuilding building = FactoryBuilding(buildingRecipes, 1);
        building.InputItems(ironIngot);
        building.InputItems(ironIngot);
        building.InputItems(ironIngot);

        building.status = BuildingStatus::Idle;

        const entt::entity entityLink = m_registry.create();
        m_registry.emplace<FactoryBuilding>(entityLink);

        // Setup sub screen for the text console
        // consoleDemoInit();

        shouldQuit = false;

        // Print some text in the demo console
        // consoleClear();

        // printf("START:   Exit to loader\n");
        // printf("SELECT:  Open Debug Menu\n");
        // printf("\nPrinting from Engine\n");
        // fixed result = fixed{1123.2115f} + fixed{1.963f};
        // printf("fixed 1123.2115 + 1.963 = %f", static_cast<float>(result));
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
        for (auto system : m_systems)
        {
            system->Update(m_registry);
        }

        TempUpdateBuildings(m_registry);

        // because i dont have a better place to put it for testing :)
        uint16_t down = keysDown();

        if (down & KEY_START)
            shouldQuit = true;
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
            check_debug_menu();

            scanKeys();
            Update();

            BeginFrame();
            
            Draw();

            EndFrame();

            if (shouldQuit)
                break;
        }
    }

}