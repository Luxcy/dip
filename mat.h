//
// Created by lux813 on 19-3-12.
//

#ifndef DIP_MAT_H
#define DIP_MAT_H
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <algorithm>
#include "global.h"
#include <iostream>
class Mat{
public:
    Mat();
    Mat(const char* filename);
    Mat(DWORD width, DWORD height);
    void show_bitInfoHead();
    void show_bitHead();
    void show_pRgb();
    BITMAPFILEHEADER Get_bitHead();
    BITMAPINFOHEADER Get_bitInfoHead();
    int Get_cols();
    int Get_rows();
    RGBQUAD** Get_data();
    void SetBackGrund(BYTE r, BYTE g, BYTE b);
    bool PixelsIsInPic(DWORD x, DWORD y);

    Mat reverseColor();
    Mat RGB2Gray();
    Mat Translate(int w, int h);
    Mat Flip(FlipType type);
    Mat Zoom(double times, InterpolationType type = NearestNeighbor);
    Mat Rotation(int degree);

//    double contrast(BYTE** dataOfBmp_gray, DWORD width, DWORD height, bool flag);

protected:
    BITMAPFILEHEADER   bitHead;
    BITMAPINFOHEADER bitInfoHead;
    DWORD rows, cols;
    RGBQUAD** dataOfBmp_src;
    RGBQUAD* pRgb;
    int pRgb_num;

};
#endif //DIP_MAT_H
