#pragma once
#include <graphics.h>

/*
*函数声明
*/
void putimagePNG(int  picture_x, int picture_y, IMAGE* picture);/*载入PNG图并去透明部分*/
int getDelay();
int fileExist(const char*);/*判断文件名是否能打开,C++可用bool类型*/