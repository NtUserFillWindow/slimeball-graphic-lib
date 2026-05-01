//MIT License

//Copyright (c) 2026 Z-Multiplier
#include "Animation.hpp"
#include "Logger.hpp"
#include <algorithm>
namespace Assets{
    Core::logger assetsLogger;
}
void Assets::Animation::show(Window::Painter& p){
    Assets::Image img(path);
    if(keyframes.empty()) return;
    auto it=std::upper_bound(keyframes.begin(),keyframes.end(),currentframe);
    if(it==keyframes.begin()){
        assetsLogger.traceLog(Core::logger::LOG_ERROR,"The time of first keyframe is not 0(but it should be)");
        throw std::runtime_error("The time of first keyframe is not 0(but it should be)");
    }
    else if(it==keyframes.end()){
        assetsLogger.traceLog(Core::logger::LOG_ERROR,"Animation play out of bound.");
        throw std::runtime_error("Animation play out of bound.");
    }
    auto prev=std::prev(it,1);
    size_t x=0,y=0;
    float rotation=0.0f;
    float xsize=0,ysize=0;
    float xshear=0,yshear=0;
    float t=float(currentframe-prev->time)/float(it->time-prev->time);
    std::unordered_map<Assets::property,float> vals;
    for(const auto& [p,val,interp]:(*it).properties){
        if(val==0){
            vals[p]=-1.0f;
        }
        vals[p]=val;
    }
    for(const auto& [p,val,interp]:(*prev).properties){
        switch(p){
            case Assets::property::ROTATE:rotation=interp(val,vals[p]<=0.0f?val:vals[p],t);break;
            case Assets::property::X_POS:x=interp(val,vals[p]<=0.0f?val:vals[p],t);break;
            case Assets::property::X_SHEAR:xshear=interp(val,vals[p]<=0.0f?val:vals[p],t);break;
            case Assets::property::X_SIZE:xsize=interp(val,vals[p]<=0.0f?val:vals[p],t);break;
            case Assets::property::Y_POS:y=interp(val,vals[p]<=0.0f?val:vals[p],t);break;
            case Assets::property::Y_SHEAR:yshear=interp(val,vals[p]<=0.0f?val:vals[p],t);break;
            case Assets::property::Y_SIZE:ysize=interp(val,vals[p]<=0.0f?val:vals[p],t);break;
        }
    }
    Assets::Matrix m(static_cast<long double>(rotation));
    img.matrix=m;
    img.transformation(false);
    m=Assets::Matrix(static_cast<double>(xsize),static_cast<double>(ysize));
    img.matrix=m;
    img.transformation(false);
    m=Assets::Matrix(static_cast<double>(xshear),static_cast<double>(yshear),true);
    img.matrix=m;
    img.transformation(false);
    p.putImage(LOCATEMODE_CENTER,{static_cast<int>(x),static_cast<int>(y)},img,255);
    return;
}