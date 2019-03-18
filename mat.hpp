//
// Created by lux813 on 19-3-16.
//

#ifndef DIP_MAT_HPP
#define DIP_MAT_HPP

#include "mat.h"
#pragma pack(1) //强制内存以1字节为单位对齐
Mat::Mat():rows(0), cols(0){
    dataOfBmp_src = NULL;
    pRgb = NULL;
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

    if(bitInfoHead.biBitCount < 24)//有调色板
    {
        //读取调色盘结信息
        long nPlantNum = bitInfoHead.biClrUsed;
        if(!nPlantNum)
            nPlantNum = long(pow(2,double(bitInfoHead.biBitCount)));    //   Mix color Plant Number;
        pRgb=new RGBQUAD[nPlantNum*sizeof(RGBQUAD)];
        memset(pRgb,0,nPlantNum*sizeof(RGBQUAD));
        pRgb_num = fread(pRgb,4,nPlantNum,pfile);

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
Mat::Mat(DWORD width, DWORD height){
    pRgb = NULL;
    cols = height;
    rows = width;
    dataOfBmp_src = new RGBQUAD*[cols];
    for(DWORD i=0; i < cols;i++)
        dataOfBmp_src[i] =new RGBQUAD[rows];
}
Mat Mat::Zoom(double times, InterpolationType type){
    DWORD new_width = DWORD(rows * times + 0.5);
    DWORD new_height = DWORD(cols * times + 0.5);
    Mat dst(new_width,new_height);
    if(type == NearestNeighbor){
        for(DWORD i=0; i<new_height; i++){
            for(DWORD j=0; j<new_width; j++){
                DWORD  ii = DWORD(i * 1.0 / times + 0.5);
                DWORD  jj = DWORD(j * 1.0 / times + 0.5);
                if(ii >= cols) ii = cols - 1;
                if(jj >= rows) jj = rows - 1;
                dst.dataOfBmp_src[i][j].rgbRed = dataOfBmp_src[ii][jj].rgbRed;
                dst.dataOfBmp_src[i][j].rgbGreen = dataOfBmp_src[ii][jj].rgbGreen;
                dst.dataOfBmp_src[i][j].rgbBlue = dataOfBmp_src[ii][jj].rgbBlue;

            }
        }
    }
    return dst;
}
Mat Mat::Flip(FlipType type) {
    Mat dst(rows,cols);
    if(type == HORIZONTAL){
        for(DWORD i=0;i<cols;i++) {
            for (DWORD j = 0; j < rows; j++) {
                dst.dataOfBmp_src[i][rows-j-1].rgbRed = dataOfBmp_src[i][j].rgbRed;
                dst.dataOfBmp_src[i][rows-j-1].rgbGreen = dataOfBmp_src[i][j].rgbGreen;
                dst.dataOfBmp_src[i][rows-j-1].rgbBlue = dataOfBmp_src[i][j].rgbBlue;
            }
        }
    }else{
        for(DWORD i=0;i<cols;i++) {
            for (DWORD j = 0; j < rows; j++) {
                dst.dataOfBmp_src[cols-i-1][j].rgbRed = dataOfBmp_src[i][j].rgbRed;
                dst.dataOfBmp_src[cols-i-1][j].rgbGreen = dataOfBmp_src[i][j].rgbGreen;
                dst.dataOfBmp_src[cols-i-1][j].rgbBlue = dataOfBmp_src[i][j].rgbBlue;
            }
        }
    }

    return dst;
}
Mat Mat::Translate(int w, int h){
    Mat dst(rows+abs(w),cols+abs(h));
    for(DWORD i=0; i<dst.Get_cols();i++){
        for(DWORD j=0;  j<dst.Get_rows(); j++){
            dst.dataOfBmp_src[i][j].rgbRed = 255;
            dst.dataOfBmp_src[i][j].rgbGreen = 255;
            dst.dataOfBmp_src[i][j].rgbBlue = 0;
        }
    }
    for(DWORD i=0;i<cols;i++) {
        for (DWORD j = 0; j < rows; j++) {
            dst.dataOfBmp_src[i+std::max(0,h)][j+std::max(0,w)].rgbRed = dataOfBmp_src[i][j].rgbRed;
            dst.dataOfBmp_src[i+std::max(0,h)][j+std::max(0,w)].rgbGreen = dataOfBmp_src[i][j].rgbGreen;
            dst.dataOfBmp_src[i+std::max(0,h)][j+std::max(0,w)].rgbBlue = dataOfBmp_src[i][j].rgbBlue;
        }
    }
    return dst;
}
Mat Mat::reverseColor(){
    Mat dst(rows,cols);
    for(DWORD i=0;i<cols;i++) {
        for (DWORD j = 0; j < rows; j++) {
            dst.dataOfBmp_src[i][j].rgbRed = 255 - dataOfBmp_src[i][j].rgbRed;
            dst.dataOfBmp_src[i][j].rgbGreen = 255 - dataOfBmp_src[i][j].rgbGreen;
            dst.dataOfBmp_src[i][j].rgbBlue = 255 - dataOfBmp_src[i][j].rgbBlue;
        }
    }
    return dst;
}
Mat Mat::RGB2Gray() {
    Mat dst(rows,cols);
    for(DWORD i=0;i<cols;i++)
    {
        for(DWORD j=0;j<rows;j++)
        {
            double gray = 0.299*dataOfBmp_src[i][j].rgbRed+0.587*dataOfBmp_src[i][j].rgbGreen+0.114*dataOfBmp_src[i][j].rgbBlue;
            dst.dataOfBmp_src[i][j].rgbRed = (BYTE)gray;
            dst.dataOfBmp_src[i][j].rgbGreen = (BYTE)gray;
            dst.dataOfBmp_src[i][j].rgbBlue = (BYTE)gray;
        }
    }
    return dst;
}

void Mat::show_pRgb(){
    printf("颜色板信息:\n");
    for (DWORD i=0; i<pRgb_num; i++)
    {
        if (i%5==0)
        {
            printf("\n");
        }
        printf("(%-3d,%-3d,%-3d)   ",(pRgb+i)->rgbRed,(pRgb+i)->rgbGreen,(pRgb+i)->rgbBlue);
    }
    printf("\n");
}
void Mat::show_bitHead(){
    printf("位图文件头:\n");
    printf("文件类型:%x\n",bitHead.bfType);
    printf("文件大小:%d\n",bitHead.bfSize);
    printf("保留字:%d\n",bitHead.bfReserved1);
    printf("保留字:%d\n",bitHead.bfReserved2);
    printf("实际位图数据的偏移字节数:%d\n",bitHead.bfOffBits);
}
void Mat::show_bitInfoHead(){
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
