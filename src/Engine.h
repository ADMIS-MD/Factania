#pragma once

#include "System.h"
#include "entt.hpp"
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

		bool shouldQuit;

		entt::registry m_registry;
	};

}