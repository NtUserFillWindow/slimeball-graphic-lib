#include "Graphics.hpp"
using namespace Graphics;

long long mainWindowDrawer(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,Painter& painter){
    //Assets::Font font(L"Arial",20,10,FONTWEIGHT_BLACK,false,false,false);
    static Assets::Image img;
    static bool loaded=false;
    if(!loaded||!img.getHBITMAP())
        img=Assets::Image(L"./test.png"),loaded=true;
    painter.drawBackground(Color((unsigned char)0,0,0,255));
    //font.loadFont();
    painter.setSize(2);
    painter.putImage(LOCATEMODE_CENTER,Point(400,300),img,255);
    painter.putPixel(600,400,Color((unsigned char)255,0,0,255));
    return 0;
}

int main(){
    auto mainWindow=createInitTransparentTopWindow(0,0,800,600,L"Window",Color((unsigned char)0,0,0));
    //auto mainWindow=createInitWindow(100,100,800,600,L"Window");
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
        PostMessage(mainWindow.second,WM_PAINT,0,0);
    }
    return 0;
}