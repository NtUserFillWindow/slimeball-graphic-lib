//MIT License

//Copyright (c) 2026 wzxTheSlimeball
#ifndef COLOR_HPP
#define COLOR_HPP

namespace Core{
    struct Color{
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
        Color(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha=255)
            :r(red),g(green),b(blue),a(alpha) {}
        Color():r(0),g(0),b(0),a(255) {}
        Color(const Color& other)=default;
        Color& operator=(const Color& other)=default;
        Color(float hue,float saturation,float lightness,unsigned char alpha=255);
    };
}
#endif // COLOR_HPP