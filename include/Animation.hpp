//MIT License

//Copyright (c) 2026 Z-Multiplier
#ifndef ANIMATION_HPP
#define ANIMATION_HPP
#include "Image.hpp"
#include "Transformation.hpp"
#include "Painter.hpp"
#include <functional>
#include <chrono>
namespace Assets{
    enum class property{
        X_POS=1,
        Y_POS=2,
        X_SIZE=3,
        Y_SIZE=4,
        ROTATE=5,
        X_SHEAR=6,
        Y_SHEAR=7,
    };//ALL absolute value
    inline bool operator<(property a,property b){
        return static_cast<uint32_t>(a)<static_cast<uint32_t>(b);
    }
    struct Keyframe{
        size_t time;
        std::vector<std::tuple<property,float,std::function<float(float,float,float)>>> properties;
        std::unordered_map<property,bool> added;
        Keyframe()=delete;
        Keyframe(size_t time):time(time){};
        inline bool operator<(const Keyframe& other)const{
            return this->time<other.time;
        }
        inline bool operator<(const size_t& other)const{
            return this->time<other;
        }
        void addProperty(property p,float val,std::function<float(float,float,float)> interp){
            if(added[p]) return;
            properties.emplace_back(p,val,interp);
            added[p]=true;
        }
    };
    inline bool operator<(const size_t& a,const Keyframe& b){
        return a<b.time;
    }
    struct Animation{
        std::wstring path;
        std::vector<Keyframe> keyframes;
        size_t currentframe;
        void step(size_t len){
            currentframe+=len;
        }
        void addKeyframe(Keyframe kf){
            keyframes.push_back(kf);
        }
        void show(Window::Painter& p);
        Animation()=delete;
        Animation(std::wstring path):path(path){};
    };
}

#endif