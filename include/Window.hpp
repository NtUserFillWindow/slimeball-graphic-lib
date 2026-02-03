//MIT License

//Copyright (c) 2026 wzxTheSlimeball
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <windows.h>
#include "Canvas.hpp"

namespace Window{
    struct Window{
        int x,y;
        int width,height;
        HWND mHWnd;
        bool mIsOpen;
        Core::Canvas mFrontBuff,mBackBuff;
    };
}
#endif // WINDOW_HPP