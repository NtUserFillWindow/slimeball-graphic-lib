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
std::vector<std::vector<float>> Game::Terrain::getCost()const{
    std::vector<std::vector<float>> ret(storage.size(),std::vector<float>(storage.back().size(),Game::Unpassable));
    for(size_t i=0;i<storage.size();i++){
        for(size_t j=0;j<storage[i].size();j++){
            ret[j][i]=costMap.at(storage[j][i]);
        }
    }
    return ret;
}
float Game::Terrain::operator()(int x,int y)const{
    if(x<0||y<0||x>static_cast<int>(storage.back().size())||y>static_cast<int>(storage.size())) return Game::Unpassable;
    return costMap.at(storage[x][y]);
}