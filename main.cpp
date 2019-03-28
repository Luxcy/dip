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
    cv::Mat image2 = cv::imread("Test.jpg");
    cv::Mat imageLog(image2.size(), CV_32FC3);

    char strFile[50] = "./img/3.bmp";
    printf("please input the .bmp source file name:\n");

    Mat src;
    src = readimg(strFile);
//    src.show_bitHead();
//    src.show_bitInfoHead();
//    src.show_pRgb();
//    Mat dst = src.reverseColor();
//    Mat dst =src.RGB2Gray();
    double contrast = src.contrast(1);
    std::cout << "contrast of 8D : " << contrast << std::endl;
    Mat dst;
//    dst = src.Translate(10,-10);
//    dst = src.Flip(VERTICAL);
//    dst = src.Zoom(0.6);
//    dst = src.Rotation(-10);
//    dst = src.imgGamma(4.0);
//    dst = src.imgLog();
//    dst = src.reverseColor();
    char save_name[50] = "5.bmp";
    printf("please input the .bmp destination file name:\n");
    saveimg(dst,save_name);

    return 0;
}