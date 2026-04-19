//MIT License

//Copyright (c) 2026 Z-Multiplier
#ifndef CLOCK_HPP
#define CLOCK_HPP
#include <vector>
#include <chrono>
#include <thread>
#include <functional>
#include <windows.h>
namespace Core{
    struct Clock{
        MSG msg;
        bool running;
        std::chrono::steady_clock::time_point nextFrame;
        std::chrono::nanoseconds gap;
        std::function<void()> loop;
        Clock()=delete;
        Clock(std::function<void()> loop,long long FPS=60);
        void run();
        operator bool()const{
            return running;
        }
        Clock(Clock& other)=delete;
        Clock(Clock&& other)=delete;
        Clock operator=(Clock& other)=delete;
        Clock operator=(Clock&& other)=delete;
        ~Clock()=default;
    };
}
#endif