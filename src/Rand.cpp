//
// Created by willb on 1/22/2026.
//

#include "Rand.h"

#include <chrono>

u32 rand::GenerateNextU32_1(u32 seed)
{
    return GenerateNextLCG<u32, 2891336453, 1>(seed);
}

u32 rand::GenerateNextU32_2(u32 seed)
{
    return GenerateNextLCG<u32, 29943829, 1>(seed);
}

u32 rand::GenerateNextU32_3(u32 seed)
{
    return GenerateNextLCG<u32, 1664525, 1013904223>(seed);
}

u32 rand::GenerateNextU32_4(u32 seed)
{
    return GenerateNextLCG<u32, 134775813, 1>(seed);
}

// Defualt seed, good enough
u32 rand_seed =
    std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

u32 rand::Rand()
{
    rand_seed = GenerateNextU32_1(rand_seed);
    return rand_seed;
}

void rand::SRand(u32 seed)
{
    rand_seed = seed;
}
