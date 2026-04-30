#define IGNORE_WARNING_LOG
#include "Graphics.hpp"
#include "BasicUI.hpp"
#include "Game.hpp"
using namespace Graphics;
int main(){
    Game::Terrain terrain({{"WALL" ,"WALL" ,"WALL" ,"WALL" ,"WALL"},
                           {"WALL" ,"EMPTY","EMPTY","EMPTY","WALL"},
                           {"WALL" ,"WALL" ,"WALL" ,"EMPTY","WALL"},
                           {"WALL" ,"EMPTY","EMPTY","EMPTY","WALL"},
                           {"WALL" ,"WALL" ,"WALL" ,"WALL" ,"WALL"}}
        ,{{"EMPTY",0.1f},{"WALL",Game::Unpassable}});
    Clock c([&](){
        auto st=std::chrono::high_resolution_clock::now();
        std::vector<Game::Point> path=Game::Astar({1,1},{3,1},terrain,Game::ExpandMode::EIGHTDIR,
                                                [&](Game::Point a,Game::Point b){
                                                    return terrain(a.x,a.y)+terrain(b.x,b.y);
                                                },
                                                [&](Game::Point a,Game::Point b){
                                                    return std::sqrt(std::pow(a.x-b.x,2)+std::pow(a.y-b.y,2));
                                                });
        auto ed=std::chrono::high_resolution_clock::now();
        for(const auto& p:path){
            std::cerr<<"x:"<<p.x<<"y:"<<p.y<<std::endl;
        }
        std::cerr<<(ed-st).count()<<std::endl;
        return;
    },1LL);//clock
    while(c){
        c.run();
    }
    return 0;
}