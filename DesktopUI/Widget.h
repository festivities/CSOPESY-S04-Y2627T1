/*
    CSOPESY S04 - Y2627T1 - GROUP 6
    This file contains the logic for the Widget class.
	This is that base class for all apps in the desktop emulator. 
    Each app must inherit from this class and implement the draw() function.
*/

#pragma once
#include <string>
#include <imgui.h>
#include <glad/glad.h>

class Widget {
protected:
    std::string name;
    bool isOpen = false;
    GLuint icon = 0;

public:
    Widget(std::string name, GLuint icon) : name(name), icon(icon) {}
    
    // getters
    std::string getName() { 
        return this->name; 
    }

    bool getIsOpen() { 
        return this->isOpen; 
    }

    GLuint getIcon() { 
        return this->icon; 
    }

    // setters
    void toggleOpen() { 
        this->isOpen = !this->isOpen; 
    }

	// to be implemented by derived classes
    virtual void draw() = 0;
};