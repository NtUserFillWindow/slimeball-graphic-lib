#define IGNORE_WARNING_LOG
#include "Graphics.hpp"
using namespace Graphics;
int main(){
    auto mainWindow=createInitWindow(0,0,407,430,L"RomanNum");
    Assets::Font f(L"Arial",20,8,FONTWEIGHT_BLACK,false,false,false);
    f.loadFont();
    int counter=0;
    mainWindow.first->thisPaint=[&](HWND,UINT,WPARAM,LPARAM,Window::Painter& p){
        p.drawBackground(Color((unsigned char)255,255,255,255));
        p.putText(LOCATEMODE_CENTER,Point(200,200),f,Utils::intToRoman(counter),Color((unsigned char)0,0,0,255));
        return 0;
    };
    mainWindow.first->thisLeftClick=[&](HWND,UINT,WPARAM,LPARAM,int,int){
        counter++;
        return 0;
    };
    Clock c([&](){});
    while(c){
        c.run();
    }
}