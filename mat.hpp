//
// Created by lux813 on 19-3-16.
//

#ifndef DIP_MAT_HPP
#define DIP_MAT_HPP

#include "mat.h"
Mat::Mat():dims(0), rows(0), cols(0){
    dataOfBmp_src = NULL;
}
Mat::Mat(const char* filename){
    FILE* pfile;
    pfile = fopen(filename,"rb");
    if(pfile!=NULL)
    {
        printf("file %s open success.\n", filename);
        fread(&bitHead,1,sizeof(BITMAPFILEHEADER),pfile);
        if(bitHead.bfType != 0x4d42)
        {
            printf("file is not .bmp file!");
        }
        fread(&bitInfoHead,1,sizeof(BITMAPINFOHEADER),pfile);

    }
    else
    {
        printf("file open fail!\n");
    }
    printf("位图信息头:\n");
    printf("结构体的长度:%d\n",bitInfoHead.biSize);
    printf("位图宽:%d\n",bitInfoHead.biWidth);
    printf("位图高:%d\n",bitInfoHead.biHeight);
    printf("biPlanes平面数:%d\n",bitInfoHead.biPlanes);
    printf("biBitCount采用颜色位数:%d\n",bitInfoHead.biBitCount);
    printf("压缩方式:%d\n",bitInfoHead.biCompression);
    printf("biSizeImage实际位图数据占用的字节数:%d\n",bitInfoHead.biSizeImage);
    printf("X方向分辨率:%d\n",bitInfoHead.biXPelsPerMeter);
    printf("Y方向分辨率:%d\n",bitInfoHead.biYPelsPerMeter);
    printf("使用的颜色数:%d\n",bitInfoHead.biClrUsed);
    printf("重要颜色数:%d\n",bitInfoHead.biClrImportant);

    RGBQUAD* pRgb=NULL;

    if(bitInfoHead.biBitCount < 24)//有调色板
    {
        //读取调色盘结信息
        long nPlantNum = bitInfoHead.biClrUsed;
        if(!nPlantNum)
            nPlantNum = long(pow(2,double(bitInfoHead.biBitCount)));    //   Mix color Plant Number;
        pRgb=new RGBQUAD[nPlantNum*sizeof(RGBQUAD)];
        memset(pRgb,0,nPlantNum*sizeof(RGBQUAD));
        int num = fread(pRgb,4,nPlantNum,pfile);

    }


    rows = bitInfoHead.biWidth;
    cols = bitInfoHead.biHeight;
    //分配内存空间把源图存入内存
    int l_width   = WIDTHBYTES(rows* bitInfoHead.biBitCount);//计算位图的实际宽度并确保它为32的倍数
    long nData = cols*l_width;
    BYTE *pColorData= new BYTE[nData];
    memset(pColorData,0,nData);

    //把位图数据信息读到数组里
    fread(pColorData,1,nData,pfile);


    dataOfBmp_src = new RGBQUAD*[cols];
    for(DWORD i=0; i < cols;i++)
        dataOfBmp_src[i] =new RGBQUAD[rows];

    if(bitInfoHead.biBitCount<24)//有调色板，即位图为非真彩色
    {
        int k;

        if(bitInfoHead.biBitCount <= 8 && !bitInfoHead.biCompression)
        {

            int pnum = 8/bitInfoHead.biBitCount;
            int mbnum = 8-bitInfoHead.biBitCount;

            for(DWORD i=0;i<cols;i++)
            {
                int k0 = (cols-i-1)*l_width;//k:取得该像素颜色数据在实际数据数组中的序号
                for(DWORD j=0;j<rows;j++)
                {
                    BYTE mixIndex= 0;
                    k = k0+(j/pnum);
                    mixIndex = pColorData[k];
                    //mixIndex:提取当前像素的颜色的在颜色表中的索引值
                    if(bitInfoHead.biBitCount < 8)
                    {
                        mixIndex = mixIndex<<((j%pnum)*bitInfoHead.biBitCount);
                        mixIndex = mixIndex>>mbnum;
                    }

                    //将像素颜色数据（RGBA）保存到数组中对应的位置
                    dataOfBmp_src[i][j].rgbRed = pRgb[mixIndex].rgbRed;
                    dataOfBmp_src[i][j].rgbGreen = pRgb[mixIndex].rgbGreen;
                    dataOfBmp_src[i][j].rgbBlue = pRgb[mixIndex].rgbBlue;
                    dataOfBmp_src[i][j].rgbReserved = pRgb[mixIndex].rgbReserved;
                }

            }
        }


        if(bitInfoHead.biBitCount == 16)
        {
            if(!bitInfoHead.biCompression)
            {
                for(DWORD i=0;i<cols;i++)
                {
                    int k0 = (cols-i-1)*l_width;
                    for(DWORD j=0;j<rows;j++)
                    {
                        WORD mixIndex= 0;
                        k = k0+j*2;
                        WORD shortTemp;
                        shortTemp = pColorData[k+1];
                        shortTemp = shortTemp<<8;

                        mixIndex = pColorData[k] + shortTemp;
                        dataOfBmp_src[i][j].rgbRed = pRgb[mixIndex].rgbRed;
                        dataOfBmp_src[i][j].rgbGreen = pRgb[mixIndex].rgbGreen;
                        dataOfBmp_src[i][j].rgbBlue = pRgb[mixIndex].rgbBlue;
                        dataOfBmp_src[i][j].rgbReserved = pRgb[mixIndex].rgbReserved;
                    }
                }
            }
        }
    }
    else//位图为24/32位真彩色
    {
        int k;
        int index = 0;

        if(bitInfoHead.biBitCount == 16)
        {
            for(DWORD i=0;i<cols;i++)
            {
                int k0 = (cols-i-1)*l_width;
                for(DWORD j=0;j<rows;j++)
                {
                    k = k0+j*2;
                    if(!bitInfoHead.biCompression)//555格式
                    {
                        dataOfBmp_src[i][j].rgbBlue=pColorData[k]&0x1F;
                        dataOfBmp_src[i][j].rgbGreen=(((pColorData[k+1]<<6)&0xFF)>>3)+(pColorData[k]>>5);
                        dataOfBmp_src[i][j].rgbRed=(pColorData[k+1]<<1)>>3;
                        dataOfBmp_src[i][j].rgbReserved = 0;

                    }
                }
            }
        }
        if(bitInfoHead.biBitCount == 24 && !bitInfoHead.biCompression)
        {
            for(DWORD i=0;i<cols;i++)
            {
                int k0 = (cols-i-1)*l_width;
                for(DWORD j=0;j<rows;j++)
                {
                    k = k0+(j*3);
                    dataOfBmp_src[i][j].rgbRed = pColorData[k+2];
                    dataOfBmp_src[i][j].rgbGreen = pColorData[k+1];
                    dataOfBmp_src[i][j].rgbBlue = pColorData[k];
                    dataOfBmp_src[i][j].rgbReserved = 0;

                }
            }
        }
        if(bitInfoHead.biBitCount == 32 && !bitInfoHead.biCompression)
        {
            for(DWORD i=0;i<cols;i++)
            {
                int k0 = (cols-i-1)*l_width;
                for(DWORD j=0;j<rows;j++)
                {
                    k = k0+(j*4);
                    dataOfBmp_src[i][j].rgbRed = pColorData[k+2];
                    dataOfBmp_src[i][j].rgbGreen = pColorData[k+1];
                    dataOfBmp_src[i][j].rgbBlue = pColorData[k];
                    dataOfBmp_src[i][j].rgbReserved = pColorData[k+3];

                }
            }

        }

    }

}

BITMAPFILEHEADER Mat::Get_bitHead(){
    return bitHead;

}
BITMAPINFOHEADER Mat::Get_bitInfoHead(){
    return bitInfoHead;
}
int Mat::Get_cols(){
    return cols;
}
int Mat::Get_rows(){
    return  rows;
}
RGBQUAD** Mat::Get_data(){
    return dataOfBmp_src;
}

#endif //DIP_MAT_HPP
