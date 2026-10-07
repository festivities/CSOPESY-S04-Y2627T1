#pragma once
#include "Widget.h"

class App2: public Widget {
public:
    App2(GLuint icon);
    void draw() override;
};