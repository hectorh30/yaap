#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <iostream>
#include <sstream>
#include <dpmi.h>

#include "canvas.h"
#include "mouse.h"
#include "polygon.h"

#define CMD_EXIT -1

#define CMD_NONE 0
#define CMD_PENCIL 1
#define CMD_TEXT 2
#define CMD_LINE 3
#define CMD_CIRCLE 4
#define CMD_RECTANGLE 5
#define CMD_ELLIPSE 6
#define CMD_PICK_COLOR 7
#define CMD_BUCKET_FILL 8
#define CMD_POLYGON 9
#define CMD_SPRAY 10
#define CMD_NEW 11
#define CMD_ERASER 12
#define CMD_POLYGON_FILL 13
#define CMD_POLYGON_MOVE 14
#define CMD_POLYGON_PICK_HOUSE 15
#define CMD_POLYGON_PICK_TREE 16
#define CMD_POLYGON_SCALE 17
#define CMD_POLYGON_SHEAR_X 18
#define CMD_POLYGON_SHEAR_Y 19
#define CMD_POLYGON_ROTATE 20
#define CMD_POLYGON_TOGGLE_FILL 21
#define CMD_COPY 22
#define CMD_CUT 23
#define CMD_PASTE 24
#define CMD_SELECT 25
#define CMD_MARKER 26

class Manager {
    
    // TODO: add defensive programming in between commands (something as a flag of 'drawing in progress')
    
    private:
        // Fields
        Canvas * canvas;
        Mouse * mouse;
        Polygon * polygon;
        
        Polygon tree;
        Polygon house; 
        Polygon * selectedPolygon, * notSelectedPolygon;
        
        int current_cmd;
        int vertex_x, vertex_y;
        bool isFirstMovement;
        bool isPolygonStarted;
        bool fill_polygon;
        int temp_int;
        int polygon_init_x, polygon_init_y;
        // TODO: move this to the mouse class
        bool is_left_down, is_right_down;
        
        // Methods
        void initialize();
        void updateCommandText(int cmd);
        void executeCMD(int cmd);
        bool isVertexSet();
        void setVertex(int x, int y);
        void unsetVertex();
        void resetFirstMovement();
        void preCMDChange(int current_cmd, int new_cmd);
        void executeOutOfCanvasCMD(int current_cmd);

        //~ bool isPolygonStarted();
        void resetPolygonInitialPoint();
        void setPolygonInitialPoint(int x, int y);

    public:
        // Fields
        
        // Methods
        Manager();
        void setSVGA();
        void unsetSVGA();
        bool isKeyAvailable();
        int getKeyboardFlags();
        int getCMD();
        char getKeyPressed();
        int processKey(int echo);
        void launch();
        void setTextWritingMode(int init_x, int init_y);
};

Manager::Manager() {

    current_cmd = CMD_NONE;

    vertex_x = -1;
    vertex_y = -1;

    fill_polygon = false;
    temp_int = -1;

    polygon_init_x = polygon_init_y = -1;

    is_left_down = is_right_down = false;

    isPolygonStarted = false;

    polygon = new Polygon();

    isFirstMovement = true;

    canvas = new Canvas();

    mouse = new Mouse(canvas);

}

void Manager::initialize() {

    setSVGA();

    mouse->initialize();

    canvas->drawSeparatorLine();
    canvas->drawColorPalette(660,510);
    canvas->drawCurrentColor(660,535);
    canvas->drawWidthPalette(560,510);

    updateCommandText(current_cmd);
}

void Manager::launch() {

    initialize();

    // Main loop
    int new_cmd;

    int count = 0;
    int show = 0;
    std::ostringstream convert;

    while (current_cmd != CMD_EXIT) 
    {
        // Keyboard updating
        new_cmd = getCMD();

        if (current_cmd != new_cmd) {
            preCMDChange(current_cmd, new_cmd);
            current_cmd = new_cmd;
        }

        // Mouse updating
        mouse->updateStatus();

        count = count % 600;
        if (count == 0)
        {
            show++;
            show % 200;
            convert << show;
            canvas->setHelpText(convert.str().c_str());
        }
        count++;

        // Rendering
        if (mouse->isInsideCanvas()) {
            executeCMD(current_cmd);
        } else {
            executeOutOfCanvasCMD(current_cmd);
        }

        // Resetting
        mouse->resetStatus();
        canvas->setMouseCoordinatesText(mouse->getMainX(),mouse->getMainY());
    }

    updateCommandText(current_cmd);
    unsetSVGA();
}

void Manager::executeCMD(int cmd) {

    switch(cmd) {
        case CMD_NONE:
            break;

        case CMD_SELECT:
            // If left click is hold
            if (mouse->getLeftHold()) {
                // If theres already a vertex set
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp rectangle
                        if (isFirstMovement) {
                            // if (canvas->rectangle_selected)
                            canvas->removeSelectionRectangle();
                        } else {
                            canvas->drawNegativeRectangle(vertex_x,vertex_y, mouse->getOldX() - 1,mouse->getOldY() - 1);
                        }

                        canvas->drawNegativeRectangle(vertex_x,vertex_y, mouse->getMainX() - 1,mouse->getMainY() - 1);
                        isFirstMovement = false;
                    }
                } 
                // If not, set an initial vertex
                else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                }
            } else {
                if (isVertexSet()) {

                    // Last temp rectangle is the selection rectangle
                    canvas->setSelectionRectangle(vertex_x, vertex_y, mouse->getOldX() - 1, mouse->getOldY() - 1);

                    // Reset stuff as action is finished
                    unsetVertex();
                    resetFirstMovement();
                }
            }
            break;

        case CMD_COPY:
            // canvas->copySelectionRectangle();
            break;

        case CMD_CUT:
            canvas->cutSelectionRectangle();
            break;

        case CMD_PASTE:
            if (mouse->getLeftHold()) {
                canvas->pasteSelectionRectangleFromVertex(mouse->getMainX() - 1, mouse->getMainY() - 1);
                canvas->removeSelectionRectangle();
            }
            break;

        case CMD_PENCIL:
            if (mouse->getLeftHold()) {
                if (isVertexSet())
                    canvas->drawLine(vertex_x,vertex_y,mouse->getMainX()-1,mouse->getMainY()-1);  
                    
                setVertex(mouse->getMainX() - 1, mouse->getMainY() - 1);
            } else {
                if (isVertexSet())
                    unsetVertex();
            }
            break;

        case CMD_LINE:
            if (mouse->getLeftHold()) {
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp line
                        if (!isFirstMovement)
                            canvas->drawNegativeLine(vertex_x,vertex_y,mouse->getOldX()-1,mouse->getOldY()-1);
                        
                        canvas->drawNegativeLine(vertex_x,vertex_y,mouse->getMainX()-1,mouse->getMainY()-1);
                        isFirstMovement = false;
                    }
                } else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                }
            } else {
                if (isVertexSet()) {
                    // Remove last temp line
                    canvas->drawNegativeLine(vertex_x,vertex_y,mouse->getOldX()-1,mouse->getOldY()-1);
                    
                    // Draw final line
                    canvas->drawLine(vertex_x,vertex_y,mouse->getMainX()-1,mouse->getMainY()-1);
                    
                    // Reset stuff as action is finished
                    resetFirstMovement();
                    unsetVertex();
                }
            }
            break;

        case CMD_CIRCLE:
            // TODO: remove weird circle at center of the circle
            if (mouse->getLeftHold()) {
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Remove previous temp circle
                        if (!isFirstMovement) {
                            if (abs(vertex_x - mouse->getOldX() - 1) > abs(vertex_y - mouse->getOldY() - 1))
                                canvas->drawNegativeCircle(vertex_x,vertex_y,abs(vertex_x - mouse->getOldX() - 1));
                            else
                                canvas->drawNegativeCircle(vertex_x,vertex_y,abs(vertex_y - mouse->getOldY() - 1));
                        }
                        
                        // Draw new temp circle
                        if (abs(vertex_x - mouse->getMainX() - 1) > abs(vertex_y - mouse->getMainY() - 1))
                            canvas->drawNegativeCircle(vertex_x,vertex_y, abs(vertex_x - mouse->getMainX() - 1));
                        else
                            canvas->drawNegativeCircle(vertex_x,vertex_y, abs(vertex_y - mouse->getMainY() - 1));
                            
                        isFirstMovement = false;
                    }
                } else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                }
            } else {
                if (isVertexSet()) {
                    
                    // Remove last temp circle
                    if (abs(vertex_x - mouse->getOldX() - 1) > abs(vertex_y - mouse->getOldY() - 1))
                        canvas->drawNegativeCircle(vertex_x,vertex_y,abs(vertex_x - mouse->getOldX() - 1));
                    else
                        canvas->drawNegativeCircle(vertex_x,vertex_y,abs(vertex_y - mouse->getOldY() - 1));
                    
                    // Draw final circle
                    if (abs(vertex_x - mouse->getMainX() - 1) > abs(vertex_y - mouse->getMainY() - 1))
                        canvas->drawCircle(vertex_x,vertex_y, abs(vertex_x - mouse->getMainX() - 1));
                    else
                        canvas->drawCircle(vertex_x,vertex_y, abs(vertex_y - mouse->getMainY() - 1));
                    
                    // Reset stuff as action is finished
                    resetFirstMovement();
                    unsetVertex();
                }
            }
            break;

        case CMD_ELLIPSE:
            if (mouse->getLeftHold()) {
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp ellipse
                        if (!isFirstMovement)
                            canvas->drawNegativeEllipse(vertex_x,vertex_y, mouse->getOldX() - 1,mouse->getOldY() - 1);
                            
                        canvas->drawNegativeEllipse(vertex_x,vertex_y, mouse->getMainX() - 1,mouse->getMainY() - 1);
                        isFirstMovement = false;
                    }
                } else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                }
            } else {
                if (isVertexSet()) {
                    // Remove last temp ellipse
                    canvas->drawNegativeEllipse(vertex_x,vertex_y,mouse->getOldX()-1,mouse->getOldY()-1);
                    
                    // Draw final ellipse
                    canvas->drawEllipse(vertex_x,vertex_y, mouse->getMainX() - 1,mouse->getMainY() - 1);
                    
                    // Reset stuff as action is finished
                    unsetVertex();
                    resetFirstMovement();
                }
            }
            break;

        case CMD_BUCKET_FILL:
            // TODO: fix bucket fill!!
            if (mouse->getLeftHold()) {
                setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
            } else {
                if (isVertexSet()) {
                    
                    mouse->erasePointer();
                    canvas->floodFillScanline(mouse->getMainX()-1,mouse->getMainY()-1);
                    mouse->drawPointer();
                    
                    unsetVertex();
                }
            }
            break;

        case CMD_POLYGON_TOGGLE_FILL:
            fill_polygon = !fill_polygon;
            if (fill_polygon)
                canvas->setCommandText("TEST: Fill polygon: true");
            else 
                canvas->setCommandText("TEST: Fill polygon: false");
            current_cmd = CMD_NONE;
            break;

        case CMD_POLYGON:
            if (mouse->getLeftHold()) { // Left click has precedence over right click
                if (!is_left_down) { // Left click
                    if (!isPolygonStarted) {
                        setPolygonInitialPoint(mouse->getMainX()-1,mouse->getMainY()-1);
                        setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                        
                        isPolygonStarted = true;
                        
                        polygon = new Polygon();
                        polygon->addPoint(mouse->getMainX()-1,mouse->getMainY()-1);
                        
                    } else {
                        
                        // Remove last temp line
                        canvas->drawNegativeLine(vertex_x,vertex_y,mouse->getOldX()-1,mouse->getOldY()-1);    
                        
                        // Draw polygon line
                        canvas->drawLine(vertex_x,vertex_y,mouse->getMainX()-1,mouse->getMainY()-1);
                        
                        // Add polygon point
                        polygon->addPoint(mouse->getMainX()-1,mouse->getMainY()-1);
                        setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    }
                    
                    resetFirstMovement();
                    is_left_down = true;
                }
            }  else if (mouse->getRightHold()) {
                if (!is_right_down) { // Right click
                    // Remove last temp line
                    canvas->drawNegativeLine(vertex_x,vertex_y,mouse->getOldX()-1,mouse->getOldY()-1);
                    
                    // Draw final line
                    canvas->drawLine(vertex_x,vertex_y,polygon_init_x,polygon_init_y);
                    
                    // Add polygon point
                    polygon->addPoint(mouse->getMainX()-1,mouse->getMainY()-1);
                    
                    mouse->erasePointer();
                    
                    if (fill_polygon)
                        canvas->drawFilledPolygon(*polygon);
                    else 
                        canvas->drawPolygon(*polygon);
                        
                    mouse->drawPointer();
                    
                    // Reset stuff
                    isPolygonStarted = false;
                    resetFirstMovement();
                    unsetVertex();
                    is_right_down = true;   
                }
            } else {
                if (is_left_down) { // Left release
                    is_left_down = false;
                } else if (is_right_down) { // Right release
                    is_right_down = false;
                } else { // Only moving
                    if (isPolygonStarted) {
                        if (mouse->isMoved()) {
                            if (!isFirstMovement) {
                                canvas->drawNegativeLine(vertex_x,vertex_y,mouse->getOldX()-1,mouse->getOldY()-1);    
                            }
                                
                            canvas->drawNegativeLine(vertex_x,vertex_y,mouse->getMainX()-1,mouse->getMainY()-1);
                        }
                        
                        isFirstMovement = false;
                    }
                }
            }
            break;

        case CMD_SPRAY:
            if (mouse->getLeftHold())
                canvas->spray(mouse->getMainX() - 1, mouse->getMainY() - 1);
            break;

        case CMD_NEW:
            
            mouse->erasePointer();
            canvas->clear();
            mouse->drawPointer();

            current_cmd = CMD_NONE;
            canvas->setCommandText("None");
            break;

        case CMD_PICK_COLOR:
            if (mouse->getLeftHold()) {
                if (!is_left_down) {
                    // Left click
                    is_left_down = true;
                }
            } else {
                if (is_left_down) { 
                    // Left release
                    canvas->setCurrentColor(canvas->getPixel(mouse->getMainX() - 1, mouse->getMainY() - 1));
                    mouse->erasePointer();
                    canvas->drawCurrentColor(660,535);
                    mouse->drawPointer();
                    is_left_down = false;
                }
            }
            break;

        case CMD_ERASER:
            // TODO: check bug when painting diagonal lines
            if (mouse->getLeftHold()) {

                // TODO: remove hardcoded line width
                if (isVertexSet())
                    canvas->drawWidthLine(vertex_x, vertex_y, mouse->getMainX()-1, mouse->getMainY()-1, canvas->getBackgroundColor(), 25);

                setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
            } else {
                if (isVertexSet())
                    unsetVertex();
            }
            break;

        case CMD_POLYGON_FILL:
            canvas->erasePolygon(*selectedPolygon);
            canvas->drawFilledPolygon(*selectedPolygon);
            current_cmd = CMD_NONE;
            break;

        case CMD_MARKER:
            // TODO: check bug when painting diagonal lines
            if (mouse->getLeftHold()) {

                // TODO: remove hardcoded line width
                if (isVertexSet())
                    canvas->drawWidthLine(vertex_x, vertex_y, mouse->getMainX()-1, mouse->getMainY()-1, 25);

                setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
            } else {
                if (isVertexSet())
                    unsetVertex();
            }
            break;

        case CMD_RECTANGLE:
            // If left click is hold
            if (mouse->getLeftHold()) {
                // If theres already a vertex set
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp rectangle
                        if (!isFirstMovement) {
                            canvas->drawNegativeRectangle(vertex_x,vertex_y, mouse->getOldX() - 1,mouse->getOldY() - 1);
                        }
                            
                        canvas->drawNegativeRectangle(vertex_x,vertex_y, mouse->getMainX() - 1,mouse->getMainY() - 1);
                        isFirstMovement = false;
                    }
                } 
                // If not, set an initial vertex
                else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                }
            } else {
                if (isVertexSet()) {
                    // Remove last temp rectangle
                    canvas->drawNegativeRectangle(vertex_x,vertex_y,mouse->getOldX()-1,mouse->getOldY()-1);
                    
                    // Draw final rectangle
                    canvas->drawRectangle(vertex_x,vertex_y, mouse->getMainX() - 1,mouse->getMainY() - 1);
                    
                    // Reset stuff as action is finished
                    unsetVertex();
                    resetFirstMovement();
                }
            }
            break;

        case CMD_TEXT:
            if (mouse->getLeftHold()) {
                if (!is_left_down) {
                    // Left click
                    is_left_down = true;
                }
            } else {
                if (is_left_down) {
                    // Left release
                    mouse->erasePointer();
                    setTextWritingMode(mouse->getMainX() - 1, mouse->getMainY() - 1);
                    mouse->drawPointer();
                    
                    is_left_down = false;
                }
            }
            break;

        case CMD_POLYGON_MOVE:
            if (mouse->getLeftHold())
            {
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp polygon
                        if (!isFirstMovement) {
                            canvas->drawNegativePolygon(*selectedPolygon);
                        }
                        
                        selectedPolygon->move(mouse->getMainX() - 1 - vertex_x,mouse->getMainY() - 1 - vertex_y);
                        
                        canvas->drawNegativePolygon(*selectedPolygon);
                        isFirstMovement = false;
                        
                        setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    }
                } else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    canvas->erasePolygon(*selectedPolygon);
                }
            } else {
                if (isVertexSet()) {
                    // Remove last temp polygon
                    canvas->drawNegativePolygon(*selectedPolygon);
                    
                    // Draw final polygon
                    selectedPolygon->move(mouse->getMainX() - 1 - vertex_x,mouse->getMainY() - 1 - vertex_y);
                    
                    mouse->erasePointer();
                    if (fill_polygon)
                        canvas->drawFilledPolygon(*selectedPolygon);
                    else 
                        canvas->drawPolygon(*selectedPolygon);
                    mouse->drawPointer();
                    
                    // Reset stuff as action is finished
                    resetFirstMovement();
                    unsetVertex();
                }
            }
            break;

        case CMD_POLYGON_SCALE:
            if (mouse->getLeftHold())
            {
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp polygon
                        if (!isFirstMovement) {
                            canvas->drawNegativePolygon(*selectedPolygon);
                        }
                        
                        selectedPolygon->scale(mouse->getMainX() - 1 - vertex_x,mouse->getMainY() - 1 - vertex_y);
                        
                        canvas->drawNegativePolygon(*selectedPolygon);
                        isFirstMovement = false;
                        
                        setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    }
                } else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    canvas->erasePolygon(*selectedPolygon);
                }
            } else {
                if (isVertexSet()) {
                    // Remove last temp polygon
                    canvas->drawNegativePolygon(*selectedPolygon);
                    
                    // Draw final polygon
                    selectedPolygon->scale(mouse->getMainX() - 1 - vertex_x,mouse->getMainY() - 1 - vertex_y);
                    
                    mouse->erasePointer();
                    if (fill_polygon)
                        canvas->drawFilledPolygon(*selectedPolygon);
                    else 
                        canvas->drawPolygon(*selectedPolygon);
                    mouse->drawPointer();
                    
                    // Reset stuff as action is finished
                    resetFirstMovement();
                    unsetVertex();
                }
            }
            break;

        case CMD_POLYGON_SHEAR_X:
            if (mouse->getLeftHold()) {
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp polygon
                        if (!isFirstMovement) {
                            canvas->drawNegativePolygon(*selectedPolygon);
                        }
                        
                        selectedPolygon->shearX(mouse->getMainX() - 1 - vertex_x);
                        
                        canvas->drawNegativePolygon(*selectedPolygon);
                        isFirstMovement = false;
                        
                        setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    }
                } else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    canvas->erasePolygon(*selectedPolygon);
                }
            } else {
                if (isVertexSet()) {
                    // Remove last temp polygon
                    canvas->drawNegativePolygon(*selectedPolygon);
                    
                    // Draw final polygon
                    selectedPolygon->shearX(mouse->getMainX() - 1 - vertex_x);
                    
                    mouse->erasePointer();
                    if (fill_polygon)
                        canvas->drawFilledPolygon(*selectedPolygon);
                    else 
                        canvas->drawPolygon(*selectedPolygon);
                    mouse->drawPointer();
                    
                    // Reset stuff as action is finished
                    resetFirstMovement();
                    unsetVertex();
                }
            }
            break;

        case CMD_POLYGON_SHEAR_Y:
            if (mouse->getLeftHold()) {
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp polygon
                        if (!isFirstMovement) {
                            canvas->drawNegativePolygon(*selectedPolygon);
                        }
                        
                        selectedPolygon->shearY(mouse->getMainY() - 1 - vertex_y);
                        
                        canvas->drawNegativePolygon(*selectedPolygon);
                        isFirstMovement = false;
                        
                        setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    }
                } else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    canvas->erasePolygon(*selectedPolygon);
                }
            } else {
                if (isVertexSet()) {
                    // Remove last temp polygon
                    canvas->drawNegativePolygon(*selectedPolygon);
                    
                    // Draw final polygon
                    selectedPolygon->shearY(mouse->getMainY() - 1 - vertex_y);
                    
                    mouse->erasePointer();
                    if (fill_polygon)
                        canvas->drawFilledPolygon(*selectedPolygon);
                    else 
                        canvas->drawPolygon(*selectedPolygon);
                    mouse->drawPointer();
                    
                    // Reset stuff as action is finished
                    resetFirstMovement();
                    unsetVertex();
                }
            }
            break;

        case CMD_POLYGON_ROTATE:
            if (mouse->getLeftHold()) {
                if (isVertexSet()) {
                    if (mouse->isMoved()) {
                        // Redraw temp polygon
                        if (!isFirstMovement) {
                            canvas->drawNegativePolygon(*selectedPolygon);
                        }
                        
                        int d_x = mouse->getMainX() - 1 - vertex_x;
                        
                        if (mouse->getMainY() - 1 > selectedPolygon->getMidY())
                            selectedPolygon->rotate(-d_x);
                        else
                            selectedPolygon->rotate(d_x);
                            
                        //~ selectedPolygon->rotate(mouse->getMainX() - 1 - vertex_x,mouse->getMainY() - 1 - vertex_y);
                        
                        canvas->drawNegativePolygon(*selectedPolygon);
                        isFirstMovement = false;
                        
                        setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    }
                } else {
                    setVertex(mouse->getMainX()-1,mouse->getMainY()-1);
                    canvas->erasePolygon(*selectedPolygon);
                }
            } else {
                if (isVertexSet()) {
                    // Remove last temp polygon
                    canvas->drawNegativePolygon(*selectedPolygon);
                    
                    // Draw final polygon
                    selectedPolygon->rotate(mouse->getMainX() - 1 - vertex_x);
                    
                    mouse->erasePointer();
                    if (fill_polygon)
                        canvas->drawFilledPolygon(*selectedPolygon);
                    else 
                        canvas->drawPolygon(*selectedPolygon);
                    mouse->drawPointer();
                    
                    // Reset stuff as action is finished
                    resetFirstMovement();
                    unsetVertex();
                }
            }
            break;

        case CMD_POLYGON_PICK_HOUSE:

            selectedPolygon = &house;
            notSelectedPolygon = &tree;

            current_cmd = CMD_NONE;
            break;

        case CMD_POLYGON_PICK_TREE:

            selectedPolygon = &tree;
            notSelectedPolygon = &house;
            current_cmd = CMD_NONE;
            break;
    }
}

void Manager::setSVGA() {
    /**
     * Int 10/AX=4F02h: Set video mode VESA SVGA 800x600x256
     * 
     * http://www.ctyme.com/intr/rb-0275.htm
     */
    __asm__ (
        "movl $0x4F02, %eax\n\t"
        "movl $0x103, %ebx\n\t"
        "int $0x10"
    );
}

void Manager::unsetSVGA() {
    __asm__ (
        "movl $0x03, %eax\n\t"
        "int $0x10"
    );
}

int Manager::getCMD() {

    char cmd;

    int returnValue = current_cmd;

    bool keyAvailable = isKeyAvailable();
    if (keyAvailable)
    {
        int flags = getKeyboardFlags();
        switch(flags) {
            case 2:                         // Only left shift key pressed
                cmd = getKeyPressed();

                switch(cmd) {
                    case 'A':
                        returnValue = CMD_POLYGON_MOVE;
                        break;
                    case 'B':
                        returnValue = CMD_BUCKET_FILL;
                        break;
                    case 'C':
                        returnValue = CMD_CIRCLE;
                        break;
                    case 'D':
                        returnValue = CMD_POLYGON_PICK_TREE;
                        break;
                    case 'E':
                        returnValue = CMD_ELLIPSE;
                        break;
                    case 'F':
                        returnValue = CMD_POLYGON_SCALE;
                        break;
                    case 'G':
                        returnValue = CMD_RECTANGLE;
                        break;
                    case 'H':
                        returnValue = CMD_POLYGON_PICK_HOUSE;
                        break;
                    case 'I':
                        returnValue = CMD_POLYGON_SHEAR_X;
                        break;
                    case 'J':
                        returnValue = CMD_POLYGON_SHEAR_Y;
                        break;
                    case 'K':
                        returnValue = CMD_POLYGON_ROTATE;
                        break;
                    case 'L':
                        returnValue = CMD_LINE;
                        break;
                    case 'M':
                        returnValue = CMD_POLYGON_FILL;
                        break;
                    case 'N':
                        returnValue = CMD_NEW;
                        break;
                    case 'O':
                        returnValue = CMD_POLYGON;
                        break;
                    case 'P':
                        returnValue = CMD_PENCIL;
                        break;
                    case 'Q':
                        returnValue = CMD_EXIT;
                        break;
                    case 'R':
                        returnValue = CMD_ERASER;
                        break;
                    case 'S':
                        returnValue = CMD_SPRAY;
                        break;
                    case 'T':
                        returnValue = CMD_TEXT;
                        break;
                    case 'U':
                        returnValue = CMD_POLYGON_TOGGLE_FILL;
                        break;
                    case 'V':
                        returnValue = CMD_MARKER;
                        break;
                    case 'W':
                        returnValue = CMD_PICK_COLOR;
                        break;
                    case 'Z':
                        returnValue = CMD_SELECT;
                        break;
                    default:
                        break;
                }
                break;

            case 8:                         // Only left alt key pressed
                cmd = getKeyPressed();
                switch (cmd) {
                    case 'X':
                        returnValue = CMD_CUT;
                        break;
                    case 'C':
                        returnValue = CMD_COPY;
                        break;
                    case 'V':
                        returnValue = CMD_PASTE;
                        break;
                    default:
                        break;
                }
                break;

            default:
                getKeyPressed();
                break;
        }
    }

    return returnValue;
}

char Manager::getKeyPressed() {
    /**
     * Int 16/AH=00h: KEYBOARD - GET KEYSTROKE
     * 
     * http://www.ctyme.com/intr/rb-1754.htm
     */

    char key;
    
    __asm__ __volatile__ (
        // Test if key available
        "xorl %%eax,%%eax   \n\t"   // set EAX = 0
        "int $0x16          \n\t"   // Int 16H: AH = BIOS scan code, AL = ASCII character
        "xorb %%ah,%%ah    \n\t"    // set AL = 0
        : "=a" (key)                // Output
        :
        :
    );
    return key;
}

bool Manager::isKeyAvailable() {
    /**
     * Int 16/AH=01h: KEYBOARD - CHECK FOR KEYSTROKE
     * 
     * http://www.ctyme.com/intr/rb-1755.htm
     */

    int keystrokeAvailable = 0;
    
    __asm__ __volatile__ (
        // Test if key available
        "xorl %%ecx,%%ecx   \n\t" // set CX = 0
        "xorl %%eax,%%eax   \n\t" // set AX = 0
        "movb $0x1, %%ah    \n\t" // set AH = 1H
        "int $0x16          \n\t" // Int 16H: ZF clear if keystroke available (set otherwise), AH = BIOS scan code, AL = ASCII character
        "jz exit           \n\t" // If ZF == 1 (keystroke not available), exit with CX = 0
        "movl $0x1, %%ecx   \n\t" // If ZF is set, ECX = -1;
        "exit:              \t"
        : "=c" (keystrokeAvailable)
        :
        :
    );
    
    if (keystrokeAvailable != 0)
    {
        return true;
    } else {
        return false;
    }
}

int Manager::getKeyboardFlags() {
    /**
     * Int 16/AH=02h: KEYBOARD - GET SHIFT FLAGS
     * 
     * http://www.ctyme.com/intr/rb-1756.htm
     */

    int keyboardFlags = 0;
    
    __asm__ __volatile__ (
        // Test if key available
        "xorl %%eax,%%eax   \n\t" // set AX = 0
        "movb $0x2, %%ah    \n\t" // set AH = 2H
        "int $0x16          \n\t" // Int 16H: AL = shift flags (see #00582)
        "xorb %%ah,%%ah     \n\t" // set AH = 0
        : "=a" (keyboardFlags)
        :
        :
    );

    
    // std::ostringstream convert;

    // convert << keyboardFlags;

    // std::string str = convert.str().c_str();

    // canvas->setCommandText(convert.str().c_str());

    // if (keyboardFlags == 1 || keyboardFlags == 2 || keyboardFlags == 4)
    // {
    //     return true;
    // } else {
    //     return false;
    // }

    return keyboardFlags;
}

int Manager::processKey(int echo) {
    int keyPressed = 0;
    int shiftKeyPressed = 0;
    int cmd = CMD_NONE;
    
    asm(
        // Get shift key into ecx
        "xorl %%ecx,%%ecx   \n\t"
        "movb $0x2, %%ah    \n\t"
        "int $0x16          \n\t" // AL = shift flags
        "movb %%al, %%cl    \n\t"
        
        "xorb %%ah,%%ah     \n\t"
        "int $0x16          \n\t"
        "xorb %%ah,%%ah     \n\t" // AH = BIOS scan code, AL = ASCII character
        : "=c" (shiftKeyPressed), "=a" (keyPressed)
        :
        :
    );
    
    return cmd;
}

void Manager::updateCommandText(int cmd) {
    switch(cmd){
        case CMD_EXIT:
            canvas->setCommandText("Exit");
            break;
        case CMD_MARKER:
            canvas->setCommandText("Marker");
            break;
        case CMD_PENCIL:
            canvas->setCommandText("Pencil");
            break;
        case CMD_TEXT:
            canvas->setCommandText("Text");
            break;
        case CMD_LINE:
            canvas->setCommandText("Line");
            break;
        case CMD_CIRCLE:
            canvas->setCommandText("Circle");
            break;
        case CMD_ELLIPSE:
            canvas->setCommandText("Ellipse");
            break;
        case CMD_BUCKET_FILL:
            canvas->setCommandText("Bucket fill");
            break;
        case CMD_SPRAY:
            canvas->setCommandText("Spray");
            break;
        case CMD_NEW:
            canvas->setCommandText("New");
            break;
        case CMD_NONE:
            canvas->setCommandText("None");
            break;
        case CMD_ERASER:
            canvas->setCommandText("Eraser");
            break;
        case CMD_POLYGON_FILL:
            canvas->setCommandText("Fill polygon");
            break;
        case CMD_RECTANGLE:
            canvas->setCommandText("Rectangle");
            break;
        case CMD_POLYGON:
            canvas->setCommandText("Polygon");
            break;
        case CMD_PICK_COLOR:
            canvas->setCommandText("Color picker");
            break;
        case CMD_POLYGON_MOVE:
            canvas->setCommandText("Polygon move");
            break;
        case CMD_POLYGON_SCALE:
            canvas->setCommandText("Polygon scale");
            break;
        case CMD_POLYGON_SHEAR_X:
            canvas->setCommandText("Polygon shear in x axis");
            break;
        case CMD_POLYGON_SHEAR_Y:
            canvas->setCommandText("Polygon shear in y axis");
            break;
        case CMD_POLYGON_ROTATE:
            canvas->setCommandText("Polygon rotate");
            break;
        case CMD_CUT:
            canvas->setCommandText("Cut");
            break;
        case CMD_COPY:
            canvas->setCommandText("Copy");
            break;
        case CMD_PASTE:
            canvas->setCommandText("Paste");
            break;
        case CMD_SELECT:
            canvas->setCommandText("Select");
            break;
    }
}

bool Manager::isVertexSet() {
    return (vertex_x != -1 || vertex_y != -1);
}

void Manager::setVertex(int x, int y) {
    vertex_x = x;
    vertex_y = y;
}

void Manager::unsetVertex() {
    vertex_x = -1;
    vertex_y = -1;
}

void Manager::resetPolygonInitialPoint() {
    polygon_init_x = polygon_init_y = -1;
}

void Manager::setPolygonInitialPoint(int x, int y) {
    polygon_init_x = x;
    polygon_init_y = y;
}

void Manager::resetFirstMovement() {
    isFirstMovement = true;
}

void Manager::setTextWritingMode(int init_x, int init_y) {
    
    char current_char;
    int char_count = 0, x, y = init_y;
    bool exitTextWritingMode = false;
    
    while (!exitTextWritingMode) 
    {
        if (isKeyAvailable())
        {
            current_char = getKeyPressed();
            
            if (getKeyboardFlags() == 1)
            {
                exitTextWritingMode = true;
                
            } else {
                
                // TODO: definitely fix this
                x = init_x + (20 * char_count) ;
                if ((x+20) > canvas->getWidth()) {
                    char_count = 0;
                    y = y + 20;
                }
                
                x = x % canvas->getWidth();
                    
                canvas->setTextCursor(x, y);
                canvas->putChar(current_char,canvas->getCurrentColor());
                char_count = char_count+1;
            }
        }
    }
}

void Manager::executeOutOfCanvasCMD(int current_cmd)
{
    if (current_cmd == CMD_PICK_COLOR || 
        current_cmd == CMD_MARKER ||
        current_cmd == CMD_PENCIL)
    {
        executeCMD(current_cmd);
    }
}

void Manager::preCMDChange(int current_cmd, int new_cmd) {
    if (current_cmd == CMD_SELECT) {
        if (new_cmd != CMD_COPY && new_cmd != CMD_PASTE) {
            canvas->removeSelectionRectangle();
        }
    }

    // Update command texts
    updateCommandText(new_cmd);
}

#endif
