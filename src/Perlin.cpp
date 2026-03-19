//-----------------------------------------------------------------------------
//
//	File:   Perlin.cpp
//  Author: Nicholas Brennan
//  Date:   02/26/2026
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Includes
//-----------------------------------------------------------------------------

#include "Perlin.h"
#include <numeric>
#include <algorithm>
#include <random>

//-----------------------------------------------------------------------------
//	Globals
//-----------------------------------------------------------------------------

Perlin g_perlin;

//-----------------------------------------------------------------------------
//	Helpers
//-----------------------------------------------------------------------------

static float Fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

static float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

static float Gradient(int hash, float x, float y)
{
	switch (hash & 3)
	{
		case 0: return x + y;
		case 1: return -x + y;
		case 2: return x - y;
		case 3: return -x - y;
		default: return 0;
	}
}

//-----------------------------------------------------------------------------
//	Methods
//-----------------------------------------------------------------------------

Perlin::Perlin()
{

}

void Perlin::Seed(unsigned seed)
{
	m_permutationTable.resize(256);
	std::iota(m_permutationTable.begin(), m_permutationTable.end(), 0);

	std::mt19937 rng(seed);
	std::shuffle(m_permutationTable.begin(), m_permutationTable.end(), rng);

	m_permutationTable.insert(m_permutationTable.end(), m_permutationTable.begin(), m_permutationTable.end());
}

float Perlin::Noise(float x, float y)
{
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;

	float xf = x - floor(x);
	float yf = y - floor(y);

	float u = Fade(xf);
	float v = Fade(yf);

	int aa = m_permutationTable[m_permutationTable[X] + Y];
	int ab = m_permutationTable[m_permutationTable[X] + Y + 1];
	int ba = m_permutationTable[m_permutationTable[X + 1] + Y];
	int bb = m_permutationTable[m_permutationTable[X + 1] + Y + 1];

	float x1 = Lerp(Gradient(aa, xf, yf), Gradient(ba, xf - 1, yf), u);
	float x2 = Lerp(Gradient(ab, xf, yf - 1), Gradient(bb, xf - 1, yf - 1), u);

	return Lerp(x1, x2, v);
}