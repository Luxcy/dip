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

int main(int argc, char *argv[])
{

    char *strFile=argv[1];
    printf("please input the .bmp source file name:\n");

    Mat src;
    src = readimg(strFile);
//    src.show_bitHead();
//    src.show_bitInfoHead();
//    src.show_pRgb();
//    dst = src.reverseColor();
//    Mat dst =src.RGB2Gray();
//    double contrast = src.contrast(1);
//    std::cout << "contrast of 8D : " << contrast << std::endl;
    Mat dst;
//    dst = src.Translate(10,-10);
//    dst = src.Flip(VERTICAL);
//    dst = src.Zoom(0.6);
//    dst = src.Rotation(-10);
//    dst = src.imgGamma(4.0);
//    dst = src.imgLog();
//    dst = src.reverseColor();
//    dst = src.Bit_plane_slice(7);

//    for(int i=0; i<8; i++){
//        dst = src.Bit_plane_slice(i);
//        char bitname[50];
//        sprintf(bitname,"%d.bmp",i);
//        std::cout << "sdsadas0  " << std::endl;
//        saveimg(dst,bitname);
//    }
//基于灰度的图片增强
//    DWORD r1 = src.Get_min();
//    DWORD r2 = src.Get_max();
//    dst = src.Contrast_stretch(r1, 0, r2, 255);

//    DWORD r1 = src.Get_mean();
//    DWORD r2 = r1;
//    dst = src.Contrast_stretch(r1, 0, r2, 255);
//直方图
//显示
//    src.compute_histogram();
//    dst = src.show_histogram();
//均衡化
//    dst = src.histo_equalization();
//阈值分割
//    dst = src.thresh_seg(128);
// 规定化
//    Mat Z;
//    Z = readimg("./img/11.bmp");
//    dst = src.histogram_map(Z);
//    dst.compute_histogram();
//    dst = dst.show_histogram();

//去噪
//均值
//    dst = src.Filter(Mean);
//中值
//    dst = src.Filter(Median);
//KNN
//    dst = src.Filter(KNN);
//锐化
//    dst = src.Sharpen(SP_Horizontal,ADD);
//    dst = src.Sharpen(SP_Vertical);

//    dst = src.Sharpen(SP_Roberts);
//    dst = src.Sharpen(SP_Priwitt);
//    dst = src.Sharpen(SP_Sobel);
//    dst = src.Sharpen(SP_Laplacian,ADD);
//    dst = src.Sharpen(SP_Wallis,ADD);
//    dst = dst.reverseColor();

//分割 基于灰度图，需要变成灰度图　
//p-参数法
//    double p = 0.1507;
//    int th = src.P_segment(p);
//    std::cout << th << std::endl;
//    dst = src.Contrast_stretch(th, 0, th, 255);
//均匀性度量法 ///不能确定目标函数的凹凸性或者单调性，不能用二分或者三分的方法吧。。。。******
//    int th = src.Var_segment();
//    std::cout << th << std::endl;
//    dst = src.Contrast_stretch(th, 0, th, 255);
//    std::cout << src.SIGMA(th) << std::endl;
//聚类的方法
//   dst = src.Cluster_segment(0);

//贴标签
    dst = src.Labeled(0);
//腐蚀
//    dst = src.Corrosion();
//膨胀
//    dst = src.Dilation();
    char *save_name = argv[2];
    printf("please input the .bmp destination file name:\n");
    saveimg(dst,save_name);

    return 0;
}