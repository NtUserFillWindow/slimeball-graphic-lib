#include "Graphics.hpp"
using namespace Graphics;

long long mainWindowDrawer(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,Painter& painter){
    static int lastFrame=0;
    static bool isRest=false;
    static int directionX=1;
    static int directionY=1;
    static int floatingDelta=0;
    static unsigned int TIME=0; 
    Assets::Image img;
    Assets::Image back(L"./bk.png");
    TIME++;
    floatingDelta=5*cos(TIME/30);
    if(lastFrame==0){
        lastFrame=300;
        isRest=!isRest;
        directionX=rand()%2==0?1:-1;
        directionY=rand()%2==0?1:-1;
    }
    else{
        lastFrame--;
    }
    Window::Handle *h=Window::Handle::queryWindow(hWnd);
    RECT rect=h?h->getRect():RECT{0,0,200,200};
    int winX=rect.left;
    int winY=rect.top;
    int winW=rect.right-rect.left;
    int winH=rect.bottom-rect.top;
    img=Assets::Image(directionX==1?L"./beeright.png":L"./beeleft.png");
    painter.putImage(LOCATEMODE_LEFTUPCORNER,Point(0,0),back,255);
    painter.putImage(LOCATEMODE_CENTER,Point(winW/2,winH/2+floatingDelta),img,255);
    if(!isRest){
        int nextX=winX+directionX;
        int nextY=winY+directionY;
        if(nextX>=0&&nextX+winW<=SCREEN_MAXX&&
           nextY>=0&&nextY+winH<=SCREEN_MAXY){
            if(h){
                h->moveWindow(MOVEMODE_DELTA,directionX,directionY);
            }
        }
    }
    return 0;
}

int main(){
    srand(time(0));
    auto mainWindow=createInitTransparentTopWindow(400,400,200,210,L"Window",Color(0xFDF6A9));

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