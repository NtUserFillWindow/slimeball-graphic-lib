//MIT License

//Copyright (c) 2026 Z-Multiplier
#ifndef PHYSICOBJECT_HPP
#define PHYSICOBJECT_HPP
#include "Painter.hpp"
#include "Collision.hpp"

namespace Physics{
    extern Window::Vec2 Gravity;
    extern double AirResistance;
    struct Force{
        Window::Vec2 point;
        Window::Vec2 force;
        Force()=default;
        Force(Window::Vec2 p,Window::Vec2 vec):point(p),force(vec){};
    };
    struct Impulse{
        Window::Vec2 point;
        Window::Vec2 force;
        Impulse()=default;
        Impulse(Window::Vec2 p,Window::Vec2 vec):point(p),force(vec){};
    };
    template<int WorldWidth,int WorldHeight>
    struct RigidBody{
        size_t bindedPolygon;
        Window::Vec2 pos;
        Window::Vec2 vel;
        double angle;
        double anglevel;
        double mass;
        double invmass;
        double inertia;
        double invinertia;
        Window::Vec2 forceAcc;
        double torqueAcc;
        bool onGround=false;
        Window::Vec2 contactNormal={0,0};
        RigidBody():bindedPolygon(0),pos{0,0},vel{0,0},angle(0),anglevel(0),
                    mass(1),invmass(1),inertia(1),invinertia(1),
                    forceAcc{0,0},torqueAcc(0){}
        RigidBody(size_t polygonIndex,
                  const Window::Vec2& position,
                  double mass,
                  double inertia,
                  const Window::Vec2& velocity={0,0},
                  double angularVel=0)
                  :bindedPolygon(polygonIndex),
                  pos(position),
                  vel(velocity),
                  angle(0),
                  anglevel(angularVel),
                  mass(mass),
                  invmass(mass>0?1.0/mass:0.0),
                  inertia(inertia),
                  invinertia(inertia>0?1.0/inertia:0.0),
                  forceAcc{0,0},
                  torqueAcc(0){}
        void sync(){
            auto& box=CollideBoxManager<WorldWidth,WorldHeight>::instance()[bindedPolygon];
            Window::Vec2 localCentroid=box.calcCentroid();
            double cosA=std::cos(angle);
            double sinA=std::sin(angle);
            double rx=localCentroid.x*cosA-localCentroid.y*sinA;
            double ry=localCentroid.x*sinA+localCentroid.y*cosA;
            box.deltaX=(int)std::round(pos.x-rx);
            box.deltaY=(int)std::round(pos.y-ry);
            box.rotation=angle;
            box.cacheDirty=true;
        }
        void applyForce(const Window::Vec2& f,const Window::Vec2& worldPoint){
            forceAcc.x+=f.x;
            forceAcc.y+=f.y;
            Window::Vec2 r={worldPoint.x-pos.x,worldPoint.y-pos.y};
            torqueAcc+=r.x*f.y-r.y*f.x;
        }
        void applyImpulse(const Window::Vec2& j,const Window::Vec2& worldPoint){
            if(invmass==0.0) return;
            vel.x+=j.x*invmass;
            vel.y+=j.y*invmass;
            Window::Vec2 r={worldPoint.x-pos.x,worldPoint.y-pos.y};
            anglevel+=(r.x*j.y-r.y*j.x)*invinertia;
        }
        void applyForce(const Physics::Force force){
            applyForce(force.force,force.point);
        }
        void applyImpulse(const Physics::Impulse impulse){
            applyImpulse(impulse.force,impulse.point);
        }
        void clearAccumulators(){
            forceAcc={0.0,0.0};
            torqueAcc=0.0;
        }
        void integrate(double dt){
            if(invmass==0.0) return;
            bool hasContact=(contactNormal.x!=0.0||contactNormal.y!=0.0);
            double velLenSq=vel.x*vel.x+vel.y*vel.y;
            if(hasContact&&Gravity.x*contactNormal.x+Gravity.y*contactNormal.y>-1e-6){
                contactNormal={0,0};
                hasContact=false;
            }
            if(hasContact&&velLenSq<0.01){
                vel={0,0};
                anglevel=0;
                sync();
                return;
            }
            double drag=1.0-AirResistance*dt;
            if(drag<0.0) drag=0.0;
            vel.x*=drag;
            vel.y*=drag;
            anglevel+=torqueAcc*invinertia*dt;
            if(hasContact){
                if(Gravity.x*contactNormal.x+Gravity.y*contactNormal.y>-1e-6){
                    contactNormal={0.0,0.0};
                    hasContact=false;
                }
                else{
                    double vn=vel.x*contactNormal.x+vel.y*contactNormal.y;
                    if(vn<0){
                        vel.x-=vn*contactNormal.x;
                        vel.y-=vn*contactNormal.y;
                    }
                    Window::Vec2 delta={vel.x*dt,vel.y*dt};
                    double dn=delta.x*contactNormal.x+delta.y*contactNormal.y;
                    if(dn<0){
                        delta.x-=dn*contactNormal.x;
                        delta.y-=dn*contactNormal.y;
                    }
                    pos.x+=delta.x;
                    pos.y+=delta.y;
                }
            }
            else{
                vel.x+=forceAcc.x*invmass*dt;
                vel.y+=forceAcc.y*invmass*dt;
                pos.x+=vel.x*dt;
                pos.y+=vel.y*dt;
            }
            angle+=anglevel*dt;
            sync();
        }
    };
    template<int WorldWidth,int WorldHeight>
    struct PhysicsWorld{
    public:
        std::vector<RigidBody<WorldWidth,WorldHeight>> bodies;
        void update(double dt,int iterations=8){
            for(auto& body:bodies)
                body.clearAccumulators();
            for(auto& body:bodies){
                bool hasSupport=(body.contactNormal.x!=0.0||body.contactNormal.y!=0.0);
                if(body.invmass!=0.0&&!hasSupport){
                    body.applyForce({Gravity.x*body.mass,Gravity.y*body.mass},body.pos);
                }
            }
            for(auto& body:bodies){
                body.integrate(dt);
            }
            std::vector<bool> hadCollision(bodies.size(),false);
            for(int iter=0;iter<iterations;iter++){
                auto& manager=CollideBoxManager<WorldWidth,WorldHeight>::instance();
                auto collisions=manager.collisions();
                for(const auto& [i,j,mtv]:collisions){
                    resolveCollision(bodies[i],bodies[j],mtv);
                    hadCollision[i]=true;
                    hadCollision[j]=true;
                }
            }
            for(auto& body:bodies){
                body.sync();
            }
            for(size_t i=0;i<bodies.size();++i){
                if(!hadCollision[i]){
                    bodies[i].onGround=false;
                    bodies[i].contactNormal={0.0,0.0};
                }
            }
        }
    private:
        void resolveCollision(RigidBody<WorldWidth,WorldHeight>& a,
                              RigidBody<WorldWidth,WorldHeight>& b,
                              const Window::Vec2& mtv){
            double depth=std::sqrt(mtv.x*mtv.x+mtv.y*mtv.y);
            if(depth<1e-8) return;
            Window::Vec2 n1={mtv.x/depth,mtv.y/depth};
            Window::Vec2 n={-mtv.x/depth,-mtv.y/depth};
            double totalInvMass=a.invmass+b.invmass;
            if(totalInvMass>0.0){
                double factorA=a.invmass/totalInvMass;
                double factorB=b.invmass/totalInvMass;
                a.pos.x-=n1.x*depth*factorA;
                a.pos.y-=n1.y*depth*factorA;
                b.pos.x+=n1.x*depth*factorB;
                b.pos.y+=n1.y*depth*factorB;
            }
            double relVel=(a.vel.x-b.vel.x)*n.x+(a.vel.y-b.vel.y)*n.y;
            if(relVel<0){
                double e=0.2;
                double invMassSum=a.invmass+b.invmass;
                if(invMassSum>0.0){
                    double j=-(1.0+e)*relVel/invMassSum;
                    a.vel.x+=j*n.x*a.invmass;
                    a.vel.y+=j*n.y*a.invmass;
                    b.vel.x-=j*n.x*b.invmass;
                    b.vel.y-=j*n.y*b.invmass;
                }
            }
            a.sync();
            b.sync();
        }
    };
}
#endif