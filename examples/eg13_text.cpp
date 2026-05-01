#define IGNORE_WARNING_LOG
#include "Graphics.hpp"
#include "BasicUI.hpp"
#include "Game.hpp"
#include "Animation.hpp"
#include <algorithm>
using namespace Graphics;
auto linear=[](float a,float b,float t){return a+(b-a)*t;};
auto easeInOutQuad=[](float a,float b,float t){
    t=t<0.5f?2*t*t:1-pow(-2*t+2,2)/2;
    return a+(b-a)*t;
};
int main(){
    auto mainWindow=createInitWindow(0,0,407,430,L"Window");
    Assets::Animation anim(L"./www.bmp");
    anim.currentframe=0;
    Assets::Keyframe kf0(0);
    kf0.addProperty(Assets::property::X_POS,100,easeInOutQuad);
    kf0.addProperty(Assets::property::Y_POS,200,linear);
    kf0.addProperty(Assets::property::X_SIZE,1.0f,linear);
    kf0.addProperty(Assets::property::Y_SIZE,1.0f,linear);
    kf0.addProperty(Assets::property::ROTATE,0.0f,easeInOutQuad);
    anim.keyframes.push_back(kf0);
    Assets::Keyframe kf100(100);
    kf100.addProperty(Assets::property::X_POS,300,easeInOutQuad);
    kf100.addProperty(Assets::property::Y_POS,200,linear);
    kf100.addProperty(Assets::property::X_SIZE,2.0f,linear);
    kf100.addProperty(Assets::property::Y_SIZE,2.0f,linear);
    kf100.addProperty(Assets::property::ROTATE,2*3.1415926f,easeInOutQuad);
    anim.keyframes.push_back(kf100);
    std::sort(anim.keyframes.begin(),anim.keyframes.end());
    mainWindow.first->thisPaint=[&](HWND,UINT,WPARAM,LPARAM,Painter& p)->long long {
        p.drawBackground(Color((unsigned char)0,0,0));
        anim.show(p);
        return 0;
    };
    Clock c([&](){
        if(anim.currentframe>=99){
            anim.currentframe=0;
        }
        else{
            anim.step(1);
        }
        return;
    });//clock
    while(c){
        c.run();
    }
    return 0;
}