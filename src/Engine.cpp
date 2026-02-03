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
#include <fat.h> 

#include <debug_menu/debug_menu.h>
#include "Player.h"
#include "Conveyer.h"

#include "building.h"
#include "Console.h"

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

std::vector<Conveyer*> convTest = InitTest();


bool file_load(const char *path, void **buffer, size_t *size)
{
    // Open the file in read binary mode
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        perror("fopen");
        return false;
    }

    // Move read cursor to the end of the file
    int ret = fseek(f, 0, SEEK_END);
    if (ret != 0)
    {
        perror("fseek");
        return false;
    }

    // Check position of the cursor (we're at the end, so this is the size)
    *size = ftell(f);
    if (*size == 0)
    {
        printf("Size is 0!");
        fclose(f);
        return false;
    }

    // Move cursor to the start of the file again
    rewind(f);

    // Allocate buffer to hold data
    *buffer = malloc(*size);
    if (*buffer == NULL)
    {
        printf("Not enought memory to load %s!", path);
        fclose(f);
        return false;
    }

    // Read all data into the buffer
    if (fread(*buffer, *size, 1, f) != 1)
    {
        perror("fread");
        fclose(f);
        free(*buffer);
        return false;
    }

    // Close file
    ret = fclose(f);
    if (ret != 0)
    {
        perror("fclose");
        free(*buffer);
        return false;
    }

    return true;
}

static void load_image() {
    char* buf;
    size_t size;

}

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

        shouldQuit = false;
    }

    Engine::~Engine()
    {
        for (auto system : m_systems)
        {
            delete system;
        }
    }

    bool drawConsole = false;

    void Engine::Update()
    {
        for (auto system : m_systems)
        {
            system->Update(m_registry);
        }

        TempUpdateBuildings(m_registry);

        // because i dont have a better place to put it for testing :)
        uint16_t up = keysUp();

        if (up & KEY_START)
            shouldQuit = true;
        if (down & KEY_A) {
		        convTest[2]->UpdateBuilding(1.0f);

        if ((up & KEY_L) || (up & KEY_R)) {
            drawConsole = !drawConsole;
            ToggleConsole(drawConsole);
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

            if (drawConsole) {
                check_debug_menu();
            }

            Update();

            BeginFrame();

            Update();
            Draw();

            EndFrame();

            if (shouldQuit)
                break;
        }
    }

}