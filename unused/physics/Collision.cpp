//MIT License

//Copyright (c) 2026 Z-Multiplier
#include "Collision.hpp"
#include "Logger.hpp"
namespace Physics{
    Core::logger physicsLogger;
}
Physics::PolygonBox::PolygonBox(std::list<Window::Point> points,AABB boundary)
                        :boundary(boundary),deltaX(0),deltaY(0),rotation(0.0)
{
    if(points.size()<3) return;
    auto cross2=[](const Window::Point& O,const Window::Point& A,const Window::Point& B)->int {
        return (A.x-O.x)*(B.y-O.y)-(A.y-O.y)*(B.x-O.x);
    };
    auto removeCollinear=[&](std::list<Window::Point>& lst){
        if(lst.size()<3) return;
        auto it=lst.begin();
        while(lst.size()>=3&&it!=lst.end()){
            auto prev=(it==lst.begin())?std::prev(lst.end()):std::prev(it);
            auto next=std::next(it);
            if(next==lst.end()) next=lst.begin();
            int c=cross2(Window::toVec2(*prev),Window::toVec2(*it),Window::toVec2(*next));
            if(c==0){
                it=lst.erase(it);
                if(it==lst.end()) it=lst.begin();
            }
            else{
                ++it;
            }
        }
    };
    removeCollinear(points);
    if(points.size()<3) return;
    double area=0;
    auto it=points.begin();
    auto prev_it=std::prev(points.end());
    for(;it!=points.end();++it){
        area+=(prev_it->x+it->x)*(prev_it->y-it->y);
        prev_it=it;
    }
    if(area<0){
        points.reverse();
    }
    auto isConvex=[&](const Window::Point& prev,const Window::Point& curr,const Window::Point& next)->bool {
        return cross2(prev,curr,next)<0;
    };
    auto pointInTriangle=[&](const Window::Point& P,
                             const Window::Point& A,
                             const Window::Point& B,
                             const Window::Point& C)->bool {
        int d1=cross2(A,B,P);
        int d2=cross2(B,C,P);
        int d3=cross2(C,A,P);
        bool hasPos=(d1 > 0)||(d2 > 0)||(d3 > 0);
        bool hasNeg=(d1<0)||(d2<0)||(d3<0);
        return !(hasPos&&hasNeg);
    };
    std::list<Window::Point> vertices=points;
    triangles.clear();
    while(vertices.size()>3){
        bool earFound=false;
        auto it=vertices.begin();
        while(it!=vertices.end()){
            auto prev=(it==vertices.begin())?std::prev(vertices.end()):std::prev(it);
            auto next=std::next(it);
            if(next==vertices.end()) next=vertices.begin();
            if(!isConvex(*prev,*it,*next)){
                ++it;
                continue;
            }
            bool hasInside=false;
            for(auto vit=vertices.begin();vit!=vertices.end();++vit){
                if(vit==prev||vit==it||vit==next) continue;
                if(pointInTriangle(*vit,*prev,*it,*next)){
                    hasInside=true;
                    break;
                }
            }
            if(!hasInside){
                triangles.emplace_back(*prev,*it,*next);
                it=vertices.erase(it);
                earFound=true;
                break;
            }
            else{
                ++it;
            }
        }
        if(!earFound){
            break;
        }
    }
    if(vertices.size()==3){
        auto it=vertices.begin();
        Window::Point v0=*it;++it;
        Window::Point v1=*it;++it;
        Window::Point v2=*it;
        triangles.emplace_back(v0,v1,v2);
    }
}
inline bool Physics::overlapOnAxis(const Window::Vec2& axis,
                   const std::tuple<Window::Point,Window::Point,Window::Point>& triA,
                   const std::tuple<Window::Point,Window::Point,Window::Point>& triB){
    auto project=[&](const Window::Point& p)->double {
        return p.x*axis.x+p.y*axis.y;
    };
    auto [a1,a2,a3]=triA;
    auto [b1,b2,b3]=triB;
    double minA=std::min(project(a1),std::min(project(a2),project(a3)));
    double maxA=std::max(project(a1),std::max(project(a2),project(a3)));
    double minB=std::min(project(b1),std::min(project(b2),project(b3)));
    double maxB=std::max(project(b1),std::max(project(b2),project(b3)));
    return !(maxA<minB||maxB<minA);
}
inline bool Physics::triangleIntersect(const std::tuple<Window::Point,Window::Point,Window::Point>& triA,
                                const std::tuple<Window::Point,Window::Point,Window::Point>& triB){
    std::vector<Window::Vec2> axes;
    auto addEdgeNormal=[&](const Window::Point& p1,const Window::Point& p2){
        Window::Vec2 edge=sub(p2,p1);
        axes.push_back({edge.y,-edge.x});
    };
    auto [a1,a2,a3]=triA;
    auto [b1,b2,b3]=triB;
    addEdgeNormal(a1,a2);addEdgeNormal(a2,a3);addEdgeNormal(a3,a1);
    addEdgeNormal(b1,b2);addEdgeNormal(b2,b3);addEdgeNormal(b3,b1);
    for(const auto& axis:axes){
        if(!overlapOnAxis(axis,triA,triB)) return false;
    }
    return true;
}