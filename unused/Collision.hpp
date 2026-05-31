//MIT License

//Copyright (c) 2026 Z-Multiplier
#ifndef COLLISION_HPP
#define COLLISION_HPP
#include <vector>
#include <list>
#include <unordered_set>
#include <cmath>
#include "Painter.hpp"
#include <limits>
#include <mutex>
#include <optional>
#include "Logger.hpp"
namespace Physics{
    extern Core::logger physicsLogger;
    struct AABB{
        int left,up,width,height;
        AABB()=default;
        AABB(int l,int u,int w,int h):left(l),up(u),width(w),height(h){};
        bool collideX(const AABB& other)const{
            return left<=(other.left+other.width)&&(left+width)>=other.left;
        }
        bool collideY(const AABB& other)const{
            return up<=(other.up+other.height)&&(up+height)>=other.up;
        }
        bool collide(const AABB& other)const{
            return collideX(other)&&collideY(other);
        }
    };
    inline AABB triangleAABB(const std::tuple<Window::Point,Window::Point,Window::Point>& tri,
                      int deltaX=0,int deltaY=0,
                      double cosA=1.0,double sinA=0.0){
        auto applyTransform=[&](const Window::Point& p)->Window::Point {
            int rx=static_cast<int>(std::round(p.x*cosA-p.y*sinA));
            int ry=static_cast<int>(std::round(p.x*sinA+p.y*cosA));
            return {rx+deltaX,ry+deltaY};
        };
        auto [p1,p2,p3]=tri;
        Window::Point wp1=applyTransform(p1);
        Window::Point wp2=applyTransform(p2);
        Window::Point wp3=applyTransform(p3);
        int minX=std::min(wp1.x,std::min(wp2.x,wp3.x));
        int maxX=std::max(wp1.x,std::max(wp2.x,wp3.x));
        int minY=std::min(wp1.y,std::min(wp2.y,wp3.y));
        int maxY=std::max(wp1.y,std::max(wp2.y,wp3.y));
        return AABB(minX,minY,maxX-minX,maxY-minY);
    }
    inline Window::Point rotatePoint(const Window::Point& p,double cosA,double sinA){
        return {
            static_cast<int>(std::round(p.x*cosA-p.y*sinA)),
            static_cast<int>(std::round(p.x*sinA+p.y*cosA))
        };
    }
    inline double cross(const Window::Vec2& a,const Window::Vec2& b){
        return a.x*b.y-a.y*b.x;
    }
    inline Window::Vec2 sub(const Window::Point& a,const Window::Point& b){
        return {static_cast<double>(a.x-b.x),static_cast<double>(a.y-b.y)};
    }
    inline bool overlapOnAxis(const Window::Vec2& axis,
                       const std::tuple<Window::Point,Window::Point,Window::Point>& triA,
                       const std::tuple<Window::Point,Window::Point,Window::Point>& triB);
    inline bool triangleIntersect(const std::tuple<Window::Point,Window::Point,Window::Point>& triA,
                                  const std::tuple<Window::Point,Window::Point,Window::Point>& triB);
    inline std::vector<Window::Point> findContactPoints(const std::tuple<Window::Point,Window::Point,Window::Point>& triA,
                                                        const std::tuple<Window::Point,Window::Point,Window::Point>& triB){
        std::vector<Window::Point> points;
        auto [a1,a2,a3]=triA;
        auto [b1,b2,b3]=triB;
        auto pointInTriangle=[](const Window::Point& p,
                                const Window::Point& t1,
                                const Window::Point& t2,
                                const Window::Point& t3)->bool {
            double sign1=cross(sub(t2,t1),sub(p,t1));
            double sign2=cross(sub(t3,t2),sub(p,t2));
            double sign3=cross(sub(t1,t3),sub(p,t3));
            bool hasNeg=(sign1<0)||(sign2<0)||(sign3<0);
            bool hasPos=(sign1>0)||(sign2>0)||(sign3>0);
            return !(hasNeg&&hasPos);
        };
        if(pointInTriangle(a1,b1,b2,b3)) points.push_back(a1);
        if(pointInTriangle(a2,b1,b2,b3)) points.push_back(a2);
        if(pointInTriangle(a3,b1,b2,b3)) points.push_back(a3);
        if(pointInTriangle(b1,a1,a2,a3)) points.push_back(b1);
        if(pointInTriangle(b2,a1,a2,a3)) points.push_back(b2);
        if(pointInTriangle(b3,a1,a2,a3)) points.push_back(b3);
        auto edgeIntersection=[](const Window::Point& p1,const Window::Point& p2,
                                const Window::Point& q1,const Window::Point& q2)->std::optional<Window::Point> {
            Window::Vec2 r=sub(p2,p1);
            Window::Vec2 s=sub(q2,q1);
            double rxs=cross(r,s);
            if(std::abs(rxs)<1e-8) return std::nullopt;
            Window::Vec2 qp=sub(q1,p1);
            double t=cross(qp,s)/rxs;
            double u=cross(qp,r)/rxs;
            if(t>=0.0&&t<=1.0&&u>=0.0&&u<=1.0){
                return Window::Point{(int)std::round(p1.x+t*r.x),(int)std::round(p1.y+t*r.y)};
            }
            return std::nullopt;
        };
        std::vector<std::pair<Window::Point,Window::Point>> edgesA={{a1,a2},{a2,a3},{a3,a1}};
        std::vector<std::pair<Window::Point,Window::Point>> edgesB={{b1,b2},{b2,b3},{b3,b1}};
        for(auto& ea:edgesA){
            for(auto& eb:edgesB){
                if(auto ip=edgeIntersection(ea.first,ea.second,eb.first,eb.second)){
                    points.push_back(*ip);
                }
            }
        }
        return points;
    }
    inline std::tuple<bool,Window::Vec2,Window::Point> triangleIntersectWithMTV(
        const std::tuple<Window::Point,Window::Point,Window::Point>& triA,
        const std::tuple<Window::Point,Window::Point,Window::Point>& triB) 
    {
        std::vector<Window::Vec2> axes;
        auto addEdgeNormal=[&](const Window::Point& p1,const Window::Point& p2){
            Window::Vec2 edge={ (double)(p2.x-p1.x),(double)(p2.y-p1.y)};
            axes.push_back({edge.y,-edge.x});
        };
        auto [a1,a2,a3]=triA;
        auto [b1,b2,b3]=triB;
        addEdgeNormal(a1,a2);addEdgeNormal(a2,a3);addEdgeNormal(a3,a1);
        addEdgeNormal(b1,b2);addEdgeNormal(b2,b3);addEdgeNormal(b3,b1);
        auto project=[](const Window::Point& p,const Window::Vec2& axis)->double {
            return p.x*axis.x+p.y*axis.y;
        };
        double minOverlap=std::numeric_limits<double>::max();
        Window::Vec2 bestMTV={0.0,0.0};
        bool firstAxis=true;
        for(const auto& axis:axes){
            double projA1=project(a1,axis),projA2=project(a2,axis),projA3=project(a3,axis);
            double minA=std::min(projA1,std::min(projA2,projA3));
            double maxA=std::max(projA1,std::max(projA2,projA3));
            double projB1=project(b1,axis),projB2=project(b2,axis),projB3=project(b3,axis);
            double minB=std::min(projB1,std::min(projB2,projB3));
            double maxB=std::max(projB1,std::max(projB2,projB3));
            double overlap=std::min(maxA,maxB)-std::max(minA,minB);
            if(overlap<0){
                return {false,{0.0,0.0},{0,0}};
            }
            double axisLen=std::sqrt(axis.x*axis.x+axis.y*axis.y);
            if(axisLen<1e-8) continue;
            double overlapScaled=overlap/axisLen;
            double centerA=(minA+maxA)*0.5;
            double centerB=(minB+maxB)*0.5;
            double dir=(centerB-centerA)>=0?1.0:-1.0;
            Window::Vec2 mtv={axis.x/axisLen*overlapScaled*dir,
                              axis.y/axisLen*overlapScaled*dir};

            if(firstAxis||overlap<minOverlap){
                minOverlap=overlap;
                bestMTV=mtv;
                firstAxis=false;
            }
        }
        Window::Vec2 centerA={(a1.x+a2.x+a3.x)/3.0,(a1.y+a2.y+a3.y)/3.0};
        Window::Vec2 centerB={(b1.x+b2.x+b3.x)/3.0,(b1.y+b2.y+b3.y)/3.0};
        Window::Vec2 centerDiff={centerB.x-centerA.x,centerB.y-centerA.y};
        if(bestMTV.x*centerDiff.x+bestMTV.y*centerDiff.y<0){
            bestMTV={-bestMTV.x,-bestMTV.y};
        }
        auto contactPoints=findContactPoints(triA,triB);
        Window::Point contact{0,0};
        if(!contactPoints.empty()){
            for(const auto& p:contactPoints){
                contact.x+=p.x;
                contact.y+=p.y;
            }
            contact.x/=contactPoints.size();
            contact.y/=contactPoints.size();
        }
        else{
            contact.x=(a1.x+a2.x+a3.x+b1.x+b2.x+b3.x)/6;
            contact.y=(a1.y+a2.y+a3.y+b1.y+b2.y+b3.y)/6;
        }
        return {true,bestMTV,contact};
    }
    struct PolygonBox{
        std::vector<std::tuple<Window::Point,Window::Point,Window::Point>> triangles;
        AABB boundary;
        int deltaX,deltaY;
        double rotation;
        mutable double cachedCos=1.0;
        mutable double cachedSin=0.0;
        mutable bool cacheDirty=true;
        PolygonBox()=delete;
        PolygonBox(std::list<Window::Point> points,AABB boundary);
        void delta(int x,int y){
            deltaX+=x;deltaY+=y;
        }
        void rotate(double delta){
            rotation+=delta;cacheDirty=true;
        }
        void updateCache()const{
            if(cacheDirty){
                cachedCos=std::cos(rotation);
                cachedSin=std::sin(rotation);
                cacheDirty=false;
            }
        }
        Window::Vec2 calcCentroid()const{
            double totalArea=0.0;
            Window::Vec2 centroid={0.0,0.0};
            for(const auto& tri:triangles){
                auto [p1,p2,p3]=tri;
                double x1=p1.x,y1=p1.y;
                double x2=p2.x,y2=p2.y;
                double x3=p3.x,y3=p3.y;
                double area=0.5*std::abs((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1));
                totalArea+=area;
                double cx=(x1+x2+x3)/3.0;
                double cy=(y1+y2+y3)/3.0;
                centroid.x+=cx*area;
                centroid.y+=cy*area;
            }
            if(totalArea>0){
                centroid.x/=totalArea;
                centroid.y/=totalArea;
            }
            return centroid;
        }
    };
    template<int width,int height>
    struct CollideBoxManager{
        private:
        CollideBoxManager()=default;
        mutable std::mutex boxesMutex;
        public:
        std::vector<PolygonBox> boxes;
        ~CollideBoxManager()=default;
        static CollideBoxManager& instance(){static CollideBoxManager instance;return instance;};
        PolygonBox& operator[](size_t index){
            std::lock_guard<std::mutex> lg(boxesMutex);
            return boxes[index];
        }
        void operator+=(const PolygonBox& polygon){
            std::lock_guard<std::mutex> lg(boxesMutex);
            boxes.push_back(polygon);
        }
        void push_back(const PolygonBox& polygon){
            std::lock_guard<std::mutex> lg(boxesMutex);
            boxes.push_back(polygon);
        }
        void operator+=(PolygonBox&& polygon){
            std::lock_guard<std::mutex> lg(boxesMutex);
            boxes.push_back(std::move(polygon));
        }
        void push_back(PolygonBox&& polygon){
            std::lock_guard<std::mutex> lg(boxesMutex);
            boxes.push_back(std::move(polygon));
        }
        struct QuadTree{
            AABB boundary;
            int capacity;
            int maxDepth;
            int depth;
            bool divided;
            std::vector<std::pair<int,AABB>> objects;
            std::unique_ptr<QuadTree> children[4];
            QuadTree(const AABB& bound,int cap=8,int maxD=6,int d=0):boundary(bound),capacity(cap),maxDepth(maxD),depth(d),divided(false){}
            void subdivide(){
                int halfW=boundary.width/2;
                int halfH=boundary.height/2;
                int left=boundary.left;
                int up=boundary.up;
                children[0]=std::make_unique<QuadTree>(AABB(left,up,halfW,halfH),capacity,maxDepth,depth+1);
                children[1]=std::make_unique<QuadTree>(AABB(left+halfW,up,halfW,halfH),capacity,maxDepth,depth+1);
                children[2]=std::make_unique<QuadTree>(AABB(left,up+halfH,halfW,halfH),capacity,maxDepth,depth+1);
                children[3]=std::make_unique<QuadTree>(AABB(left+halfW,up+halfH,halfW,halfH),capacity,maxDepth,depth+1);
                for(auto& obj:objects){
                    for(int i=0;i<4;i++)
                        children[i]->insert(obj.first,obj.second);
                }
                objects.clear();
                divided=true;
            }
            bool insert(int idx,const AABB& objBound){
                if(!boundary.collide(objBound)) return false;
                if(!divided&&objects.size()<(size_t)capacity&&depth<maxDepth){
                    objects.emplace_back(idx,objBound);
                    return true;
                }
                if(!divided&&depth<maxDepth){
                    subdivide();
                }
                if(divided){
                    bool inserted=false;
                    for(int i=0;i<4;i++){
                        if(children[i]->insert(idx,objBound))
                            inserted=true;
                    }
                    if(inserted) return true;
                }
                objects.emplace_back(idx,objBound);
                return true;
            }
            void query(const AABB& range,std::vector<int>& result)const{
                if(!boundary.collide(range)) return;
                for(auto& [idx,box]:objects){
                    if(box.collide(range))
                        result.push_back(idx);
                }
                if(divided){
                    for(int i=0;i<4;i++)
                        children[i]->query(range,result);
                }
            }
        };
        std::vector<std::tuple<int,int,Window::Vec2,Window::Point>> collisions(){
            std::lock_guard<std::mutex> lg(boxesMutex);
            std::vector<std::tuple<int,int,Window::Vec2,Window::Point>> result;
            if(boxes.empty()) return result;
            int minX=0,minY=0,maxX=width,maxY=height;
            std::vector<AABB> worldBoxes(boxes.size());
            for(size_t i=0;i<boxes.size();i++){
                const auto& box=boxes[i];
                box.updateCache();
                AABB world={INT_MAX,INT_MAX,0,0};
                for(const auto& tri:box.triangles){
                    AABB triWorld=triangleAABB(tri,box.deltaX,box.deltaY,box.cachedCos,box.cachedSin);
                    int left=std::min(world.left,triWorld.left);
                    int up=std::min(world.up,triWorld.up);
                    int right=std::max(world.left+world.width,triWorld.left+triWorld.width);
                    int bottom=std::max(world.up+world.height,triWorld.up+triWorld.height);
                    world=AABB(left,up,right-left,bottom-up);
                }
                worldBoxes[i]=world;
                minX=std::min(minX,world.left);
                minY=std::min(minY,world.up);
                maxX=std::max(maxX,world.left+world.width);
                maxY=std::max(maxY,world.up+world.height);
            }
            AABB worldBoundary(minX,minY,maxX-minX,maxY-minY);
            QuadTree tree(worldBoundary,8,6);
            for(size_t i=0;i<boxes.size();i++){
                tree.insert(static_cast<int>(i),worldBoxes[i]);
            }
            auto getWorldTriangle=[&](const std::tuple<Window::Point,Window::Point,Window::Point>& tri,
                                      int dx,int dy,double cosA,double sinA){
                auto transform=[&](const Window::Point& p)->Window::Point {
                    int rx=static_cast<int>(std::round(p.x*cosA-p.y*sinA));
                    int ry=static_cast<int>(std::round(p.x*sinA+p.y*cosA));
                    return {rx+dx,ry+dy};
                };
                auto [p1,p2,p3]=tri;
                return std::make_tuple(transform(p1),transform(p2),transform(p3));
            };
            for(size_t i=0;i<boxes.size();i++){
                std::vector<int> candidates;
                tree.query(worldBoxes[i],candidates);
                std::unordered_set<int> uniqueCandidates(candidates.begin(),candidates.end());
                for(int j:uniqueCandidates){
                    if(j<=static_cast<int>(i)) continue;
                    if(!worldBoxes[i].collide(worldBoxes[j])) continue;
                    Window::Vec2 bestMTV={0,0};
                    Window::Point bestContact={0,0};
                    double minDepthSq=std::numeric_limits<double>::max();
                    bool anyCollide=false;
                    const auto& boxA=boxes[i];
                    const auto& boxB=boxes[j];
                    for(const auto& triA:boxA.triangles){
                        auto worldTriA=getWorldTriangle(triA,boxA.deltaX,boxA.deltaY,boxA.cachedCos,boxA.cachedSin);
                        for(const auto& triB:boxB.triangles){
                            auto worldTriB=getWorldTriangle(triB,boxB.deltaX,boxB.deltaY,boxB.cachedCos,boxB.cachedSin);
                            auto [collide,mtv,contact]=triangleIntersectWithMTV(worldTriA,worldTriB);
                            if(collide){
                                anyCollide=true;
                                double depthSq=mtv.x*mtv.x+mtv.y*mtv.y;
                                if(depthSq<minDepthSq){
                                    minDepthSq=depthSq;
                                    bestMTV=mtv;
                                    bestContact=contact;
                                }
                            }
                        }
                    }
                    if(anyCollide){
                        result.emplace_back(static_cast<int>(i),j,bestMTV,bestContact);
                    }
                }
            }
            return result;
        }
    };
}
#endif