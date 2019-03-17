//
// Created by lux813 on 19-3-16.
//

#ifndef DIP_OPERATOR_HPP
#define DIP_OPERATOR_HPP
#include <iostream>
#include <string>
#include <stdio.h>
#include "mat.hpp"
using namespace std;
#pragma pack(1)
Mat readimg(const char* filename)
{
    Mat src(filename);
    return src;

}
void saveimg(Mat src,char* filename)
{
    DWORD i, j;
    WORD biBitCount = 32;
    FILE* pfile;
    pfile = fopen(filename,"wb");//打开文件
    BITMAPFILEHEADER   bitHead;
    BITMAPINFOHEADER bitInfoHead;
    bitHead = src.Get_bitHead();
    bitInfoHead = src.Get_bitInfoHead();
    int width = src.Get_rows();
    int height = src.Get_cols();
    RGBQUAD** dataOfBmp = src.Get_data();
    if(pfile!=NULL)
    {
        printf("file %s open success.\n", filename);
        //写位图文件头信息
        bitHead.bfType = 0x4d42;
        bitHead.bfSize = 0;
        bitHead.bfReserved1 = 0;
        bitHead.bfReserved2 = 0;
        bitHead.bfOffBits = 54;
        if(biBitCount<=8)
            bitHead.bfOffBits += (DWORD)pow(2, biBitCount)*4;
        fwrite(&bitHead,1,sizeof(tagBITMAPFILEHEADER),pfile);

        bitInfoHead.biSize = 40;
        bitInfoHead.biWidth = width;
        bitInfoHead.biHeight = height;
        bitInfoHead.biPlanes = 1;
        bitInfoHead.biBitCount = biBitCount;
        bitInfoHead.biCompression = 0;
        bitInfoHead.biSizeImage = 0;
        bitInfoHead.biXPelsPerMeter = 0;
        bitInfoHead.biYPelsPerMeter = 0;
        bitInfoHead.biClrImportant = 0;
        bitInfoHead.biClrUsed = 0;
        fwrite(&bitInfoHead,1,sizeof(tagBITMAPINFOHEADER),pfile);

        if(biBitCount<=8)
        {
            BYTE tmp=0;

            for(i=0; i<(DWORD)pow(2, biBitCount); i++)
            {
                tmp = (BYTE)i;
                fwrite(&tmp,1,4,pfile);
            }
        }

        int l_width   = WIDTHBYTES(width * biBitCount)-width*4;//计算为确保位图数据区的实际宽度为32字节的整数倍需添加的0字节个数
        for( i=0; i<height; i++)
        {
            for( j=0; j<width; j+=1)
            {
                fwrite(&dataOfBmp[height-i-1][j],1,4,pfile);

            }

            BYTE tmp=0;
            for(j=0; j<l_width; j++)
                fwrite(&tmp,1,1,pfile);
        }
    }

    fclose(pfile);
}
#endif //DIP_OPERATOR_HPP
