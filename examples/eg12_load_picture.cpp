#include "Graphics.hpp"
using namespace Graphics;

long long mainWindowDrawer(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,Painter& painter){
    static Assets::Image img;
    static bool loaded=false;
    if(!loaded){
        img=Assets::Image(L"./eg12.png");
        if(img.getHBITMAP()){
            loaded=true;
        }
        else loaded=false;
    }
    painter.putImage(LOCATEMODE_CENTER,Point(54,54),img,255);
    return 0;
}

int main(){
    auto mainWindow=createInitWindow(0,0,800,600,0,L"Window");
    std::function<long long(HWND,UINT,WPARAM,LPARAM,Painter&)> mainWindowDrawerFunc=mainWindowDrawer;
    mainWindow.first->thisPaint=mainWindowDrawerFunc;

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