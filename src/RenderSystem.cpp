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

#include <gl2d.h>
#include <nds.h>

//-----------------------------------------------------------------------------
//	Defines
//-----------------------------------------------------------------------------

const uint32_t screen_width = 256;
const uint32_t screen_height = 192;

const uint32_t screen_half_width = screen_width / 2;
const uint32_t screen_half_height = screen_height / 2;


//-----------------------------------------------------------------------------
//	Method Implementations
//-----------------------------------------------------------------------------

namespace core {

    // Adapted from https://codeberg.org/blocksds/sdk/src/branch/master/examples/gl2d/primitives/source/main.c
    RenderSystem::RenderSystem()
    {
        videoSetMode(MODE_0_3D);

        glScreen2D();
    }

    RenderSystem::~RenderSystem()
    {

    }

    void RenderSystem::Update()
    {

    }

    void RenderSystem::Draw()
    {
        /// TODO: DELETE THIS!!!!

        // Top left quadrant of the screen

        glBoxFilledGradient(0, 0,
            screen_half_width - 1, screen_half_height - 1,
            RGB15(31, 0, 0),
            RGB15(31, 31, 0),
            RGB15(31, 0, 31),
            RGB15(0, 31, 31));

        // Top right quadrant of the screen

        glBoxFilled(screen_half_width, 0,
            screen_width - 1, screen_half_height - 1,
            RGB15(10, 10, 10));

        glBox(screen_half_width, 0,
            screen_width - 1, screen_half_height - 1,
            RGB15(20, 20, 20));

        // Bottom left quadrant of the screen

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(1));

        glTriangleFilled(0, screen_height * 3 / 4,
            screen_half_width - 1, screen_half_height,
            screen_half_width - 1, screen_height - 1,
            RGB15(31, 31, 31));

        glPolyFmt(POLY_ALPHA(16) | POLY_CULL_NONE | POLY_ID(2));

        glTriangleFilledGradient(0, screen_half_height,
            screen_half_width - 1, screen_height * 3 / 4,
            0, screen_height - 1,
            RGB15(0, 31, 0),
            RGB15(31, 0, 0),
            RGB15(0, 0, 31));

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(3));

        // Bottom right quadrant of the screen

        // Note that the pixels can't be seen in some emulators
        glPutPixel(screen_width * 3 / 4 + 10, screen_height * 3 / 4,
            RGB15(0, 31, 31));

        glPutPixel(screen_width * 3 / 4 - 10, screen_height * 3 / 4,
            RGB15(0, 31, 31));

        glLine(screen_half_width, screen_half_height,
            screen_width, screen_height,
            RGB15(31, 0, 0));

        glLine(screen_width, screen_half_height,
            screen_half_width, screen_height,
            RGB15(0, 31, 0));

    }

    void BeginFrame()
    {
        glBegin2D();
    }

    void EndFrame()
    {
        glEnd2D();
        glFlush(0);
    }

}