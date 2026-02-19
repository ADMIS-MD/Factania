#pragma once

#include "System.h"
#include "entt.hpp"
#include <vector>

extern bool drawConsole;

#define FENCE printf("FENCE -> %s - %d\n", __FILE__, __LINE__);  while (true) { if (keysDown() & KEY_A) { scanKeys(); break; } };

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

		bool shouldQuit;

		entt::registry m_registry;
	};

}