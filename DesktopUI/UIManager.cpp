#include "UIManager.h"

UIManager& UIManager::getInstance() {
    static UIManager instance;
    return instance;
}

void UIManager::addWindow(std::shared_ptr<Widget> w) {
    widgets.push_back(w);
}