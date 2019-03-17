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
class Mat{
public:
    Mat();
    Mat(const char* filename);
    BITMAPFILEHEADER Get_bitHead();
    BITMAPINFOHEADER Get_bitInfoHead();
    int Get_cols();
    int Get_rows();
    RGBQUAD** Get_data();
//    void showBmpHead(BITMAPFILEHEADER* pBmpHead);
//    void showBmpInforHead(BITMAPINFOHEADER* pBmpInforHead);
//    void showRgbQuan(RGBQUAD* pRGB, DWORD num);
//    void reverseColor(RGBQUAD** dataOfBmp_src, RGBQUAD** dataOfBmp_dst, DWORD width, DWORD height);
//    void RGB2Gray(RGBQUAD** dataOfBmp_src, RGBQUAD** dataOfBmp_dst, BYTE** dataOfBmp_gray, DWORD width, DWORD height);
//    double contrast(BYTE** dataOfBmp_gray, DWORD width, DWORD height, bool flag);
//    void saveBmp(RGBQUAD** dataOfBmp, DWORD width, DWORD height);
protected:
    BITMAPFILEHEADER   bitHead;
    BITMAPINFOHEADER bitInfoHead;
    int rows, cols;
    int dims;
    RGBQUAD** dataOfBmp_src;

};
#endif //DIP_MAT_H
