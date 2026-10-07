/*
    CSOPESY S04 - Y2627T1 - GROUP 6
	This file contains the logic for the UIManager class,
    which manages all widgets in the desktop emulator.
*/

#pragma once
#include <vector>
#include <memory>
#include "Widget.h"

class UIManager {
public:
    std::vector<std::shared_ptr<Widget>> widgets;

    static UIManager& getInstance();
    void addWindow(std::shared_ptr<Widget> w);
};