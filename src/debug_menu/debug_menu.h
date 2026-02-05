//
// Created by willb on 1/15/2026.
//

#ifndef FACTANIA_DEBUG_MENU_H
#define FACTANIA_DEBUG_MENU_H
#include <vector>
#include <functional>
#include <string>

#define MENU_WIDTH 12

class DebugNode {
public:
    virtual ~DebugNode() = default;
    virtual void update(bool& selected) = 0;
    virtual bool has_children() const { return false; }

    virtual std::vector<DebugNode*>& children() {
        static std::vector<DebugNode*> empty;
        return empty;
    }

    virtual const char* child_menu_name() const { return ""; }

    virtual void on_enter() {}
};

class SubmenuDebugNode : public DebugNode {
public:
    SubmenuDebugNode(const char* _name, std::vector<DebugNode*>& _nodes);
    const char* name;
    std::vector<DebugNode*>& nodes;
    void update(bool& selected) override;

    bool has_children() const override { return true; }
    std::vector<DebugNode*>& children() override { return nodes; }
    const char* child_menu_name() const override { return name; }
};

class MarkerDebugNode : public DebugNode {
public:
    MarkerDebugNode(const char* _text) : text(_text) {};
    const char* text;
    void update(bool& selected) override;
};

class FunctionResultNode : public DebugNode {
public:
    FunctionResultNode(
        const std::function<std::string()>& _func,
        std::function<void(bool& selected)> _update = [](bool& selected){}) : func(_func), selectedUpdate(_update) {};
    std::function<std::string()> func;
    std::function<void(bool& selected)> selectedUpdate;
    void update(bool& selected) override;
};

class CallbackDebugNode : public DebugNode {
public:
    CallbackDebugNode(const char* _name, std::function<std::string()> _callback);
    void update(bool& selected) override;

    bool has_children() const override { return true; }
    std::vector<DebugNode*>& children() override { return result_page; }
    const char* child_menu_name() const override { return name; }

    void on_enter() override;
private:
    const char* name;
    std::function<std::string()> callback;

    std::string result;
    std::vector<DebugNode*> result_page;
};

void check_debug_menu();
void add_debug_node_to_root(DebugNode* node);

#endif //FACTANIA_DEBUG_MENU_H