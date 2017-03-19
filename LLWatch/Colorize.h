// ------------------------------------------------------------------------------------------------
//  Colorize.h - Windows console stream colorizer
//
// Author: Dennis Lang - 2015
// http://LanDenLabs.com/
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

#pragma once

#include <Windows.h>
#include <iostream>
#include <sstream>

class Colorize 
{
public:
    enum colorFg
    {
        blackFg=0,
        redFg=FOREGROUND_RED,
        greenFg=FOREGROUND_GREEN,
        blueFg=FOREGROUND_BLUE,
        yellowFg=FOREGROUND_GREEN|FOREGROUND_RED,
        whiteFg=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,

        redIFg=FOREGROUND_RED|FOREGROUND_INTENSITY,
        greenIFg=FOREGROUND_GREEN|FOREGROUND_INTENSITY,
        blueIFg=FOREGROUND_BLUE|FOREGROUND_INTENSITY,
        yellowIFg=FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY,
        whiteIFg=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY,
    };

    enum colorBg
    {
        blackBg=0,
        redBg=BACKGROUND_RED,
        greenBg=BACKGROUND_GREEN,
        blueBg=BACKGROUND_BLUE,
        yellowBg=BACKGROUND_GREEN|BACKGROUND_RED,
        whiteBg=BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE,
    };

    static colorFg sFgColor;
    static colorBg sBgColor;

    // Output console text with inline colorization via encoding: !BF (B=background, F=foreground)
    //  
    // Color hex codes (!0c = red foreground, !1c - red foreground dark blue background)
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
    //    Colorize.write(cout, "!0cHello %aWorld!0f");
    static std::ostream& write(std::ostream& out, const char*);
	static std::ostream& write(std::ostream& out, const char* str, unsigned length)
	{
		return write(out, std::string(str, length).c_str());
	}
   
    static std::ostream& setColor(std::ostream& out, Colorize::colorFg fg, Colorize::colorBg bg);
};



inline std::ostream& operator<< (std::ostream& out, const Colorize::colorFg& fg)
{
    return Colorize::setColor(out, fg, Colorize::sBgColor);
}
