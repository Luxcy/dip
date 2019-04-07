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
/*
 * 函数功能：展示灰度图的直方图
*/
Mat Mat::show_histogram()
{
    DWORD maxL = -1;
    for(int i=0; i<L; i++){
        if(histogram[i] > maxL){
            maxL = histogram[i];
        }
    }
    maxL += 20;
    Mat dst(L,maxL);
    for(int i=0; i<maxL; i++) {
        for (int j=0; j<L; j++) {
            dst.dataOfBmp_src[i][j].rgbRed = 255;
            dst.dataOfBmp_src[i][j].rgbGreen = 255;
            dst.dataOfBmp_src[i][j].rgbBlue = 255;
        }
    }

    for(int j=0; j<L; j++) {
        for (int i = maxL-1; i >= maxL-histogram[j];i--) {
            std::cout << i << " " << j << std::endl;
            dst.dataOfBmp_src[i][j].rgbRed = 0;
            dst.dataOfBmp_src[i][j].rgbGreen = 0;
            dst.dataOfBmp_src[i][j].rgbBlue = 0;
        }
    }
    return dst;
}
/*函数功能：灰度图的直方图
  输入参数：无
  输出值： 无
*/
void Mat::compute_histogram()
{
    for(int i=0; i<L; i++){
        histogram[i] = 0;
    }

    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            int index = int(dataOfBmp_src[i][j].rgbRed);
            histogram[index]+=1;
        }
    }
    for(int i=0; i<L; i++){
        histogram[i] = DWORD(histogram[i]*1.0/cols/rows*200);
    }
}
/*函数功能：位切片(灰度图) 如果是彩色图，先变成灰度图
  输入参数：k　表示从低位起数的第k位 0-7
  输出值： 位切片图片
*/
Mat Mat::Bit_plane_slice(int k)
{
    Mat dst(rows,cols);
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            char* str=new char[8];
            itoa(dataOfBmp_src[i][j].rgbRed,str,2);
            dst.dataOfBmp_src[i][j].rgbRed = BYTE(str[k]-'0');
            dst.dataOfBmp_src[i][j].rgbGreen = BYTE(str[k]-'0');
            dst.dataOfBmp_src[i][j].rgbBlue = BYTE(str[k]-'0');
        }
    }
    return dst;
}
/*函数功能：灰度图中平均的像素值
  输入参数：无
  输出值： 平均像素值
*/
DWORD Mat::Get_mean()
{
    double av = 0;
    for(DWORD i=0; i<cols; i++) {
        double avr = 0;
        for (DWORD j = 0; j < rows; j++) {
            double val = double(dataOfBmp_src[i][j].rgbRed);
            avr += val;
        }
        avr = avr/rows;
        av += avr;
    }
    return DWORD(av/cols);
}
/*函数功能：灰度图中最大的像素值
  输入参数：无
  输出值： 最大的像素值
*/
DWORD Mat::Get_max()
{
    DWORD max = 0;
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            DWORD val = DWORD(dataOfBmp_src[i][j].rgbRed);
            if(val > max){
                max = val;
            }
        }
    }
    return max;
}
/*函数功能：灰度图中最小的像素值
  输入参数：无
  输出值： 最小的像素值
*/
DWORD Mat::Get_min()
{
    DWORD min = 256;
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            DWORD val = DWORD(dataOfBmp_src[i][j].rgbRed);
            if(val < min){
                min = val;
            }
        }
    }
    return min;
}
/*函数功能：分段函数增强　(灰度图) 如果是彩色图，先变成灰度图
  输入参数：r1,s1为第一个拐点，r2,s2为第二个拐点
  输出值： 增强后的图片
*/
Mat Mat::Contrast_stretch(DWORD r1, DWORD s1, DWORD r2, DWORD s2)
{
    double gamma = 3.0;
    Mat dst(rows,cols);
    int tag[256];
    for(DWORD i=0; i<r1; i++){
        tag[i] = DWORD((i - 0) * (s1 - 0) * 1.0 / (r1 - 0) + 0 + 0.5);
    }
    for(DWORD i=r1; i<r2; i++){
        tag[i] = DWORD((i - r1) * (s2 - s1) * 1.0 / (r2 - r1) + s1 + 0.5);
    }
    for(DWORD i=r2; i<256; i++){
        tag[i] = DWORD((i - r2) * (255 - s2) * 1.0 / (255 - r2) + s2 + 0.5);
    }

    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            dst.dataOfBmp_src[i][j].rgbRed = BYTE(tag[dataOfBmp_src[i][j].rgbRed]);
            dst.dataOfBmp_src[i][j].rgbGreen = BYTE(tag[dataOfBmp_src[i][j].rgbGreen]);
            dst.dataOfBmp_src[i][j].rgbBlue = BYTE(tag[dataOfBmp_src[i][j].rgbBlue]);
        }
    }
    return dst;
}
/*函数功能：log增强　(0-255)
  输入参数：gamma 公式 s = clog(r + 1)
  输出值： 增强后的图片
*/
Mat Mat::imgLog()
{
    double c = 255.0 / log(256.0);
    Mat dst(rows,cols);
    int tag[256];
    std::cout << log(exp(1)) << std::endl;
    for(int i=0; i<256; i++){
        tag[i] = int(c * log(i * 1.0 + 1));
    }
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            dst.dataOfBmp_src[i][j].rgbRed = BYTE(tag[dataOfBmp_src[i][j].rgbRed]);
            dst.dataOfBmp_src[i][j].rgbGreen = BYTE(tag[dataOfBmp_src[i][j].rgbGreen]);
            dst.dataOfBmp_src[i][j].rgbBlue = BYTE(tag[dataOfBmp_src[i][j].rgbBlue]);
        }
    }
    return dst;
}
/*函数功能：Gamma 增强　(0-255)
  输入参数：gamma 公式 s = (r/255) ^ (gamma) * 255
  输出值： 增强后的图片
*/
Mat Mat::imgGamma(double gamma)
{
    Mat dst(rows,cols);
    int tag[256];
    for(int i=0; i<256; i++){
        tag[i] = int(pow(i * 1.0 / 255.0, gamma) * 255.0);
    }
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            dst.dataOfBmp_src[i][j].rgbRed = BYTE(tag[dataOfBmp_src[i][j].rgbRed]);
            dst.dataOfBmp_src[i][j].rgbGreen = BYTE(tag[dataOfBmp_src[i][j].rgbGreen]);
            dst.dataOfBmp_src[i][j].rgbBlue = BYTE(tag[dataOfBmp_src[i][j].rgbBlue]);
        }
    }
    return dst;
}
/*函数功能：灰度图象四近邻（flag=0）或八近邻（flag=1）对比度
  输入参数：BYTE* dataOfBmp_gray --- 灰度图像所有像素（以行为序）对应的灰度值；
            DWORD width, DWORD height --- 原图像和输出图像的宽度和高度
            （以像素为单位）
			bool flag --- 四近邻或八近邻标志， flag=0为四近邻，flag=1为八近邻
  输出值：  四近邻（flag=0）或八近邻（flag=1）对比度
*/
double Mat::contrast(bool flag)
{
    DWORD width = rows;
    DWORD height = cols;
    DWORD i, j;

    BYTE** dataOfBmp_gray=NULL;
    dataOfBmp_gray = new BYTE*[height];
    for(i=0; i<height; i++)
        dataOfBmp_gray[i] = new BYTE[width];

    for(DWORD i=0;i<cols;i++)
    {
        for(DWORD j=0;j<rows;j++)
        {
            double gray = 0.299*dataOfBmp_src[i][j].rgbRed+0.587*dataOfBmp_src[i][j].rgbGreen+0.114*dataOfBmp_src[i][j].rgbBlue;
            dataOfBmp_gray[i][j] = (BYTE)gray;
        }
    }


    double contrast_sum = 0;
    int tmp0 = 2, tmp1 = 3, tmp2 = 4;
    int num = 0;

    if(flag)
    {
        tmp0 = 3;
        tmp1 = 5;
        tmp2 = 8;
    }
    num = 4*tmp0+((width-2)+(height-2))*2*tmp1+((width-2)*(height-2))*tmp2;

    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            if(i>0)
            {
                contrast_sum += pow((dataOfBmp_gray[i-1][j]-dataOfBmp_gray[i][j]),2.0);
                if(flag)
                {
                    if(j>0)
                    {
                        contrast_sum += pow((dataOfBmp_gray[i-1][j-1]-dataOfBmp_gray[i][j]),2.0);
                    }
                    if(j<width-1)
                    {
                        contrast_sum += pow((dataOfBmp_gray[i-1][j+1]-dataOfBmp_gray[i][j]),2.0);
                    }
                }
            }
            if(i<height-1)
            {
                contrast_sum += pow((dataOfBmp_gray[i+1][j]-dataOfBmp_gray[i][j]),2.0);
                if(flag)
                {
                    if(j>0)
                    {
                        contrast_sum += pow((dataOfBmp_gray[i+1][j-1]-dataOfBmp_gray[i][j]),2.0);
                    }
                    if(j<width-1)
                    {
                        contrast_sum += pow((dataOfBmp_gray[i+1][j+1]-dataOfBmp_gray[i][j]),2.0);
                    }
                }
            }

            if(j>0)
            {
                contrast_sum += pow((dataOfBmp_gray[i][j-1]-dataOfBmp_gray[i][j]),2.0);
            }

            if(j<width-1)
            {
                contrast_sum += pow((dataOfBmp_gray[i][j+1]-dataOfBmp_gray[i][j]),2.0);
            }
        }
    }

    return contrast_sum/num;
}
/*
 * @define　以图片中心为旋转点
 * @param degree 逆时针旋转的角度　0-360
*/
Mat Mat::Rotation(int degree){
    double angle = degree * 1.0 / 180.0 * PI;
    double cosA = cos(angle);
    double sinA = sin(angle);
    double rx0 = cols * 0.5;
    double ry0 = rows * 0.5;

    double srcx[4] = {0, 0, cols*1.0-1, cols*1.0-1};
    double srcy[4] = {0, rows*1.0-1, 0, rows*1.0-1};
    double dstx[4],dsty[4];
    for(int i=0; i<4; i++){
        dstx[i] = (srcx[i]- rx0) * cosA - (srcy[i] - ry0) * sinA + rx0;
        dsty[i] = (srcx[i] - rx0) * sinA + (srcy[i] - ry0) * cosA + rx0;
    }
    DWORD new_Width = DWORD(std::max(fabs(dstx[0] - dstx[3]), fabs(dstx[1] - dstx[2]))) + 1;
    DWORD new_heigth = DWORD(std::max(fabs(dsty[0] - dsty[3]), fabs(dsty[1] - dsty[2]))) + 1;

    Mat dst(new_Width,new_heigth);
    dst.SetBackGrund(255,255,0);

    double rx1 = new_heigth * 0.5;
    double ry1 = new_Width * 0.5;

    for(DWORD i=0; i<new_heigth; i++) {
        for (DWORD j = 0; j < new_Width; j++) {
            //顺变换:src平移到(0,0)，逆时针旋转角度A,在平移到dst的中心点(rx1,ry1)
            ///注意：dst的中心是(rx1,ry1).逆变换是由dst平移到(0,0)，再顺时针旋转角度A，在平移到src的中心点（rx0,ry0）
            int src_x = int((i * 1.0 - rx1) * cosA + (j * 1.0 - ry1) * sinA + rx0 + 0.5);
            int src_y = int(-(i * 1.0 - rx1) * sinA + (j * 1.0 - ry1) * cosA + ry0 + 0.5);
            if(PixelsIsInPic(src_x,src_y)){
                dst.dataOfBmp_src[i][j].rgbRed = dataOfBmp_src[src_x][src_y].rgbRed;
                dst.dataOfBmp_src[i][j].rgbGreen = dataOfBmp_src[src_x][src_y].rgbGreen;
                dst.dataOfBmp_src[i][j].rgbBlue = dataOfBmp_src[src_x][src_y].rgbBlue;
            }
        }
    }
    return dst;
}
Mat Mat::Zoom(double times, InterpolationType type){
    DWORD new_width = DWORD(rows * times + 0.5);
    DWORD new_height = DWORD(cols * times + 0.5);
    Mat dst(new_width,new_height);
    ///[todo]　补全其他两种
    if(type == NearestNeighbor){
        for(DWORD i=0; i<new_height; i++){
            for(DWORD j=0; j<new_width; j++){
                DWORD  ii = DWORD(i * 1.0 / times + 0.5);
                DWORD  jj = DWORD(j * 1.0 / times + 0.5);
                if(PixelsIsInPic(ii,jj)){
                    dst.dataOfBmp_src[i][j].rgbRed = dataOfBmp_src[ii][jj].rgbRed;
                    dst.dataOfBmp_src[i][j].rgbGreen = dataOfBmp_src[ii][jj].rgbGreen;
                    dst.dataOfBmp_src[i][j].rgbBlue = dataOfBmp_src[ii][jj].rgbBlue;
                }
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
    dst.SetBackGrund(255,255,0);
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
bool Mat::PixelsIsInPic(DWORD x, DWORD y){
    return x>=0&&x<cols&&y>=0&&y<rows;
}

void Mat::SetBackGrund(BYTE r, BYTE g, BYTE b){
    for(DWORD i=0; i<cols;i++){
        for(DWORD j=0;  j<rows; j++){
            dataOfBmp_src[i][j].rgbRed = r;
            dataOfBmp_src[i][j].rgbGreen = g;
            dataOfBmp_src[i][j].rgbBlue = b;
        }
    }
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
