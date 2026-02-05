#include "Painter.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <windows.h>

Core::logger PainterLogger;
bool Window::Painter::alphaBlender(int x,int y,int width,int height,const Core::Color &color){
    if(x<0||y<0||width<0||height<0){
        PainterLogger.traceLog(Core::logger::LOG_WARNING,"Invalid rectangle,This will do literally nothing");
        return true;
    }
    if(color.a==0)return true;
    if(color.a==255){
        HBRUSH brush=CreateSolidBrush(color.toCOLORREF());
        if(!brush){
            PainterLogger.traceLog(Core::logger::LOG_ERROR,"Failed to create brush");
            return false;
        }
        RECT rect={x,y,x+width,y+height};
        FillRect(this->thisHDC,&rect,brush);
        DeleteObject(brush);
        return true;
    }
    HDC memHDC=CreateCompatibleDC(this->thisHDC);
    if(!memHDC){
        PainterLogger.traceLog(Core::logger::LOG_ERROR,"Failed to create memory DC");
        return false;
    }
    HBITMAP hBmp=CreateCompatibleBitmap(this->thisHDC,width,height);
    if(!hBmp){
        PainterLogger.traceLog(Core::logger::LOG_ERROR,"Failed to create bitmap");
        DeleteDC(memHDC);
        return false;
    }
    HBITMAP hOldBmp=(HBITMAP)SelectObject(memHDC,hBmp);
    HBRUSH hBrush=CreateSolidBrush(color.toCOLORREF());
    if(!hBrush){
        PainterLogger.traceLog(Core::logger::LOG_ERROR,"Failed to create brush");
        DeleteDC(memHDC);
        DeleteObject(hBmp);
        return false;
    }
    RECT rect={0,0,width,height};
    FillRect(memHDC,&rect,hBrush);
    DeleteObject(hBrush);
    BLENDFUNCTION bf={AC_SRC_OVER,0,color.a,0};
    bool result=AlphaBlend(this->thisHDC,x,y,width,height,memHDC,0,0,width,height,bf);
    SelectObject(memHDC,hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(memHDC);
    return (result!=FALSE);
}
bool Window::Painter::drawBackground(const Core::Color &color){
    RECT rect;
    GetClientRect(this->thisBindHWnd,&rect);
    return this->alphaBlender(0,0,rect.right,rect.bottom,color);
}
void Window::Painter::updateHDC()
{
    switch(this->nowHDC){
        case WINDOW:{
            EndPaint(this->thisBindHWnd,&this->ps);
            this->thisHDC=BeginPaint(this->thisBindHWnd,&this->ps);
            break;
        }
        case BUFFER:{
            this->thisHDC=this->thisBindHandle->getBuffer().memHDC;
            break;
        }
    }
}
void Window::Painter::present(){
    this->thisBindHandle->update();
}
void Window::Painter::switchHDC(){
    switch(this->nowHDC){
        case WINDOW:{
            this->nowHDC=BUFFER;
            break;
        }
        case BUFFER:{
            this->nowHDC=WINDOW;
            break;
        }
    }
    this->updateHDC();
}