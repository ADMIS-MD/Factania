//
// Created by willb on 1/22/2026.
//

#pragma once

#include <nds/ndstypes.h>

namespace rand {
    //https://en.wikipedia.org/wiki/Linear_congruential_generator
    template<typename INT, INT A, INT C>
    INT GenerateNextLCG(INT seed)
    {
        return seed * A + C;
    }

    // Different LCG functions with different seeds (uses GenerateNext)
    u32 GenerateNextU32_1(u32 seed);
    u32 GenerateNextU32_2(u32 seed);
    u32 GenerateNextU32_3(u32 seed);
    u32 GenerateNextU32_4(u32 seed);

    u32 Rand();
    void SRand(u32 seed);
}