//
// Created by lux813 on 19-3-12.
//

#ifndef DIP_MAT_H
#define DIP_MAT_H
#include "base.hpp"
#include "global.h"

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
    DWORD Get_max();
    DWORD Get_min();
    DWORD Get_mean();
    RGBQUAD** Get_data();
    void SetBackGrund(BYTE r, BYTE g, BYTE b);
    bool PixelsIsInPic(DWORD x, DWORD y);

    void compute_histogram();
    Mat show_histogram();

    Mat reverseColor();
    Mat RGB2Gray();

    Mat Translate(int w, int h);
    Mat Flip(FlipType type);
    Mat Zoom(double times, InterpolationType type = NearestNeighbor);
    Mat Rotation(int degree);

    double contrast( bool flag);
    Mat imgGamma(double gamma);
    Mat imgLog();
    Mat Contrast_stretch(DWORD r1, DWORD s1, DWORD r2, DWORD s2);
    Mat Bit_plane_slice(int k);

protected:
    BITMAPFILEHEADER   bitHead;
    BITMAPINFOHEADER bitInfoHead;
    DWORD rows, cols;
    RGBQUAD** dataOfBmp_src;
    RGBQUAD* pRgb;
    DWORD histogram[256];
    int pRgb_num;

};
#endif //DIP_MAT_H
