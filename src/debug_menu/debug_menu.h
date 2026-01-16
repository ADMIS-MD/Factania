//
// Created by willb on 1/15/2026.
//

#ifndef FACTANIA_DEBUG_MENU_H
#define FACTANIA_DEBUG_MENU_H
#include <vector>
#include <functional>
#include <string>

class DebugNode {
public:
    virtual void update(bool& selected) = 0;
};

class SubmenuDebugNode : public DebugNode {
public:
    SubmenuDebugNode(const char* _name, std::vector<DebugNode*>& _nodes);
    const char* name;
    std::vector<DebugNode*>& nodes;
    void update(bool& selected) override;
};

class MarkerDebugNode : public DebugNode {
public:
    MarkerDebugNode(const char* _text) : text(_text) {};
    const char* text;
    void update(bool& selected) override;
};

class FunctionResultNode : public DebugNode {
public:
    FunctionResultNode(const std::function<std::string()>& _func) : func(_func) {};
    std::function<std::string()> func;
    void update(bool& selected) override;
};

void check_debug_menu();
void add_debug_node_to_root(DebugNode* node);

#endif //FACTANIA_DEBUG_MENU_H