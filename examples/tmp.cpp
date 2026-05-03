#define IGNORE_WARNING_LOG
#include "Graphics.hpp"
#include "Color.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
using namespace Graphics;

//这里是所有可以调的参数
int check_speed=3;   //每多少帧模拟一次液体流动
int gravity_x=0;      //x轴重力
int gravity_y=5;      //y轴重力
int sticky=0.2;       //液体粘性，数值越大越不容易流动

int randomBinary() {
    static std::default_random_engine generator(
        std::chrono::system_clock::now().time_since_epoch().count()
    );
    static std::uniform_int_distribution<int> distribution(0, 1);
    return distribution(generator);
}
std::vector<Point> getLinePixels(Point p0, Point p1) {
    std::vector<Point> pixels;
    if (p0.x == p1.x && p0.y == p1.y) {
        pixels.push_back(p0);
        return pixels;
    }
    int dx = std::abs(p1.x - p0.x);
    int dy = std::abs(p1.y - p0.y);
    int sx = (p0.x < p1.x) ? 1 : -1;
    int sy = (p0.y < p1.y) ? 1 : -1;
    int err = dx - dy;
    int x = p0.x;
    int y = p0.y;
    while (true) {
        pixels.push_back({x, y});
        if (x == p1.x && y == p1.y) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
    return pixels;
}
struct Pixel{
    int type;
    int x_speed;
    int y_speed;
};
std::vector<std::vector<Pixel>> map(80, std::vector<Pixel>(100, Pixel{0, 0, 0}));
std::vector<std::vector<Pixel>> maps2(80, std::vector<Pixel>(100, Pixel{0, 0, 0}));
std::vector<std::vector<Pixel>> old_map(80, std::vector<Pixel>(100, Pixel{0, 0, 0}));
bool drag_mode=true;
bool left_drag=true;
bool dragging=false;
Point old_point{-1,-1};
Point now_point;
int check_clock=0;
bool init=true;
long long mainWindowDrawer(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,Painter& painter){
    if(init){painter.drawBackground(Color((unsigned char)0,0,0,255));init=false;}
    check_clock++;
    if(check_clock>=check_speed){
        check_clock=0;
        for(int i=1;i<79;i++){
            for(int j=1;j<99;j++){
                if(map[i][j].type==2){

                    map=maps2;
                }
            }
        }
    }   
    for(int i=0;i<80;i++){
        for(int j=0;j<100;j++){
            if(old_map[i][j].type!=map[i][j].type || i==0 || i==79 || j==0 || j==98){
                std::vector<Point> tmpp={
                    Point(j*10,i*10),
                    Point(j*10+10,i*10),
                    Point(j*10+10,i*10+10),
                    Point(j*10,i*10+10)
                };
                if(map[i][j].type==1){
                    painter.solidPolygon(tmpp,Color((unsigned char)200,200,200,255));
                }
                if(map[i][j].type==2){
                    painter.solidPolygon(tmpp,Color((unsigned char)0,0,200,255));
                }
                else if(map[i][j].type==0){
                    painter.solidPolygon(tmpp,Color((unsigned char)0,0,0,255));
                }
            }
        }
    }
    old_map=map;
    return 0;
}

long long left_click(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y){
    dragging=false;
    old_point={-1,-1};
    return 0;
}
long long right_click(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y){
    dragging=false;
    old_point={-1,-1};
    return 0;
}
long long mouse_move(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y){
    if(dragging){
        now_point={x,y};
        int gridX=x/10;
        int gridY=y/10;
        if(gridX>=1 && gridX<98 && gridY>=1 && gridY<79){
            if(drag_mode){
                if(old_point.x!=-1 && old_point.y!=-1){
                    std::vector<Point> linePixels=getLinePixels(old_point,now_point);
                    for(const auto& pixel:linePixels){
                        int gridX=pixel.x/10;
                        int gridY=pixel.y/10;
                        if(gridX>=1 && gridX<98 && gridY>=1 && gridY<79){
                            map[gridY][gridX].type=left_drag?1:2;
                        }
                    }
                }
            }
            else{
                if(old_point.x!=-1 && old_point.y!=-1){
                    std::vector<Point> linePixels=getLinePixels(old_point,now_point);
                    for(const auto& pixel:linePixels){
                        int gridX=pixel.x/10;
                        int gridY=pixel.y/10;
                        if(gridX>=1 && gridX<98 && gridY>=1 && gridY<79){
                            map[gridY][gridX].type=0;
                        }
                    }
                }
                map[gridY][gridX].type=0;
            }
        }
        old_point=now_point;
    }
    return 0;
}
long long mouse_held_right(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y,unsigned long long duration){
    dragging=false;
    old_point={-1,-1};
    return 0;
}
long long mouse_instant_right(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y){
    int gridX=x/10;
    int gridY=y/10;
    dragging=true;
    left_drag=false;
    if(gridX>=1 && gridX<98 && gridY>=1 && gridY<76){
        drag_mode=!map[gridY][gridX].type;
    }
    return 0;
}
long long mouse_held_left(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y,unsigned long long duration){
    dragging=false;
    old_point={-1,-1};
    return 0;
}
long long mouse_instant_left(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam,int x,int y){
    int gridX=x/10;
    int gridY=y/10;
    left_drag=true;
    dragging=true;
    if(gridX>=1 && gridX<98 && gridY>=1 && gridY<76){
        drag_mode=!map[gridY][gridX].type;
    }
    return 0;
}
int main(){
    srand(time(0));
    auto mainWindow=createInitWindow(300,300,997,950,L"液体模拟");

    std::function<long long(HWND,UINT,WPARAM,LPARAM,Painter&)> mainWindowDrawerFunc=mainWindowDrawer;
    mainWindow.first->thisPaint=mainWindowDrawerFunc;

    std::function<long long(HWND,UINT,WPARAM,LPARAM,int x,int y)> left_click_func=left_click;
    mainWindow.first->thisLeftClick=left_click_func;

    std::function<long long(HWND,UINT,WPARAM,LPARAM,int x,int y)> right_click_func=right_click;
    mainWindow.first->thisRightClick=right_click_func;

    std::function<long long(HWND,UINT,WPARAM,LPARAM,int x,int y,unsigned long long)> mouse_held_func_right=mouse_held_right;
    std::function<long long(HWND,UINT,WPARAM,LPARAM,int x,int y,unsigned long long)> mouse_held_func_left=mouse_held_left;
    mainWindow.first->thisLeftHeld=mouse_held_func_left;
    mainWindow.first->thisRightHeld=mouse_held_func_right;

    std::function<long long(HWND,UINT,WPARAM,LPARAM,int x,int y)> mouse_instant_func_right=mouse_instant_right;
    std::function<long long(HWND,UINT,WPARAM,LPARAM,int x,int y)> mouse_instant_func_left=mouse_instant_left;
    mainWindow.first->thisInstantLeftClick=mouse_instant_func_left;
    mainWindow.first->thisInstantRightClick=mouse_instant_func_right;

    std::function<long long(HWND,UINT,WPARAM,LPARAM,int x,int y)> mouse_move_func=mouse_move;
    mainWindow.first->thisMouseMove=mouse_move_func;

    for(int i=0;i<80;i++){
        for(int j=0;j<100;j++){
            if(i==0 || i==79){
                map[i][j].type=1;
            }
            if(j==0 || j==98){
                map[i][j].type=1;
            }
        }
    }
    maps2=map;
    old_map=map;
    MSG msg={};
    while(msg.message!=WM_QUIT){
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(16);
        globalHandleManager.updateAll();
        PostMessage(mainWindow.second,WM_PAINT,0,0);
    }
    return 0;
}