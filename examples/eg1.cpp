#include "Graphics.hpp"
using namespace Graphics;
int main(){
    //freopen("console.log","w",stderr);
    //if you need
    SetConsoleOutputCP(CP_UTF8);//Of course you need that
    globalLogger.traceLog(logger::LOG_NOTE,"Hello,world!");//Output "Hello,world!" to log
    globalLogger.traceLog(logger::LOG_DEBUG,"Debug");//The stream is clog in default global logger
    globalLogger.traceLog(logger::LOG_ERROR,"ERROR");//The error log
    Graphics::logger localLogger(std::cout);//You can also make an other one
    //These goes to cout stream
    localLogger.traceLog(logger::LOG_FATAL,"FATAL ERROR");//Note that the [Note] and [Fatal] logs won't come with time
    localLogger.traceLog(logger::LOG_WARNING,"Warning");
    localLogger.traceLog(logger::LOG_INFO,"INFO message");
    //Also could be formatted
    std::string str="Hello,world!";
    int num=5;
    localLogger.formatLog(logger::LOG_INFO,"%d,%s",num,str.c_str());
    //And variable debug for short
    num++;
    localLogger.varLog(logger::LOG_INFO,"num",num);
    system("pause");
    return 0;
}