//
// Created by willb on 1/15/2026.
//

#include "build_details_menu.h"

#include <ios>
#include <nds/system.h>
#include <sstream>
#include "generated_defines.h"

static std::string get_build_details() {
    std::stringstream ss;
    ss << "short : " << FACTANIA_GIT_COMMIT_HASH_SHORT << "\n";
    ss << "  date  : " << FACTANIA_BUILD_DATE << "\n";
    ss << "  branch: " << FACTANIA_GIT_BRANCH_NAME << "\n";
    ss << "  commit: " << FACTANIA_GIT_BRANCH_COMMIT_NUMBER << "\n";
    return ss.str();
}

std::vector<DebugNode*> build_details_debug_page = {
    new FunctionResultNode{get_build_details}
};

