//MIT License

//Copyright (c) 2026 Z-Multiplier
#ifndef BASICUI_HPP
#define BASICUI_HPP
#include <optional>
#include <variant>
#include <utility>
#include <vector>
#include <functional>
#include "Image.hpp"
#include "Color.hpp"
#include "Painter.hpp"
namespace UI{
    using Texture=std::variant<Core::Color,Assets::Image>;
    struct Button{
        char locateMode;
        Window::Point locator;
        int width;
        int height;
        Texture texture;
        Core::Color frame;
        bool usingImage;
        bool show(Window::Painter& p);
        bool isOnHover(int,int);
        std::vector<std::function<void()>> react;
        Button(char locateMode,Window::Point locator,int w,int h,Assets::Image img,
               Core::Color frame):
               locateMode(locateMode),locator(locator),width(w),height(h),texture(std::move(img)),frame(frame),
               usingImage(true){};
        Button(char locateMode,Window::Point locator,int w,int h,Core::Color body,
               Core::Color frame):
               locateMode(locateMode),locator(locator),width(w),height(h),texture(body),frame(frame),
               usingImage(false){};
        ~Button()=default;
        Button()=default;
        Button(Button& btn)=delete;
        Button& operator=(Button& btn)=delete;
        Button(Button&& btn)=default;
        Button& operator=(Button&& btn)=default;
    };
    struct Checkbox{
        char locateMode;
        Window::Point locator;
        int size;
        int borderRadius;
        Core::Color body;
        Core::Color tick;
        bool ticked;
        bool show(Window::Painter& p);
        bool switchStatus(int x,int y);
    };
};

#endif