//MIT License

//Copyright (c) 2026 Z-Multiplier
#include "GameObject.hpp"
namespace Game{
   Core::logger gameLogger; 
}
bool Game::AIBehavior::check(const std::unordered_map<string,std::any>& attributes,const std::unordered_set<std::string>& status)const{
    for(const auto& f:this->conditions){
        if(!f(attributes,status)){
            return false;
        }
    }
    return true;
}
void Game::BasicLife::pushBehavior(const AIBehavior& beh){
    this->behaviors.emplace_back(beh);
}
void Game::BasicLife::update(){
    this->updateFunction(attributes,status);
    for(const auto& beh:this->behaviors){
        if(beh.check(attributes,status)){
            beh.behavior(attributes,status);
            return;
        }
    }
}