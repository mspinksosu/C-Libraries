/***************************************************************************//**
 * @brief Blank C File Template
 * 
 * @author Matthew Spinks
 * 
 * @date 12/2/14   Original creation
 * @date 2/4/22    Modified
 * 
 * @file blank.c
 * 
 * @details
 *      The basic algorithm is called Bresenham's line algorithm. For each 
 * pixel, we calculate an error. For a line with positive slope, x will 
 * increase by 1 step and y will increase by 0 or 1 step. Each time x increases
 * the error increases by dy/dx. When the error is greater than 0.5, it means 
 * we are closer to the pixel above us and we go up to the next y position.
 * 
 *      Murphy's algorithm is a modification to Bresenham's that will draw 
 * perpendicular lines above and below the direction of the line in order to
 * make a thicker line
 * 
 ******************************************************************************/

#include "MurphyLine.h"

// ***** Defines ***************************************************************


// ***** Global Variables ******************************************************

// local function pointers
static void (*Draw_Pixel)(uint16_t x, uint16_t y, uint16_t rgb565Color);


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
    perpendicular lines, so we have divide width by 2 first. */
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
            /* Draw lines perpendicular to this one in order to draw a 
            thicker line. This is Murphy's modified Bresenham algorithm. */
            // TODO more notes on pError 
            DrawPerpLinesX(x, y, dx, dy, pxStep, pyStep, pError,
                           widthLeft, widthRight, error, rgb565Color);
            if(error >= threshold)
            {
                y += yStep;
                error += errorDiag;
                if(pError >= threshold)
                {
                    /* When both the lines do a diagonal move at the same time
                    the perpendicular line will be in the wrong place and a
                    hole will be left in the line. To fix this we draw an extra
                    perpendicular on the next phase to fill it in before 
                    looping around. */
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
    /* This is called Murphy's modified Bresenham algorigthm. It draws thick
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

    /* substitue for sqrt(dx^2 + dy^2) */
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
                                   widthLeft, widthRight, rgb565Color);
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
    /* This is called Murphy's modified Bresenham algorigthm. It draws thick
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