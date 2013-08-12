#ifndef CANVAS_H
#define CANVAS_H

#include <string>
#include <iostream>
#include <sstream>
#include <conio.h>
#include <bios.h>
#include <dos.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <cmath>

#include "polygon.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

class Canvas {
    private:
        int curbank;
        void setBank(unsigned long absoluteOffset);

    public:
        //Fields
        int current_color;
        int background_color;

        // TODO: make this constants
        int CANVAS_HEIGHT;
        int CANVAS_WIDTH;

        // Methods
        Canvas();
        void setTextCursor(int x, int y);
        void putChar(char character);
        void putChar(char character, int color);
        void setCommandText(const char *_text);
        void setMouseCoordinatesText(int x, int y);
        int getHeight();
        int getWidth();
        int getBackgroundColor();
        void drawSeparatorLine();
        void clear();
        void drawPalette(int x, int y);
        void drawCurrentColor(int x, int y);
        void setCurrentColor(int color);
        int getCurrentColor();
        void showHelp();

        // Point plotting
        void putPixel(int x, int y, int color);
        void putPixel(int x, int y);
        int getPixel(int,int);

        // Advanced plotting
        void putWidthPixel(int x, int y, int color, int width);
        void drawNegativeLine(int x_0, int y_0, int x_1, int y_1);

        void drawLine(int x_0, int y_0, int x_1, int y_1, int color);
        void drawLine(int x_0, int y_0, int x_1, int y_1);
        void drawWidthLine(int x_0, int y_0, int x_1, int y_1, int color,int width);

        void drawRectangle(int x_0, int y_0, int x_1, int y_1);
        void drawRectangle(int x_0, int y_0, int x_1, int y_1, int color);
        void drawNegativeRectangle(int x_0, int y_0, int x_1, int y_1);
        void drawSelectionRectangle(int x_0, int y_0, int x_1, int y_1);

        void drawCircle(int cx, int cy, int radius);
        void drawCircle(int cx, int cy, int radius, int color);
        void drawNegativeCircle(int cx, int cy, int radius);

        void drawEllipse(int x0, int y0, int x1, int y1);
        void drawNegativeEllipse(int x0, int y0, int x1, int y1);
        void floodFillScanline(int x, int y, int newColor, int oldColor);
        void floodFillScanline(int x, int y);

        void drawPolygon(Polygon polygon);
        void drawNegativePolygon(Polygon polygon);
        void drawPolygon(Polygon polygon, int color);
        void erasePolygon(Polygon polygon);

        void drawFilledPolygon(Polygon polygon, int color);
        void drawFilledPolygon(Polygon polygon);

        void spray(int x, int y, int color, int radius, int intensity);
        void spray(int x, int y);
};

Canvas::Canvas() {
    curbank = 0;
    current_color = 15;
    background_color = 0;
    
    CANVAS_HEIGHT = 500;
    CANVAS_WIDTH = 800;

    

}

int Canvas::getHeight() {
    return CANVAS_HEIGHT;
}

int Canvas::getWidth() {
    return CANVAS_WIDTH;
}

int Canvas::getBackgroundColor() {
    return background_color;
}

void Canvas::putWidthPixel(int x, int y, int color, int width)
{
    int i,j;
    
    putPixel(x, y, color);
    
    // Paint a filled box instead of a single pixel
    for (i = 1; i < width; i++){
        for (j = 1; j < width; j++)
            putPixel(x-j, y-i, color);
    }
}

void Canvas::putPixel(int x, int y) {
    
    putPixel(x,y,current_color);
}

void Canvas::putPixel(int x, int y, int color) {
    
    if(x >= WINDOW_WIDTH || x < 0 || y >= WINDOW_HEIGHT || y < 0) {
        return;
    }

    unsigned char col = (unsigned char) color;

    unsigned long absOff = (unsigned long) (y) * WINDOW_WIDTH + x;

    setBank(absOff);

    _farpokeb(_dos_ds, 0xA0000 + (absOff & 0xFFFF), color);

}

int Canvas::getPixel(int x,int y) {
    
    unsigned long absOff = (unsigned long) (y) * WINDOW_WIDTH + x;
    
    setBank(absOff);
    
    int returnValue = (int) _farpeekb(_dos_ds,0xA0000 + (absOff & 0xFFFF));
    
    return returnValue;
}

void Canvas::setBank(unsigned long absoluteOffset) {

    int bank = (int)(absoluteOffset >> 16);

    // if necesary, change bank
    if (bank != curbank)
    {
        union REGS regs;

        regs.x.ax = 0x4F05;
        regs.x.bx = 0;
        regs.x.dx = bank;
        int86(0x10,&regs,&regs);

        /*
        regs.x.ax = 0x4F05;
        regs.x.bx = 1;
        regs.x.dx = bank;
        int86(0x_10,&regs,&regs);
        */

        curbank = bank;
    }
}

void Canvas::drawLine(int x_0, int y_0, int x_1, int y_1) {
    drawLine(x_0,y_0,x_1,y_1,current_color);
}

void Canvas::drawLine(int x_0, int y_0, int x_1, int y_1, int color) {
    /*
     * Based on lineBresenham(int x_0, int y_0, int x_1, int y_1)
     * source: http://www.codekeep.net/snippets/e39b2d9e-0843-4405-8e31-44e212ca1c45.aspx
     */

    int F, x, y, temp;

    if (x_0 > x_1)  // Swap points if p1 is on the right of p2
    {
        temp = x_0;
        x_0 = x_1;
        x_1 = temp;
        
        temp = y_0;
        y_0 = y_1;
        y_1 = temp;
    }

    // Handle trivial cases separately for algorithm speed up.
    // Trivial case 1: m = +/-INF (Vertical line)
    if (x_0 == x_1)
    {        
        if (y_0 > y_1)  // Swap y-coordinates if p1 is above p2
        {
            temp = y_0;
            y_0 = y_1;
            y_1 = temp;
        }

        x = x_0;
        y = y_0;
        while (y <= y_1)
        {
            putPixel(x, y, color);
            y++;
        }
        return;
    }
    // Trivial case 2: m = 0 (Horizontal line)
    else if (y_0 == y_1)
    {
        x = x_0;
        y = y_0;

        while (x <= x_1)
        {
            putPixel(x, y, color);
            x++;
        }
        return;
    }


    int dy            = y_1 - y_0;  // y-increment from p1 to p2
    int dx            = x_1 - x_0;  // x-increment from p1 to p2
    int dy2           = (dy << 1);  // dy << 1 == 2*dy
    int dx2           = (dx << 1);
    int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
    int dy2_plus_dx2  = dy2 + dx2;


    if (dy >= 0)    // m >= 0
    {        
        // Case 1: 0 <= m <= 1 (Original case)        
        if (dy <= dx)   
        {
            F = dy2 - dx;    // initial F

            x = x_0;
            y = y_0;
            while (x <= x_1)
            {
                putPixel(x, y, color);
                if (F <= 0)
                {
                    F += dy2;
                }
                else
                {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        }
        // Case 2: 1 < m < INF (Mirror about y=x line
        // replace all dy by dx and dx by dy)
        else
        {
            F = dx2 - dy;    // initial F

            y = y_0;
            x = x_0;
            while (y <= y_1)
            {
                putPixel(x, y, color);
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    }
    else    // m < 0
    {
        // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
        if (dx >= -dy)
        {
            F = -dy2 - dx;    // initial F

            x = x_0;
            y = y_0;
            while (x <= x_1)
            {
                putPixel(x, y, color);
                if (F <= 0)
                {
                    F -= dy2;
                }
                else
                {
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        }
        // Case 4: -INF < m < -1 (Mirror about x-axis and mirror 
        // about y=x line, replace all dx by -dy and dy by dx)
        else    
        {
            F = dx2 + dy;    // initial F

            y = y_0;
            x = x_0;
            while (y >= y_1)
            {
                putPixel(x, y, color);
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
}

void Canvas::drawNegativeLine(int x_0, int y_0, int x_1, int y_1) {
    /*
     * Same as drawLine() but paints 15 - [the current pixel]
    */
     
    int F, x, y, temp;

    if (x_0 > x_1)
    {
        temp = x_0;
        x_0 = x_1;
        x_1 = temp;
        
        temp = y_0;
        y_0 = y_1;
        y_1 = temp;
    }

    if (x_0 == x_1)
    {
        if (y_0 > y_1) 
        {
            temp = y_0;
            y_0 = y_1;
            y_1 = temp;
        }

        x = x_0;
        y = y_0;
        while (y <= y_1)
        {
            putPixel(x, y, 15 - getPixel(x,y));
            y++;
        }
        return;
    }
    
    else if (y_0 == y_1)
    {
        x = x_0;
        y = y_0;

        while (x <= x_1)
        {
            putPixel(x, y, 15 - getPixel(x,y));
            x++;
        }
        return;
    }


    int dy            = y_1 - y_0;  
    int dx            = x_1 - x_0;  
    int dy2           = (dy << 1);  
    int dx2           = (dx << 1);
    int dy2_minus_dx2 = dy2 - dx2;  
    int dy2_plus_dx2  = dy2 + dx2;


    if (dy >= 0)
    {
        if (dy <= dx)   
        {
            F = dy2 - dx;   

            x = x_0;
            y = y_0;
            while (x <= x_1)
            {
                putPixel(x, y, 15 - getPixel(x,y));
                if (F <= 0)
                {
                    F += dy2;
                }
                else
                {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        } else {
            F = dx2 - dy; 

            y = y_0;
            x = x_0;
            while (y <= y_1)
            {
                putPixel(x, y, 15 - getPixel(x,y));
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    } else {
        if (dx >= -dy)
        {
            F = -dy2 - dx;   

            x = x_0;
            y = y_0;
            while (x <= x_1)
            {
                putPixel(x, y, 15 - getPixel(x,y));
                if (F <= 0)
                {
                    F -= dy2;
                }
                else
                {
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        } else {
            F = dx2 + dy; 

            y = y_0;
            x = x_0;
            while (y >= y_1)
            {
                putPixel(x, y, 15 - getPixel(x,y));
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
}

void Canvas::drawWidthLine(int x_0, int y_0, int x_1, int y_1, int color, int width) {
    /*
     * Same as drawLine() but uses putWidthPixel()
    */
     
    int F, x, y, temp;

    if (x_0 > x_1)
    {
        temp = x_0;
        x_0 = x_1;
        x_1 = temp;
        
        temp = y_0;
        y_0 = y_1;
        y_1 = temp;
    }

    if (x_0 == x_1)
    {
        if (y_0 > y_1) 
        {
            temp = y_0;
            y_0 = y_1;
            y_1 = temp;
        }

        x = x_0;
        y = y_0;
        while (y <= y_1)
        {
            putWidthPixel(x, y, color, width);
            y++;
        }
        return;
    }
    
    else if (y_0 == y_1)
    {
        x = x_0;
        y = y_0;

        while (x <= x_1)
        {
            putWidthPixel(x, y, color, width);
            x++;
        }
        return;
    }


    int dy            = y_1 - y_0;  
    int dx            = x_1 - x_0;  
    int dy2           = (dy << 1);  
    int dx2           = (dx << 1);
    int dy2_minus_dx2 = dy2 - dx2;  
    int dy2_plus_dx2  = dy2 + dx2;


    if (dy >= 0)
    {
        if (dy <= dx)   
        {
            F = dy2 - dx;   

            x = x_0;
            y = y_0;
            while (x <= x_1)
            {
                putWidthPixel(x, y, color, width);
                if (F <= 0)
                {
                    F += dy2;
                }
                else
                {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        } else {
            F = dx2 - dy; 

            y = y_0;
            x = x_0;
            while (y <= y_1)
            {
                putWidthPixel(x, y, color, width);
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    } else {
        if (dx >= -dy)
        {
            F = -dy2 - dx;   

            x = x_0;
            y = y_0;
            while (x <= x_1)
            {
                putWidthPixel(x, y, color, width);
                if (F <= 0)
                {
                    F -= dy2;
                }
                else
                {
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        } else {
            F = dx2 + dy; 

            y = y_0;
            x = x_0;
            while (y >= y_1)
            {
                putWidthPixel(x, y, color, width);
                if (F <= 0)
                {
                    F += dx2;
                }
                else
                {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
}

void Canvas::setTextCursor(int x, int y) {
    // Input: coordinates x,y in pixels
    
    // Canvas set for 90 columns and 35 rows
    
    //Límites de área de texto (en filas y columnas)
    // TODO: remove hardcoded numbers
    unsigned int tx1 = (unsigned int) floor((90*x)/WINDOW_WIDTH);
    unsigned int ty1 = (unsigned int) floor((38*y)/WINDOW_HEIGHT); 
    
    
    // INT 10 - VIDEO - SET CURSOR POSITION
    // http://www.delorie.com/djgpp/doc/rbinter/id/92/0.html
    __asm__ (
        "movb $0x02, %%ah       \n\t"
        "movb %%bl, %%dl       \n\t"
        "movb %%cl, %%dh       \n\t"
        "xorb %%bh, %%bh       \n\t"
        "int $0x10"                         
        :   // Output
        : "b"(tx1), "c"(ty1)  // Input
        :
    );
}

void Canvas::putChar(char character)
{    
    putChar(character,current_color);
}

void Canvas::putChar(char character, int color)
{    
    // INT 10 - VIDEO - WRITE CHARACTER AND ATTRIBUTE AT CURSOR POSITION
    // http://www.delorie.com/djgpp/doc/rbinter/id/04/1.html
    __asm__ (
        "movb $0x09, %%ah       \n\t"
        "xorb %%bh, %%bh        \n\t"
        "movw $0x1, %%cx        \n\t"
        "int $0x10"
        :   // Output
        : "a"(character),"b"(color) // Input
        :
    );
}

void Canvas::setCommandText(const char *text) {
    setTextCursor(15,510); // Pass the x,y pixel coordinates of the text pointer
    printf("%-25s",text);
}

void Canvas::setMouseCoordinatesText(int x, int y) {
    setTextCursor(15,580); // Pass the x,y coordinates of the mouse to display
    printf("%d,%d",x,y);
}

void Canvas::drawSeparatorLine() {

    drawLine(0, getHeight()+1, getWidth(), getHeight()+1, 92);
    drawLine(0, getHeight()+2, getWidth(), getHeight()+2, 92);
    drawLine(0, getHeight()+3, getWidth(), getHeight()+3, 92);
    drawLine(0, getHeight()+4, getWidth(), getHeight()+4, 92);
    drawLine(0, getHeight()+5, getWidth(), getHeight()+5, 92);

}

void Canvas::drawCircle(int cx, int cy, int radius) {
    drawCircle(cx,cy,radius,current_color);
}

void Canvas::drawCircle(int cx, int cy, int radius, int color)
{
    int x = -radius, y = 0, err = 2-2*radius; /* II. Quadrant */ 
    do {
        putPixel(cx-x, cy+y, color); /*   I. Quadrant */
        putPixel(cx-y, cy-x, color); /*  II. Quadrant */
        putPixel(cx+x, cy-y, color); /* III. Quadrant */
        putPixel(cx+y, cy+x, color); /*  IV. Quadrant */
        radius = err;
        if (radius <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
        if (radius > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
}

void Canvas::drawNegativeCircle(int cx, int cy, int radius) {
    // Same as drawCirlce() but paints the current pixel - 15
    int x = -radius, y = 0, err = 2-2*radius; /* II. Quadrant */ 
    do {
        putPixel(cx-x, cy+y, 15 - getPixel(cx-x, cy+y)); /*   I. Quadrant */
        putPixel(cx-y, cy-x, 15 - getPixel(cx-y, cy-x)); /*  II. Quadrant */
        putPixel(cx+x, cy-y, 15 - getPixel(cx+x, cy-y)); /* III. Quadrant */
        putPixel(cx+y, cy+x, 15 - getPixel(cx+y, cy+x)); /*  IV. Quadrant */
        radius = err;
        if (radius <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
        if (radius > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
}

void Canvas::drawRectangle(int x_0, int y_0, int x_1, int y_1) {
    drawLine(x_0,y_0,x_1,y_0);
    drawLine(x_0,y_1,x_1,y_1);
    drawLine(x_0,y_0,x_0,y_1);
    drawLine(x_1,y_0,x_1,y_1);
}

void Canvas::drawRectangle(int x_0, int y_0, int x_1, int y_1, int color) {
    drawLine(x_0,y_0,x_1,y_0,color);
    drawLine(x_0,y_1,x_1,y_1,color);
    drawLine(x_0,y_0,x_0,y_1,color);
    drawLine(x_1,y_0,x_1,y_1,color);
}

void Canvas::drawSelectionRectangle(int x_0, int y_0, int x_1, int y_1) {
    drawNegativeLine(x_0,y_0,x_1,y_0);
    drawNegativeLine(x_0,y_1,x_1,y_1);
    drawNegativeLine(x_0,y_0,x_0,y_1);
    drawNegativeLine(x_1,y_0,x_1,y_1);


}

void Canvas::drawNegativeRectangle(int x_0, int y_0, int x_1, int y_1) {
    drawNegativeLine(x_0,y_0,x_1,y_0);
    drawNegativeLine(x_0,y_1,x_1,y_1);
    drawNegativeLine(x_0,y_0,x_0,y_1);
    drawNegativeLine(x_1,y_0,x_1,y_1);
}

void Canvas::drawEllipse(int x0, int y0, int x1, int y1)
{
    /*
     * source: http://free.pages.at/easyfilter/bresenham.html
     */
    
   int a = abs(x1-x0), b = abs(y1-y0), b1 = b&1; /* values of diameter */
   long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
   long err = dx+dy+b1*a*a, e2; /* error of 1.step */

   if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
   if (y0 > y1) y0 = y1; /* .. exchange them */
   y0 += (b+1)/2; y1 = y0-b1;   /* starting pixel */
   a *= 8*a; b1 = 8*b*b;

   do {
       putPixel(x1, y0); /*   I. Quadrant */
       putPixel(x0, y0); /*  II. Quadrant */
       putPixel(x0, y1); /* III. Quadrant */
       putPixel(x1, y1); /*  IV. Quadrant */
       e2 = 2*err;
       if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */ 
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
   } while (x0 <= x1);
   
   while (y0-y1 < b) {  /* too early stop of flat ellipses a=1 */
       putPixel(x0-1, y0); /* -> finish tip of ellipse */
       putPixel(x1+1, y0++); 
       putPixel(x0-1, y1);
       putPixel(x1+1, y1--); 
   }
}

void Canvas::drawNegativeEllipse(int x0, int y0, int x1, int y1)
{
   int a = abs(x1 - x0), b = abs(y1 - y0), b1 = b&1; /* values of diameter */
   
   long dx = 4*(1-a)*b*b, dy = 4*(b1+1)*a*a; /* error increment */
   
   long err = dx+dy+b1*a*a, e2; /* error of 1.step */

   if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
   if (y0 > y1) y0 = y1; /* .. exchange them */
   y0 += (b+1)/2; y1 = y0-b1;   /* starting pixel */
   a *= 8*a; b1 = 8*b*b;

   do {
       putPixel(x1, y0, 15 - getPixel(x1,y0)); /*   I. Quadrant */
       putPixel(x0, y0, 15 - getPixel(x0,y0)); /*  II. Quadrant */
       putPixel(x0, y1, 15 - getPixel(x0,y1)); /* III. Quadrant */
       putPixel(x1, y1, 15 - getPixel(x1,y1)); /*  IV. Quadrant */
       e2 = 2*err;
       if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */ 
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
   } while (x0 <= x1);
   
   while (y0 - y1 < b) {  /* too early stop of flat ellipses a=1 */
       putPixel(x0-1, y0, 15 - getPixel(x0 - 1,y0)); /* -> finish tip of ellipse */
       putPixel(x1+1, y0, 15 - getPixel(x1+1,y0)); 
       y0 = y0 + 1;
       putPixel(x0-1, y1, 15 - getPixel(x0 - 1, y1));
       putPixel(x1+1, y1, 15 - getPixel(x1 + 1, y1)); 
       y1 = y1 - 1;
   }
}

void Canvas::floodFillScanline(int x, int y) {
    floodFillScanline(x,y,current_color,getPixel(x,y));
}

void Canvas::floodFillScanline(int x, int y, int newColor, int oldColor) {
    
    /*
     * Based on: http://lodev.org/cgtutor/floodfill.html#Recursive_Scanline_Floodfill_Algorithm
     */
    
    // TODO: remove hardcoding to canvas parameters
    int w = 800;
    int h = 600;
    
    
    if(oldColor == newColor) 
        return;
        
    if(getPixel(x,y) != oldColor) 
        return;
    
    int y1;
    
    //draw current scanline from start position to the top [?]
    //draw current scanline from start position to the bottom
    y1 = y;
    while (y1 < h && getPixel(x,y1) == oldColor)
    {
        putPixel(x,y1,newColor);
        y1++;
    }
    
    //draw current scanline from start position to the bottom [?]
    y1 = y - 1;
    while (y1 >= 0 && getPixel(x,y1) == oldColor)
    {
        putPixel(x,y1, newColor);
        y1--;
    }
    
    //test for new scanlines to the left
    y1 = y;
    while(y1 < h && getPixel(x,y1) == newColor)
    {
        if(x > 0 && getPixel(x - 1,y1) == oldColor)
        {
            floodFillScanline(x - 1, y1, newColor, oldColor);
        }
        y1++;
    }
    
    y1 = y - 1;
    while(y1 >= 0 && getPixel(x,y1) == newColor)
    {
        if(x > 0 && getPixel(x - 1,y1) == oldColor)
        {
            floodFillScanline(x - 1, y1, newColor, oldColor);
        }
        y1--;
    }
    
    //test for new scanlines to the right
    y1 = y;
    while(y1 < h && getPixel(x,y1) == newColor)
    {
        if(x < w - 1 && getPixel(x + 1,y1) == oldColor)
        {
            floodFillScanline(x + 1, y1, newColor, oldColor);
        }
        y1++;
    }
    y1 = y - 1;
    while(y1 >= 0 && getPixel(x,y1) == newColor)
    {
        if(x < w - 1 && getPixel(x + 1,y1) == oldColor)
        {
            floodFillScanline(x + 1, y1, newColor, oldColor);
        }
        y1--;
    }
}

void Canvas::spray(int x, int y) {
    // Remove hardcoded width and intensity
    spray(x,y,current_color,50,1);
}

void Canvas::spray(int x, int y, int color, int radius, int intensity){
    
    int i = 0;
    int x_rand, y_rand;
    
    putPixel(x, y, color);
    
    for (i = 0; i < intensity; i ++) {
        x_rand = (1 + rand()%(radius - 1)) >> 1;
        y_rand = (1 + rand()%(radius - 1)) >> 1;
        
        putPixel(x + x_rand, y + y_rand);
        putPixel(x + x_rand, y - y_rand);
        putPixel(x - x_rand, y + y_rand);
        putPixel(x - x_rand, y - y_rand);
    }
}

void Canvas::clear() {
    int x;
    int y;
    
    for (y = 0; y < CANVAS_HEIGHT; y++)
        for (x = 0; x < CANVAS_WIDTH; x++)
            putPixel(x,y,background_color);
}

void Canvas::drawPalette(int x, int y) {
    
    for (int i = 0; i < 16; i++){
        drawLine(x+i*8+0, y, x+i*8+0, y+15, i);
        drawLine(x+i*8+1, y, x+i*8+1, y+15, i);
        drawLine(x+i*8+2, y, x+i*8+2, y+15, i);
        drawLine(x+i*8+3, y, x+i*8+3, y+15, i);
        drawLine(x+i*8+4, y, x+i*8+4, y+15, i);
        drawLine(x+i*8+5, y, x+i*8+5, y+15, i);
        drawLine(x+i*8+6, y, x+i*8+6, y+15, i);
        drawLine(x+i*8+7, y, x+i*8+7, y+15, i);
    }

    drawLine(x-1,y-1,x+16*8,y-1);
    drawLine(x-1,y+16,x+16*8,y+16);
    drawLine(x-1,y,x-1,y+15);
    drawLine(x+16*8,y,x+16*8,y+15);
    
}

void Canvas::drawCurrentColor(int x, int y) {
    
    for (int i = 0; i < 16; i++){
        drawLine(x+i*8+0, y, x+i*8+0, y+15, current_color);
        drawLine(x+i*8+1, y, x+i*8+1, y+15, current_color);
        drawLine(x+i*8+2, y, x+i*8+2, y+15, current_color);
        drawLine(x+i*8+3, y, x+i*8+3, y+15, current_color);
        drawLine(x+i*8+4, y, x+i*8+4, y+15, current_color);
        drawLine(x+i*8+5, y, x+i*8+5, y+15, current_color);
        drawLine(x+i*8+6, y, x+i*8+6, y+15, current_color);
        drawLine(x+i*8+7, y, x+i*8+7, y+15, current_color);
    }

    drawLine(x-1,y-1,x+16*8,y-1);
    drawLine(x-1,y+16,x+16*8,y+16);
    drawLine(x-1,y,x-1,y+15);
    drawLine(x+16*8,y,x+16*8,y+15);
    
}

void Canvas::setCurrentColor(int color) {
    current_color = color;
}

int Canvas::getCurrentColor() {
    return current_color;
}

void Canvas::erasePolygon(Polygon polygon) {
    drawPolygon(polygon,background_color);
}

void Canvas::drawPolygon(Polygon polygon) {
    drawPolygon(polygon,current_color);
}

void Canvas::drawPolygon(Polygon polygon, int color) {
    int x_0 = (int) polygon.getPointX(0), x_1;
    int y_0 = (int) polygon.getPointY(0), y_1;
    int i;
    
    //Draw edges
    for (i = 1;i < polygon.size; i++){
    
        x_1 = (int) polygon.getPointX(i);
        y_1 = (int) polygon.getPointY(i);
        
        drawLine(x_0,y_0,x_1,y_1, color);
        
        x_0 = x_1;
        y_0 = y_1;
    }
    
    //Get last and first vertex
    x_1 = (int) polygon.getPointX(i-1);
    y_1 = (int) polygon.getPointY(i-1);
    x_0 = (int) polygon.getPointX(0);
    y_0 = (int) polygon.getPointY(0);
    
    //Draw last edge
    if ((x_0 != x_1) || (y_0 != y_1)){
        drawLine(x_0,y_0,x_1,y_1, color);
    }
}

void Canvas::drawNegativePolygon(Polygon polygon) {
    /*
     * Same as drawPolygon() but with negative color
     */
    int x_0 = (int) polygon.getPointX(0), x_1;
    int y_0 = (int) polygon.getPointY(0), y_1;
    int i;
    
    for (i = 1;i < polygon.size; i++){
    
        x_1 = (int) polygon.getPointX(i);
        y_1 = (int) polygon.getPointY(i);
        
        drawNegativeLine(x_0,y_0,x_1,y_1);
        
        x_0 = x_1;
        y_0 = y_1;
    }
    
    x_1 = (int) polygon.getPointX(i-1);
    y_1 = (int) polygon.getPointY(i-1);
    x_0 = (int) polygon.getPointX(0);
    y_0 = (int) polygon.getPointY(0);
    
    if ((x_0 != x_1) || (y_0 != y_1)){
        drawNegativeLine(x_0,y_0,x_1,y_1);
    }
}

void Canvas::drawFilledPolygon(Polygon polygon) {
    drawFilledPolygon(polygon,current_color);
}

void Canvas::drawFilledPolygon(Polygon polygon, int color) {
    
    /*
     * Based on: http://www.cs.rit.edu/~icss571/filling/how_to.html
     */ 
     
     /*
      * all_edges[i][0] = min_y;
      * all_edges[i][1] = max_y;
      * all_edges[i][2] = corresponding_x;
      * all_edges[i][3] = slope;
      */
     
    int i, j;
    int x_0, y_0, x_1, y_1;
    
    double all_edges [polygon.size][4];
    double temp_edge [4];
    
    // Initialize edges
    int edges_count = 0;
    for (i = 1; i <= polygon.size; i++) {
        x_0 = (int) polygon.getPointX(i - 1);
        x_1 = (int) polygon.getPointX(i % polygon.size); 
        y_0 = (int) polygon.getPointY(i - 1);
        y_1 = (int) polygon.getPointY(i % polygon.size);

        if(y_1 != y_0) {
            if(y_0 < y_1) {
                all_edges[edges_count][0] = y_0;
                all_edges[edges_count][1] = y_1;
                all_edges[edges_count][2] = x_0;
            } else {
                all_edges[edges_count][0] = y_1;
                all_edges[edges_count][1] = y_0;
                all_edges[edges_count][2] = x_1;
            }
            all_edges[edges_count][3] = ((double) x_1 - x_0) / ((double) y_1 - y_0);
            edges_count++;
        }
    }

    //  Bubble sort of the nodes
    i = 0;
    while (i < edges_count - 1) {
        if 
        (
            (all_edges[i][0] > all_edges[i+1][0]) || 
            (all_edges[i][0] == all_edges[i+1][0] && (all_edges[i][1] > all_edges[i+1][1]))
        )
        {
            temp_edge[0] = all_edges[i][0];
            temp_edge[1] = all_edges[i][1];
            temp_edge[2] = all_edges[i][2];
            temp_edge[3] = all_edges[i][3];
            
            all_edges[i][0] = all_edges[i+1][0];
            all_edges[i][1] = all_edges[i+1][1];
            all_edges[i][2] = all_edges[i+1][2];
            all_edges[i][3] = all_edges[i+1][3];
            
            all_edges[i+1][0] = temp_edge[0];
            all_edges[i+1][1] = temp_edge[1];
            all_edges[i+1][2] = temp_edge[2];
            all_edges[i+1][3] = temp_edge[3];
            
            if(i) 
                i--;
        } else {
            i++; 
        }
    }

    // Polygon fill
    bool paint = false; 
    bool isPainted = true;
    
    int y = (int) all_edges[0][0]; 
    int x;
    
    while(isPainted) {
        // Sort list according to x
        i = 0;
        while (i < edges_count - 1) {
        if (all_edges[i][2] > all_edges[i+1][2]) {
            
            temp_edge[0] = all_edges[i][0];
            temp_edge[1] = all_edges[i][1];
            temp_edge[2] = all_edges[i][2];
            temp_edge[3] = all_edges[i][3];
            
            all_edges[i][0] = all_edges[i+1][0];
            all_edges[i][1] = all_edges[i+1][1];
            all_edges[i][2] = all_edges[i+1][2];
            all_edges[i][3] = all_edges[i+1][3];
            
            all_edges[i+1][0] = temp_edge[0];
            all_edges[i+1][1] = temp_edge[1];
            all_edges[i+1][2] = temp_edge[2];
            all_edges[i+1][3] = temp_edge[3];
            
            if(i) 
                i--;
            } else {
                i++; 
            }  
        }

        isPainted = false;
        paint = false;
        
        for (i = 0; i < edges_count; i++) {
            if(all_edges[i][0] <= y && y < all_edges[i][1]) {
                isPainted = true;
                x = (int)(all_edges[i][2] + 0.5);

                if (paint) {
                    for(j = x_0 ; j < x; j++) {
                        putPixel(j, y, color);
                    }
                } else {
                    putPixel(x, y, color);
                    x_0 = x+1;
                }
                
                paint = !paint;
                
                all_edges[i][2] += all_edges[i][3];
            }
        }
        y++;
    }
}

void Canvas::showHelp() {
    
}

#endif
