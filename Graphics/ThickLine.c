/***************************************************************************//**
 * @brief Murphy's Modified Bresenham Line Algorithm
 * 
 * @file ThickLine.c
 * 
 * @author Matthew Spinks <https://github.com/mspinksosu>
 * 
 * @date 5/3/23    Original creation
 * 
 * @details
 *      The basic algorithm is called Bresenham's line algorithm, named after
 * Jack Bresenham. For each pixel, we calculate an error. For a line with 
 * positive slope, x will increase by 1 step and y will increase by 0 or 1 
 * step. Each time x increases the error increases by dy/dx. When the error is 
 * greater than 0.5, it means we are closer to the pixel above us and we go up 
 * to the next y position.
 * 
 * This library is based on Alan Murphy's own work. Murphy's algorithm is a 
 * modification to Bresenham's that will draw perpendicular lines above and 
 * below the direction of the line in order to make a thicker line.
 * 
 * This version is designed for use with microcontroller's and small TFT
 * screens. It has been optimized slightly for speed, which I will explain
 * below. There are a lot of if-statements with the pxStep and pyStep that 
 * could optimized further, but I left them as is to make it easier to
 * understand.
 * 
 * In the original paper, published in 1978 (IBM Technical Disclosure Bulletin 
 * Vol. 20 No. 12 pages 5358-5366), there was a constant number called "k" 
 * which was used to help shape the width of the thick line. The formula for 
 * the thickness is 2*thickness*k. But, a lot of versions of this code simply 
 * omit the formula and apply "2*thickness*sqrt(dx^2 + dy^2)" instead. The 
 * sqrt(dx^2 + dy^2) is the constant "k". Computers of the era used an 
 * approximation instead and I have done this same for this version. The paper 
 * gives a rough approximation of x + y/4 as well as a piece-wise equation. 
 * The algorithm works just fine with an approximation. The line will be a 
 * little jagged, but that is hardly a concern with a small TFT screen. If you 
 * are porting this to a more powerful processor you can try substituting the 
 * ideal value to see if there is any noticeable difference.
 * 
 * @section license License
 * SPDX-FileCopyrightText: © 2023 Matthew Spinks
 * SPDX-License-Identifier: MIT-0
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sellcopies of the Software, and to permit persons to whom the Software is
 * furnished to do so.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

// local function pointers
static void (*DrawPixel)(uint16_t x, uint16_t y, uint16_t rgb565Color);


// ***** Static Functions Prototypes *******************************************

static void DrawLineX(int16_t x1, int16_t y1, int16_t x2, int16_t y2, 
    uint8_t width, uint16_t rgb565Color);

static void DrawPerpLinesX(int16_t x1, int16_t y1, int16_t dx, int16_t dy, 
    int16_t xStep, int16_t yStep, int16_t errorInit, uint16_t widthLeft, 
    uint16_t widthRight, int16_t widthInit, uint16_t rgb565Color);

static void DrawLineY(int16_t x1, int16_t y1, int16_t x2, int16_t y2, 
    uint8_t width, uint16_t rgb565Color);

static void DrawPerpLinesY(int16_t x1, int16_t y1, int16_t dx, int16_t dy, 
    int16_t xStep, int16_t yStep, int16_t errorInit, uint16_t widthLeft, 
    uint16_t widthRight, int16_t widthInit, uint16_t rgb565Color);

// *****************************************************************************

void Murphy_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, 
    uint8_t width, uint16_t rgb565Color)
{
    /* Normally x will increase by 1 step and y will increase by 0 or 1 step.
    For a steep slope, y will increase by 1 step and x will increase by 
    0 or 1 step. All the equations for x and y will be interchanged. */
    if(abs(y2 - y1) > abs(x2 - x1))
    {
        DrawLineY(x1, y1, x2, y2, width, rgb565Color);
    }
    else
    {
        DrawLineX(x1, y1, x2, y2, width, rgb565Color);
    }
}

// *****************************************************************************

static void DrawLineX(int16_t x1, int16_t y1, int16_t x2, int16_t y2, 
    uint8_t width, uint16_t rgb565Color)
{
    int16_t dx, dy, error, x, y, xStep, yStep, errorDiag, errorSquare,
            threshold, pError, pxStep, pyStep, k, widthLeft, widthRight;
    uint16_t length;

    dx = x2 - x1;
    dy = y2 - y1;
    xStep = yStep = 1;
    /* Find the step directions for the perpendicular lines. For lines in 
    quadrants I and III, we draw the left hand perpendicular first. So its
    direction is just the x, y step values rotated counterclockwise. For 
    quadrants II and IV the other perpendicular needs to be drawn first, 
    otherwise there will be artifacts in the line. So for those quadrants the
    step values are the x, y step values rotated clockwise. */
    if(dx < 0)
        pxStep = 1;
    else
        pxStep = -1;

    if(dy < 0)
        pyStep = -1;
    else
        pyStep = 1;
    /* For a negative dx/dy, the steps decrease by one and the dx/dy values
    become inverted. */
    if(dx < 0)
    {
        dx = -dx;
        xStep = -1;
    }

    if(dy < 0)
    {
        dy = -dy;
        yStep = -1;
    }
    /* For each loop, we are going to determine if we should move left/right
    or diagonally to the next pixel above or below. Whenever we need to move
    diagonally, we must recalulate our error value. This is "errorDiag". Our 
    normal move in the left/right direction is called a "square" move, which 
    uses "errorSquare". */
    x = x1;
    y = y1;
    length = dx + 1;
    error = pError = 0;
    threshold = -2*dy + dx;
    errorDiag = -2*dx;
    errorSquare = 2*dy;
    // error = pError = 2*dy - dx; // TODO test different type of loop
    // errorDiag = 2*dy - 2*dx;
    // errorSquare = 2*dy;

    /* Normally, the equations for the error would go like this:
    error + dy/dx >= 0.5        error condition
    error = error + dy/dx - 1   diagonal move
    error = error + dy/dx       square move
    
    Multiplying everything by 2dx yields:
    error + 2dy >= dx           error condition
    error = error + 2dy - 2dx   diagonal move
    error = error + 2dy         square move

    /* k is a constant whose value is ideally sqrt(dx^2 + dy^2). The width 
    of a single perpendicular line is pWidth*2k. We are drawing two
    perpendicular lines, so we have divide the total width by 2 first. */
    uint8_t oddWidth = 0;
    if(width & 0x01)
        oddWidth = 1; // add one to right side for odd widths
    width >>= 1;
    /* Because there is a restricted domain for the values dx and dy, k can be
    approximated as dx + dy / 4, which should be fine for a microcontroller.
    For better results you can also use the following formula: */
    if((dy+dy+dy) > dx)
        k = dx - (dx >> 3) + (dy >> 1); // dx - (dx / 8) + (dy / 2)
    else
        k = dx + (dy >> 3); // dx + (dy / 8)

    widthLeft = width*2*k;
    widthRight = (width + oddWidth)*2*k;

    for(uint16_t p = 0; p < length; p++)
    {
        if(width == 0)
        {
            /* This is the basic algorithm for drawing a single line */
            if(DrawPixel)
                DrawPixel(x, y, rgb565Color);
            if(error >= threshold)
            {
                y += yStep;
                error += errorDiag;
            }
            error += errorSquare;
            x += xStep;
        }
        else
        {
            /* Draw lines perpendicular to this one in order to draw a thicker 
            line. If we were to just draw a perpendicular line at each x y 
            there would be a lot of holes in the line each time the sequence 
            does a diagonal move. The solution is instead of the perpendicular 
            line always starting with error = 0, we give it an initial error 
            value. Whenever we make a square move, we need the perpendicular
            line to keep the same error value so that the sequence doesn't 
            shift up and create a gap. So we will only update this "pError" 
            value whenever we make a diagonal move. */
            DrawPerpLinesX(x, y, dx, dy, pxStep, pyStep, pError,
                           widthLeft, widthRight, error, rgb565Color);
            if(error >= threshold)
            {
                y += yStep;
                error += errorDiag;
                if(pError >= threshold)
                {
                    /* When both the base loop and perpendicular lines do a 
                    diagonal move at the same time the perpendicular line will 
                    be too far on the next loop and hole will be left in the 
                    line. To fix this, we draw a perpendicular on the next 
                    phase to fill it in before we loop around. */
                    DrawPerpLinesX(x, y, dx, dy, pxStep, pyStep, 
                                   pError + errorDiag + errorSquare, 
                                   widthLeft, widthRight, error, rgb565Color);
                    pError += errorDiag;
                }
                pError += errorSquare;
            }
            error += errorSquare;
            x += xStep;
        }
    }
}

// *****************************************************************************

static void DrawPerpLinesX(int16_t x1, int16_t y1, int16_t dx, int16_t dy, 
    int16_t xStep, int16_t yStep, int16_t errorInit, uint16_t widthLeft, 
    uint16_t widthRight, int16_t widthInit, uint16_t rgb565Color)
{
    int16_t x, y, error, errorDiag, errorSquare, threshold, tk;
    // TODO error is not initialized in this function. Add more notes
    /* This is called Murphy's modified Bresenham algorithm. It draws thick
    lines by drawing perpendicular lines above and below the first line. */
    x = x1;
    y = y1;
    error = errorInit;
    threshold = -2*dy + dx;
    errorDiag = -2*dx;
    errorSquare = 2*dy;
    // errorDiag = 2*dy - 2*dx; // TODO test different type of loop
    // errorSquare = 2*dy;
    tk = dx + dy - widthInit;

    while(tk <= widthLeft)
    {
        if(DrawPixel)
            DrawPixel(x, y, rgb565Color);
        if(error >= threshold)
        {
            x += xStep;
            error += errorDiag;
            tk += 2*dy;
        }
        error += errorSquare;
        y += yStep;
        tk += 2*dx;
    }

    /* Reset and draw the right side perpendicular line */
    // TODO add notes about bottom perpendicular line
    x = x1;
    y = y1;
    xStep = -xStep;
    yStep = -yStep;
    error = -errorInit;
    tk = dx + dy + widthInit;

    while(tk <= widthRight)
    {
        if(DrawPixel)
            DrawPixel(x, y, rgb565Color);
        if(error > threshold)
        {
            x += xStep;
            error += errorDiag;
            tk += 2*dy;
        }
        error += errorSquare;
        y += yStep;
        tk += 2*dx;
    }
}

// *****************************************************************************

static void DrawLineY(int16_t x1, int16_t y1, int16_t x2, int16_t y2, 
    uint8_t width, uint16_t rgb565Color)
{
    int16_t dx, dy, error, x, y, xStep, yStep, errorDiag, errorSquare,
            threshold, pError, pxStep, pyStep, k, widthLeft, widthRight;
    uint16_t length;

    dx = x2 - x1;
    dy = y2 - y1;
    xStep = yStep = 1;
   /* Find the direction for the perpendicular lines. */
    if(dx < 0)
        pxStep = 1;
    else
        pxStep = -1;

    if(dy < 0)
        pyStep = -1;
    else
        pyStep = 1;
    /* For a negative dx/dy, the steps decrease by one and the dx/dy values
    become inverted. */
    if(dx < 0)
    {
        dx = -dx;
        xStep = -1;
    }

    if(dy < 0)
    {
        dy = -dy;
        yStep = -1;
    }
    
    x = x1;
    y = y1;
    length = dy + 1;
    error = pError = 0;
    threshold = -2*dx + dy;
    errorDiag = -2*dy;
    errorSquare = 2*dx;

    uint8_t oddWidth = 0;
    if(width & 0x01)
        oddWidth = 1; // add one to right side for odd widths
    width >>= 1;

    /* substitute for sqrt(dx^2 + dy^2) */
    if((dy+dy+dy) > dx)
        k = dx - (dx >> 3) + (dy >> 1); // dx - (dx / 8) + (dy / 2)
    else
        k = dx + (dy >> 3); // dx + (dy / 8)

    widthLeft = width*2*k;
    widthRight = (width + oddWidth)*2*k;

    for(uint16_t p = 0; p < length; p++)
    {
        if(width == 0)
        {
            /* This is the basic algorithm for drawing a single line */
            if(DrawPixel)
                DrawPixel(x, y, rgb565Color);
            if(error >= threshold)
            {
                x += xStep;
                error += errorDiag;
            }
            error += errorSquare;
            y += yStep;
        }
        else
        {
            /* Draw lines perpendicular to this one in order to draw a 
            thicker line. This is Murphy's modified Bresenham algorithm. */
            DrawPerpLinesY(x, y, dx, dy, pxStep, pyStep, pError,
                            widthLeft, widthRight, error, rgb565Color);
            if(error >= threshold)
            {
                x += xStep;
                error += errorDiag;
                if(pError >= 0)
                {
                    /* When both the lines do a diagonal move at the same time
                    the perpendicular line will be in the wrong place and a
                    hole will be left in the line. To fix this we draw an extra
                    perpendicular on the next phase to fill it in before 
                    looping around. */
                    DrawPerpLinesY(x, y, dx, dy, pxStep, pyStep, 
                                   pError + errorDiag + errorSquare, 
                                   widthLeft, widthRight, error, rgb565Color);
                    pError += errorDiag;
                }
                pError += errorSquare;
            }
            error += errorSquare;
            y += yStep;
        }
    }
}

// *****************************************************************************

static void DrawPerpLinesY(int16_t x1, int16_t y1, int16_t dx, int16_t dy, 
    int16_t xStep, int16_t yStep, int16_t errorInit, uint16_t widthLeft, 
    uint16_t widthRight, int16_t widthInit, uint16_t rgb565Color)
{
    int16_t x, y, error, errorDiag, errorSquare, threshold, tk;
    // TODO error is not initialized in this function. Add more notes
    /* This is called Murphy's modified Bresenham algorithm. It draws thick
    lines by drawing perpendicular lines above and below the first line. */
    x = x1;
    y = y1;
    error = -errorInit;
    threshold = -2*dx + dy;
    errorDiag = -2*dy;
    errorSquare = 2*dx;
    // errorDiag = 2*dx - 2*dy; // TODO test different type of loop
    // errorSquare = 2*dx;
    tk = dx + dy + widthInit;

    while(tk <= widthLeft)
    {
        if(DrawPixel)
            DrawPixel(x, y, rgb565Color);
        if(error > threshold)
        {
            y += yStep;
            error += errorDiag;
            tk += 2*dx;
        }
        error += errorSquare;
        x += xStep;
        tk += 2*dy;
    }

    /* Reset and draw the right side perpendicular line */
    // TODO add notes about bottom perpendicular line
    x = x1;
    y = y1;
    xStep = -xStep;
    yStep = -yStep;
    error = errorInit;
    tk = dx + dy - widthInit;

    while(tk <= widthRight)
    {
        if(DrawPixel)
            DrawPixel(x, y, rgb565Color);
        if(error >= threshold)
        {
            y += yStep;
            error += errorDiag;
            tk += 2*dx;
        }
        error += errorSquare;
        x += xStep;
        tk += 2*dy;
    }
}

/*
 End of File
 */