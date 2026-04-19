#define IGNORE_WARNING_LOG //Ignore all the warning logs
#include "Graphics.hpp"//main file
#include "BasicUI.hpp"//UI file
using namespace Graphics;//Just for convenience
int main(){
    auto mainWindow=createInitWindow(0,0,207,230,L"Window");//init 1 window.
    //+7 +30 if for the non-client area
    UI::Button btn(LOCATEMODE_CENTER,{100,100},50,30,Color((unsigned char)200,200,200),Color((unsigned char)0,0,0));
    //init 1 button
    btn.react.push_back([=](){std::cerr<<"Btn clicked!"<<std::endl;return;});//onclick
    UI::Checkbox cb(LOCATEMODE_CENTER,{100,150},20,3,Color((unsigned char)255,127,0),Color((unsigned char)255,255,255),false);
    mainWindow.first->thisPaint=[&](HWND,UINT,WPARAM,LPARAM,Painter& p)->long long {
        p.drawBackground(Color((unsigned char)255,255,255));
        btn.show(p);
        cb.show(p);
        return 0;
    };//drawing function
    mainWindow.first->thisInstantLeftClick=[&](HWND,UINT,WPARAM,LPARAM,int x,int y)->long long {
        if(btn.isOnHover(x,y)){
            btn.react.back()();
        }
        cb.switchStatus(x,y);
        return 0;
    };//clicking function
    Clock c([&](){
        return;
    });//clock
    while(c){
        c.run();
    }
    return 0;
}