#include "Graphics.hpp"
using namespace Graphics;
int main()
{
    //Create the first window
    auto mainWindow=std::make_shared<Handle>(0,0,800,600,0,L"First Window");
    mainWindow->registerToManager();//! this is necessary, again
    HWND mainHWnd=mainWindow->initWindow(L"First Window",GetModuleHandle(NULL));
    globalLogger.traceLog(Core::logger::LOG_INFO,"main created");
    auto mainWindow2=std::make_shared<Handle>(100,100,800,600,0,L"Second Window");
    mainWindow2->registerToManager();
    HWND mainHWnd2=mainWindow2->initWindow(L"Second Window",GetModuleHandle(NULL));//Create the second window
    MSG msg={};
    while(msg.message!=WM_QUIT){
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if(mainHWnd!=NULL&&mainHWnd2!=NULL){
            globalLogger.traceLog(Core::logger::LOG_NOTE,"loop #");
        };//Just make the compiler shut up
        Sleep(100);
        globalHandleManager.checkAndQuit();
    }
    return 0;
}