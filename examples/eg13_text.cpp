#include "Graphics.hpp"
using namespace Graphics;

long long mainWindowDrawer(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,Painter& painter){
    Assets::Font font(L"Arial",20,10,FONTWEIGHT_BLACK,false,false,false);
    painter.drawBackground(Color((unsigned char)255,255,255,255));
    font.loadFont();
    painter.setSize(2);
    painter.putText(LOCATEMODE_LEFTUPCORNER,Point(400,300),font,L"Hello,world!lu",Color((unsigned char)0,0,0,255));
    painter.putText(LOCATEMODE_LEFTBOTTOMCORNER,Point(400,300),font,L"Hello,world!lb",Color((unsigned char)0,0,0,255));
    painter.putText(LOCATEMODE_RIGHTUPCORNER,Point(400,300),font,L"Hello,world!ru",Color((unsigned char)0,0,0,255));
    painter.putText(LOCATEMODE_RIGHTBOTTOMCORNER,Point(400,300),font,L"Hello,world!rb",Color((unsigned char)0,0,0,255));
    painter.putPixel(400,300,Color((unsigned char)255,0,0,255));
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