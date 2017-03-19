// ------------------------------------------------------------------------------------------------
//  Colorize.cpp - Windows console stream colorizer
//
// Author: Dennis Lang - 2015
// http://LanDenLabs.com
//
// This file is part of LLWatch project.
//
// ----- License ----
//
// Copyright (c) 2015 Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ------------------------------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS
#include "Colorize.h"

Colorize::colorFg Colorize::sFgColor = Colorize::whiteFg;
Colorize::colorBg Colorize::sBgColor = Colorize::blackBg;
static WORD sDefColor = 0xff0f;   // White text on black background;

void init(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsoleOutput, &consoleInfo);
	sDefColor = consoleInfo.wAttributes;
}

// ------------------------------------------------------------------------------------------------
std::ostream& Colorize::setColor(std::ostream& out, Colorize::colorFg fg, Colorize::colorBg bg)
{
    HANDLE hOut = INVALID_HANDLE_VALUE;
    if ((void*)std::cout.rdbuf() == (void*)out.rdbuf())
        hOut = GetStdHandle(STD_OUTPUT_HANDLE); 
    else if ((void*)std::cerr.rdbuf() == (void*)out.rdbuf())
        hOut = GetStdHandle(STD_ERROR_HANDLE); 
    if (hOut != INVALID_HANDLE_VALUE) 
    {
        sFgColor = fg;
        sBgColor = bg;
        SetConsoleTextAttribute(hOut, sFgColor | sBgColor);
    }
    return out;
}

// ------------------------------------------------------------------------------------------------
// Output console text with inline colorization via encoding: !BF (B=background, F=foreground)
//  
// Color hex codes preceed with ! followed by Background and Foreground color values.
//
//     ---- Hex Color Values ---
//     0 Black           8 DarkGray    
//     1 DarkBlue        9 Blue        
//     2 DarkGreen       a Green      
//     3 DarkCyan        b Cyan       
//     4 DarkRed         c Red        
//     5 DarkMagenta     d Magenta    
//     6 DarkYellow      e Yellow     
//     7 Gray            f White   
//     
//  Example (Red Hello, Green World):
//    Colorize.write(cout, "!0cRed %aGreen!0f");
//
std::ostream& Colorize::write(std::ostream& out, const char* str) 
{
    const char* colorPtr;
    const char* prevPtr = str;
    while ((colorPtr = strchr(prevPtr, '!')) != NULL)
    {
        unsigned int color; 
        if (sscanf(colorPtr+1, "%02x", &color) == 1)
        {
            if (colorPtr != prevPtr) 
            {
                out.write(prevPtr, colorPtr - prevPtr);
                out.flush();
            }
            setColor(out, colorFg(color & 0x0f), colorBg(color & 0xf0));
            prevPtr = colorPtr+3;
        }
        else
        {            
            if (colorPtr != prevPtr) 
                out.write(prevPtr, colorPtr - prevPtr);
            prevPtr = colorPtr+1;
        }
    }

    if (colorPtr != prevPtr) 
        out << prevPtr;
    // setColor(out, colorFg(sDefColor & 0x0f), colorBg(sDefColor & 0xf0));
    return out;
}
