// Sangbeom Kim
// 01/20/2026
#pragma once

typedef float fixed;

class Player 
{
	Player(fixed position[]) : position(position) {}

	enum State {
		MOVING, MINING, BUILDING, NUM_STATE
	};

	void Update();

private:
	fixed* position = new fixed[3];

	State state;
};