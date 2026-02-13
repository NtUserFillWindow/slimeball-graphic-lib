//MIT License

//Copyright (c) 2026 wzxTheSlimeball
#include "Image.hpp"
#include "Window.hpp"
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
HBITMAP Assets::saveScreenAsHBITMAP(HWND targetHWnd){
    if(!targetHWnd) return NULL;
    RECT rc;
    GetWindowRect(targetHWnd,&rc);
    int width=rc.right-rc.left;
    int height=rc.bottom-rc.top;
    HDC hdcScreen=GetDC(NULL);
    HDC hdcMem=CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap=CreateCompatibleBitmap(hdcScreen,width,height);
    HBITMAP hOld=(HBITMAP)SelectObject(hdcMem,hBitmap);
    BOOL bRet=BitBlt(hdcMem,0,0,width,height,hdcScreen,rc.left,rc.top,SRCCOPY);
    SelectObject(hdcMem,hOld);
    DeleteDC(hdcMem);
    ReleaseDC(NULL,hdcScreen);
    return bRet?hBitmap:NULL;
}
static int GetEncoderClsid(const WCHAR* mimeType, CLSID* pClsid)
{
    UINT num=0;
    UINT size=0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if(size == 0) return -1;
    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL) return -1;
    Gdiplus::GetImageEncoders(num,size,pImageCodecInfo);
    for(UINT j=0;j<num;++j)
    {
        if(wcscmp(pImageCodecInfo[j].MimeType,mimeType)==0)
        {
            *pClsid=pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return static_cast<int>(j);
        }
    }
    free(pImageCodecInfo);
    return -1;
}

bool Assets::saveScreen(HWND targetHWnd,std::wstring fileName,std::wstring type){
    if(!targetHWnd) return false;
    HBITMAP hBmp=saveScreenAsHBITMAP(targetHWnd);
    if(!hBmp) return false;
    Gdiplus::Bitmap *bmp=Gdiplus::Bitmap::FromHBITMAP(hBmp, NULL);
    if(!bmp){
        DeleteObject(hBmp);
        return false;
    }
    std::wstring t=type;
    for(auto &c:t) c=towlower(c);
    if(t.size()>0 && t[0]==L'.') t=t.substr(1);
    const WCHAR* mime=L"image/png";
    if(t==L"png") mime=L"image/png";
    else if(t==L"jpg"||t==L"jpeg") mime=L"image/jpeg";
    else if(t==L"bmp") mime=L"image/bmp";
    else if(t==L"gif") mime=L"image/gif";
    else if(t==L"tiff"||t==L"tif") mime=L"image/tiff";
    CLSID clsid;
    if(GetEncoderClsid(mime,&clsid)<0){
        delete bmp;
        DeleteObject(hBmp);
        return false;
    }
    Gdiplus::Status status=bmp->Save(fileName.c_str(),&clsid,NULL);
    delete bmp;
    DeleteObject(hBmp);
    return status==Gdiplus::Ok;
}
