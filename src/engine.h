namespace core {

	class engine
	{
	public:
		engine();
		~engine();

		void update();
		void draw();

		void run();
	private:
		// test variables
		float x, y, z;
		int angle_x, angle_z;

		bool shouldQuit;
	};

}