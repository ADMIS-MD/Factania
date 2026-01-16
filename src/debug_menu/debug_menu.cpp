//
// Created by willb on 1/15/2026.
//

#include "debug_menu.h"

#include <nds.h>
#include <vector>

#include "build_details_menu.h"
#include "memory_debug_info.h"


std::vector<DebugNode*> main_nodes = {
    new SubmenuDebugNode{"Memory Info", memory_info_debug_page},
    new SubmenuDebugNode{"Build Info", build_details_debug_page}
};

struct DebugState {
    std::vector<DebugNode*>& nodes;
    int idx;
    const char* menu_name;
};

void debug_print(std::vector<DebugState>& node_stack, bool& selected) {
    consoleClear();

    DebugState* top = &node_stack.back();
    if (keysDown() & KEY_B) {
        if (selected)
            selected = !selected;
        else
            node_stack.pop_back();
        return;
    }

    if (!selected) {
        if (keysDown() & KEY_UP)
            top->idx = std::max(0, top->idx - 1);
        if (keysDown() & KEY_DOWN) {
            top->idx = std::min(static_cast<int>(top->nodes.size() - 1), top->idx + 1);
        }

        if (keysDown() & KEY_A) {
            if (auto submenu = dynamic_cast<SubmenuDebugNode*>(top->nodes[top->idx]); submenu != nullptr) {
                node_stack.push_back({submenu->nodes, 0, submenu->name});
                top = &node_stack.back();
            } else {
                selected = true;
            }
        }
    }

    // Printing
    printf("### %s\n", top->menu_name);
    for (int i = 0; i < top->nodes.size(); ++i) {
        if (i == top->idx) {
            if (selected) {
                printf("!>");
            } else {
                printf("> ");
            }
        }
        else
            printf("  ");

        top->nodes[i]->update(selected);
    }
}

SubmenuDebugNode::SubmenuDebugNode(const char *_name, std::vector<DebugNode *> &_nodes): name(_name), nodes(_nodes) {}

void SubmenuDebugNode::update(bool &selected) {
    printf("%s >>\n", name);
}

void MarkerDebugNode::update(bool &selected) {
    printf("text\n");
}

void FunctionResultNode::update(bool &selected) {
    printf("%s\n", func().c_str());
}

void check_debug_menu() {
    if (keysDown() & KEY_SELECT) {
        consoleDebugInit(DebugDevice_NOCASH);
        consoleClear();

        std::vector<DebugState> node_stack {{main_nodes, 0, "Debug Menu"}};
        bool selected = false;

        debug_print(node_stack, selected);

        while (true) {
            scanKeys();
            debug_print(node_stack, selected);

            if (node_stack.size() == 0)
                break;

            swiWaitForVBlank();
        }

        consoleClear();
        consoleDebugInit(DebugDevice_NULL);
    }
};
