// Sangbeom Kim
// 01/15/2026

#include "debug_menu.h"

class ResultLeafNode : public DebugNode {
public:
    explicit ResultLeafNode(std::string* _result)
        : result(_result) {}

    std::string* result;

    void update(bool& selected) override {
        if (result) {
            std::printf("%s\n", result->c_str());
        }
        else {
            std::printf("You should not see this\n");
        }
    }
};

CallbackDebugNode::CallbackDebugNode(const char* _name, std::function<std::string()> _action)
    : name(_name), callback(std::move(_action)), result("You should not see this") {
    result_page.push_back(new ResultLeafNode(&result));
}

void CallbackDebugNode::update(bool& selected) {
    printf("%-*.*s >>\n", MENU_WIDTH, MENU_WIDTH, name);
}

void CallbackDebugNode::on_enter() {
    if (callback) {
        result = callback();
    }
    else {
        result = "Callback not registered";
    }
}