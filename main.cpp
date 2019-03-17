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
//    cv::Mat src;
//    src = cv::imread("dsad");

    char strFile[50] = "1.bmp";
    printf("please input the .bmp source file name:\n");
//    scanf("%s",strFile);

    Mat src;
    src = readimg(strFile);
    char save_name[50] = "4.bmp";
    printf("please input the .bmp destination file name:\n");
//    scanf("%s",save_name);
    saveimg(src,save_name);
//    readimg(strFile);
//    Mat src(strFile);


    cout << "dasda" << std::endl;
    return 0;
}