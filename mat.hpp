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
 * 函数功能：锐化 kernal
 */
int Mat::sharpen_kernal(int sx, int sy, ColorType ctype, SharpenType stype)
{
    int numk = 9;
    int x[] = {-1,-1,-1,0,0,0,1,1,1};
    int y[] = {-1,0,1,-1,0,1,-1,0,1};
    for(int i=0;i<numk;i++){
        if(sx+x[i]<0||sx+x[i]>=cols||sy+y[i]<0||sx+x[i]>=rows){
            return 0;
        }
    }
    int num = 0;
    int kernal[numk];
    for(int i=0; i<numk; i++) {
        if (stype == SP_Horizontal) {
            kernal[i] = Horizontal[i];
        }else if (stype == SP_Vertical) {
            kernal[i] = Vertical[i];
        }else if (stype == SP_Sobel_x) {
            kernal[i] = Sobelx[i];
        }else if (stype == SP_Sobel_y) {
            kernal[i] = Sobely[i];
        }else if (stype == SP_Priwitt_x) {
            kernal[i] = Priwittx[i];
        }else if (stype == SP_Priwitt_y) {
            kernal[i] = Priwitty[i];
        }else if(stype == SP_Laplacian){
            kernal[i] = Laplacian[i];
        }
    }
    if(stype == SP_Wallis){
        double ker[numk];
        for(int i=0; i<numk; i++) {
            ker[i] = Wallis[i];
        }
        double sum = 0;
        if (ctype == Red) {
            for (int i = 0; i < numk; i++) {
                sum += 46*log(dataOfBmp_src[sx + x[i]][sy + y[i]].rgbRed + 1) * ker[i];
            }
        } else if (ctype == Blue) {
            for (int i = 0; i < numk; i++) {
                sum += 46*log(dataOfBmp_src[sx + x[i]][sy + y[i]].rgbBlue + 1) * ker[i];
            }
        } else {
            for (int i = 0; i < numk; i++) {
                sum += 46*log(dataOfBmp_src[sx + x[i]][sy + y[i]].rgbGreen + 1) * ker[i];
            }
        }
        return int(sum);

    }else {
        if (ctype == Red) {
            for (int i = 0; i < numk; i++) {
                num += dataOfBmp_src[sx + x[i]][sy + y[i]].rgbRed * kernal[i];
            }
        } else if (ctype == Blue) {
            for (int i = 0; i < numk; i++) {
                num += dataOfBmp_src[sx + x[i]][sy + y[i]].rgbBlue * kernal[i];
            }
        } else {
            for (int i = 0; i < numk; i++) {
                num += dataOfBmp_src[sx + x[i]][sy + y[i]].rgbGreen * kernal[i];
            }
        }
        return num;
    }
}
/*
 * 函数功能：一阶锐化：水平　垂直　无方向
 */
Mat Mat::Sharpen(SharpenType stype, ProNegType ptype)
{
    Mat dst(rows,cols);
    int min_r = L, min_g = L , min_b = L;
    int max_r = -1, max_g = -1 , max_b = -1;
    int rgbRed[cols][rows];
    int rgbGreen[cols][rows];
    int rgbBlue[cols][rows];
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            if(stype == SP_Horizontal || stype == SP_Vertical || stype == SP_Laplacian || stype == SP_Wallis){
                rgbRed[i][j] = sharpen_kernal(i,j,Red,stype);
                rgbGreen[i][j] = sharpen_kernal(i,j,Green,stype);
                rgbBlue[i][j] = sharpen_kernal(i,j,Blue,stype);

            }
            else if(stype == SP_Roberts){
                if((i+1)<0||i+1>=cols||j+1<0|j+1>=rows){
                    rgbRed[i][j] = 0;
                    rgbGreen[i][j] = 0;
                    rgbBlue[i][j] = 0;
                }else{
                    rgbRed[i][j] = abs(dataOfBmp_src[i+1][j+1].rgbRed-dataOfBmp_src[i][j].rgbRed) + abs(dataOfBmp_src[i+1][j].rgbRed-dataOfBmp_src[i][j+1].rgbRed);
                    rgbGreen[i][j] = abs(dataOfBmp_src[i+1][j+1].rgbGreen-dataOfBmp_src[i][j].rgbGreen) + abs(dataOfBmp_src[i+1][j].rgbGreen-dataOfBmp_src[i][j+1].rgbGreen);
                    rgbBlue[i][j] = abs(dataOfBmp_src[i+1][j+1].rgbBlue-dataOfBmp_src[i][j].rgbBlue) + abs(dataOfBmp_src[i+1][j].rgbBlue-dataOfBmp_src[i][j+1].rgbBlue);
                }
            }else if(stype == SP_Sobel){

                rgbRed[i][j] = sqrt(pow(sharpen_kernal(i,j,Red,SP_Sobel_x),2) + pow(sharpen_kernal(i,j,Red,SP_Sobel_y),2));
                rgbGreen[i][j] = sqrt(pow(sharpen_kernal(i,j,Green,SP_Sobel_x),2) + pow(sharpen_kernal(i,j,Green,SP_Sobel_y),2));
                rgbBlue[i][j] = sqrt(pow(sharpen_kernal(i,j,Blue,SP_Sobel_x),2) + pow(sharpen_kernal(i,j,Blue,SP_Sobel_y),2));

            }else if(stype == SP_Priwitt){

                rgbRed[i][j] = sqrt(pow(sharpen_kernal(i,j,Red,SP_Priwitt_x),2) + pow(sharpen_kernal(i,j,Red,SP_Priwitt_y),2));
                rgbGreen[i][j] = sqrt(pow(sharpen_kernal(i,j,Green,SP_Priwitt_x),2) + pow(sharpen_kernal(i,j,Green,SP_Priwitt_y),2));
                rgbBlue[i][j] = sqrt(pow(sharpen_kernal(i,j,Blue,SP_Priwitt_x),2) + pow(sharpen_kernal(i,j,Blue,SP_Priwitt_y),2));
            }

            if(min_r > rgbRed[i][j]) min_r = rgbRed[i][j];
            if(min_g > rgbGreen[i][j]) min_g = rgbGreen[i][j];
            if(min_b > rgbBlue[i][j]) min_b = rgbBlue[i][j];

            if(max_r < rgbRed[i][j]) max_r = rgbRed[i][j];
            if(max_g < rgbGreen[i][j]) max_g = rgbGreen[i][j];
            if(max_b < rgbBlue[i][j]) max_b = rgbBlue[i][j];

        }

    }
    if(ptype == ADD)
    {
        for(DWORD i=0; i<cols; i++) {
            for (DWORD j = 0; j < rows; j++) {
                dst.dataOfBmp_src[i][j].rgbRed = BYTE((rgbRed[i][j] + abs(min_r)) * (L - 1) / (max_r + abs(min_r)));
                dst.dataOfBmp_src[i][j].rgbGreen = BYTE((rgbGreen[i][j] + abs(min_g)) * (L - 1) / (abs(min_g) + max_g));
                dst.dataOfBmp_src[i][j].rgbBlue = BYTE((rgbBlue[i][j] + abs(min_b)) * (L - 1) / (abs(min_b) + max_b));
            }
        }
    }else if(ptype == ABS)
    {
        for(DWORD i=0; i<cols; i++) {
            for (DWORD j = 0; j < rows; j++) {
                dst.dataOfBmp_src[i][j].rgbRed = BYTE(abs(rgbRed[i][j]) * (L - 1) / max_r);
                dst.dataOfBmp_src[i][j].rgbGreen = BYTE(abs(rgbGreen[i][j]) * (L - 1) / max_g);
                dst.dataOfBmp_src[i][j].rgbBlue = BYTE(abs(rgbBlue[i][j]) * (L - 1) / max_b);
            }
        }
    }
//    std::cout << "dsa" << std::endl;
    return dst;
}
/*
 * 函数功能：kernel 乘法　３×３
 */
int Mat::kernal_mul(int sx, int sy, ColorType ctype, kernalType ktype)
{
    int numk = 9;
    int x[] = {-1,-1,-1,0,0,0,1,1,1};
    int y[] = {-1,0,1,-1,0,1,-1,0,1};
    for(int i=0;i<numk;i++){
        if(sx+x[i]<0||sx+x[i]>=cols||sy+y[i]<0||sx+x[i]>=rows){
            if(ctype == Red) return dataOfBmp_src[sx][sy].rgbRed;
            else if(ctype == Green) return dataOfBmp_src[sx][sy].rgbGreen;
            else return dataOfBmp_src[sx][sy].rgbBlue;
        }
    }
    double num = 0;
    if(ktype == Kone){
        if(ctype == Red){

            for(int i=0;i<numk;i++){
                num += dataOfBmp_src[sx+x[i]][sy+y[i]].rgbRed * kernalone[i];
            }
        } else if(ctype == Blue){

            for(int i=0;i<numk;i++){
                num += dataOfBmp_src[sx+x[i]][sy+y[i]].rgbBlue * kernalone[i];
            }
        } else{

            for(int i=0;i<numk;i++){
                num += dataOfBmp_src[sx+x[i]][sy+y[i]].rgbGreen * kernalone[i];
            }
        }
    }
    return int(num/9.0)>(L-1)?L-1:int(num/9.0);
}
/*
 * 函数功能：kernel 中值　３×３
 */
int Mat::kernal_med(int sx, int sy, ColorType ctype, kernalType ktype)
{
    int numk = 9;
    int x[] = {-1,-1,-1,0,0,0,1,1,1};
    int y[] = {-1,0,1,-1,0,1,-1,0,1};
    for(int i=0;i<numk;i++){
        if(sx+x[i]<0||sx+x[i]>=cols||sy+y[i]<0||sx+x[i]>=rows){
            if(ctype == Red) return dataOfBmp_src[sx][sy].rgbRed;
            else if(ctype == Green) return dataOfBmp_src[sx][sy].rgbGreen;
            else return dataOfBmp_src[sx][sy].rgbBlue;
        }
    }
    std::vector<int> pixels;
    if(ktype == Kone){
        if(ctype == Red){

            for(int i=0;i<numk;i++){
                pixels.push_back(dataOfBmp_src[sx+x[i]][sy+y[i]].rgbRed * kernalone[i]);
            }
        } else if(ctype == Blue){

            for(int i=0;i<numk;i++){
                pixels.push_back(dataOfBmp_src[sx+x[i]][sy+y[i]].rgbBlue * kernalone[i]);
            }
        } else{

            for(int i=0;i<numk;i++){
                pixels.push_back(dataOfBmp_src[sx+x[i]][sy+y[i]].rgbGreen * kernalone[i]);
            }
        }
    }
    sort(pixels.begin(), pixels.end());
    return pixels[(numk+1)/2];
}
/*
 * 函数功能：kernel knn　３×３
 */
int Mat::kernal_knn(int sx, int sy, ColorType ctype, kernalType ktype)
{
    int numk = 9;
    int x[] = {-1,-1,-1,0,0,0,1,1,1};
    int y[] = {-1,0,1,-1,0,1,-1,0,1};
    for(int i=0;i<numk;i++){
        if(sx+x[i]<0||sx+x[i]>=cols||sy+y[i]<0||sx+x[i]>=rows){
            if(ctype == Red) return dataOfBmp_src[sx][sy].rgbRed;
            else if(ctype == Green) return dataOfBmp_src[sx][sy].rgbGreen;
            else return dataOfBmp_src[sx][sy].rgbBlue;
        }
    }
    std::vector<minker> pixels;
    if(ktype == Kone){
        if(ctype == Red){

            for(int i=0;i<numk;i++){
                minker st;
                st.pix = dataOfBmp_src[sx+x[i]][sy+y[i]].rgbRed;
                st.diff = abs(dataOfBmp_src[sx+x[i]][sy+y[i]].rgbRed - dataOfBmp_src[sx][sy].rgbRed);
                pixels.push_back(st);
            }
        } else if(ctype == Blue){

            for(int i=0;i<numk;i++){
                minker st;
                st.pix = dataOfBmp_src[sx+x[i]][sy+y[i]].rgbBlue;
                st.diff = abs(dataOfBmp_src[sx+x[i]][sy+y[i]].rgbBlue - dataOfBmp_src[sx][sy].rgbBlue);
                pixels.push_back(st);

            }
        } else{

            for(int i=0;i<numk;i++){
                minker st;
                st.pix = dataOfBmp_src[sx+x[i]][sy+y[i]].rgbGreen;
                st.diff = abs(dataOfBmp_src[sx+x[i]][sy+y[i]].rgbGreen - dataOfBmp_src[sx][sy].rgbGreen);
                pixels.push_back(st);
            }
        }
    }
    sort(pixels.begin(), pixels.end(), comp);
    double num = 0;
    for(int i=0; i<5; i++){
        num+=pixels[i].pix;
    }
    return int(num/9.0)>(L-1)?L-1:int(num/9.0);
}

/*
 * 函数功能：滤波器　
 * 输入:　均值　Mean 高斯
 * 中值滤波器　Median 椒盐
 * KNN
 */
Mat Mat::Filter(FilterType ftype)
{
    Mat dst(rows,cols);
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            if(ftype == Median){
                dst.dataOfBmp_src[i][j].rgbRed = BYTE(kernal_med(i,j,Red,Kone));
                dst.dataOfBmp_src[i][j].rgbGreen = BYTE(kernal_med(i,j,Green,Kone));
                dst.dataOfBmp_src[i][j].rgbBlue = BYTE(kernal_med(i,j,Blue,Kone));
            }else if(ftype == Mean){
                dst.dataOfBmp_src[i][j].rgbRed = BYTE(kernal_mul(i,j,Red,Kone));
                dst.dataOfBmp_src[i][j].rgbGreen = BYTE(kernal_mul(i,j,Green,Kone));
                dst.dataOfBmp_src[i][j].rgbBlue = BYTE(kernal_mul(i,j,Blue,Kone));
            }else if(ftype == KNN){
                dst.dataOfBmp_src[i][j].rgbRed = BYTE(kernal_knn(i,j,Red,Kone));
                dst.dataOfBmp_src[i][j].rgbGreen = BYTE(kernal_knn(i,j,Green,Kone));
                dst.dataOfBmp_src[i][j].rgbBlue = BYTE(kernal_knn(i,j,Blue,Kone));
            }

        }
    }
    return dst;
}
/*
 * 函数功能：直方图的规定化
*/
Mat Mat::histogram_map(Mat Z)
{
    Mat dst(rows,cols);
    compute_histogram();
    double pr[L];
    pr[0] = histogram[0];
    for(int i=1; i<L; i++){
        pr[i] = pr[i-1] + histogram[i];
    }

    Z.compute_histogram();
    double pz[L];
    pz[0] = Z.histogram[0];
    for(int i=1; i<L; i++){
        pz[i] = pz[i-1] + Z.histogram[i];
    }

    double diff[L][L];
    for(int i=0; i<L; i++){
        for(int j=0; j<L; j++){
            diff[i][j] = fabs(pr[i] - pz[j]);
        }
    }
    int s[L];

    for(int i=0; i<L; i++){
        double min = diff[i][0];
        int index = 0;
        for(int j=1; j<L; j++){
            if(diff[i][j] < min){
                min = diff[i][j];
                index = j;
            }
        }
        s[i] = index;
    }

    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            dst.dataOfBmp_src[i][j].rgbRed = BYTE(s[dataOfBmp_src[i][j].rgbRed]);
            dst.dataOfBmp_src[i][j].rgbGreen = BYTE(s[dataOfBmp_src[i][j].rgbGreen]);
            dst.dataOfBmp_src[i][j].rgbBlue = BYTE(s[dataOfBmp_src[i][j].rgbBlue]);
        }
    }
    return dst;
}
/*
 * 函数功能：阈值分割
 * 输入：阈值
*/
Mat Mat::thresh_seg(DWORD thresh)
{
    RGB2Gray();
    Mat dst(rows,cols);
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            if(dataOfBmp_src[i][j].rgbRed > thresh){
                dst.dataOfBmp_src[i][j].rgbRed = 255;
                dst.dataOfBmp_src[i][j].rgbGreen = 255;
                dst.dataOfBmp_src[i][j].rgbBlue = 255;
            } else{
                dst.dataOfBmp_src[i][j].rgbRed = 0;
                dst.dataOfBmp_src[i][j].rgbGreen = 0;
                dst.dataOfBmp_src[i][j].rgbBlue = 0;
            }
        }
    }
    return dst;
}
/*
 * 函数功能：直方图的均衡化
*/
Mat Mat::histo_equalization()
{
    compute_histogram();
    Mat dst(rows,cols);
    double pr[L];
    pr[0] = histogram[0];
    for(int i=1; i<L; i++){
        pr[i] = pr[i-1] + histogram[i];
    }
    int s[L];
    s[0] = 0;
    for(int i=1; i<L; i++){
        s[i] =  int((L - 1) * pr[i] + 0.5);
    }
    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            dst.dataOfBmp_src[i][j].rgbRed = BYTE(s[dataOfBmp_src[i][j].rgbRed]);
            dst.dataOfBmp_src[i][j].rgbGreen = BYTE(s[dataOfBmp_src[i][j].rgbGreen]);
            dst.dataOfBmp_src[i][j].rgbBlue = BYTE(s[dataOfBmp_src[i][j].rgbBlue]);
        }
    }
    return dst;
}
/*
 * 函数功能：展示灰度图的直方图
*/
Mat Mat::show_histogram()
{
    DWORD maxL = -1;
    DWORD double_histo[256];
    for(int i=0; i<L; i++)
        double_histo[i] = histogram[i] * 200;
    for(int i=0; i<L; i++){
        if(double_histo[i] > maxL){
            maxL = double_histo[i];
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
        for (int i = maxL-1; i >= maxL-double_histo[j];i--) {
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
    Mat dst(cols, rows);
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

    for(int i=0; i<L; i++){
        histogram[i] = 0;
    }

    for(DWORD i=0; i<cols; i++) {
        for (DWORD j = 0; j < rows; j++) {
            int index = int(dst.dataOfBmp_src[i][j].rgbRed);
            histogram[index]+=1;
        }
    }
    for(int i=0; i<L; i++){
        histogram[i] = histogram[i] / (cols * rows * 1.0);
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
