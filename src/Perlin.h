#pragma once

#include <vector>

class Perlin
{
public:
	Perlin(unsigned seed);

	float Noise(float x, float y);

private:
	std::vector<int> m_permutationTable;
};