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
#include "save.h"

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

static void draw_box(float bx_, float by_, float bz_, float ex_, float ey_, float ez_)
{
    // Begin and end coordinates
    int bx = floattov16(bx_);
    int ex = floattov16(ex_);
    int by = floattov16(by_);
    int ey = floattov16(ey_);
    int bz = floattov16(bz_);
    int ez = floattov16(ez_);

    glBegin(GL_QUADS);

    glColor3f(1, 0, 0);

    glVertex3v16(bx, by, bz);
    glVertex3v16(bx, ey, bz);
    glVertex3v16(ex, ey, bz);
    glVertex3v16(ex, by, bz);

    glColor3f(0, 1, 0);

    glVertex3v16(bx, by, ez);
    glVertex3v16(ex, by, ez);
    glVertex3v16(ex, ey, ez);
    glVertex3v16(bx, ey, ez);

    glColor3f(0, 0, 1);

    glVertex3v16(bx, by, bz);
    glVertex3v16(ex, by, bz);
    glVertex3v16(ex, by, ez);
    glVertex3v16(bx, by, ez);

    glColor3f(1, 0, 1);

    glVertex3v16(bx, ey, bz);
    glVertex3v16(bx, ey, ez);
    glVertex3v16(ex, ey, ez);
    glVertex3v16(ex, ey, bz);

    glColor3f(0, 1, 1);

    glVertex3v16(bx, by, bz);
    glVertex3v16(bx, by, ez);
    glVertex3v16(bx, ey, ez);
    glVertex3v16(bx, ey, bz);

    glColor3f(1, 1, 0);

    glVertex3v16(ex, by, bz);
    glVertex3v16(ex, ey, bz);
    glVertex3v16(ex, ey, ez);
    glVertex3v16(ex, by, ez);

    glEnd();
}

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

        // Setup sub screen for the text console
        consoleDemoInit();

        videoSetMode(MODE_0_3D);

        glInit();

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

        // Loading save data
        memset(&savedata, 0, sizeof(savedata));

        bool loaded = save_load(&savedata);

        if (loaded) {
            x = savedata.x;
            y = savedata.y;
            z = savedata.z;
            angle_x = savedata.angle_x;
            angle_z = savedata.angle_z;

            printf("Save file loaded: Revision %i\n", savedata.save_count);
        }
        else {
            printf("Save file not found\n");
        }

        // Save Debug Menu Callbacks
        static std::vector<DebugNode*> s_save_menu_nodes;

        s_save_menu_nodes.push_back(new CallbackDebugNode("Save Game", [this]() -> std::string {
            savedata.save_count++;
            savedata.x = x;
            savedata.y = y;
            savedata.z = z;
            savedata.angle_x = angle_x;
            savedata.angle_z = angle_z;

            if (save_write(&savedata)) {
                return "Game Saved. Revision " + std::to_string(savedata.save_count);
            }
            return "Save Failed";
        }));

        s_save_menu_nodes.push_back(new CallbackDebugNode("Load Save", [this]() -> std::string {
            if (save_load(&savedata)) {
                x = savedata.x;
                y = savedata.y;
                z = savedata.z;
                angle_x = savedata.angle_x;
                angle_z = savedata.angle_z;

                return "Save Loaded. Revision " + std::to_string(savedata.save_count);
            }
            return "Save file not found";
        }));

        s_save_menu_nodes.push_back(new CallbackDebugNode("Delete Save", [this]() -> std::string {
            if (save_delete()) {
                savedata.save_count = 0;
                return "Save deleted";
            }
            else {
                return "Failed to delete save";
            }
        }));

        add_debug_node_to_root(new SubmenuDebugNode("Save Menu", s_save_menu_nodes));

        load_image();

        // Print some controls
        printf("PAD:     Move\n");
        printf("A,B,X,Y: Rotate\n");
        printf("\n");
        printf("START:   Exit to loader\n");
        printf("SELECT:  Open Debug Menu\n");
        printf("\nPrinting from Engine\n");
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
            system->Update();
        }

        scanKeys();

        uint16_t keys = keysHeld();
        uint16_t down = keysDown();

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

        if (down & KEY_START)
            shouldQuit = true;
    }

    void Engine::Draw()
    {
        for (auto system : m_systems)
        {
            system->Draw();
        }

        // Render 3D scene
        // ---------------

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

        glPolyFmt(POLY_ALPHA(10) | POLY_ID(0) | POLY_CULL_FRONT);

        draw_box(-0.75, -0.75, -0.75,
            0.75, 0.75, 0.75);

        glPolyFmt(POLY_ALPHA(10) | POLY_ID(1) | POLY_CULL_BACK);

        draw_box(-0.75, -0.75, -0.75,
            0.75, 0.75, 0.75);

        // Draw test image

        // Tell the hardware that we have sorted translucent polygons manually.

        glFlush(GL_TRANS_MANUALSORT);
    }

    void Engine::Run()
    {
        while (1)
        {
            // As far as I'm aware, this is our "tick", so it should run
            // indepedent from any specific loop. Please correct me if wrong -Nick
            swiWaitForVBlank();

            Update();
            Draw();
            check_debug_menu();

            if (shouldQuit)
                break;
        }
    }

}