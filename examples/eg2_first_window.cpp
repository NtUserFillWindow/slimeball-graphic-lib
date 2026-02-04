#include "Graphics.hpp"
using namespace Graphics;
int main()
{
    //Create the first Window!
    auto mainWindow=std::make_shared<Handle>(0,0,800  ,600   ,0    ,L"First Window");//Like this
    //                                       x,y,width,height,style,title(wstring)
    mainWindow->registerToManager();//! this is necessary, you could remove it and see if something interesting will happen:)
    HWND mainHWnd=mainWindow->initWindow(L"First Window",GetModuleHandle(NULL));
    globalLogger.traceLog(Core::logger::LOG_INFO,"main created");
    MSG msg={};
    while(msg.message!=WM_QUIT){//Actually this can be replaced by many other conditions.
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //Proceed your main loop code here
        if(mainHWnd!=NULL){
            globalLogger.traceLog(Core::logger::LOG_NOTE,"loop #");
        };//Just make the compiler shut up
        Sleep(100);
        globalHandleManager.checkAndQuit();//literally
    }
    //And of course, return!
    return 0;
}