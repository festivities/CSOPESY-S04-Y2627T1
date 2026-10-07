#pragma once
#include "Widget.h"

class TaskManager : public Widget {
public:
    TaskManager (GLuint icon);
    void draw() override;
};