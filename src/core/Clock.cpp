//MIT License

//Copyright (c) 2026 Z-Multiplier
#include "Clock.hpp"
#include "Window.hpp"
void Core::Clock::run(){
    while(PeekMessage(&msg,nullptr,0,0,PM_REMOVE)){
        if(msg.message==WM_QUIT){
            running=false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if(!running){
        return;
    }
    auto now=std::chrono::steady_clock::now();
    if(now>=nextFrame){
        loop();
        Window::globalHandleManager.updateAll();
        nextFrame+=gap;
        if(now>nextFrame+gap){
            nextFrame=now+gap;
        }
    }
    std::this_thread::sleep_until(nextFrame);
}
Core::Clock::Clock(std::function<void()> loop,long long FPS):running(true),loop(loop){
    long long g=1e9/FPS;
    this->gap=std::chrono::nanoseconds(g);
    nextFrame=std::chrono::steady_clock::now()+gap;
}