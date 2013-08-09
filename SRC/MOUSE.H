#ifndef MOUSE_H
#define MOUSE_H

#include <dos.h>
#include <dpmi.h>

#include "canvas.h"

class Mouse {
    private:
        // Fields
        bool insideCanvas;
        int main_x,main_y,old_x,old_y;
        Canvas * canvas;
        bool leftClick, rightClick, leftHold, rightHold;
        
        // Methods
        void setBoundaries(int x_min, int y_min, int x_max, int y_max);
        void unsetButtons();
        
    public:
        Mouse(Canvas * canvas);
        void updateStatus();
        void resetStatus();
        void setPosition(int x, int y);
        bool isInsideCanvas();
        
        void initialize();
        bool isMoved();
        int getMainX();
        int getMainY();
        int getOldX();
        int getOldY();
        bool getLeftClick();
        bool getRightClick();
        bool getLeftHold();
        bool getRightHold();
        void drawPointer();
        void erasePointer();
};

Mouse::Mouse(Canvas * _canvas) {
    canvas = _canvas;
}

void Mouse::initialize() {
    
    __asm__ __volatile__ (
        "movl $0x0, %eax     \n\t"
        "int $0x33          \n\t"
    );
    
    // TODO: remove hardcoding
    setBoundaries(0,0,800,600);
    unsetButtons();
    
    setPosition(400,300);
    drawPointer();
}

int Mouse::getMainX() {
    return main_x;
}

int Mouse::getMainY() {
    return main_y;
}

int Mouse::getOldX() {
    return old_x;
}

int Mouse::getOldY() {
    return old_y;
}

bool Mouse::getLeftClick() {
    return leftClick;
}

bool Mouse::getLeftHold() {
    return leftHold;
}

bool Mouse::getRightClick() {
    return rightClick;
}

bool Mouse::getRightHold() {
    return rightHold;
}

void Mouse::setBoundaries(int x_min, int y_min, int x_max, int y_max) {
    union REGS regs;
    
    regs.x.ax = 0x07;
    regs.x.cx = x_min;
    regs.x.dx = x_max;
    int86(0x33, &regs, &regs);
    
    regs.x.ax = 0x08;
    regs.x.cx = y_min;
    regs.x.dx = y_max;
    int86(0x33, &regs, &regs);
}

void Mouse::unsetButtons() {
    leftClick = false;
    rightClick = false;
    leftHold = false;
    rightHold = false;
}

void Mouse::updateStatus() {
    
    //~ erasePointer(main_x,main_y);
    
    int buttonsStatus;
    
    // INT 33h,  03h (3)        Get Mouse Position and Button Status
    // http://www.ousob.com/ng/progref/ng38e12.php
    __asm__ (
        "xorl %%ebx,%%ebx   \n\t"   // set EBX = 0
        "xorl %%ecx,%%ecx   \n\t"   // set ECX = 0
        "xorl %%edx,%%edx   \n\t"   // set EDX = 0
        "movl $0x3, %%eax     \n\t"
        "int $0x33"
        : "=b"(buttonsStatus), "=c" (main_x), "=d" (main_y)             // Output
        :
        :
    );
    
    if (isMoved()) 
    {
        erasePointer();
    }
        
    if (buttonsStatus == 0 && leftHold) // left click up from being down, left click
    {
        leftHold = false;
        leftClick = true;
    } 
    
    if (buttonsStatus == 1) // Left click down
    {
        leftClick = false;
        leftHold = true;
    }
    
    if (buttonsStatus == 0 && rightHold) // Right click up from being down, right click
    {
        rightHold = false;
        rightClick = true;
    }
        
    if (buttonsStatus == 2) // Right click down
    {
        rightClick = false;
        rightHold = true;
    }
}

void Mouse::resetStatus() {
    
    if (isMoved())
    {
        drawPointer();
        old_x = main_x;
        old_y = main_y;
    }
    
    unsetButtons();
}

bool Mouse::isMoved() {
    return old_x != main_x || old_y != main_y;
}

void Mouse::erasePointer() {
    // Remove old mouse pointer
    canvas->drawNegativeLine(old_x, old_y, old_x, old_y+15);
    canvas->drawNegativeLine(old_x, old_y+15, old_x+11, old_y+11);
    canvas->drawNegativeLine(old_x, old_y, old_x+11, old_y+11);
}

void Mouse::drawPointer() {    
    // Draw new mouse pointer
    canvas->drawNegativeLine(main_x, main_y, main_x, main_y+15);
    canvas->drawNegativeLine(main_x, main_y+15, main_x+11, main_y+11);
    canvas->drawNegativeLine(main_x, main_y, main_x+11, main_y+11);
}

void Mouse::setPosition(int x, int y) {
    union REGS regs;
    
    regs.x.ax = 0x04;
    regs.x.cx = x;
    regs.x.dx = y;
    int86(0x33, &regs, &regs);
    
    old_x = main_x = x;
    old_y = main_y = y;
}

bool Mouse::isInsideCanvas() {
    
    if (main_y < canvas->getHeight())
    {
        return true;
    } else {
        return false;
    }
}

#endif
