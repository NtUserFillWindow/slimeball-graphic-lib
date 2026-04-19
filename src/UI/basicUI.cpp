//MIT License

//Copyright (c) 2026 Z-Multiplier
#include "BasicUI.hpp"
bool UI::Button::show(Window::Painter& p){
    Window::Point locate=Window::calculateDrawPosition(locateMode,locator,width,height);
    if(usingImage){
        if(!p.putImage(locateMode,locator,std::get<Assets::Image>(texture),255)){
            return false;
        }
        if(!p.hollowPolygon({locate,Window::Point(locate.x+width,locate.y),
            Window::Point(locate.x+width,locate.y+height),Window::Point(locate.x,locate.y+height)},
            frame)){
            return false;
        }
    }
    else{
        if(!p.solidPolygon({locate,Window::Point(locate.x+width,locate.y),
            Window::Point(locate.x+width,locate.y+height),Window::Point(locate.x,locate.y+height)},
            std::get<Core::Color>(texture))){
            return false;
        }
        if(!p.hollowPolygon({locate,Window::Point(locate.x+width,locate.y),
            Window::Point(locate.x+width,locate.y+height),Window::Point(locate.x,locate.y+height)},
            frame)){
            return false;
        }
    }
    return true;
}
bool UI::Button::isOnHover(int x,int y){
    auto [lx,ly]=Window::calculateDrawPosition(locateMode,locator,width,height);
    return lx<x&&
           ly<y&&
           lx+this->width>x&&
           ly+this->height>y;
}
bool UI::Checkbox::show(Window::Painter& p){
    return true;
}
bool UI::Checkbox::switchStatus(int x,int y){
    return false;
}