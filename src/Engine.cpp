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
#include "Player.h"

#include <errno.h>
#include <dlfcn.h>
#include <Math.h>
#include <stdio.h>

#include <nds.h>
#include <debug_menu/debug_menu.h>
#include "Save.h"
#include "Player.h"
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
        // Add Systems Here
        m_systems.push_back(new RenderSystem());

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

        // Setup sub screen for the text console
        consoleDemoInit();

        shouldQuit = false;

        // Print some text in the demo console
        consoleClear();

        //// Loading save data
        //memset(&savedata, 0, sizeof(savedata));

        //bool loaded = LoadSave(&savedata);

        //if (loaded) {
        //    //x = savedata.x;
        //    //y = savedata.y;
        //    //z = savedata.z;
        //    //angle_x = savedata.angle_x;
        //    //angle_z = savedata.angle_z;

        //    printf("Save file loaded: Revision %i\n", savedata.save_count);
        //}
        //else {
        //    printf("Save file not found\n");
        //}

        // Save Debug Menu Callbacks
        //static std::vector<DebugNode*> s_save_menu_nodes;

        //s_save_menu_nodes.push_back(new CallbackDebugNode("Save Game", [this]() -> std::string {
        //    /*savedata.save_count++;
        //    savedata.x = x;
        //    savedata.y = y;
        //    savedata.z = z;
        //    savedata.angle_x = angle_x;
        //    savedata.angle_z = angle_z;

        //    if (save_write(&savedata)) {
        //        return "Game Saved. Revision " + std::to_string(savedata.save_count);
        //    }
        //    return "Save Failed";*/
        //}));

        //s_save_menu_nodes.push_back(new CallbackDebugNode("Load Save", [this]() -> std::string {
        //    /*if (save_load(&savedata)) {
        //        x = savedata.x;
        //        y = savedata.y;
        //        z = savedata.z;
        //        angle_x = savedata.angle_x;
        //        angle_z = savedata.angle_z;

        //        return "Save Loaded. Revision " + std::to_string(savedata.save_count);
        //    }
        //    return "Save file not found";*/
        //}));

        //s_save_menu_nodes.push_back(new CallbackDebugNode("Delete Save", [this]() -> std::string {
        //    /*if (save_delete()) {
        //        savedata.save_count = 0;
        //        return "Save deleted";
        //    }
        //    else {
        //        return "Failed to delete save";
        //    }*/
        //}));

        //add_debug_node_to_root(new SubmenuDebugNode("Save Menu", s_save_menu_nodes));

        load_image();

        CreatePlayer(registry);

        printf("START:   Exit to loader\n");
        printf("SELECT:  Open Debug Menu\n");
        printf("\nPrinting from Engine\n");
        fixed result = fixed{1123.2115f} + fixed{1.963f};
        printf("fixed 1123.2115 + 1.963 = %f", static_cast<float>(result));
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
            system->Update(registry);
        }

        UpdatePlayer(registry);

        // because i dont have a better place to put it for testing :)
        uint16_t down = keysDown();

        if (down & KEY_START)
            shouldQuit = true;
        if (down & KEY_A) {
			convTest[2]->UpdateBuilding(1.0f);
        }
    }

    void Engine::Draw()
    {
        for (auto system : m_systems)
        {
            system->Draw(registry);
        }
    }

    void Engine::Run()
    {
        while (1)
        {
            // As far as I'm aware, this is our "tick", so it should run
            // indepedent from any specific loop. Please correct me if wrong -Nick
            swiWaitForVBlank();

            BeginFrame();

			printTest(convTest);

            Update();
            Draw();
            check_debug_menu();

            EndFrame();

            if (shouldQuit)
                break;
        }
    }

}