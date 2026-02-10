#include "Graphics.hpp"
using namespace Graphics;

vector<std::pair<Point,int>> circles;

long long mainWindowDrawer(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,Painter& painter){
    painter.drawBackground(Color((unsigned char)255,255,255,255));
    for(const auto& circle:circles){
        painter.solidCircle(circle.first,circle.second,Color((unsigned char)0,0,0,255));
    }
    return 0;
}

long long mainWindowLeftClick(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y){
    circles.push_back(std::make_pair(Point(x,y),10));
    return 0;
}

long long mainWindowLeftHeld(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y,unsigned long long duration){
    circles.push_back(std::make_pair(Point(x,y),duration/1e9*2+10));//duration is in nanoseconds.Great!(?)
    return 0;
}

int main(){
    auto mainWindow=createInitWindow(0,0,800,600,0,L"Window");
    std::function<long long(HWND,UINT,WPARAM,LPARAM,Painter&)> mainWindowDrawerFunc=mainWindowDrawer;
    mainWindow.first->thisPaint=mainWindowDrawerFunc;

    mainWindow.first->thisLeftClick=mainWindowLeftClick;
    mainWindow.first->thisLeftHeld=mainWindowLeftHeld;

    MSG msg={};
    while(msg.message!=WM_QUIT){
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(16);
        globalHandleManager.updateAll();
    }
    return 0;
}