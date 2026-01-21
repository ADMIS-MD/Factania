//-----------------------------------------------------------------------------
//
//	File:   RenderSystem.cpp
//  Author: Nicholas Brennan
//  Date:   01/20/2026
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Includes
//-----------------------------------------------------------------------------

#include "RenderSystem.h"

#include <nds.h>

//-----------------------------------------------------------------------------
//	Method Implementations
//-----------------------------------------------------------------------------

// !! TEMP !!
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

namespace core {

    RenderSystem::RenderSystem()
    {
        // taken temporarily from the cube test
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
    }

    RenderSystem::~RenderSystem()
    {

    }

    void RenderSystem::Update()
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
    }

    void RenderSystem::Draw()
    {

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

    void BeginFrame()
    {

    }

    void EndFrame()
    {

    }

}