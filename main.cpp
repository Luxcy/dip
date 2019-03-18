#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <iostream>
#include<opencv2/opencv.hpp>
#include "operator.hpp"
#include "mat.hpp"
using namespace std;

int main()
{

    char strFile[50] = "2.bmp";
    printf("please input the .bmp source file name:\n");

    Mat src;
    src = readimg(strFile);
    src.show_bitHead();
    src.show_bitInfoHead();
//    src.show_pRgb();
//    Mat dst = src.reverseColor();
//    Mat dst =src.RGB2Gray();
    Mat dst;
//    dst = src.Translate(10,-10);
//    dst = src.Flip(VERTICAL);
    dst = src.Zoom(0.5);
    char save_name[50] = "5.bmp";
    printf("please input the .bmp destination file name:\n");
    saveimg(dst,save_name);

    return 0;
}