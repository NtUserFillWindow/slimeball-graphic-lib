#define IGNORE_WARNING_LOG
#include "Graphics.hpp"
#include "BasicUI.hpp"
#include "Game.hpp"
#include "Animation.hpp"
#include "Physics.hpp"
#include <algorithm>

using namespace Graphics;

int main() {
    auto mainWindow = createInitWindow(0, 0, 407, 430, L"Physics Demo");
    constexpr int WORLD_W = 401, WORLD_H = 401;

    // 1. 先向碰撞管理器添加多边形
    auto& manager = Physics::CollideBoxManager<WORLD_W, WORLD_H>::instance();

    // 地面（矩形，静态）
    std::list<Window::Point> groundPoints = { {0, 380}, {400, 380}, {400, 400}, {0, 400} };
    Physics::PolygonBox ground(groundPoints, {0, 380, 400, 20});
    manager += ground;   // 索引 0

    // 动态方块（正方形）
    std::list<Window::Point> boxPoints = { {180, 100}, {220, 100}, {220, 140}, {180, 140} };
    Physics::PolygonBox box(boxPoints, {180, 100, 40, 40});
    manager += box;      // 索引 1

    std::list<Window::Point> ceilPoints = { {0, 0}, {400, 0}, {400, 20}, {0, 20} };
    Physics::PolygonBox ceil(ceilPoints, {0, 0, 400, 20});
    manager += ceil;   // 索引 2

    std::list<Window::Point> wallPoints = { {380, 20}, {400, 20}, {400, 380}, {380, 380} };
    Physics::PolygonBox wall(wallPoints, {380, 20, 20, 400});
    manager += wall;   // 索引 3
    
    std::list<Window::Point> walllPoints = { {0, 20}, {20, 20}, {20, 380}, {0, 380} };
    Physics::PolygonBox walll(walllPoints, {0, 20, 20, 400});
    manager += walll;   // 索引 4
    // 2. 创建物理世界和刚体
    Physics::PhysicsWorld<WORLD_W, WORLD_H> pw;

    // 地面刚体（静态）
    pw.bodies.emplace_back(0,                  // 多边形索引
                           Window::Vec2{200, 390},  // 质心位置
                           0.0, 0.0);          // 质量、惯量 = 0 表示静态
    // 方块刚体（动态）
    double mass = 1.0;
    double inertia = mass * (40*40 + 40*40) / 12.0;  // 正方形惯量近似
    pw.bodies.emplace_back(1,                  // 多边形索引
                           Window::Vec2{200, 120},  // 初始质心
                           mass, inertia,
                           Window::Vec2{0, 0},     // 水平初速度
                           0.0);                 // 角速度初值

    pw.bodies.emplace_back(2,
                           Window::Vec2{200,10},
                           0.0,0.0);

    pw.bodies.emplace_back(3,
                           Window::Vec2{390,200},
                           0.0,0.0);

    
    pw.bodies.emplace_back(4,
                           Window::Vec2{20,200},
                           0.0,0.0);

    // 设置重力
    Physics::Gravity = {0.0, 98.0};
    Physics::AirResistance = 0.05;
    // 渲染回调
    mainWindow.first->thisPaint = [&](HWND, UINT, WPARAM, LPARAM, Painter& p) -> long long {
        p.drawBackground(Color((unsigned char)0, 0, 0));   // 黑色背景

        // 获取所有多边形
        const auto& boxes = manager.boxes;
        for (size_t i = 0; i < boxes.size(); ++i) {
            const auto& box = boxes[i];
            box.updateCache();  // 确保旋转缓存最新
            double cosA = box.cachedCos;
            double sinA = box.cachedSin;
            int dx = box.deltaX;
            int dy = box.deltaY;

            // 遍历所有三角形
            for (const auto& tri : box.triangles) {
                auto [p1, p2, p3] = tri;

                // 应用旋转变换
                auto transform = [&](const Window::Point& pt) -> Window::Point {
                    int rx = static_cast<int>(std::round(pt.x * cosA - pt.y * sinA));
                    int ry = static_cast<int>(std::round(pt.x * sinA + pt.y * cosA));
                    return { rx + dx, ry + dy };
                };

                Window::Point wp1 = transform(p1);
                Window::Point wp2 = transform(p2);
                Window::Point wp3 = transform(p3);

                // 绘制三角形边框（使用白色线条）
                p.line(wp1, wp2, Color((unsigned char)255, 255, 255));
                p.line(wp2, wp3, Color((unsigned char)255, 255, 255));
                p.line(wp3, wp1, Color((unsigned char)255, 255, 255));
            }
        }

        return 0;
    };
    mainWindow.first->thisLeftClick=[&](HWND,UINT,WPARAM,LPARAM,int,int)->long long {
        pw.bodies[1].applyImpulse({10,-100},pw.bodies[1].pos);
        return 0;
    };
    // 物理定时器
    Clock c([&](){
        pw.update(1.0 / 60.0);   // 固定时间步长 60 FPS
    });
    while(c){
        c.run();
        std::cerr<<c.fps()<<std::endl;
    }

    return 0;
}