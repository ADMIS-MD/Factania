
//-----------------------------------------------------------------------------
//
//	File:   Engine.h
//  Author: Nicholas Brennan
//  Date:   01/15/2026
//
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------
//	Include
//-----------------------------------------------------------------------------

#include "System.h"

//-----------------------------------------------------------------------------
//	Defines
//-----------------------------------------------------------------------------

enum E_SYSTEM_TYPE {
	SYSTEM_RENDER,
	NUM_SYSTEMS
};

//-----------------------------------------------------------------------------
//	Class Definition
//-----------------------------------------------------------------------------

namespace core {

	class Engine
	{
	public:
		Engine();
		~Engine();

		void Update();
		void Draw();

		System* GetSystem(E_SYSTEM_TYPE type);

		void Run();
	private:
		// test variables
		float x, y, z;
		int angle_x, angle_z;

		bool shouldQuit;

		System* m_systems[NUM_SYSTEMS];
		int m_numSystems = 0;
	};

}