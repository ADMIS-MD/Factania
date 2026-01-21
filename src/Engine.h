#pragma once

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
		// test variables
		float x, y, z;
		int angle_x, angle_z;

		bool shouldQuit;
	};

}