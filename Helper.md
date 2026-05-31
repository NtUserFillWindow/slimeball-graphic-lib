# Z-Multiplier Lib食用手册

## 1. Clock

本库使用`Clock`控制循环

`Clock`可以这样定义：

```cpp
Core::Clock c([&](){
    //loop
},/*FPS*/)
```

其中，`FPS`为可选参数，默认为`60`

定义`Clock`后，写一个简单的`while`即可进入循环：

```cpp
while(c){
    c.run();
    //double fps=c.fps();
}
```

理论上，语句写在`while`里和写在`Clock`的构造里没有区别，但习惯上一般只在`while`里写有关fps的操作

## 2. Window

本库封装了三种常用的窗口的创建，分别为普通窗口，透明点击不穿透窗口和透明点击穿透窗口

创建窗口十分简单，如下：

```cpp
auto mainWindow=createInitWindow(0,0,407,430,L"Title");
```

注意参数中`width`和`height`包含了非客户区

此时`mainWindow`的类型是`std::pair<std::shared_ptr<Window::Handle>,HWND>`

## 3. Callback

本库具有完善的窗口回调

可见于`Window.hpp`中，命名均为`this***`