//MIT License

//Copyright (c) 2026 wzxTheSlimeball

#include "Window.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <memory>
#include <algorithm>
namespace Window{
    Core::logger WindowLogger;
    HandleManager globalHandleManager;
    std::unordered_map<HWND,Handle*> Handle::mHWndMap;
    LRESULT CALLBACK Handle::Handle::thisWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
        Handle *pThis=nullptr;
        if(mHWndMap.find(hWnd)!=mHWndMap.end()){
            pThis=mHWndMap[hWnd];
        }
        if(uMsg==WM_NCCREATE||uMsg==WM_CREATE){
            CREATESTRUCT* cs=reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis=reinterpret_cast<Handle*>(cs->lpCreateParams);
            if(pThis){
                pThis->mHWnd=hWnd;
                mHWndMap[hWnd]=pThis;
            }
        }
        if(pThis==nullptr){
            WindowLogger.traceLog(Core::logger::LOG_INFO,"Failed to find pointer \"pThis\",This will return default(or if this is the first log that the Window throws,it's all fine)");
            return DefWindowProc(hWnd,uMsg,wParam,lParam);
        }
        if(pThis->mID==0){
            WindowLogger.traceLog(Core::logger::LOG_ERROR,"You have not registered this to the manager yet!");
            return 0;
        }
        switch(uMsg){
            case WM_DESTROY:{
                if(!pThis->thisDestroy){
                    WindowLogger.traceLog(Core::logger::LOG_WARNING,"The function\"thisDestroy\" is not defined yet,Skipping.");
                }
                else{
                    pThis->thisDestroy(hWnd,uMsg,wParam,lParam);
                }
                pThis->destroy();
                break;
            }
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc=BeginPaint(hWnd,&ps);
                FillRect(hdc,&ps.rcPaint,(HBRUSH)(COLOR_WINDOW+1));
                EndPaint(hWnd,&ps);
                break;
            }
        }
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }
    HWND Handle::Handle::initWindow(const wchar_t* className,HINSTANCE hInstance){
        if(Utils::classIsRegistered(className)==FALSE){
            const wchar_t *CLASSNAME=className;
            WNDCLASS wc={};
            wc.lpfnWndProc=thisWindowProc;
            wc.lpszClassName=CLASSNAME;
            wc.hInstance=hInstance;
            RegisterClass(&wc);
        }
        this->mHWnd=CreateWindowEx(this->mWindowStyle,className,this->mTitle.c_str(),WS_OVERLAPPEDWINDOW,this->x,this->y,this->width,this->height,
                                   this->mParentWindow==nullptr?NULL:this->mParentWindow->mHWnd,NULL,hInstance,this);
        if(this->mHWnd!=NULL){
            ShowWindow(this->mHWnd, SW_SHOW);
            UpdateWindow(this->mHWnd);
        }
        return this->mHWnd;
    }
    Handle::Handle(int x,int y,int w,int h,int windowStyle,std::wstring title){
        this->x=x;
        this->y=y;
        this->width=w;
        this->height=h;
        this->mWindowStyle=windowStyle;
        this->mTitle=title;
        this->mHWnd=NULL;
        this->mParentWindow=nullptr;
        this->mID=globalHandleManager.getCnt();
    }
    Handle::Handle(int x,int y,int w,int h,int windowStyle,std::wstring title,Handle *ParentWindow){
        this->x=x;
        this->y=y;
        this->width=w;
        this->height=h;
        this->mWindowStyle=windowStyle;
        this->mTitle=title;
        this->mHWnd=NULL;
        this->mParentWindow=ParentWindow;
        this->mID=globalHandleManager.getCnt();
    }
    Handle::~Handle(){
        mHWndMap.erase(this->mHWnd);
    }
    bool HandleManager::isEmpty(){
        return this->handles.empty();
    }
    bool HandleManager::checkAndQuit(){
        if(this->isEmpty()){
            WindowLogger.traceLog(Core::logger::LOG_INFO,"Quitting program");
            PostQuitMessage(0);
            return true;
        }
        return false;
    }
    void HandleManager::push(std::shared_ptr<Handle> handle){
        this->handles.push_back(handle);
        this->handlesID.push_back(cnt++);
    }
    long long HandleManager::getCnt(){
        return cnt;
    }
    HandleManager::~HandleManager(){
        for(auto &handle:handles){
            handle.reset();
        }
    }
    void HandleManager::pop(long long ID){
        auto it = std::find(this->handlesID.begin(), this->handlesID.end(), ID);
        if(it != this->handlesID.end()){
            size_t index = std::distance(this->handlesID.begin(), it);
            this->handles.erase(this->handles.begin() + index);
            this->handlesID.erase(it);
        }
    }
    void Handle::registerToManager(){
        this->mID=globalHandleManager.getCnt();
        globalHandleManager.push(shared_from_this());
    }
    void Handle::destroy(){
        this->isActive=false;
        globalHandleManager.pop(this->mID);
    }
}