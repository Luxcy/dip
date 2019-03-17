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
#include "global.h"
#include <iostream>
class Mat{
public:
    Mat();
    Mat(const char* filename);
    Mat(int width, int height);
    void show_bitInfoHead();
    void show_bitHead();
    void show_pRgb();
    BITMAPFILEHEADER Get_bitHead();
    BITMAPINFOHEADER Get_bitInfoHead();
    int Get_cols();
    int Get_rows();
    RGBQUAD** Get_data();

    Mat reverseColor();
    Mat RGB2Gray();
//    double contrast(BYTE** dataOfBmp_gray, DWORD width, DWORD height, bool flag);

protected:
    BITMAPFILEHEADER   bitHead;
    BITMAPINFOHEADER bitInfoHead;
    int rows, cols;
    RGBQUAD** dataOfBmp_src;
    RGBQUAD* pRgb;
    int pRgb_num;

};
#endif //DIP_MAT_H
