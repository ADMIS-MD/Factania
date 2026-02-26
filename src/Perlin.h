#pragma once

#include <vector>

class Perlin
{
public:
	Perlin();

	void Seed(unsigned seed);
	float Noise(float x, float y);

private:
	std::vector<int> m_permutationTable;
};

extern Perlin g_perlin;