//MIT License

//Copyright (c) 2026 wzxTheSlimeball
#include "Image.hpp"
#include <iostream>

namespace Assets{
    namespace{
        struct GDIManager{
            ULONG_PTR GDIToken;
            GDIManager(){
                Gdiplus::GdiplusStartupInput gpsi;
                Gdiplus::GdiplusStartup(&GDIToken,&gpsi,nullptr);
            }
            ~GDIManager(){
                Gdiplus::GdiplusShutdown(GDIToken);
            }
            GDIManager(const GDIManager& other)=delete;
            GDIManager operator=(const GDIManager& other)=delete;
        };
        static GDIManager thisStaticGDIManager;
    }
}

Assets::Image::Image(){
    this->width=0;
    this->height=0;
    this->thisBmp=nullptr;
    this->thisHBITMAP=nullptr;
}
unsigned long long Assets::Image::getWidth()const{
    return this->width;
}
unsigned long long Assets::Image::getHeight()const{
    return this->height;
}
Assets::Image::~Image(){
    if(thisBmp){
        delete thisBmp;
        thisBmp=nullptr;
    }
    if(thisHBITMAP){
        DeleteObject(thisHBITMAP);
        thisHBITMAP=nullptr;
    }
}
Assets::Image::Image(Image&& other)noexcept:width(other.width),height(other.height),thisBmp(other.thisBmp),thisContent(std::move(other.thisContent)){
    other.thisBmp=nullptr;
    other.width=other.height=0;
}
Assets::Image& Assets::Image::operator=(Image&& other)noexcept{
    if(this!=&other){
        if(thisBmp){
            delete thisBmp;
        }
        width=other.width;
        height=other.height;
        thisBmp=other.thisBmp;
        thisContent=other.thisContent;
        other.width=other.height=0;
        other.thisBmp=nullptr;
        other.thisContent.clear();
    }
    return *this;
}
Assets::Image::Image(const std::wstring& widePath){
    thisBmp=Gdiplus::Bitmap::FromFile(widePath.c_str());
    if(thisBmp){
        if(thisBmp->GetLastStatus()==Gdiplus::Ok){
            this->width=thisBmp->GetWidth();
            this->height=thisBmp->GetHeight();
        }
        else{
            delete thisBmp;
            thisBmp=nullptr;
            width=height=0;
        }
    }
}
bool Assets::Image::syncData(){
    thisContent.resize(width*height);
    Gdiplus::BitmapData bmpdata;
    Gdiplus::Rect rect(0,0,width,height);
    Gdiplus::Status status=thisBmp->LockBits(&rect,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&bmpdata);
    if(status!=Gdiplus::Ok){
        thisContent.clear();
        return false;
    }
    unsigned int *src=static_cast<unsigned int*>(bmpdata.Scan0);
    size_t count=width*height;
    for(size_t i=0;i<count;i++){
        thisContent[i]=src[i];
    }
    thisBmp->UnlockBits(&bmpdata);
    return true;
}
HBITMAP Assets::Image::getHBITMAP()const{
    if(thisHBITMAP) return thisHBITMAP;
    if(!thisBmp) return nullptr;
    ULONG_PTR imgWidth=thisBmp->GetWidth();
    ULONG_PTR imgHeight=thisBmp->GetHeight();
    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0,0,imgWidth,imgHeight);
    if(thisBmp->LockBits(&rect,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&bitmapData)!=Gdiplus::Ok){
        return nullptr;
    }
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth=imgWidth;
    bmi.bmiHeader.biHeight=-static_cast<LONG>(imgHeight);
    bmi.bmiHeader.biPlanes=1;
    bmi.bmiHeader.biBitCount=32;
    bmi.bmiHeader.biCompression=BI_RGB;
    void* pBits=nullptr;
    HBITMAP hAlphaBitmap=CreateDIBSection(nullptr,&bmi,DIB_RGB_COLORS,&pBits,nullptr,0);
    if(!hAlphaBitmap){
        thisBmp->UnlockBits(&bitmapData);
        return nullptr;
    }
    BYTE* destRow=static_cast<BYTE*>(pBits);
    BYTE* srcRow=static_cast<BYTE*>(bitmapData.Scan0);
    LONG destStride=static_cast<LONG>(imgWidth) * 4;
    LONG srcStride=bitmapData.Stride;
    for(ULONG_PTR y=0;y<imgHeight;++y){
        LONG copyBytes=std::min(destStride,abs(srcStride));
        memcpy(destRow,srcRow,copyBytes);
        destRow+=destStride;
        srcRow+=srcStride;
    }
    thisBmp->UnlockBits(&bitmapData);
    thisHBITMAP=hAlphaBitmap;
    return hAlphaBitmap;
}