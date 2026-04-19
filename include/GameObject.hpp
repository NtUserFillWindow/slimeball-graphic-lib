//MIT License

//Copyright (c) 2026 Z-Multiplier
#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <unordered_map>
#include "Logger.hpp"
#include <any>
#include <unordered_set>
#include <functional>

namespace Game{
    extern Core::logger gameLogger;
    struct AIBehavior{
        std::vector<std::function<bool(const std::unordered_map<string,std::any>&,const std::unordered_set<std::string>&)>> conditions;
        std::function<void(std::unordered_map<string,std::any>&,std::unordered_set<std::string>&)> behavior;
        bool check(const std::unordered_map<string,std::any>&,const std::unordered_set<std::string>&)const;
    };
    struct BasicLife{
        private:
        std::unordered_map<string,std::any> attributes;
        std::vector<AIBehavior> behaviors;
        public:
        std::function<void(std::unordered_map<string,std::any>&,std::unordered_set<std::string>&)> updateFunction;
        std::unordered_set<std::string> status;
        template<typename T>
        T getAttribute(const string& key,T defaultValue=T())const{
            if(!attributes.count(key)){return defaultValue;}
            try{
                T res=std::any_cast<T>(attributes.at(key));
                return res;
            }catch(std::bad_any_cast bac){
                gameLogger.traceLog(Core::logger::LOG_ERROR,"Type error,maybe you stored something different before.");
                gameLogger.traceLog(Core::logger::LOG_NOTE,bac.what());
                return defaultValue;
            }
        }
        template<typename T>
        bool setAttribute(const string& key,T val){
            try{
                attributes[key]=val;
                return true;
            }catch(std::exception e){
                gameLogger.traceLog(Core::logger::LOG_ERROR,"Something happened");
                gameLogger.formatLog(Core::logger::LOG_NOTE,"what():%s",e.what());
                return false;
            }
        }
        void pushBehavior(const AIBehavior& beh);
        void update();
        BasicLife()=delete;
        BasicLife(std::unordered_map<string,std::any> attr,std::vector<AIBehavior> behs,
                  std::function<void(std::unordered_map<string,std::any>&,std::unordered_set<std::string>&)> updateFunc,
                  std::unordered_set<std::string> status):attributes(std::move(attr)),behaviors(std::move(behs)),
                                                          updateFunction(std::move(updateFunc)),status(std::move(status)){};
        ~BasicLife()=default;
    };
}

#endif