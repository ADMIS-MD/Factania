//
// Created by willb on 1/15/2026.
//

#include "memory_debug_info.h"
#include <malloc.h>
#include <string>
#include <sstream>
#include <nds.h>

static std::string get_memory_info() {
    struct mallinfo m = mallinfo();

    std::stringstream ss;
    ss << "free : " << static_cast<float>(m.fordblks + (uintptr_t)(getHeapLimit() - getHeapEnd())) / (1e+6) << " mb\n";
    ss << "  used : " << static_cast<float>(m.uordblks) / (1e+6) << " mb\n";
    ss << "  heap\n";
    ss << "    start: 0x" << std::hex << (uintptr_t)getHeapStart() << "\n";
    ss << "    end  : 0x" << std::hex << (uintptr_t)getHeapEnd() << "\n";
    ss << "    limit: 0x" << std::hex << (uintptr_t)getHeapLimit() << "\n";
    return ss.str();
}

std::vector<DebugNode*> memory_info_debug_page = {
    new FunctionResultNode{get_memory_info}
};

