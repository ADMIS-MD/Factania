#pragma once

#include "System.h"
#include <vector>

namespace core {

	class Engine
	{
	public:
		Engine();
		~Engine();

		void Update();
		void Draw();

		void Run();
	private:
		std::vector<System*> m_systems;

		// test variables
		float x, y, z;
		int angle_x, angle_z;

		bool shouldQuit;
	};

}