//MIT License

//Copyright (c) 2026 Z-Multiplier

#ifndef UTILS_HPP
#define UTILS_HPP
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#include "Window.hpp"
#include <windows.h>
#include <string>
#include <random>

namespace Utils{
    BOOL classIsRegistered(wchar_t const*CLASSNAME);
    class Random{
        std::random_device dev;
        std::mt19937 gen;
        public:
            Random():gen(dev()){}
            int range(int min,int max);
            float real(int min,int max);
    };
    class Timer{
            std::chrono::steady_clock::time_point start;
        public:
            Timer(){reset();}
            void reset(){start=std::chrono::steady_clock::now();}
            double elapsed()const{
                return std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
            }
            bool reached(double seconds)const{return elapsed()>=seconds;}
    };
    inline std::string wstring2utf8(const std::wstring& wstr){
        if(wstr.empty()) return{};
        int size_needed=WideCharToMultiByte(CP_UTF8,0,wstr.c_str(),(int)wstr.size(),NULL,0,NULL,NULL);
        std::string utf8str(size_needed,0);
        WideCharToMultiByte(CP_UTF8,0,wstr.c_str(),(int)wstr.size(),&utf8str[0],size_needed,NULL,NULL);
        return utf8str;
    }
    inline std::wstring utf82wstring(const std::string& utf8str){
        if(utf8str.empty()) return std::wstring();
        int size_needed=MultiByteToWideChar(CP_UTF8,0,utf8str.c_str(),(int)utf8str.size(),NULL,0);
        std::wstring wstr(size_needed,0);
        MultiByteToWideChar(CP_UTF8,0,utf8str.c_str(),(int)utf8str.size(),&wstr[0],size_needed);
        return wstr;
    }
    namespace{
        inline int lowbit(int x){
            return x&-x;
        }
    }
    template<typename T>
    struct FenwickTree{
        vector<T> tree;
        int size;
        void add(int index,int delta){
            index++;
            while(index<=size){
                tree[index]+=delta;
                index+=lowbit(index);
            }
        }
        T sum(int index){
            index++;
            int res=0;
            while(index>0){
                res+=tree[index];
                index-=lowbit(index);
            }
            return res;
        }
        T query(int l,int r){
            return sum(r)-sum(l-1);
        }
        FenwickTree(int size,const vector<T>& target):size(size){
            tree.resize(size+1);
            for(int i=0;i<size;i++){
                add(i,target.at(i));
            }
        }
    };
    struct UnionFind{
        vector<int> parent;
        vector<int> rank;
        int size;
        UnionFind(int s){
            size=s;
            parent.resize(s);
            rank.resize(s);
            for(int i=0;i<s;i++){
                parent[i]=i;
                rank[i]=0;
            }
        }
        int find(int x){
            if(parent[x]!=x){
                parent[x]=find(parent[x]);
            }
            return parent[x];
        }
        void merge(int x,int y){
            int rootX=find(x);
            int rootY=find(y);
            if(rootX==rootY) return;
            if(rank[rootX]==rank[rootY]){
                rank[rootX]++;
                parent[rootY]=rootX;
            }
            else if(rank[rootX]<rank[rootY]){
                parent[rootX]=rootY;
            }
            else{
                parent[rootY]=rootX;
            }
        }
        bool query(int x,int y){
            return find(x)==find(y);
        }
    };
    template<typename T>
    struct SegmentTree{
        struct segmentNode{
            long long l,r;
            T val;
            T lazy;
        };
        long long size;
        vector<segmentNode> seg;
        vector<T> origin;
        void build(long long id,long long l,long long r){
            seg[id]={l,r,T(),T()};
            if(l==r){
                seg[id].val=origin[l];
                return;
            }
            build(id<<1,l,(l+r)/2);
            build(id<<1|1,(l+r)/2+1,r);
            seg[id].val=seg[id<<1].val+seg[id<<1|1].val;
        }
        SegmentTree(vector<T> origin):origin(origin){
            size=origin.size();
            seg.resize(4*size);
            build(1,0,size-1);
        }
        void pushDown(long long id){
            T& tag=seg[id].lazy;
            if(tag==T()) return;
            seg[id<<1].val+=tag*(seg[id<<1].r-seg[id<<1].l+1);
            seg[id<<1|1].val+=tag*(seg[id<<1|1].r-seg[id<<1|1].l+1);
            seg[id<<1].lazy+=tag;
            seg[id<<1|1].lazy+=tag;
            tag=0;
        }
        void modify(T val,long long l,long long r,long long id=1){
            if(l>seg[id].r||r<seg[id].l) return;
            if(l<=seg[id].l&&seg[id].r<=r){
                seg[id].val+=val*(seg[id].r-seg[id].l+1);
                seg[id].lazy+=val;
                return;
            }
            pushDown(id);
            modify(val,l,r,id<<1);
            modify(val,l,r,id<<1|1);
            seg[id].val=seg[id<<1].val+seg[id<<1|1].val;
        }
        T query(long long l,long long r,long long id=1){
            if(l>seg[id].r||r<seg[id].l) return T();
            if(l<=seg[id].l&&seg[id].r<=r){
                return seg[id].val;
            }
            pushDown(id);
            T lans=query(l,r,id<<1);
            T rans=query(l,r,id<<1|1);
            return lans+rans;
        }
    };
}

#endif